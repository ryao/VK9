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
 
#ifndef COPENGLPIXELSHADER9_H
#define COPENGLPIXELSHADER9_H

#include "IDirect3DPixelShader9.h" // Base class: IDirect3DPixelShader9
#include "COpenGLResource9.h"

class COpenGLPixelShader9 : public IDirect3DPixelShader9,public COpenGLResource9
{
public:
	COpenGLPixelShader9();
	~COpenGLPixelShader9();

	CGLMProgram				*m_pixProgram;
	unsigned int					m_pixHighWater;		// count of active constant slots referenced by shader.
	unsigned int					m_pixSamplerMask;	// (1<<n) mask of samplers referemnced by this pixel shader
												// this can help FlushSamplers avoid SRGB flipping on textures not being referenced...
	unsigned int					m_pixSamplerTypes;  // SAMPLER_TYPE_2D, etc.
	unsigned int					m_pixFragDataMask;  // (1<<n) mask of gl_FragData[n] referenced by this pixel shader

public:
	//virtual HRESULT GetDevice(IDirect3DDevice9** ppDevice);
	//virtual HRESULT GetFunction(void* pData, UINT* pSizeOfData);
};

#endif // COPENGLPIXELSHADER9_H
