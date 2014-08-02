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
 
#include "i_direct3d_surface9.h"
#include <iostream> 

IDirect3DSurface9::IDirect3DSurface9()
{
}

IDirect3DSurface9::~IDirect3DSurface9()
{
}

HRESULT IDirect3DSurface9::GetContainer(REFIID riid,void **ppContainer)
{
	std::cout << "IDirect3DSurface9::GetContainer(REFIID riid,void **ppContainer)" << std::endl;	
}

HRESULT IDirect3DSurface9::GetDC(HDC *phdc)
{
	std::cout << "IDirect3DSurface9::GetDC(HDC *phdc)" << std::endl;	
}

HRESULT IDirect3DSurface9::GetDesc(D3DSURFACE_DESC *pDesc)
{
	std::cout << "IDirect3DSurface9::GetDesc(D3DSURFACE_DESC *pDesc)" << std::endl;	
}

HRESULT IDirect3DSurface9::LockRect(D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags)
{
	std::cout << "IDirect3DSurface9::LockRect(D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags)" << std::endl;	
}

HRESULT IDirect3DSurface9::ReleaseDC(HDC hdc)
{
	std::cout << "IDirect3DSurface9::ReleaseDC(HDC hdc)" << std::endl;	
}

HRESULT IDirect3DSurface9::UnlockRect()
{
	std::cout << "IDirect3DSurface9::UnlockRect()" << std::endl;	
}
