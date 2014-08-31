/*
 * Copyright (C) 2014 Christopher Joseph Dean Schaefer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
 
#ifndef IDIRECT3DSWAPCHAIN9_H
#define IDIRECT3DSWAPCHAIN9_H

#include "i_unknown.h" // Base class: IUnknown
#include "i_direct3d_surface9.h"

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
