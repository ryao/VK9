//========= Copyright Valve Corporation, All rights reserved. ============//
//                       TOGL CODE LICENSE
//
//  Copyright 2011-2014 Valve Corporation
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
// dxabstract.h
//
//==================================================================================================

/*
 * Code from ToGL has been modified to fit the design.
 */
 
#ifndef COPENGLCUBETEXTURE9_H
#define COPENGLCUBETEXTURE9_H

#include "IDirect3DCubeTexture9.h" // Base class: IDirect3DCubeTexture9
#include "COpenGLBaseTexture9.h"
#include "COpenGLSurface9.h"

class COpenGLCubeTexture9 : public IDirect3DCubeTexture9,public COpenGLBaseTexture9
{
public:
	COpenGLCubeTexture9();
	~COpenGLCubeTexture9();

	COpenGLSurface9		*m_surfZero[6];			// surfs of top level.

public:
	//virtual HRESULT AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect);
	virtual HRESULT GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface);
	virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);
	//virtual HRESULT LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
	//virtual HRESULT UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level);
};

#endif // COPENGLCUBETEXTURE9_H
