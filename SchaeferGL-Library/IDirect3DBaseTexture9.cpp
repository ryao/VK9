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