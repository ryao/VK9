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
 
#ifndef COPENGLBASETEXTURE9_H
#define COPENGLBASETEXTURE9_H

#include "IDirect3DBaseTexture9.h" // Base class: IDirect3DBaseTexture9
#include "COpenGLResource9.h"

class COpenGLBaseTexture9 : public IDirect3DBaseTexture9,public COpenGLResource9
{
public:
	COpenGLBaseTexture9();
	~COpenGLBaseTexture9();

	D3DSURFACE_DESC			m_descZero;			// desc of top level.
	CGLMTex					*m_tex;				// a CGLMTex can represent all forms of tex

public:
	//virtual VOID GenerateMipSubLevels();
	//virtual D3DTEXTUREFILTERTYPE GetAutoGenFilterType();
	//virtual DWORD GetLOD();
	virtual DWORD GetLevelCount();
	//virtual HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);
	//virtual DWORD SetLOD(DWORD LODNew);
	
	
	virtual D3DRESOURCETYPE GetType();
};

#endif // COPENGLBASETEXTURE9_H
