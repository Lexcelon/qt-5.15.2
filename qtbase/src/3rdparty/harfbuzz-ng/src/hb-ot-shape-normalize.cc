/*
 * Copyright © 2011,2012  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#include "hb-ot-shape-normalize-private.hh"
#include "hb-ot-shape-complex-private.hh"
#include "hb-ot-shape-private.hh"


/*
 * HIGHLEVEL DESIGN:
 *
 * This file exports one main function: _hb_ot_shape_normalize().
 *
 * This function closely reflects the Unicode Normalization Algorithm,
 * yet it's different.
 *
 * Each shaper specifies whether it prefers decomposed (NFD) or composed (NFC).
 * The logic however tries to use whatever the font can support.
 *
 * In general what happens is that: each grapheme is decomposed in a chain
 * of 1:2 decompositions, marks reordered, and then recomposed if desired,
 * so far it's like Unicode Normalization.  However, the decomposition and
 * recomposition only happens if the font supports the resulting characters.
 *
 * The goals are:
 *
 *   - Try to render all canonically equivalent strings similarly.  To really
 *     achieve this we have to always do the full decomposition and then
 *     selectively recompose from there.  It's kinda too expensive though, so
 *     we skip some cases.  For example, if composed is desired, we simply
 *     don't touch 1-character clusters that are supported by the font, even
 *     though their NFC may be different.
 *
 *   - When a font has a precomposed character for a sequence but the 'ccmp'
 *     feature in the font is not adequate, use the precomposed character
 *     which typically has better mark positioning.
 *
 *   - When a font does not support a combining mark, but supports it precomposed
 *     with previous base, use that.  This needs the itemizer to have this
 *     knowledge too.  We need to provide assistance to the itemizer.
 *
 *   - When a font does not support a character but supports its canonical
 *     decomposition, well, use the decomposition.
 *
 *   - The complex shapers can customize the compose and decompose functions to
 *     offload some of their requirements to the normalizer.  For example, the
 *     Indic shaper may want to disallow recomposing of two matras.
 */

static bool
decompose_unicode (const hb_ot_shape_normalize_context_t *c,
		   hb_codepoint_t  ab,
		   hb_codepoint_t *a,
		   hb_codepoint_t *b)
{
  return (bool) c->unicode->decompose (ab, a, b);
}

static bool
compose_unicode (const hb_ot_shape_normalize_context_t *c,
		 hb_codepoint_t  a,
		 hb_codepoint_t  b,
		 hb_codepoint_t *ab)
{
  return (bool) c->unicode->compose (a, b, ab);
}

static inline void
set_glyph (hb_glyph_info_t &info, hb_font_t *font)
{
  (void) font->get_nominal_glyph (info.codepoint, &info.glyph_index());
}

static inline void
output_char (hb_buffer_t *buffer, hb_codepoint_t unichar, hb_codepoint_t glyph)
{
  buffer->cur().glyph_index() = glyph;
  buffer->output_glyph (unichar); /* This is very confusing indeed. */
  _hb_glyph_info_set_unicode_props (&buffer->prev(), buffer);
}

static inline void
next_char (hb_buffer_t *buffer, hb_codepoint_t glyph)
{
  buffer->cur().glyph_index() = glyph;
  buffer->next_glyph ();
}

static inline void
skip_char (hb_buffer_t *buffer)
{
  buffer->skip_glyph ();
}

/* Returns 0 if didn't decompose, number of resulting characters otherwise. */
static inline unsigned int
decompose (const hb_ot_shape_normalize_context_t *c, bool shortest, hb_codepoint_t ab)
{
  hb_codepoint_t a, b, a_glyph, b_glyph;
  hb_buffer_t * const buffer = c->buffer;
  hb_font_t * const font = c->font;

  if (!c->decompose (c, ab, &a, &b) ||
      (b && !font->get_nominal_glyph (b, &b_glyph)))
    return 0;

  bool has_a = (bool) font->get_nominal_glyph (a, &a_glyph);
  if (shortest && has_a) {
    /* Output a and b */
    output_char (buffer, a, a_glyph);
    if (likely (b)) {
      output_char (buffer, b, b_glyph);
      return 2;
    }
    return 1;
  }

  unsigned int ret;
  if ((ret = decompose (c, shortest, a))) {
    if (b) {
      output_char (buffer, b, b_glyph);
      return ret + 1;
    }
    return ret;
  }

  if (has_a) {
    output_char (buffer, a, a_glyph);
    if (likely (b)) {
      output_char (buffer, b, b_glyph);
      return 2;
    }
    return 1;
  }

  return 0;
}

