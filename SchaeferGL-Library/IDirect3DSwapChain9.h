//========= Copyright Christopher Joseph Dean Schaefer, All rights reserved. ============//
//                       SchaeferGL CODE LICENSE
//
//  Copyright 2014 Christopher Joseph Dean Schaefer
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
//
//==================================================================================================
 
#ifndef IDIRECT3DSWAPCHAIN9_H
#define IDIRECT3DSWAPCHAIN9_H

#include "IUnknown.h" // Base class: IUnknown
#include "IDirect3DSurface9.h"

class IDirect3DDevice9;

class IDirect3DSwapChain9 : public IUnknown
{
public:
	IDirect3DSwapChain9();
	~IDirect3DSwapChain9();

	/*
	 * Retrieves a back buffer from the swap chain of the device.
	 */
	virtual HRESULT GetBackBuffer(UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer);

	/*
	 * Retrieves the device associated with the swap chain.
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **ppDevice);
	
	/*
	 * Retrieves the display mode's spatial resolution, color resolution, and refresh frequency.
	 */
	virtual HRESULT GetDisplayMode(D3DDISPLAYMODE *pMode);
	
	/*
	 * Generates a copy of the swapchain's front buffer and places that copy in a system memory buffer provided by the application.
	 */
	virtual HRESULT GetFrontBufferData(IDirect3DSurface9 *pDestSurface);
	
	/*
	 * Retrieves the presentation parameters associated with a swap chain.
	 */
	virtual HRESULT GetPresentParameters(D3DPRESENT_PARAMETERS *pPresentationParameters);
	
	/*
	 * Returns information describing the raster of the monitor on which the swap chain is presented.
	 */
	virtual HRESULT GetRasterStatus(D3DRASTER_STATUS *pRasterStatus);
	
	/*
	 * Presents the contents of the next buffer in the sequence of back buffers owned by the swap chain.
	 */
	virtual HRESULT Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion,DWORD dwFlags);
};

#endif // IDIRECT3DSWAPCHAIN9_H
