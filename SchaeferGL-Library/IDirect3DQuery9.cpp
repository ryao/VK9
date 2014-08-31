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
 
#include "IDirect3DQuery9.h"
#include <iostream> 

IDirect3DQuery9::IDirect3DQuery9()
{
}

IDirect3DQuery9::~IDirect3DQuery9()
{
}

HRESULT IDirect3DQuery9::GetData(void *pData,DWORD dwSize,DWORD dwGetDataFlags)
{
	std::cout << "IDirect3DQuery9::GetData(void *pData,DWORD dwSize,DWORD dwGetDataFlags)" << std::endl;
	
	return E_NOTIMPL;
}

DWORD IDirect3DQuery9::GetDataSize()
{
	std::cout << "IDirect3DQuery9::GetDataSize()" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DQuery9::GetDevice(IDirect3DDevice9 **pDevice)
{
	std::cout << "IDirect3DQuery9::GetDevice(IDirect3DDevice9 **pDevice)" << std::endl;	
	
	return E_NOTIMPL;
}

D3DQUERYTYPE IDirect3DQuery9::GetType()
{
	std::cout << "IDirect3DQuery9::GetType()" << std::endl;	

	return D3DQUERYTYPE::D3DQUERYTYPE_BANDWIDTHTIMINGS;
}

HRESULT IDirect3DQuery9::Issue(DWORD dwIssueFlags)
{
	std::cout << "IDirect3DQuery9::Issue(DWORD dwIssueFlags)" << std::endl;	
	
	return E_NOTIMPL;
}