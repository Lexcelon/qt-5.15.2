// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/template_expressions.h"

#include <stddef.h>

#include "base/logging.h"
#include "base/optional.h"
#include "base/stl_util.h"
#include "base/values.h"
#include "net/base/escape.h"

#if DCHECK_IS_ON()
#include "third_party/re2/src/re2/re2.h"  // nogncheck
#endif

namespace {
const char kLeader[] = "$i18n";
const size_t kLeaderSize = base::size(kLeader) - 1;
const char kKeyOpen = '{';
const char kKeyClose = '}';
const char kHtmlTemplateEnd[] = "<!--_html_template_end_-->";
const char kHtmlTemplateStart[] = "<!--_html_template_start_-->";
const size_t kHtmlTemplateStartSize = base::size(kHtmlTemplateStart) - 1;

enum HtmlTemplateType { INVALID = 0, NONE = 1, VALID = 2 };

struct HtmlTemplate {
  base::StringPiece::size_type start;
  base::StringPiece::size_type length;
  HtmlTemplateType type;
};

HtmlTemplate FindHtmlTemplate(const base::StringPiece& source) {
  HtmlTemplate out;
  base::StringPiece::size_type found = source.find(kHtmlTemplateStart);

  // No template found, return early.
  if (found == base::StringPiece::npos) {
    out.type = NONE;
    return out;
  }

  out.start = found + kHtmlTemplateStartSize;
  base::StringPiece::size_type found_end =
      source.find(kHtmlTemplateEnd, out.start);
  // Template is not terminated.
  if (found_end == base::StringPiece::npos) {
    out.type = INVALID;
    return out;
  }

  out.length = found_end - out.start;
  // Check for a nested template
  if (source.substr(out.start, out.length).find(kHtmlTemplateStart) !=
      base::StringPiece::npos) {
    out.type = INVALID;
    return out;
  }

  out.type = VALID;
  return out;
}

// Escape quotes and backslashes ('"\).
std::string PolymerParameterEscape(const std::string& in_string) {
  std::string out;
  out.reserve(in_string.size() * 2);
  for (const char c : in_string) {
    switch (c) {
      case '\\':
        out.append("\\\\");
        break;
      case '\'':
        out.append("\\'");
        break;
      case '"':
        out.append("&quot;");
        break;
      case ',':
        out.append("\\\\,");
        break;
      default:
        out += c;
    }
  }
  return out;
}

bool EscapeForJS(const std::string& in_string,
                 base::Optional<char> in_previous,
                 std::string* out_string) {
  out_string->reserve(in_string.size() * 2);
  bool last_was_dollar = in_previous && in_previous.value() == '$';
  for (const char c : in_string) {
    switch (c) {
      case '`':
        out_string->append("\\`");
        break;
      case '{':
        // Do not allow "${".
        if (last_was_dollar)
          return false;
        *out_string += c;
        break;
      default:
        *out_string += c;
    }
    last_was_dollar = c == '$';
  }
  return true;
}

#if DCHECK_IS_ON()
// Checks whether the replacement has an unsubstituted placeholder, e.g. "$1".
bool HasUnexpectedPlaceholder(const std::string& key,
                              const std::string& replacement) {
  // TODO(crbug.com/988031): Fix display aria labels.
#if defined(OS_CHROMEOS)
  if (key == "displayResolutionText")
    return false;
#endif
  return re2::RE2::PartialMatch(replacement, re2::RE2(R"(\$\d)"));
}
#endif  // DCHECK_IS_ON()

bool ReplaceTemplateExpressionsInternal(
    base::StringPiece source,
    const ui::TemplateReplacements& replacements,
    bool is_javascript,
    std::string* formatted) {
  const size_t kValueLengthGuess = 16;
  formatted->reserve(source.length() + replacements.size() * kValueLengthGuess);
  // Two position markers are used as cursors through the |source|.
  // The |current_pos| will follow behind |next_pos|.
  size_t current_pos = 0;
  while (true) {
    size_t next_pos = source.find(kLeader, current_pos);

    if (next_pos == std::string::npos) {
      formatted->append(source.begin() + current_pos, source.end());
      break;
    }

    formatted->append(source.data() + current_pos, next_pos - current_pos);
    current_pos = next_pos + kLeaderSize;

    size_t context_end = source.find(kKeyOpen, current_pos);
    CHECK_NE(context_end, std::string::npos);
    std::string context(source.substr(current_pos, context_end - current_pos));
    current_pos = context_end + sizeof(kKeyOpen);

    size_t key_end = source.find(kKeyClose, current_pos);
    CHECK_NE(key_end, std::string::npos);

    std::string key =
        source.substr(current_pos, key_end - current_pos).as_string();
    CHECK(!key.empty());

    auto value = replacements.find(key);
    CHECK(value != replacements.end()) << "$i18n replacement key \"" << key
                                       << "\" not found";

    std::string replacement = value->second;
    if (is_javascript) {
      // Run JS escaping first.
      base::Optional<char> last = formatted->empty()
                                      ? base::nullopt
                                      : base::make_optional(formatted->back());
      std::string escaped_replacement;
      if (!EscapeForJS(replacement, last, &escaped_replacement))
        return false;
      replacement = escaped_replacement;
    }

    if (context.empty()) {
      // Make the replacement HTML safe.
      replacement = net::EscapeForHTML(replacement);
    } else if (context == "Raw") {
      // Pass the replacement through unchanged.
    } else if (context == "Polymer") {
      // Escape quotes and backslash for '$i18nPolymer{}' use (i.e. quoted).
      replacement = PolymerParameterEscape(replacement);
    } else {
      CHECK(false) << "Unknown context " << context;
    }

#if DCHECK_IS_ON()
    // Replacements in Polymer WebUI may invoke JavaScript to replace string
    // placeholders. In other contexts, placeholders should already be replaced.
    if (context != "Polymer") {
      DCHECK(!HasUnexpectedPlaceholder(key, replacement))
          << "Dangling placeholder found in " << key;
    }
#endif

    formatted->append(replacement);

    current_pos = key_end + sizeof(kKeyClose);
  }
  return true;
}

}  // namespace

