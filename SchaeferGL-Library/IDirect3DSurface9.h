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
 
#ifndef IDIRECT3DSURFACE9_H
#define IDIRECT3DSURFACE9_H

#include "IDirect3DResource9.h" // Base class: IDirect3DResource9

class IDirect3DSurface9 : public IDirect3DResource9
{
public:
	IDirect3DSurface9();
	~IDirect3DSurface9();

	/*
	 * Provides access to the parent cube texture or texture object if this surface is a cube texture or mipmap child.
	 * This method can also provide access to the parent swap chain if hte surface is a back-buffer child.
	 */
	virtual HRESULT GetContainer(REFIID riid,void **ppContainer);

	/*
	 * Retrieves a device context.
	 */
	virtual HRESULT GetDC(HDC *phdc);
	
	/*
	 * Retrieves a description of the surface.
	 */
	virtual HRESULT GetDesc(D3DSURFACE_DESC *pDesc);
	
	/*
	 * Locks a rectangle on a surface.
	 */
	virtual HRESULT LockRect(D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags);
	
	/*
	 * Release a device context handle.
	 */
	virtual HRESULT ReleaseDC(HDC hdc);
	
	/*
	 * Unlocks a rectangle on a surface.
	 */
	virtual HRESULT UnlockRect();
};

typedef struct IDirect3DSurface9 *LPDIRECT3DSURFACE9, *PDIRECT3DSURFACE9;

#endif // IDIRECT3DSURFACE9_H
