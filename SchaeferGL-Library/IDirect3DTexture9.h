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
 
#ifndef IDIRECT3DTEXTURE9_H
#define IDIRECT3DTEXTURE9_H

#include "IDirect3DBaseTexture9.h" // Base class: IDirect3DBaseTexture9

class IDirect3DSurface9;

class IDirect3DTexture9 : public IDirect3DBaseTexture9
{
public:
	IDirect3DTexture9();
	~IDirect3DTexture9();

	/*
	 * Adds a dirty region to a texture resource.
	 */
	virtual HRESULT AddDirtyRect(const RECT *pDirtyRect);

	/*
	 * Retrieves a level description of a texture resource.
	 */
	virtual HRESULT GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
	
	/*
	 * Retrieves the specified texture surface level.
	 */
	virtual HRESULT GetSurfaceLevel(UINT Level,IDirect3DSurface9 **ppSurfaceLevel);
	
	/*
	 * Locks a rectangle on a texture resource.
	 */
	virtual HRESULT LockRect(UINT Level,D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags);
	
	/*
	 * Unlocks a rectangle on a texture resource.
	 */
	virtual HRESULT UnlockRect(UINT Level);
};

#endif // IDIRECT3DTEXTURE9_H
