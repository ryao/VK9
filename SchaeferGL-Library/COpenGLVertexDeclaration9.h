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
 
#ifndef COPENGLVERTEXDECLARATION9_H
#define COPENGLVERTEXDECLARATION9_H

#include "d3d9.h" // Base class: IDirect3DVertexDeclaration9
#include "COpenGLUnknown.h"

class COpenGLDevice9;

class COpenGLVertexDeclaration9 : public IDirect3DVertexDeclaration9
{
public:
	COpenGLVertexDeclaration9();
	~COpenGLVertexDeclaration9();

	int	m_refcount[2];
	bool m_mark;

	COpenGLDevice9		*m_device;
	unsigned int					m_elemCount;
	D3DVERTEXELEMENT9_GL	m_elements[ MAX_D3DVERTEXELEMENTS ];
		
	unsigned __int8					m_VertexAttribDescToStreamIndex[256];

public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DVertexDeclaration9
	virtual HRESULT STDMETHODCALLTYPE GetDeclaration(D3DVERTEXELEMENT9* pDecl, UINT* pNumElements);
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);

	ULONG STDMETHODCALLTYPE AddRef( int which, char *comment = NULL );
	ULONG STDMETHODCALLTYPE	Release( int which, char *comment = NULL );
};

#endif // COPENGLVERTEXDECLARATION9_H