static inline void
decompose_current_character (const hb_ot_shape_normalize_context_t *c, bool shortest)
{
  hb_buffer_t * const buffer = c->buffer;
  hb_codepoint_t u = buffer->cur().codepoint;
  hb_codepoint_t glyph;

  if (shortest && c->font->get_nominal_glyph (u, &glyph))
  {
    next_char (buffer, glyph);
    return;
  }

  if (decompose (c, shortest, u))
  {
    skip_char (buffer);
    return;
  }

  if (!shortest && c->font->get_nominal_glyph (u, &glyph))
  {
    next_char (buffer, glyph);
    return;
  }

  if (_hb_glyph_info_is_unicode_space (&buffer->cur()))
  {
    hb_codepoint_t space_glyph;
    hb_unicode_funcs_t::space_t space_type = buffer->unicode->space_fallback_type (u);
    if (space_type != hb_unicode_funcs_t::NOT_SPACE && c->font->get_nominal_glyph (0x0020u, &space_glyph))
    {
      _hb_glyph_info_set_unicode_space_fallback_type (&buffer->cur(), space_type);
      next_char (buffer, space_glyph);
      buffer->scratch_flags |= HB_BUFFER_SCRATCH_FLAG_HAS_SPACE_FALLBACK;
      return;
    }
  }

  if (u == 0x2011u)
  {
    /* U+2011 is the only sensible character that is a no-break version of another character
     * and not a space.  The space ones are handled already.  Handle this lone one. */
    hb_codepoint_t other_glyph;
    if (c->font->get_nominal_glyph (0x2010u, &other_glyph))
    {
      next_char (buffer, other_glyph);
      return;
    }
  }

  next_char (buffer, glyph); /* glyph is initialized in earlier branches. */
}

static inline void
handle_variation_selector_cluster (const hb_ot_shape_normalize_context_t *c, unsigned int end, bool short_circuit)
{
  /* TODO Currently if there's a variation-selector we give-up, it's just too hard. */
  hb_buffer_t * const buffer = c->buffer;
  hb_font_t * const font = c->font;
  for (; buffer->idx < end - 1 && !buffer->in_error;) {
    if (unlikely (buffer->unicode->is_variation_selector (buffer->cur(+1).codepoint))) {
      /* The next two lines are some ugly lines... But work. */
      if (font->get_variation_glyph (buffer->cur().codepoint, buffer->cur(+1).codepoint, &buffer->cur().glyph_index()))
      {
	buffer->replace_glyphs (2, 1, &buffer->cur().codepoint);
      }
      else
      {
        /* Just pass on the two characters separately, let GSUB do its magic. */
	set_glyph (buffer->cur(), font);
	buffer->next_glyph ();
	set_glyph (buffer->cur(), font);
	buffer->next_glyph ();
      }
      /* Skip any further variation selectors. */
      while (buffer->idx < end && unlikely (buffer->unicode->is_variation_selector (buffer->cur().codepoint)))
      {
	set_glyph (buffer->cur(), font);
	buffer->next_glyph ();
      }
    } else {
      set_glyph (buffer->cur(), font);
      buffer->next_glyph ();
    }
  }
  if (likely (buffer->idx < end)) {
    set_glyph (buffer->cur(), font);
    buffer->next_glyph ();
  }
}

static inline void
decompose_multi_char_cluster (const hb_ot_shape_normalize_context_t *c, unsigned int end, bool short_circuit)
{
  hb_buffer_t * const buffer = c->buffer;
  for (unsigned int i = buffer->idx; i < end && !buffer->in_error; i++)
    if (unlikely (buffer->unicode->is_variation_selector (buffer->info[i].codepoint))) {
      handle_variation_selector_cluster (c, end, short_circuit);
      return;
    }

  while (buffer->idx < end && !buffer->in_error)
    decompose_current_character (c, short_circuit);
}

static inline void
decompose_cluster (const hb_ot_shape_normalize_context_t *c, unsigned int end, bool might_short_circuit, bool always_short_circuit)
{
  if (likely (c->buffer->idx + 1 == end))
    decompose_current_character (c, might_short_circuit);
  else
    decompose_multi_char_cluster (c, end, always_short_circuit);
}


static int
compare_combining_class (const hb_glyph_info_t *pa, const hb_glyph_info_t *pb)
{
  unsigned int a = _hb_glyph_info_get_modified_combining_class (pa);
  unsigned int b = _hb_glyph_info_get_modified_combining_class (pb);

  return a < b ? -1 : a == b ? 0 : +1;
}


