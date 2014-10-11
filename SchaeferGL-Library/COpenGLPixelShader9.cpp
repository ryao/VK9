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
 
#include "COpenGLPixelShader9.h"
#include "COpenGLDevice9.h"

COpenGLPixelShader9::COpenGLPixelShader9()
{
	m_refcount[0] = 1;
	m_refcount[1] = 0;
	m_mark = (IUNKNOWN_ALLOC_SPEW_MARK_ALL != 0);	// either all are marked, or only the ones that have SetMark(true) called on them	
}

COpenGLPixelShader9::~COpenGLPixelShader9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	GLMPRINTF(( ">-A- ~IDirect3DPixelShader9" ));

	if (m_device)
	{
		m_device->ReleasedPixelShader( this );

		if (m_pixProgram)
		{
			m_pixProgram->m_ctx->DelProgram( m_pixProgram );
			m_pixProgram = NULL;
		}
		m_device = NULL;
	}
	
	GLMPRINTF(( "<-A- ~IDirect3DPixelShader9" ));	
}

ULONG STDMETHODCALLTYPE COpenGLPixelShader9::AddRef(void)
{
	this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE COpenGLPixelShader9::QueryInterface(REFIID riid,void  **ppv)
{
	
}

ULONG STDMETHODCALLTYPE COpenGLPixelShader9::Release(void)
{
	this->Release(0);
}

ULONG STDMETHODCALLTYPE COpenGLPixelShader9::AddRef(int which, char *comment)
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

ULONG STDMETHODCALLTYPE	COpenGLPixelShader9::Release(int which, char *comment)
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

HRESULT STDMETHODCALLTYPE COpenGLPixelShader9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLPixelShader9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLPixelShader9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLPixelShader9::GetType()
{
	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE COpenGLPixelShader9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE COpenGLPixelShader9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLPixelShader9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE COpenGLPixelShader9::GetDevice(IDirect3DDevice9** ppDevice)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE COpenGLPixelShader9::GetFunction(void* pData, UINT* pSizeOfData)
{
	return E_NOTIMPL;
}
