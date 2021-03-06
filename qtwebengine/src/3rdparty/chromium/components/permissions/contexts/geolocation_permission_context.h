// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PERMISSIONS_CONTEXTS_GEOLOCATION_PERMISSION_CONTEXT_H_
#define COMPONENTS_PERMISSIONS_CONTEXTS_GEOLOCATION_PERMISSION_CONTEXT_H_

#include "base/callback.h"
#include "base/macros.h"
#include "build/build_config.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/permissions/permission_context_base.h"
#include "content/public/browser/browser_context.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/device/public/mojom/geolocation_control.mojom.h"

class PrefService;

namespace content {
class WebContents;
}

namespace permissions {

class GeolocationPermissionContext : public PermissionContextBase {
 public:
  // Delegate which allows embedders to modify the logic of the geolocation
  // permission context.
  class Delegate {
   public:
    virtual ~Delegate() = default;

    // Allows the delegate to override the context's DecidePermission() logic.
    // If this returns true, the base context's DecidePermission() will not be
    // called.
    virtual bool DecidePermission(content::WebContents* web_contents,
                                  const PermissionRequestID& id,
                                  const GURL& requesting_origin,
                                  bool user_gesture,
                                  BrowserPermissionCallback* callback,
                                  GeolocationPermissionContext* context) = 0;

    // Called when UpdateTabContext() is called on the context.
    virtual void UpdateTabContext(const PermissionRequestID& id,
                                  const GURL& requesting_frame,
                                  bool allowed) = 0;

#if defined(OS_ANDROID)
    // Returns whether or not the Android location permission should be
    // requested.
    virtual bool ShouldRequestAndroidLocationPermission(
        content::WebContents* web_contents) = 0;

    using PermissionUpdatedCallback = base::OnceCallback<void(bool)>;
    // Requests Android location permission, and calls |callback| with the
    // reslt.
    virtual void RequestAndroidPermission(
        content::WebContents* web_contents,
        PermissionUpdatedCallback callback) = 0;

    // Returns whether or not this |web_contents| is interactable.
    virtual bool IsInteractable(content::WebContents* web_contents) = 0;

    // Gets the pref service associated with |browser_context|.
    virtual PrefService* GetPrefs(content::BrowserContext* browser_context) = 0;

    // Returns whether |requesting_origin| is the default search engine.
    virtual bool IsRequestingOriginDSE(content::BrowserContext* browser_context,
                                       const GURL& requesting_origin) = 0;

    // Called after NotifyPermissionSet() has been called from this context.
    virtual void FinishNotifyPermissionSet(const PermissionRequestID& id,
                                           const GURL& requesting_origin,
                                           const GURL& embedding_origin) = 0;
#endif
  };

  GeolocationPermissionContext(content::BrowserContext* browser_context,
                               std::unique_ptr<Delegate> delegate);
  ~GeolocationPermissionContext() override;

  void DecidePermission(content::WebContents* web_contents,
                        const PermissionRequestID& id,
                        const GURL& requesting_origin,
                        const GURL& embedding_origin,
                        bool user_gesture,
                        BrowserPermissionCallback callback) override;

  // Make this public for use by the delegate implementation.
  using PermissionContextBase::NotifyPermissionSet;

 protected:
  std::unique_ptr<Delegate> delegate_;

 private:
  void UpdateTabContext(const PermissionRequestID& id,
                        const GURL& requesting_frame,
                        bool allowed) override;
  bool IsRestrictedToSecureOrigins() const override;

  device::mojom::GeolocationControl* GetGeolocationControl();

  mojo::Remote<device::mojom::GeolocationControl> geolocation_control_;

  DISALLOW_COPY_AND_ASSIGN(GeolocationPermissionContext);
};

}  // namespace permissions

#endif  // COMPONENTS_PERMISSIONS_CONTEXTS_GEOLOCATION_PERMISSION_CONTEXT_H_
