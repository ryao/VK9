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
 
#include "COpenGLVolumeTexture9.h"
#include "COpenGLDevice9.h"

COpenGLVolumeTexture9::COpenGLVolumeTexture9()
{
	m_refcount[0] = 1;
	m_refcount[1] = 0;
	m_mark = (IUNKNOWN_ALLOC_SPEW_MARK_ALL != 0);	// either all are marked, or only the ones that have SetMark(true) called on them
}

COpenGLVolumeTexture9::~COpenGLVolumeTexture9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF((">-A-  ~IDirect3DVolumeTexture9"));

	if (m_device)
	{
		m_device->ReleasedTexture( this );

		// let IDirect3DBaseTexture9::~IDirect3DBaseTexture9 free up m_tex
		// we handle m_surfZero
		
		if (m_surfZero)
		{
			ULONG refc = m_surfZero->Release( 0, "~IDirect3DVolumeTexture9 public release (surfZero)" ); (void)refc;
			Assert( !refc );
			m_surfZero = NULL;
		}
		// leave m_device alone!
	}

	GLMPRINTF(("<-A-  ~IDirect3DVolumeTexture9"));	
}

ULONG STDMETHODCALLTYPE COpenGLVolumeTexture9::AddRef(void)
{
	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE COpenGLVolumeTexture9::Release(void)
{
	return this->Release(0);
}

ULONG STDMETHODCALLTYPE COpenGLVolumeTexture9::AddRef(int which, char *comment)
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

ULONG STDMETHODCALLTYPE	COpenGLVolumeTexture9::Release(int which, char *comment)
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

HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLVolumeTexture9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

//D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLVolumeTexture9::GetType()
//{
//	return D3DRTYPE_SURFACE;
//}

void STDMETHODCALLTYPE COpenGLVolumeTexture9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE COpenGLVolumeTexture9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE COpenGLVolumeTexture9::GenerateMipSubLevels()
{
	return; //TODO: implement GenerateMipSubLevels
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE COpenGLVolumeTexture9::GetAutoGenFilterType()
{
	return D3DTEXF_NONE; //TODO: implement GetAutoGenFilterType
}

DWORD STDMETHODCALLTYPE COpenGLVolumeTexture9::GetLOD()
{
	return 0; //TODO: implement GetLOD
}


DWORD STDMETHODCALLTYPE COpenGLVolumeTexture9::GetLevelCount()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_tex->m_layout->m_mipCount;	
}


HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLVolumeTexture9::SetLOD(DWORD LODNew)
{
	return 0; //TODO: implement SetLOD
}

D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLVolumeTexture9::GetType()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_restype;	//D3DRTYPE_TEXTURE;	
}

HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::AddDirtyBox(const D3DBOX* pDirtyBox)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	if (Level > static_cast<unsigned int>(m_tex->m_layout->m_mipCount))
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}

	D3DVOLUME_DESC result = m_volDescZero;
	// then mutate it for the level of interest
	
	GLMTexLayoutSlice *slice = &m_tex->m_layout->m_slices[ m_tex->CalcSliceIndex( 0, Level ) ];

	result.Width = slice->m_xSize;
	result.Height = slice->m_ySize;
	result.Depth = slice->m_zSize;
	
	*pDesc = result;

	return S_OK;	
}


HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	GLMTexLockParams lockreq;
	memset( &lockreq, 0, sizeof(lockreq) );
	
	lockreq.m_tex		= this->m_tex;
	lockreq.m_face		= 0;
	lockreq.m_mip		= Level;

	lockreq.m_region.xmin = pBox->Left;
	lockreq.m_region.ymin = pBox->Top;
	lockreq.m_region.zmin = pBox->Front;
	lockreq.m_region.xmax = pBox->Right;
	lockreq.m_region.ymax = pBox->Bottom;
	lockreq.m_region.zmax = pBox->Back;
	
	char	*lockAddress;
	int		yStride;
	int		zStride;
	
	lockreq.m_tex->Lock( &lockreq, &lockAddress, &yStride, &zStride );

	pLockedVolume->RowPitch = yStride;
	pLockedVolume->SlicePitch = yStride;
	pLockedVolume->pBits = lockAddress;	
	
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE COpenGLVolumeTexture9::UnlockBox(UINT Level)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	GLMTexLockParams lockreq;
	memset( &lockreq, 0, sizeof(lockreq) );
	
	lockreq.m_tex		= this->m_tex;
	lockreq.m_face		= 0;
	lockreq.m_mip		= Level;

	this->m_tex->Unlock( &lockreq );
	
	return S_OK;	
}