namespace ui {

void TemplateReplacementsFromDictionaryValue(
    const base::DictionaryValue& dictionary,
    TemplateReplacements* replacements) {
  for (base::DictionaryValue::Iterator it(dictionary); !it.IsAtEnd();
       it.Advance()) {
    std::string str_value;
    if (it.value().GetAsString(&str_value))
      (*replacements)[it.key()] = str_value;
  }
}

bool ReplaceTemplateExpressionsInJS(base::StringPiece source,
                                    const TemplateReplacements& replacements,
                                    std::string* formatted) {
  CHECK(formatted->empty());
  base::StringPiece remaining = source;
  while (true) {
    // Replacement is only done in JS for the contents of HTML _template
    // strings.
    HtmlTemplate current_template = FindHtmlTemplate(remaining);

    // If there was an error finding a template, return false.
    if (current_template.type == INVALID)
      return false;

    // If there are no more templates, copy the remaining JS to the output and
    // return true.
    if (current_template.type == NONE) {
      formatted->append(remaining.as_string());
      return true;
    }

    // Copy the JS before the template to the output.
    formatted->append(remaining.substr(0, current_template.start).as_string());

    // Retrieve the HTML portion of the source.
    base::StringPiece html_template =
        remaining.substr(current_template.start, current_template.length);

    // Perform replacements with JS escaping.
    std::string formatted_html;
    if (!ReplaceTemplateExpressionsInternal(html_template, replacements, true,
                                            &formatted_html)) {
      return false;
    }

    // Append the formatted HTML template.
    formatted->append(formatted_html);

    // Increment to the end of the current template.
    remaining =
        remaining.substr(current_template.start + current_template.length);
  }
  return true;
}

std::string ReplaceTemplateExpressions(
    base::StringPiece source,
    const TemplateReplacements& replacements) {
  std::string formatted;
  ReplaceTemplateExpressionsInternal(source, replacements, false, &formatted);
  return formatted;
}
}  // namespace ui