void
_hb_ot_shape_normalize (const hb_ot_shape_plan_t *plan,
			hb_buffer_t *buffer,
			hb_font_t *font)
{
  if (unlikely (!buffer->len)) return;

  _hb_buffer_assert_unicode_vars (buffer);

  hb_ot_shape_normalization_mode_t mode = plan->shaper->normalization_preference;
  const hb_ot_shape_normalize_context_t c = {
    plan,
    buffer,
    font,
    buffer->unicode,
    plan->shaper->decompose ? plan->shaper->decompose : decompose_unicode,
    plan->shaper->compose   ? plan->shaper->compose   : compose_unicode
  };

  bool always_short_circuit = mode == HB_OT_SHAPE_NORMALIZATION_MODE_NONE;
  bool might_short_circuit = always_short_circuit ||
			     (mode != HB_OT_SHAPE_NORMALIZATION_MODE_DECOMPOSED &&
			      mode != HB_OT_SHAPE_NORMALIZATION_MODE_COMPOSED_DIACRITICS_NO_SHORT_CIRCUIT);
  unsigned int count;

  /* We do a fairly straightforward yet custom normalization process in three
   * separate rounds: decompose, reorder, recompose (if desired).  Currently
   * this makes two buffer swaps.  We can make it faster by moving the last
   * two rounds into the inner loop for the first round, but it's more readable
   * this way. */


  /* First round, decompose */

  buffer->clear_output ();
  count = buffer->len;
  for (buffer->idx = 0; buffer->idx < count && !buffer->in_error;)
  {
    unsigned int end;
    for (end = buffer->idx + 1; end < count; end++)
      if (likely (!HB_UNICODE_GENERAL_CATEGORY_IS_MARK (_hb_glyph_info_get_general_category (&buffer->info[end]))))
        break;

    decompose_cluster (&c, end, might_short_circuit, always_short_circuit);
  }
  buffer->swap_buffers ();


  /* Second round, reorder (inplace) */

  count = buffer->len;
  for (unsigned int i = 0; i < count; i++)
  {
    if (_hb_glyph_info_get_modified_combining_class (&buffer->info[i]) == 0)
      continue;

    unsigned int end;
    for (end = i + 1; end < count; end++)
      if (_hb_glyph_info_get_modified_combining_class (&buffer->info[end]) == 0)
        break;

    /* We are going to do a O(n^2).  Only do this if the sequence is short,
     * but not too short ;). */
    if (end - i < 2 || end - i > HB_OT_SHAPE_COMPLEX_MAX_COMBINING_MARKS) {
      i = end;
      continue;
    }

    buffer->sort (i, end, compare_combining_class);

    if (plan->shaper->reorder_marks)
      plan->shaper->reorder_marks (plan, buffer, i, end);

    i = end;
  }


  if (mode == HB_OT_SHAPE_NORMALIZATION_MODE_NONE ||
      mode == HB_OT_SHAPE_NORMALIZATION_MODE_DECOMPOSED)
    return;

  /* Third round, recompose */

  /* As noted in the comment earlier, we don't try to combine
   * ccc=0 chars with their previous Starter. */

  buffer->clear_output ();
  count = buffer->len;
  unsigned int starter = 0;
  bool combine = true;
  buffer->next_glyph ();
  while (buffer->idx < count && !buffer->in_error)
  {
    hb_codepoint_t composed, glyph;
    if (combine &&
	/* We don't try to compose a non-mark character with it's preceding starter.
	 * This is both an optimization to avoid trying to compose every two neighboring
	 * glyphs in most scripts AND a desired feature for Hangul.  Apparently Hangul
	 * fonts are not designed to mix-and-match pre-composed syllables and Jamo. */
	HB_UNICODE_GENERAL_CATEGORY_IS_MARK (_hb_glyph_info_get_general_category (&buffer->cur())))
    {
      if (/* If there's anything between the starter and this char, they should have CCC
	   * smaller than this character's. */
	  (starter == buffer->out_len - 1 ||
	   info_cc (buffer->prev()) < info_cc (buffer->cur())) &&
	  /* And compose. */
	  c.compose (&c,
		     buffer->out_info[starter].codepoint,
		     buffer->cur().codepoint,
		     &composed) &&
	  /* And the font has glyph for the composite. */
	  font->get_nominal_glyph (composed, &glyph))
      {
	/* Composes. */
	buffer->next_glyph (); /* Copy to out-buffer. */
	if (unlikely (buffer->in_error))
	  return;
	buffer->merge_out_clusters (starter, buffer->out_len);
	buffer->out_len--; /* Remove the second composable. */
	/* Modify starter and carry on. */
	buffer->out_info[starter].codepoint = composed;
	buffer->out_info[starter].glyph_index() = glyph;
	_hb_glyph_info_set_unicode_props (&buffer->out_info[starter], buffer);

	continue;
      }
      else if (/* We sometimes custom-tailor the sorted order of marks. In that case, stop
		* trying to combine as soon as combining-class drops. */
	       starter < buffer->out_len - 1 &&
	       info_cc (buffer->prev()) > info_cc (buffer->cur()))
        combine = false;
    }

    /* Blocked, or doesn't compose. */
    buffer->next_glyph ();

    if (info_cc (buffer->prev()) == 0)
    {
      starter = buffer->out_len - 1;
      combine = true;
    }
  }
  buffer->swap_buffers ();

}
