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
	m_refcount[0] = 1;
	m_refcount[1] = 0;
	m_mark = (IUNKNOWN_ALLOC_SPEW_MARK_ALL != 0);	// either all are marked, or only the ones that have SetMark(true) called on them
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

ULONG STDMETHODCALLTYPE COpenGLVertexBuffer9::AddRef(void)
{
	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE COpenGLVertexBuffer9::QueryInterface(REFIID riid,void  **ppv)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE COpenGLVertexBuffer9::Release(void)
{
	return this->Release(0);
}

ULONG STDMETHODCALLTYPE COpenGLVertexBuffer9::AddRef(int which, char *comment)
{
	Assert( which >= 0 );
	Assert( which < 2 );
	m_refcount[which]++;
		
	#if IUNKNOWN_ALLOC_SPEW
		if (m_mark)
		{
			GLMPRINTF(("-A- IUAddRef  (%08x,%d) refc -> (%d,%d) [%s]",this,which,m_refcount[0],m_refcount[1],comment?comment:"..."))	;
			if (!comment)
			{
				GLMPRINTF((""))	;	// place to hang a breakpoint
			}
		}
	#endif	

	return m_refcount[0];
}

ULONG STDMETHODCALLTYPE	COpenGLVertexBuffer9::Release(int which, char *comment)
{
	Assert( which >= 0 );
	Assert( which < 2 );
		
	//int oldrefcs[2] = { m_refcount[0], m_refcount[1] };
	bool deleting = false;
		
	m_refcount[which]--;
	if ( (!m_refcount[0]) && (!m_refcount[1]) )
	{
		deleting = true;
	}
		
	#if IUNKNOWN_ALLOC_SPEW
		if (m_mark)
		{
			GLMPRINTF(("-A- IURelease (%08x,%d) refc -> (%d,%d) [%s] %s",this,which,m_refcount[0],m_refcount[1],comment?comment:"...",deleting?"->DELETING":""));
			if (!comment)
			{
				GLMPRINTF((""))	;	// place to hang a breakpoint
			}
		}
	#endif

	if (deleting)
	{
		if (m_mark)
		{
			GLMPRINTF((""))	;		// place to hang a breakpoint
		}
		delete this;
		return 0;
	}
	else
	{
		return m_refcount[0];
	}
}

HRESULT STDMETHODCALLTYPE COpenGLVertexBuffer9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLVertexBuffer9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLVertexBuffer9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLVertexBuffer9::GetType()
{
	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE COpenGLVertexBuffer9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE COpenGLVertexBuffer9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLVertexBuffer9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE COpenGLVertexBuffer9::UnlockActualSize( unsigned int nActualSize, const void *pActualData)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	//tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "VB UnlockActualSize" );

	m_vtxBuffer->Unlock( nActualSize, pActualData );	
}


HRESULT STDMETHODCALLTYPE COpenGLVertexBuffer9::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE COpenGLVertexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	//tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "VB Lock" );

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

HRESULT STDMETHODCALLTYPE COpenGLVertexBuffer9::Unlock()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	
	//tmZoneFiltered( TELEMETRY_LEVEL2, 25, TMZF_NONE, "VB Unlock" );

	m_vtxBuffer->Unlock();
	return S_OK;	
}
