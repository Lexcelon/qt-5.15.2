// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_TRUST_TOKENS_TRUST_TOKEN_KEY_COMMITMENT_GETTER_H_
#define SERVICES_NETWORK_TRUST_TOKENS_TRUST_TOKEN_KEY_COMMITMENT_GETTER_H_

#include <memory>

#include "base/callback.h"
#include "services/network/public/mojom/trust_tokens.mojom.h"
#include "url/origin.h"

namespace network {

// Class TrustTokenKeyCommitmentGetter fetches key commitments asynchronously.
// These are used for precondition checking before issuance, and for validating
// received tokens in issuance responses.
class TrustTokenKeyCommitmentGetter {
 public:
  virtual ~TrustTokenKeyCommitmentGetter() = default;
  virtual void Get(
      const url::Origin& origin,
      base::OnceCallback<void(mojom::TrustTokenKeyCommitmentResultPtr)> on_done)
      const = 0;
};

}  // namespace network

#endif  // SERVICES_NETWORK_TRUST_TOKENS_TRUST_TOKEN_KEY_COMMITMENT_GETTER_H_
