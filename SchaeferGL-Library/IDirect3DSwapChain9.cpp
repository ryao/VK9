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
 
#include "IDirect3DSwapChain9.h"
#include <iostream> 

IDirect3DSwapChain9::IDirect3DSwapChain9()
{
}

IDirect3DSwapChain9::~IDirect3DSwapChain9()
{
}

HRESULT IDirect3DSwapChain9::GetBackBuffer(UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)
{
	std::cout << "IDirect3DSwapChain9::GetBackBuffer(UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSwapChain9::GetDevice(IDirect3DDevice9 **ppDevice)
{
	std::cout << "IDirect3DSwapChain9::GetDevice(IDirect3DDevice9 **ppDevice)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSwapChain9::GetDisplayMode(D3DDISPLAYMODE *pMode)
{
	std::cout << "IDirect3DSwapChain9::GetDisplayMode(D3DDISPLAYMODE *pMode)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSwapChain9::GetFrontBufferData(IDirect3DSurface9 *pDestSurface)
{
	std::cout << "IDirect3DSwapChain9::GetFrontBufferData(IDirect3DSurface9 *pDestSurface)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSwapChain9::GetPresentParameters(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	std::cout << "IDirect3DSwapChain9::GetPresentParameters(D3DPRESENT_PARAMETERS *pPresentationParameters)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSwapChain9::GetRasterStatus(D3DRASTER_STATUS *pRasterStatus)
{
	std::cout << "IDirect3DSwapChain9::GetRasterStatus(D3DRASTER_STATUS *pRasterStatus)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSwapChain9::Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion,DWORD dwFlags)
{
	std::cout << "IDirect3DSwapChain9::Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion,DWORD dwFlags)" << std::endl;	
	
	return E_NOTIMPL;
}
