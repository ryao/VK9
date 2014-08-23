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
 
#ifndef GALLIUMD3DRESOURCE9_H
#define GALLIUMD3DRESOURCE9_H

#include "i_direct3d_resource9.h" // Base class: IDirect3DResource9
#include "GalliumD3DUnknown.h"
#include "pipe/p_state.h"
#include "util/u_hash_table.h"

#include "nine_pdata.h"

class GalliumD3DResource9 : public IDirect3DResource9, public GalliumD3DUnknown
{
protected:	
	pipe_resource* mResource;
	pipe_resource mResourceConfiguration;

    uint8_t* mData;
	util_hash_table* mPrivateData;
	
    D3DRESOURCETYPE mType;
    D3DPOOL mPool;
    DWORD mPriority;
    DWORD mUsage;

public:
	GalliumD3DResource9(GalliumD3DDevice9* device,GalliumD3DUnknown* container,BOOL Allocate,D3DRESOURCETYPE Type,D3DPOOL Pool);
	~GalliumD3DResource9();

	/*
	 * Frees the specified private data associated with this resource.
	 */
	virtual HRESULT FreePrivateData(REFGUID refguid);
	
	/*
	 * Retrieves the device associated with a resource.
	 */	
	virtual HRESULT GetDevice(IDirect3DDevice9** ppDevice);
	
	/*
	 * Retrieves the priority for this resource.
	 */	
	virtual DWORD GetPriority();
	
	/*
	 * Copies the private data associated with the resource to a provided buffer.
	 */	
	virtual HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	
	/*
	 * Returns the type of the resource.
	 */
	virtual D3DRESOURCETYPE GetType();
	
	/*
	 * Preloads a managed resource.
	 */	
	virtual void PreLoad();
	
	/*
	 * Assigns the priority of a resource for scheduling purposes.
	 */
	virtual DWORD SetPriority(DWORD PriorityNew);
	
	/*
	 * Associates data with the resource that is intended for use by the application
	 */
	virtual HRESULT SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags);
};


#endif // GALLIUMD3DRESOURCE9_H
