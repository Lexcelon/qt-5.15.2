// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/public/platform/modules/mediastream/web_media_stream_audio_sink.h"

#include "base/logging.h"
#include "third_party/blink/public/platform/web_media_stream_source.h"
#include "third_party/blink/public/platform/web_media_stream_track.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_audio_track.h"

namespace blink {

void WebMediaStreamAudioSink::AddToAudioTrack(
    WebMediaStreamAudioSink* sink,
    const blink::WebMediaStreamTrack& track) {
  DCHECK(track.Source().GetType() == blink::WebMediaStreamSource::kTypeAudio);
  MediaStreamAudioTrack* native_track = MediaStreamAudioTrack::From(track);
  DCHECK(native_track);
  native_track->AddSink(sink);
}

void WebMediaStreamAudioSink::RemoveFromAudioTrack(
    WebMediaStreamAudioSink* sink,
    const blink::WebMediaStreamTrack& track) {
  MediaStreamAudioTrack* native_track = MediaStreamAudioTrack::From(track);
  DCHECK(native_track);
  native_track->RemoveSink(sink);
}

media::AudioParameters WebMediaStreamAudioSink::GetFormatFromAudioTrack(
    const blink::WebMediaStreamTrack& track) {
  MediaStreamAudioTrack* native_track = MediaStreamAudioTrack::From(track);
  DCHECK(native_track);
  return native_track->GetOutputFormat();
}

}  // namespace blink
