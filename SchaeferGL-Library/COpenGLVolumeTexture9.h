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
 
#ifndef COPENGLVOLUMETEXTURE9_H
#define COPENGLVOLUMETEXTURE9_H

#include "d3d9.h" // Base class: IDirect3DVolumeTexture9
#include "COpenGLBaseTexture9.h"

class COpenGLVolumeTexture9 : public IDirect3DVolumeTexture9,public COpenGLBaseTexture9
{
public:
	COpenGLVolumeTexture9();
	~COpenGLVolumeTexture9();

	COpenGLSurface9		*m_surfZero;			// surf of top level.
	D3DVOLUME_DESC			m_volDescZero;			// volume desc top level

public:
	virtual HRESULT STDMETHODCALLTYPE AddDirtyBox(const D3DBOX* pDirtyBox);
	virtual HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc);
	virtual HRESULT STDMETHODCALLTYPE GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel);
	virtual HRESULT STDMETHODCALLTYPE LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE UnlockBox(UINT Level);
};

#endif // COPENGLVOLUMETEXTURE9_H
