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


#include "IDirect3DIndexBuffer9.h"
#include <iostream> 

IDirect3DIndexBuffer9::IDirect3DIndexBuffer9()
{
}

IDirect3DIndexBuffer9::~IDirect3DIndexBuffer9()
{
}

HRESULT IDirect3DIndexBuffer9::GetDesc(D3DINDEXBUFFER_DESC *pDesc)
{
	std::cout << "IDirect3DIndexBuffer9::GetDesc(D3DINDEXBUFFER_DESC *pDesc)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DIndexBuffer9::Lock(UINT OffsetToLock,UINT SizeToLock,VOID **ppbData,DWORD Flags)
{
	std::cout << "IDirect3DIndexBuffer9::Lock(UINT OffsetToLock,UINT SizeToLock,VOID **ppbData,DWORD Flags)" << std::endl;		
	
	return E_NOTIMPL;
}

HRESULT IDirect3DIndexBuffer9::Unlock()
{
	std::cout << "IDirect3DIndexBuffer9::Unlock()" << std::endl;	

	return E_NOTIMPL;
}
