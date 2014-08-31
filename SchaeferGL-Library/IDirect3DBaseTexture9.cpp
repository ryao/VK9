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
 
#include "IDirect3DBaseTexture9.h"
#include <iostream> 

IDirect3DBaseTexture9::IDirect3DBaseTexture9()
{
}

IDirect3DBaseTexture9::~IDirect3DBaseTexture9()
{
}

VOID IDirect3DBaseTexture9::GenerateMipSubLevels()
{
	std::cout << "IDirect3DBaseTexture9::GenerateMipSubLevels()" << std::endl;	

	return;
}

D3DTEXTUREFILTERTYPE IDirect3DBaseTexture9::GetAutoGenFilterType()
{
	std::cout << "IDirect3DBaseTexture9::GetAutoGenFilterType()" << std::endl;	

	return D3DTEXTUREFILTERTYPE::D3DTEXF_NONE;
}

DWORD IDirect3DBaseTexture9::GetLevelCount()
{
	std::cout << "IDirect3DBaseTexture9::GetLevelCount()" << std::endl;	

	return 0;
}

DWORD IDirect3DBaseTexture9::GetLOD()
{
	std::cout << "IDirect3DBaseTexture9::GetLOD()" << std::endl;

	return 0;
}

HRESULT IDirect3DBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	std::cout << "IDirect3DBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)" << std::endl;	

	return E_NOTIMPL;
}

DWORD IDirect3DBaseTexture9::SetLOD(DWORD LODNew)
{
	std::cout << "IDirect3DBaseTexture9::SetLOD(DWORD LODNew)" << std::endl;	

	return 0;
}