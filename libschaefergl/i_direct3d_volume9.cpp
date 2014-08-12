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
 
#include "i_direct3d_volume9.h"
#include <iostream> 

IDirect3DVolume9::IDirect3DVolume9()
{
}

IDirect3DVolume9::~IDirect3DVolume9()
{
}

HRESULT IDirect3DVolume9::FreePrivateData(REFGUID refguid)
{
	std::cout << "IDirect3DVolume9::FreePrivateData(REFGUID refguid)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolume9::GetContainer(REFIID riid,void **ppContainer)
{
	std::cout << "IDirect3DVolume9::GetContainer(REFIID riid,void **ppContainer)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolume9::GetDesc(D3DVOLUME_DESC *pDesc)
{
	std::cout << "IDirect3DVolume9::GetDesc(D3DVOLUME_DESC *pDesc)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolume9::GetDevice(IDirect3DDevice9 **ppDevice)
{
	std::cout << "IDirect3DVolume9::GetDevice(IDirect3DDevice9 **ppDevice)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolume9::GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData)
{
	std::cout << "IDirect3DVolume9::GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolume9::LockBox(D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags)
{
	std::cout << "IDirect3DVolume9::LockBox(D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolume9::SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags)
{
	std::cout << "IDirect3DVolume9::SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolume9::UnlockBox()
{
	std::cout << "IDirect3DVolume9::UnlockBox()" << std::endl;	
	
	return E_NOTIMPL;
}