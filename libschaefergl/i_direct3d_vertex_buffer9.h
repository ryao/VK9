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
 
#ifndef IDIRECT3DVERTEXBUFFER9_H
#define IDIRECT3DVERTEXBUFFER9_H

#include "i_direct3d_resource9.h" // Base class: IDirect3DResource9

class IDirect3DVertexBuffer9 : public IDirect3DResource9
{
public:
	IDirect3DVertexBuffer9();
	~IDirect3DVertexBuffer9();

	/*
	 * Retrieves a description of the vertex buffer resource.
	 */
	virtual HRESULT GetDesc(D3DVERTEXBUFFER_DESC *pDesc);
	
	/*
	 * Locks a range of vertex data and obtains a pointer to the vertex buffer memory.
	 */
	virtual HRESULT Lock(UINT OffsetToLock,UINT SizeToLock,VOID **ppbData,DWORD Flags);
	
	/*
	 * Unlocks vertex data.
	 */
	virtual HRESULT Unlock();
};

#endif // IDIRECT3DVERTEXBUFFER9_H
