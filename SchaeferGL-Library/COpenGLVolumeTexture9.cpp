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
