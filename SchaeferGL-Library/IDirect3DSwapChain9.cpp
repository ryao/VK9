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
