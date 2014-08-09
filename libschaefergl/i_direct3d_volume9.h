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
 
#ifndef IDIRECT3DVOLUME9_H
#define IDIRECT3DVOLUME9_H

#include "i_unknown.h" // Base class: IUnknown

class IDirect3DDevice9;

class IDirect3DVolume9 : public IUnknown
{
public:
	IDirect3DVolume9();
	~IDirect3DVolume9();

	/*
	 * Frees the specified private data associated with this volume.
	 */
	virtual HRESULT FreePrivateData(REFGUID refguid);
	
	/*
	 * Provides access to the parent volume texture object.
	 */
	virtual HRESULT GetContainer(REFIID riid,void **ppContainer);
	
	/*
	 * Retieves a description of the volume.
	 */
	virtual HRESULT GetDesc(D3DVOLUME_DESC *pDesc);
	
	/*
	 * Retrieves the device associated with a volume.
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **ppDevice);
	
	/*
	 * Copies the private data associated with the volume to a provided buffer.
	 */
	virtual HRESULT GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData);
	
	/*
	 * Locks a box on a volume resource.
	 */
	virtual HRESULT LockBox(D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags);
	
	/*
	 * Associates data with the volume that is intended for use by the application.
	 */
	virtual HRESULT SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags);
	
	/*
	 * Unlocks a box on a volume resource.
	 */
	virtual HRESULT UnlockBox();
};

typedef struct IDirect3DVolume9 *LPDIRECT3DVOLUME9, *PDIRECT3DVOLUME9;

#endif // IDIRECT3DVOLUME9_H
