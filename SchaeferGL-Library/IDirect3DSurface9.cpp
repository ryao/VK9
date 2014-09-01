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
 
#include "IDirect3DSurface9.h"
#include <iostream> 

IDirect3DSurface9::IDirect3DSurface9()
{
}

IDirect3DSurface9::~IDirect3DSurface9()
{
}

HRESULT IDirect3DSurface9::GetContainer(REFIID riid,void **ppContainer)
{
	std::cout << "IDirect3DSurface9::GetContainer(REFIID riid,void **ppContainer)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3DSurface9::GetDC(HDC *phdc)
{
	std::cout << "IDirect3DSurface9::GetDC(HDC *phdc)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSurface9::GetDesc(D3DSURFACE_DESC *pDesc)
{
	std::cout << "IDirect3DSurface9::GetDesc(D3DSURFACE_DESC *pDesc)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3DSurface9::LockRect(D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags)
{
	std::cout << "IDirect3DSurface9::LockRect(D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSurface9::ReleaseDC(HDC hdc)
{
	std::cout << "IDirect3DSurface9::ReleaseDC(HDC hdc)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DSurface9::UnlockRect()
{
	std::cout << "IDirect3DSurface9::UnlockRect()" << std::endl;

	return E_NOTIMPL;
}
