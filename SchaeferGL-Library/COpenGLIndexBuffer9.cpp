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
 
#include "COpenGLIndexBuffer9.h"

COpenGLIndexBuffer9::COpenGLIndexBuffer9()
{
	
}

COpenGLIndexBuffer9::~COpenGLIndexBuffer9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	GLMPRINTF(( ">-A- ~IDirect3DIndexBuffer9" ));
	
	if (m_device)
	{
		m_device->ReleasedIndexBuffer( this );

		if (m_ctx && m_idxBuffer)
		{
			GLMPRINTF(( ">-A- ~IDirect3DIndexBuffer9 deleting m_idxBuffer" ));
			m_ctx->DelBuffer( m_idxBuffer );
			GLMPRINTF(( "<-A- ~IDirect3DIndexBuffer9 deleting m_idxBuffer - done" ));
		}
		m_device = NULL;
	}
	else
	{
	}
	
	GLMPRINTF(( "<-A- ~IDirect3DIndexBuffer9" ));	
}

void COpenGLIndexBuffer9::UnlockActualSize( unsigned int nActualSize, const void *pActualData = NULL )
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "IB UnlockActualSize" );

	m_idxBuffer->Unlock( nActualSize, pActualData );	
}

HRESULT COpenGLIndexBuffer9::GetDesc(D3DINDEXBUFFER_DESC* pDesc)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	*pDesc = m_idxDesc;
	return S_OK;	
}

HRESULT COpenGLIndexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	// FIXME would be good to have "can't lock twice" logic

	tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "IB Lock" );
	
	GLMBuffLockParams lockreq;
	lockreq.m_nOffset		= OffsetToLock;
	lockreq.m_nSize			= SizeToLock;
	lockreq.m_bNoOverwrite	= ( Flags & D3DLOCK_NOOVERWRITE ) != 0;
	lockreq.m_bDiscard		= ( Flags & D3DLOCK_DISCARD ) != 0;
	
	m_idxBuffer->Lock( &lockreq, (char**)ppbData );

	return S_OK;	
}

HRESULT COpenGLIndexBuffer9::Unlock()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "IB Unlock" );

	m_idxBuffer->Unlock();

	return S_OK;	
}
