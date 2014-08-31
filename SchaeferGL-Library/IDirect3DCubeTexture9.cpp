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
