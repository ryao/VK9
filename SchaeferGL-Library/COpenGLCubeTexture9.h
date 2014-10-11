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

#include "d3d9.h" // Base class: IDirect3DCubeTexture9
#include "COpenGLBaseTexture9.h"
#include "COpenGLSurface9.h"

class COpenGLCubeTexture9 : public IDirect3DCubeTexture9
{
public:
	COpenGLCubeTexture9();
	~COpenGLCubeTexture9();

	COpenGLDevice9	*m_device;		// parent device
	D3DRESOURCETYPE		m_restype;

	int	m_refcount[2];
	bool m_mark;

	D3DSURFACE_DESC			m_descZero;			// desc of top level.
	CGLMTex					*m_tex;				// a CGLMTex can represent all forms of tex

	COpenGLSurface9		*m_surfZero[6];			// surfs of top level.

public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DResource9
	virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);
	virtual DWORD STDMETHODCALLTYPE GetPriority();
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual D3DRESOURCETYPE STDMETHODCALLTYPE GetType();
	virtual void STDMETHODCALLTYPE PreLoad();
	virtual DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);

	//IDirect3DBaseTexture9
	virtual VOID STDMETHODCALLTYPE GenerateMipSubLevels();
	virtual D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE GetAutoGenFilterType();
	virtual DWORD STDMETHODCALLTYPE GetLOD();
	virtual DWORD STDMETHODCALLTYPE GetLevelCount();
	virtual HRESULT STDMETHODCALLTYPE SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);
	virtual DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew);

	//IDirect3DCubeTexture9
	virtual HRESULT STDMETHODCALLTYPE AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect);
	virtual HRESULT STDMETHODCALLTYPE GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface);
	virtual HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);
	virtual HRESULT STDMETHODCALLTYPE LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level);

	ULONG STDMETHODCALLTYPE AddRef( int which, char *comment = NULL );
	ULONG STDMETHODCALLTYPE	Release( int which, char *comment = NULL );
};

#endif // COPENGLCUBETEXTURE9_H
