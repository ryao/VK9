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