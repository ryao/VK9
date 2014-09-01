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
 
#include "IDirect3DCubeTexture9.h"
#include <iostream> 

IDirect3DCubeTexture9::IDirect3DCubeTexture9()
{
}

IDirect3DCubeTexture9::~IDirect3DCubeTexture9()
{
}

HRESULT IDirect3DCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType,const RECT *pDirtyRect)
{
	std::cout << "IDirect3DCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType,const RECT *pDirtyRect)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3DCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9 **ppCubeMapSurface)
{
	std::cout << "IDirect3DCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9 **ppCubeMapSurface)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3DCubeTexture9::GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc)
{
	std::cout << "IDirect3DCubeTexture9::GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc)" << std::endl;	

	return E_NOTIMPL;
}

HRESULT IDirect3DCubeTexture9::LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags)
{
	std::cout << "IDirect3DCubeTexture9::LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags)" << std::endl;	

	return E_NOTIMPL;
}

HRESULT IDirect3DCubeTexture9::UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level)
{
	std::cout << "UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level)" << std::endl;	

	return E_NOTIMPL;
}
