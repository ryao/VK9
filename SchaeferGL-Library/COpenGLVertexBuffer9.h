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
 
#ifndef COPENGLVERTEXBUFFER9_H
#define COPENGLVERTEXBUFFER9_H

#include "IDirect3DVertexBuffer9.h" // Base class: IDirect3DVertexBuffer9
#include "COpenGLResource9.h"

class COpenGLVertexBuffer9 : public IDirect3DVertexBuffer9,COpenGLResource9
{
private:
	
public:
	COpenGLVertexBuffer9();
	~COpenGLVertexBuffer9();

	GLMContext				*m_ctx;
	CGLMBuffer				*m_vtxBuffer;
	D3DVERTEXBUFFER_DESC	m_vtxDesc;		// to satisfy GetDesc

	void UnlockActualSize( uint nActualSize, const void *pActualData = NULL );
	
public:
	//virtual HRESULT GetDesc(D3DVERTEXBUFFER_DESC* pDesc);
	virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
	virtual HRESULT Unlock();
};

#endif // COPENGLVERTEXBUFFER9_H
