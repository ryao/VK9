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
 
#include "GalliumD3DResource9.h"
#include "galliumd3d_device9.h"

#include "pipe/p_screen.h"

#include "util/u_hash_table.h"
#include "util/u_inlines.h"

GalliumD3DResource9::GalliumD3DResource9(GalliumD3DDevice9* device,GalliumD3DUnknown* container,BOOL Allocate,D3DRESOURCETYPE Type,D3DPOOL Pool)
: GalliumD3DUnknown(device,container)
{
	pipe_screen* screen;
	
	this->mResourceConfiguration.screen = screen = this->mDevice->GetScreen();	
	
	if (Allocate) 
	{
		//resource_create: Create a new resource from a template.
		this->mResource = screen->resource_create(screen, &this->mResourceConfiguration);
		if (this->mResource==NULL)
		{
			return; //D3DERR_OUTOFVIDEOMEMORY
		}
	}
	
	this->mData = NULL;
    this->mType = Type;
    this->mPool = Pool;
    this->mPriority = 0;

    this->mPrivateData = util_hash_table_create(ht_guid_hash, ht_guid_compare);
    if (this->mPrivateData==NULL)
	{
        return; //E_OUTOFMEMORY
	}
}

GalliumD3DResource9::~GalliumD3DResource9()
{
	if (this->mPrivateData!=NULL) 
	{
        util_hash_table_foreach(this->mPrivateData, ht_guid_delete, NULL);
        util_hash_table_destroy(this->mPrivateData);
    }
}

HRESULT GalliumD3DResource9::FreePrivateData(REFGUID refguid)
{
	struct pheader* header=NULL;
	
    header = (pheader*)util_hash_table_get(this->mPrivateData, refguid);
	
    if (header==NULL)
	{
        return D3DERR_NOTFOUND;
	}
	
	if (header->unknown) 
	{ 
		IUnknown* unknown = *(IUnknown **)header->data;
		if(unknown!=NULL)
		{
			unknown->Release();	
		}
	}
    FREE(header);
    util_hash_table_remove(this->mPrivateData, refguid);

    return D3D_OK;	
}

HRESULT GalliumD3DResource9::GetDevice(IDirect3DDevice9** ppDevice)
{
	GalliumD3DDevice9* device = NULL;
	IDirect3DDevice9* deviceInterface = NULL;	
	
	deviceInterface = (IDirect3DDevice9*)device;
	ppDevice = &deviceInterface;
	
	return D3D_OK;	
}

DWORD GalliumD3DResource9::GetPriority()
{
	return this->mPriority;
}

HRESULT GalliumD3DResource9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	pheader* header=NULL;

	header = (pheader*)util_hash_table_get(this->mPrivateData, refguid);

	if (header==NULL) 
	{ 
		return D3DERR_NOTFOUND; 
	}
	
	if (pData==NULL) 
	{
        *pSizeOfData = header->size;
        return D3D_OK;
    }
	
	if (*pSizeOfData < header->size) 
	{
        return D3DERR_MOREDATA;
    }
	
	if (header->unknown) 
	{ 
		IUnknown* unknown = *(IUnknown **)header->data;
		if(unknown!=NULL)
		{
			unknown->AddRef();		
		}
	}
	
	memcpy(pData, header->data, header->size);
	
	return D3D_OK;
}

D3DRESOURCETYPE GalliumD3DResource9::GetType()
{
	return this->mType;
}

void GalliumD3DResource9::PreLoad()
{
	/*
	 * Need to do more research to see what to do with this.
	 */
}

DWORD GalliumD3DResource9::SetPriority(DWORD PriorityNew)
{
	DWORD PriorityOld = this->mPriority;
	this->mPriority = PriorityNew;
	return this->mPriority;
}

HRESULT GalliumD3DResource9::SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags)
{
	pheader* header=NULL;
	const void* userData = pData;
	pipe_error err;
	
	header = CALLOC_VARIANT_LENGTH_STRUCT(pheader, SizeOfData-1); //define from u_memory.h boils down to CALLOC call.
	if (header==NULL) 
	{ 
		return E_OUTOFMEMORY; 
	}
	
    header->unknown = (Flags & D3DSPD_IUNKNOWN) ? TRUE : FALSE;
	
	//Remove old data for refguid before adding new data.
	this->FreePrivateData(refguid);
	
	if (header->unknown) 
	{
        userData = &pData;
    }

	header->size = SizeOfData;
    memcpy(header->data, userData, header->size);
	
	err = util_hash_table_set(this->mPrivateData, refguid, header);
    if (err == PIPE_OK) 
	{
		if (header->unknown) 
		{ 
			IUnknown* unknown = *(IUnknown **)header->data;
			if(unknown!=NULL)
			{
				unknown->AddRef();		
			}
		}
        return D3D_OK;
    }
	
	FREE(header);
	
    if (err == PIPE_ERROR_OUT_OF_MEMORY) 
	{ 
		return E_OUTOFMEMORY; 
	}

    return D3DERR_DRIVERINTERNALERROR;
}