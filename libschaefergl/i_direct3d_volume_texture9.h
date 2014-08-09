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
 
#ifndef IDIRECT3DVOLUMETEXTURE9_H
#define IDIRECT3DVOLUMETEXTURE9_H

#include "i_direct3d_base_texture9.h" // Base class: IDirect3DBaseTexture9
#include "i_direct3d_volume9.h"

class IDirect3DVolumeTexture9 : public IDirect3DBaseTexture9
{
public:
	IDirect3DVolumeTexture9();
	~IDirect3DVolumeTexture9();

	/*
	 * Adds a dirty region to a volume texture resource.
	 */
	virtual HRESULT AddDirtyBox(const D3DBOX *pDirtyBox);

	/*
	 * Retrieves a level description of a volume texture resource.
	 */
	virtual HRESULT GetLevelDesc(UINT Level,D3DVOLUME_DESC *pDesc);
	
	/*
	 * Retrieves the specified volume texture level.
	 */
	virtual HRESULT GetVolumeLevel(UINT Level,IDirect3DVolume9 **ppVolumeLevel);

	/*
	 * Locks a box on a volume texture resource.
	 */
	virtual HRESULT LockBox(UINT Level,D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags);
	
	/*
	 * Unlocks a box on a volume texture resource.
	 */
	virtual HRESULT UnlockBox(UINT Level);
};

typedef struct IDirect3DVolumeTexture9 *LPDIRECT3DVOLUMETEXTURE9, *PDIRECT3DVOLUMETEXTURE9;

#endif // IDIRECT3DVOLUMETEXTURE9_H
