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
 
#include "i_direct3d_resource9.h"
#include <iostream> 

IDirect3DResource9::IDirect3DResource9()
{
}

IDirect3DResource9::~IDirect3DResource9()
{
}

HRESULT IDirect3DResource9::FreePrivateData(REFGUID refguid)
{
	std::cout << "IDirect3DResource9::FreePrivateData(REFGUID refguid)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DResource9::GetDevice(IDirect3DDevice9 **ppDevice)
{
	std::cout << "IDirect3DResource9::GetDevice(IDirect3DDevice9 **ppDevice)" << std::endl;
	
	return E_NOTIMPL;
}

DWORD IDirect3DResource9::GetPriority()
{
	std::cout << "IDirect3DResource9::GetPriority()" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DResource9::GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData)
{
	std::cout << "IDirect3DResource9::GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData)" << std::endl;
	
	return E_NOTIMPL;
}

D3DRESOURCETYPE IDirect3DResource9::GetType()
{
	std::cout << "IDirect3DResource9::GetType()" << std::endl;
}

void IDirect3DResource9::PreLoad()
{
	std::cout << "IDirect3DResource9::PreLoad()" << std::endl;
	
	return;
}

DWORD IDirect3DResource9::SetPriority(DWORD PriorityNew)
{
	std::cout << "IDirect3DResource9::SetPriority(DWORD PriorityNew)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DResource9::SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags)
{
	std::cout << "IDirect3DResource9::SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags)" << std::endl;
	
	return E_NOTIMPL;	
}