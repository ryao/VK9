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
 
#ifndef IDIRECT3DCUBETEXTURE9_H
#define IDIRECT3DCUBETEXTURE9_H

#include "i_direct3d_base_texture9.h" // Base class: IDirect3DBaseTexture9
#include "i_direct3d_surface9.h"

class IDirect3DCubeTexture9 : public IDirect3DBaseTexture9
{
public:
	IDirect3DCubeTexture9();
	~IDirect3DCubeTexture9();

	/*
	 * Adds a dirty region to a cube texture resource.
	 */
	HRESULT AddDirtyRect(D3DCUBEMAP_FACES FaceType,const RECT *pDirtyRect);
	
	/*
	 * Retrieves a cube texture map surface.
	 */
	HRESULT GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9 **ppCubeMapSurface);
	
	/*
	 * Retrieves a description of one face of the specified cube texture level.
	 */
	HRESULT GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
	
	/*
	 * Locks a rectangle on a cube texture resource.
	 */
	HRESULT LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags);
	
	/*
	 * Unlocks a rectangle on a cube texture resource.
	 */
	HRESULT UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level);
};

#endif // IDIRECT3DCUBETEXTURE9_H
