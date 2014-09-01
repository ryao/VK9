//========= Copyright Christopher Joseph Dean Schaefer, All rights reserved. ============//
//                       SchaeferGL CODE LICENSE
//
//  Copyright 2014 Christopher Joseph Dean Schaefer
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
//
//==================================================================================================
 
#include "IDirect3DResource9.h"
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

	return D3DRESOURCETYPE::D3DRTYPE_SURFACE;
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