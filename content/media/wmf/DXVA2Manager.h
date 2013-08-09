/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#if !defined(DXVA2Manager_h_)
#define DXVA2Manager_h_

#include "WMF.h"
#include "nsAutoPtr.h"
#include "mozilla/Mutex.h"
#include "nsRect.h"

class nsIntSize;

namespace mozilla {

namespace layers {
class Image;
class ImageContainer;
}

class DXVA2Manager {
public:

  // Creates and initializes a DXVA2Manager. We can use DXVA2 via either
  // D3D9Ex or D3D11.
  static DXVA2Manager* CreateD3D9DXVA();
  static DXVA2Manager* CreateD3D11DXVA();

  // Returns a pointer to the D3D device manager responsible for managing the
  // device we're using for hardware accelerated video decoding. If we're using
  // D3D9Ex, this is an IDirect3DDeviceManager9. For D3D11 this is an
  // IMFDXGIDeviceManager. It is safe to call this on any thread.
  virtual IUnknown* GetDXVADeviceManager() = 0;

  // Creates an Image for the video frame stored in aVideoSample.
  virtual HRESULT CopyToImage(IMFSample* aVideoSample,
                              const nsIntRect& aRegion,
                              layers::ImageContainer* aContainer,
                              layers::Image** aOutImage) = 0;

  virtual ~DXVA2Manager();

protected:
  Mutex mLock;
  DXVA2Manager();
};

} // namespace mozilla

#endif // DXVA2Manager_h_
