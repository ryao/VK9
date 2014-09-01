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
 
#ifndef IDIRECT3DCUBETEXTURE9_H
#define IDIRECT3DCUBETEXTURE9_H

#include "IDirect3DBaseTexture9.h" // Base class: IDirect3DBaseTexture9
#include "IDirect3DSurface9.h"

class IDirect3DCubeTexture9 : public IDirect3DBaseTexture9
{
public:
	IDirect3DCubeTexture9();
	~IDirect3DCubeTexture9();

	/*
	 * Adds a dirty region to a cube texture resource.
	 */
	virtual HRESULT AddDirtyRect(D3DCUBEMAP_FACES FaceType,const RECT *pDirtyRect);
	
	/*
	 * Retrieves a cube texture map surface.
	 */
	virtual HRESULT GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9 **ppCubeMapSurface);
	
	/*
	 * Retrieves a description of one face of the specified cube texture level.
	 */
	virtual HRESULT GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
	
	/*
	 * Locks a rectangle on a cube texture resource.
	 */
	virtual HRESULT LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags);
	
	/*
	 * Unlocks a rectangle on a cube texture resource.
	 */
	virtual HRESULT UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level);
};

#endif // IDIRECT3DCUBETEXTURE9_H
