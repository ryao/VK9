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
 
#include "COpenGLVertexBuffer9.h"
#include "COpenGLDevice9.h"

COpenGLVertexBuffer9::COpenGLVertexBuffer9()
{
}

COpenGLVertexBuffer9::~COpenGLVertexBuffer9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	GLMPRINTF(( ">-A- ~IDirect3DVertexBuffer9" ));
	
	if (m_device)
	{
		m_device->ReleasedVertexBuffer( this );

		if (m_ctx && m_vtxBuffer)
		{
			GLMPRINTF(( ">-A- ~IDirect3DVertexBuffer9 deleting m_vtxBuffer" ));
			m_ctx->DelBuffer( m_vtxBuffer );
			m_vtxBuffer = NULL;
			GLMPRINTF(( "<-A- ~IDirect3DVertexBuffer9 deleting m_vtxBuffer - done" ));
		}
		m_device = NULL;
	}
	
	GLMPRINTF(( "<-A- ~IDirect3DVertexBuffer9" ));	
}

void COpenGLVertexBuffer9::UnlockActualSize( unsigned int nActualSize, const void *pActualData = NULL )
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "VB UnlockActualSize" );

	m_vtxBuffer->Unlock( nActualSize, pActualData );	
}

/*
HRESULT COpenGLVertexBuffer9::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
	
}
*/

HRESULT COpenGLVertexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "VB Lock" );

	// FIXME would be good to have "can't lock twice" logic

	Assert( !(Flags & D3DLOCK_READONLY) );	// not impl'd
//	Assert( !(Flags & D3DLOCK_NOSYSLOCK) );	// not impl'd - it triggers though
	
	GLMBuffLockParams lockreq;
	lockreq.m_nOffset		= OffsetToLock;
	lockreq.m_nSize			= SizeToLock;
	lockreq.m_bNoOverwrite	= (Flags & D3DLOCK_NOOVERWRITE) != 0;
	lockreq.m_bDiscard		= (Flags & D3DLOCK_DISCARD) != 0;
			
	m_vtxBuffer->Lock( &lockreq, (char**)ppbData );

	GLMPRINTF(("-X- IDirect3DDevice9::Lock on D3D buf %p (GL name %d) offset %d, size %d => address %p", this, this->m_vtxBuffer->m_nHandle, OffsetToLock, SizeToLock, *ppbData));
	return S_OK;	
}

HRESULT COpenGLVertexBuffer9::Unlock()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	
	tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "VB Unlock" );

	m_vtxBuffer->Unlock();
	return S_OK;	
}
