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
 
#include "IDirect3DVolumeTexture9.h"
#include <iostream> 

IDirect3DVolumeTexture9::IDirect3DVolumeTexture9()
{
}

IDirect3DVolumeTexture9::~IDirect3DVolumeTexture9()
{
}

HRESULT IDirect3DVolumeTexture9::AddDirtyBox(const D3DBOX *pDirtyBox)
{
	std::cout << "AddDirtyBox(const D3DBOX *pDirtyBox)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolumeTexture9::GetLevelDesc(UINT Level,D3DVOLUME_DESC *pDesc)
{
	std::cout << "GetLevelDesc(UINT Level,D3DVOLUME_DESC *pDesc)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolumeTexture9::GetVolumeLevel(UINT Level,IDirect3DVolume9 **ppVolumeLevel)
{
	std::cout << "GetVolumeLevel(UINT Level,IDirect3DVolume9 **ppVolumeLevel)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVolumeTexture9::LockBox(UINT Level,D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags)
{
	std::cout << "LockBox(UINT Level,D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags)" << std::endl;

	return E_NOTIMPL;	
}

HRESULT IDirect3DVolumeTexture9::UnlockBox(UINT Level)
{
	std::cout << "UnlockBox(UINT Level)" << std::endl;	
	
	return E_NOTIMPL;
}

