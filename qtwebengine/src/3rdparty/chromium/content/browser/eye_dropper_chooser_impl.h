// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_EYE_DROPPER_CHOOSER_IMPL_H_
#define CONTENT_BROWSER_EYE_DROPPER_CHOOSER_IMPL_H_

#include "content/public/browser/frame_service_base.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "third_party/blink/public/mojom/choosers/color_chooser.mojom.h"

namespace content {

class EyeDropperChooserImpl final
    : public FrameServiceBase<blink::mojom::EyeDropperChooser> {
 public:
  static void Create(RenderFrameHost*,
                     mojo::PendingReceiver<blink::mojom::EyeDropperChooser>);

  // EyeDropperChooser implementation.
  void Choose(ChooseCallback) override;

 private:
  EyeDropperChooserImpl(RenderFrameHost*,
                        mojo::PendingReceiver<blink::mojom::EyeDropperChooser>);

  DISALLOW_COPY_AND_ASSIGN(EyeDropperChooserImpl);
};

}  // namespace content

#endif  // CONTENT_BROWSER_EYE_DROPPER_CHOOSER_IMPL_H_