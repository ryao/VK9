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
 
#include "COpenGLBaseTexture9.h"

COpenGLBaseTexture9::COpenGLBaseTexture9()
{
}

COpenGLBaseTexture9::~COpenGLBaseTexture9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	GLMPRINTF(( ">-A- ~IDirect3DBaseTexture9" ));

	if (m_device)
	{
		Assert( m_device->m_ObjectStats.m_nTotalTextures >= 1 );
		m_device->m_ObjectStats.m_nTotalTextures--;

		GLMPRINTF(( "-A- ~IDirect3DBaseTexture9 taking normal delete path on %08x, device is %08x ", this, m_device ));
		m_device->ReleasedTexture( this );
		
		if (m_tex)
		{
			GLMPRINTF(("-A- ~IDirect3DBaseTexture9 deleted '%s' @ %08x (GLM %08x) %s",m_tex->m_layout->m_layoutSummary, this, m_tex, m_tex->m_debugLabel ? m_tex->m_debugLabel : "" ));

			m_device->ReleasedCGLMTex( m_tex );

			m_tex->m_ctx->DelTex( m_tex );
			m_tex = NULL;
		}
		else
		{
			GLMPRINTF(( "-A- ~IDirect3DBaseTexture9 : whoops, no tex to delete here ?" ));
		}		
		m_device = NULL;	// ** THIS ** is the only place to scrub this.  Don't do it in the subclass destructors.
	}
	else
	{
		GLMPRINTF(( "-A- ~IDirect3DBaseTexture9 taking strange delete path on %08x, device is %08x ", this, m_device ));
	}

	GLMPRINTF(( "<-A- ~IDirect3DBaseTexture9" ));	
}

/*
VOID COpenGLBaseTexture9::GenerateMipSubLevels()
{
}
D3DTEXTUREFILTERTYPE COpenGLBaseTexture9::GetAutoGenFilterType()
{
}
DWORD COpenGLBaseTexture9::GetLOD()
{
}
*/

DWORD COpenGLBaseTexture9::GetLevelCount()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_tex->m_layout->m_mipCount;	
}

/*
HRESULT COpenGLBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
}
DWORD COpenGLBaseTexture9::SetLOD(DWORD LODNew)
{
}
*/

virtual D3DRESOURCETYPE COpenGLBaseTexture9::GetType()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_restype;	//D3DRTYPE_TEXTURE;	
}