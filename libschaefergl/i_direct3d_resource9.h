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
 
#ifndef IDIRECT3DRESOURCE9_H
#define IDIRECT3DRESOURCE9_H

#include "i_unknown.h" // Base class: IUnknown
//#include "i_direct3d_device9.h"
class IDirect3DDevice9;

class IDirect3DResource9 : public IUnknown
{
public:
	IDirect3DResource9();
	~IDirect3DResource9();

	/*
	 * Frees the specified private data associated with this resource.
	 */
	virtual HRESULT FreePrivateData(REFGUID refguid);

	/*
	 * Retrieves the device associated with a resource.
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **ppDevice);
	
	/*
	 * Retrieves the priority for this resource.
	 */
	virtual DWORD GetPriority();
	
	/*
	 * Copies the private data associated with the resource to a provided buffer.
	 */
	virtual HRESULT GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData);
	
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

#endif // IDIRECT3DRESOURCE9_H
