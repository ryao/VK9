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
 
#ifndef IDIRECT3DQUERY9_H
#define IDIRECT3DQUERY9_H

#include "i_unknown.h" // Base class: IUnknown

class IDirect3DDevice9;

class IDirect3DQuery9 : public IUnknown
{
public:
	IDirect3DQuery9();
	~IDirect3DQuery9();

	/*
	 * Polls a queried resource to get the query state or a query result.
	 */
	virtual HRESULT GetData(void *pData,DWORD dwSize,DWORD dwGetDataFlags);
	
	/*
	 * Gets the number of bytes in the query data.
	 */
	virtual DWORD GetDataSize();

	/*
	 * Gets the device that is being queried.
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **pDevice);

	/*
	 * Gets the query type.
	 */
	virtual D3DQUERYTYPE GetType();

	/*
	 * Issue a query.
	 */
	virtual HRESULT Issue(DWORD dwIssueFlags);
	
};

typedef struct IDirect3DQuery9 *LPDIRECT3DQUERY9, *PDIRECT3DQUERY9;

#endif // IDIRECT3DQUERY9_H
