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
 
#ifndef GALLIUMD3DUNKNOWN_H
#define GALLIUMD3DUNKNOWN_H

#include "i_unknown.h" // Base class: IUnknown

#include "pipe/p_compiler.h"

#include "util/u_memory.h"

class GalliumD3DUnknown;
class GalliumD3DDevice9;

class GalliumD3DUnknown : public IUnknown
{
protected:
	int32_t mReferenceCount;
	
	GalliumD3DUnknown* mContainer;
	GalliumD3DDevice9* mDevice;
	
public:
	GalliumD3DUnknown(GalliumD3DDevice9* device,GalliumD3DUnknown* container);
	~GalliumD3DUnknown();

	/*
	 * Increments the reference count for an interface on an object. 
	 * This method should be called for every new copy of a pointer to an interface on an object.
	 */
	virtual ULONG AddRef();
	
	/*
	 * Retrieves pointers to the supported interfaces on an object.
	 */
	//virtual HRESULT QueryInterface(REFIID riid, void** ppvObject);
	
	/*
	 * Decrements the reference count for an interface on an object.
	 */
	virtual ULONG Release();
};

#endif // GALLIUMD3DUNKNOWN_H
