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
 
#ifndef IDIRECT3DTEXTURE9_H
#define IDIRECT3DTEXTURE9_H

#include "i_direct3d_base_texture9.h" // Base class: IDirect3DBaseTexture9

class IDirect3DSurface9;

class IDirect3DTexture9 : public IDirect3DBaseTexture9
{
public:
	IDirect3DTexture9();
	~IDirect3DTexture9();

	/*
	 * Adds a dirty region to a texture resource.
	 */
	virtual HRESULT AddDirtyRect(const RECT *pDirtyRect);

	/*
	 * Retrieves a level description of a texture resource.
	 */
	virtual HRESULT GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
	
	/*
	 * Retrieves the specified texture surface level.
	 */
	virtual HRESULT GetSurfaceLevel(UINT Level,IDirect3DSurface9 **ppSurfaceLevel);
	
	/*
	 * Locks a rectangle on a texture resource.
	 */
	virtual HRESULT LockRect(UINT Level,D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags);
	
	/*
	 * Unlocks a rectangle on a texture resource.
	 */
	virtual HRESULT UnlockRect(UINT Level);
};

#endif // IDIRECT3DTEXTURE9_H
