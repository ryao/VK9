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
 
#include "i_direct3d_volume_texture9.h"
#include <iostream> 

IDirect3DVolumeTexture9::IDirect3DVolumeTexture9()
{
}

IDirect3DVolumeTexture9::~IDirect3DVolumeTexture9()
{
}

HRESULT AddDirtyBox(const D3DBOX *pDirtyBox)
{
	std::cout << "AddDirtyBox(const D3DBOX *pDirtyBox)" << std::endl;	
}

HRESULT GetLevelDesc(UINT Level,D3DVOLUME_DESC *pDesc)
{
	std::cout << "GetLevelDesc(UINT Level,D3DVOLUME_DESC *pDesc)" << std::endl;	
}

HRESULT GetVolumeLevel(UINT Level,IDirect3DVolume9 **ppVolumeLevel)
{
	std::cout << "GetVolumeLevel(UINT Level,IDirect3DVolume9 **ppVolumeLevel)" << std::endl;	
}

HRESULT LockBox(UINT Level,D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags)
{
	std::cout << "LockBox(UINT Level,D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags)" << std::endl;	
}

HRESULT UnlockBox(UINT Level)
{
	std::cout << "UnlockBox(UINT Level)" << std::endl;	
}

