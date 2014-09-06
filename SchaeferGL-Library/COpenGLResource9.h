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
 
#ifndef COPENGLRESOURCE9_H
#define COPENGLRESOURCE9_H

#include "IDirect3DResource9.h" // Base class: IDirect3DResource9
#include "COpenGLUnknown.h"

class COpenGLDevice9;

class COpenGLResource9 : public IDirect3DResource9,public COpenGLUnknown
{
public:
	COpenGLResource9();
	~COpenGLResource9();

	COpenGLDevice9	*m_device;		// parent device
	D3DRESOURCETYPE		m_restype;

public:
	//virtual HRESULT FreePrivateData(REFGUID refguid);
	//virtual DWORD GetPriority();
	//virtual HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	//virtual D3DRESOURCETYPE GetType();
	//virtual void PreLoad();
	virtual DWORD SetPriority(DWORD PriorityNew);
	//virtual HRESULT SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);
};

#endif // COPENGLRESOURCE9_H
