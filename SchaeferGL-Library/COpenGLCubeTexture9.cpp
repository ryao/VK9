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
 
#include "COpenGLCubeTexture9.h"
#include "COpenGLDevice9.h"
#include "COpenGLSurface9.h"

COpenGLCubeTexture9::COpenGLCubeTexture9()
{
	m_refcount[0] = 1;
	m_refcount[1] = 0;
	m_mark = (IUNKNOWN_ALLOC_SPEW_MARK_ALL != 0);	// either all are marked, or only the ones that have SetMark(true) called on them
}

COpenGLCubeTexture9::~COpenGLCubeTexture9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	GLMPRINTF(( ">-A- ~IDirect3DCubeTexture9" ));

	if (m_device)
	{
		GLMPRINTF(( "-A- ~IDirect3DCubeTexture9 taking normal delete path on %08x, device is %08x, surfzero[0] is %08x ", this, m_device, m_surfZero[0] ));
		m_device->ReleasedTexture( this );

		// let IDirect3DBaseTexture9::~IDirect3DBaseTexture9 free up m_tex
		// we handle the surfZero array for the faces
		
		for( int face = 0; face < 6; face ++)
		{
			if (m_surfZero[face])
			{
				Assert( m_surfZero[face]->m_device == m_device );
				ULONG refc = m_surfZero[face]->Release( 0, "~IDirect3DCubeTexture9 public release (surfZero)");
				if ( refc!=0 )
				{
					GLMPRINTF(( "-A- ~IDirect3DCubeTexture9 seeing non zero refcount on surfzero[%d] => %d ", face, refc ));
				}
				m_surfZero[face] = NULL;
			}
		}
		// leave m_device alone!
	}
	else
	{
		GLMPRINTF(( "-A- ~IDirect3DCubeTexture9 taking strange delete path on %08x, device is %08x, surfzero[0] is %08x ", this, m_device, m_surfZero[0] ));
	}

	GLMPRINTF(( "<-A- ~IDirect3DCubeTexture9" ));	
}

ULONG STDMETHODCALLTYPE COpenGLCubeTexture9::AddRef(void)
{
	this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	
}

ULONG STDMETHODCALLTYPE COpenGLCubeTexture9::Release(void)
{
	this->Release(0);
}

ULONG STDMETHODCALLTYPE COpenGLCubeTexture9::AddRef(int which, char *comment)
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

ULONG STDMETHODCALLTYPE	COpenGLCubeTexture9::Release(int which, char *comment)
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

HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLCubeTexture9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

//D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLCubeTexture9::GetType()
//{
//	return D3DRTYPE_SURFACE;
//}

void STDMETHODCALLTYPE COpenGLCubeTexture9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE COpenGLCubeTexture9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE COpenGLCubeTexture9::GenerateMipSubLevels()
{
	return; //TODO: implement GenerateMipSubLevels
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE COpenGLCubeTexture9::GetAutoGenFilterType()
{
	return D3DTEXF_NONE; //TODO: implement GetAutoGenFilterType
}

DWORD STDMETHODCALLTYPE COpenGLCubeTexture9::GetLOD()
{
	return 0; //TODO: implement GetLOD
}


DWORD STDMETHODCALLTYPE COpenGLCubeTexture9::GetLevelCount()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_tex->m_layout->m_mipCount;	
}


HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLCubeTexture9::SetLOD(DWORD LODNew)
{
	return 0; //TODO: implement SetLOD
}

D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLCubeTexture9::GetType()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_restype;	//D3DRTYPE_TEXTURE;	
}

HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	// we create and pass back a surface, and the client is on the hook to release it...

	m_device->m_ObjectStats.m_nTotalSurfaces++;

	COpenGLSurface9 *surf = new COpenGLSurface9;
	surf->m_restype = (D3DRESOURCETYPE)0;	// 0 is special and means this 'surface' does not own its m_tex
	
	GLMTexLayoutSlice *slice = &m_tex->m_layout->m_slices[ m_tex->CalcSliceIndex( FaceType, Level ) ];
	
	surf->m_device = this->m_device;
	
	surf->m_desc = m_descZero;
		surf->m_desc.Width = slice->m_xSize;
		surf->m_desc.Height = slice->m_ySize;
		
	surf->m_tex	= m_tex;
	surf->m_face = FaceType;
	surf->m_mip = Level;

	*ppCubeMapSurface = surf;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE COpenGLCubeTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	GL_BATCH_PERF_CALL_TIMER;
	Assert (Level < static_cast<uint>(m_tex->m_layout->m_mipCount));

	D3DSURFACE_DESC result = m_descZero;
	// then mutate it for the level of interest
	
	GLMTexLayoutSlice *slice = &m_tex->m_layout->m_slices[ m_tex->CalcSliceIndex( 0, Level ) ];

	result.Width = slice->m_xSize;
	result.Height = slice->m_ySize;

	*pDesc = result;

	return S_OK;	
}


HRESULT COpenGLCubeTexture9::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	return E_NOTIMPL;
}

HRESULT COpenGLCubeTexture9::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level)
{
	return E_NOTIMPL;
}
