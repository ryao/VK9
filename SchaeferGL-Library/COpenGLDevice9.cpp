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
 
#include "COpenGLDevice9.h"

void	d3drect_to_glmbox( D3DRECT *src, GLScissorBox_t *dst )
{
	// to convert from a d3d rect to a GL rect you have to fix up the vertical axis, since D3D Y=0 is the top, but GL Y=0 is the bottom.
	// you can't fix it without knowing the height.

	dst->width	= src->x2 - src->x1;
	dst->x		= src->x1;				// left edge

	dst->height	= src->y2 - src->y1;
	dst->y		= src->y1;				// bottom edge - take large Y from d3d and subtract from surf height.
}

COpenGLDevice9::COpenGLDevice9() :
	m_nValidMarker( D3D_DEVICE_VALID_MARKER )
{
	
}

COpenGLDevice9::~COpenGLDevice9()
{
	Assert( m_nValidMarker == D3D_DEVICE_VALID_MARKER );
#if GL_BATCH_PERF_ANALYSIS && GL_BATCH_PERF_ANALYSIS_WRITE_PNGS
	delete m_pBatch_vis_bitmap;
#endif
	
	delete m_pDummy_vtx_buffer;
	for ( int i = 0; i < 4; i++ )
		SetRenderTarget( i, NULL );
	SetDepthStencilSurface( NULL );
	if ( m_pDefaultColorSurface )
	{
		m_pDefaultColorSurface->Release( 0, "IDirect3DDevice9::~IDirect3DDevice9 release color surface" ); 
		m_pDefaultColorSurface = NULL;
	}
	if ( m_pDefaultDepthStencilSurface )
	{
		m_pDefaultDepthStencilSurface->Release( 0, "IDirect3DDevice9::~IDirect3DDevice9 release depth surface" ); 
		m_pDefaultDepthStencilSurface = NULL;
	}
	
	if ( m_pFBOs )
	{
	ResetFBOMap();
	}

	GLMPRINTF(( "-D- IDirect3DDevice9::~IDirect3DDevice9 signpost" ));	// want to know when this is called, if ever

	g_pD3D_Device = NULL;
	if ( m_ObjectStats.m_nTotalFBOs ) GLMDebugPrintf( "Leaking %i FBOs\n", m_ObjectStats.m_nTotalFBOs );
	if ( m_ObjectStats.m_nTotalVertexShaders ) ConMsg( "Leaking %i vertex shaders\n", m_ObjectStats.m_nTotalVertexShaders );
	if ( m_ObjectStats.m_nTotalPixelShaders ) ConMsg( "Leaking %i pixel shaders\n", m_ObjectStats.m_nTotalPixelShaders );
	if ( m_ObjectStats.m_nTotalVertexDecls ) ConMsg( "Leaking %i vertex decls\n", m_ObjectStats.m_nTotalVertexDecls );
	if ( m_ObjectStats.m_nTotalIndexBuffers ) ConMsg( "Leaking %i index buffers\n", m_ObjectStats.m_nTotalIndexBuffers );
	if ( m_ObjectStats.m_nTotalVertexBuffers ) ConMsg( "Leaking %i vertex buffers\n", m_ObjectStats.m_nTotalVertexBuffers );
	if ( m_ObjectStats.m_nTotalTextures ) ConMsg( "Leaking %i textures\n", m_ObjectStats.m_nTotalTextures );
	if ( m_ObjectStats.m_nTotalSurfaces ) ConMsg( "Leaking %i surfaces\n", m_ObjectStats.m_nTotalSurfaces );
	if ( m_ObjectStats.m_nTotalQueries ) ConMsg( "Leaking %i queries\n", m_ObjectStats.m_nTotalQueries );
	if ( m_ObjectStats.m_nTotalRenderTargets ) ConMsg( "Leaking %i render targets\n", m_ObjectStats.m_nTotalRenderTargets );
	GLMgr::aGLMgr()->DelContext( m_ctx );
	m_ctx = NULL;
	m_nValidMarker = 0xDEADBEEF;	
}

HRESULT	COpenGLDevice9::Create( IDirect3DDevice9Params *params )
{
	g_pD3D_Device = this;

	GLMDebugPrintf( "IDirect3DDevice9::Create: BackBufWidth: %u, BackBufHeight: %u, D3DFMT: %u, BackBufCount: %u, MultisampleType: %u, MultisampleQuality: %u\n",
		params->m_presentationParameters.BackBufferWidth,
		params->m_presentationParameters.BackBufferHeight,
		params->m_presentationParameters.BackBufferFormat,
		params->m_presentationParameters.BackBufferCount,
		params->m_presentationParameters.MultiSampleType,
		params->m_presentationParameters.MultiSampleQuality );
		
	UnpackD3DRSITable();
	
	m_ObjectStats.clear();
	m_PrevObjectStats.clear();
	
#if GL_BATCH_PERF_ANALYSIS && GL_BATCH_PERF_ANALYSIS_WRITE_PNGS
	m_pBatch_vis_bitmap = NULL;
#endif

	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF((">-X-IDirect3DDevice9::Create"));
	HRESULT result = S_OK;
				
	// create an IDirect3DDevice9
	// make a GLMContext and set up some drawables
	m_params = *params;

	m_ctx = NULL;
		
	V_memset( m_pRenderTargets, 0, sizeof( m_pRenderTargets ) );
	m_pDepthStencil = NULL;
		
	m_pDefaultColorSurface = NULL;
	m_pDefaultDepthStencilSurface = NULL;
	
	memset( m_streams, 0, sizeof(m_streams) );
	memset( m_vtx_buffers, 0, sizeof( m_vtx_buffers ) );
	memset( m_textures, 0, sizeof(m_textures) );
	//memset( m_samplers, 0, sizeof(m_samplers) );

	m_indices.m_idxBuffer = NULL;

	m_vertexShader = NULL;
	m_pixelShader = NULL;

	m_pVertDecl = NULL;
			
	//============================================================================
	// param block for GLM context create
	GLMDisplayParams	glmParams;	
	ConvertPresentationParamsToGLMDisplayParams( &params->m_presentationParameters, &glmParams );

	glmParams.m_mtgl						=	true;	// forget this idea -> (params->m_behaviorFlags & D3DCREATE_MULTITHREADED) != 0;
	// the call above fills in a bunch of things, but doesn't know about anything outside of the presentation params.
	// those tend to be the things that do not change after create, so we do those here in Create.

	glmParams.m_focusWindow					=	params->m_focusWindow;	

		#if 0	//FIXME-HACK
			// map the D3D "adapter" to a renderer/display pair
			// (that GPU will have to stay set as-is for any subsequent mode changes)
		
			int glmRendererIndex = -1;
			int glmDisplayIndex = -1;
		
			GLMRendererInfoFields		glmRendererInfo;
			GLMDisplayInfoFields		glmDisplayInfo;
		
			// the D3D "Adapter" number feeds the fake adapter index
			bool adaptResult = GLMgr::aGLMgr()->GetDisplayDB()->GetFakeAdapterInfo( params->m_adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo );
			Assert(!adaptResult);

			glmParams.m_rendererIndex				=	glmRendererIndex;
			glmParams.m_displayIndex				=	glmDisplayIndex;
				// glmParams.m_modeIndex  hmmmmm, client doesn't give us a mode number, just a resolution..
		#endif
	
	m_ctx = GLMgr::aGLMgr()->NewContext( this, &glmParams );
	if (!m_ctx)
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Create (error out)"));
		return (HRESULT) -1;
	}
	
	// make an FBO to draw into and activate it.
	m_ctx->m_drawingFBO = m_ctx->NewFBO();					
				
	// bind it to context.  will receive attachments shortly.
	m_ctx->BindFBOToCtx( m_ctx->m_drawingFBO, GL_FRAMEBUFFER_EXT );
	
	m_bFBODirty = false;

	m_pFBOs = new CGLMFBOMap();
	m_pFBOs->SetLessFunc( RenderTargetState_t::LessFunc );

	// we create two IDirect3DSurface9's.  These will be known as the internal render target 0 and the depthstencil.
	
	GLMPRINTF(("-X- IDirect3DDevice9::Create making color render target..."));
	// color surface
	result = this->CreateRenderTarget( 
		m_params.m_presentationParameters.BackBufferWidth,			// width
		m_params.m_presentationParameters.BackBufferHeight,			// height
		m_params.m_presentationParameters.BackBufferFormat,			// format
		m_params.m_presentationParameters.MultiSampleType,			// MSAA depth
		m_params.m_presentationParameters.MultiSampleQuality,		// MSAA quality
		true,														// lockable
		&m_pDefaultColorSurface,										// ppSurface
		NULL,														// shared handle
		"InternalRT0"
		);

	if (result != S_OK)
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Create (error out)"));
		return result;
	}
		// do not do an AddRef..

	GLMPRINTF(("-X- IDirect3DDevice9::Create making color render target complete -> %08x", m_pDefaultColorSurface ));

	GLMPRINTF(("-X- IDirect3DDevice9::Create setting color render target..."));
	result = this->SetRenderTarget(0, m_pDefaultColorSurface);
	if (result != S_OK)
	{
		GLMPRINTF(("< IDirect3DDevice9::Create (error out)"));
		return result;
	}
	GLMPRINTF(("-X- IDirect3DDevice9::Create setting color render target complete."));

	Assert (m_params.m_presentationParameters.EnableAutoDepthStencil);

	GLMPRINTF(("-X- IDirect3DDevice9::Create making depth-stencil..."));
    result = CreateDepthStencilSurface(
		m_params.m_presentationParameters.BackBufferWidth,			// width
		m_params.m_presentationParameters.BackBufferHeight,			// height
		m_params.m_presentationParameters.AutoDepthStencilFormat,	// format
		m_params.m_presentationParameters.MultiSampleType,			// MSAA depth
		m_params.m_presentationParameters.MultiSampleQuality,		// MSAA quality
		TRUE,														// enable z-buffer discard ????
		&m_pDefaultDepthStencilSurface,								// ppSurface
		NULL														// shared handle
		);
	if (result != S_OK)
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Create (error out)"));
		return result;
	}
		// do not do an AddRef here..

	GLMPRINTF(("-X- IDirect3DDevice9::Create making depth-stencil complete -> %08x", m_pDefaultDepthStencilSurface));
	GLMPRINTF(("-X- Direct3DDevice9::Create setting depth-stencil render target..."));
	result = this->SetDepthStencilSurface(m_pDefaultDepthStencilSurface);
	if (result != S_OK)
	{
		DXABSTRACT_BREAK_ON_ERROR();
		GLMPRINTF(("<-X- IDirect3DDevice9::Create (error out)"));
		return result;
	}
	GLMPRINTF(("-X- IDirect3DDevice9::Create setting depth-stencil render target complete."));

	UpdateBoundFBO();

	bool ready = m_ctx->m_drawingFBO->IsReady();
	if (!ready)
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Create (error out)"));
		return (HRESULT)-1;
	}

	// this next part really needs to be inside GLMContext.. or replaced with D3D style viewport setup calls.
	m_ctx->GenDebugFontTex();
	
	// blast the gl state mirror...
	memset( &this->gl, 0, sizeof( this->gl ) );

	InitStates();

	GLScissorEnable_t		defScissorEnable		= { true };
	GLScissorBox_t			defScissorBox			= { 0,0, m_params.m_presentationParameters.BackBufferWidth,m_params.m_presentationParameters.BackBufferHeight };
	GLViewportBox_t			defViewportBox			= { 0,0, m_params.m_presentationParameters.BackBufferWidth,m_params.m_presentationParameters.BackBufferHeight, m_params.m_presentationParameters.BackBufferWidth | ( m_params.m_presentationParameters.BackBufferHeight << 16 ) };
	GLViewportDepthRange_t	defViewportDepthRange	= { 0.1, 1000.0 };
	GLCullFaceEnable_t		defCullFaceEnable		= { true };
	GLCullFrontFace_t		defCullFrontFace		= { GL_CCW };
	
	gl.m_ScissorEnable		=	defScissorEnable;
	gl.m_ScissorBox			=	defScissorBox;
	gl.m_ViewportBox		=	defViewportBox;
	gl.m_ViewportDepthRange	=	defViewportDepthRange;
	gl.m_CullFaceEnable		=	defCullFaceEnable;
	gl.m_CullFrontFace		=	defCullFrontFace;
		
	FullFlushStates();
	
	GLMPRINTF(("<-X- IDirect3DDevice9::Create complete"));

	// so GetClientRect can return sane answers
	//uint width, height;		
	RenderedSize( m_params.m_presentationParameters.BackBufferWidth, m_params.m_presentationParameters.BackBufferHeight, true );	// true = set
			
#if GL_TELEMETRY_GPU_ZONES
	g_TelemetryGPUStats.Clear();
#endif

	GL_BATCH_PERF( 
		g_nTotalD3DCalls = 0, g_nTotalD3DCycles = 0, m_nBatchVisY = 0, m_nBatchVisFrameIndex = 0, m_nBatchVisFileIdx = 0, m_nNumProgramChanges = 0, m_flTotalD3DTime = 0, m_nTotalD3DCalls = 0, 
		m_flTotalD3DTime = 0, m_nTotalGLCalls = 0, m_flTotalGLTime = 0, m_nOverallDraws = 0, m_nOverallPrims = 0, m_nOverallD3DCalls = 0, m_flOverallD3DTime = 0, m_nOverallGLCalls = 0, m_flOverallGLTime = 0, m_nOverallProgramChanges = 0, 
		m_flOverallPresentTime = 0, m_flOverallPresentTimeSquared = 0, m_nOverallPresents = 0, m_flOverallSwapWindowTime = 0, m_flOverallSwapWindowTimeSquared = 0, m_nTotalPrims = 0; );

	g_nTotalDrawsOrClears = 0;

	gGL->m_nTotalGLCycles = 0;
	gGL->m_nTotalGLCalls = 0;

	m_pDummy_vtx_buffer = new CGLMBuffer( m_ctx, kGLMVertexBuffer, 4096, 0 );
	m_vtx_buffers[0] = m_pDummy_vtx_buffer;
	m_vtx_buffers[1] = m_pDummy_vtx_buffer;
	m_vtx_buffers[2] = m_pDummy_vtx_buffer;
	m_vtx_buffers[3] = m_pDummy_vtx_buffer;
	
	return result;	
}

void COpenGLDevice9::UpdateBoundFBO()
{
	RenderTargetState_t renderTargetState;
	for ( unsigned int i = 0; i < 4; i++ )
	{
		renderTargetState.m_pRenderTargets[i] = m_pRenderTargets[i] ? m_pRenderTargets[i]->m_tex : NULL;
	}
	renderTargetState.m_pDepthStencil = m_pDepthStencil ? m_pDepthStencil->m_tex : NULL;
	CUtlMap < RenderTargetState_t, CGLMFBO * >::IndexType_t index = m_pFBOs->Find( renderTargetState );

	if ( m_pFBOs->IsValidIndex( index ) )
	{
		Assert( (*m_pFBOs)[index] );
		
		m_ctx->m_drawingFBO = (*m_pFBOs)[index];
	} 
	else 
	{
		CGLMFBO *newFBO = m_ctx->NewFBO();

		m_pFBOs->Insert( renderTargetState, newFBO );
		
		uint nNumBound = 0;

		for ( unsigned int i = 0; i < 4; i++ )
		{
			if ( !m_pRenderTargets[i] )
				continue;

			GLMFBOTexAttachParams rtParams;
			memset( &rtParams, 0, sizeof(rtParams) );

			rtParams.m_tex		= m_pRenderTargets[i]->m_tex;
			rtParams.m_face		= m_pRenderTargets[i]->m_face;
			rtParams.m_mip		= m_pRenderTargets[i]->m_mip;
			rtParams.m_zslice	= 0;

			newFBO->TexAttach( &rtParams, (EGLMFBOAttachment)(kAttColor0 + i) );
			nNumBound++;
		}

		if ( m_pDepthStencil ) 
		{
			GLMFBOTexAttachParams	depthParams;
			memset( &depthParams, 0, sizeof(depthParams) );

			depthParams.m_tex = m_pDepthStencil->m_tex;

			EGLMFBOAttachment destAttach = (depthParams.m_tex->m_layout->m_format->m_glDataFormat != 34041) ? kAttDepth : kAttDepthStencil;

			newFBO->TexAttach( &depthParams, destAttach );
			nNumBound++;
		}
		
		(void)nNumBound;

		Assert( nNumBound );

#if GLMDEBUG
		Assert( newFBO->IsReady() );
#endif

		m_ctx->m_drawingFBO = newFBO;
	}

	m_ctx->BindFBOToCtx( m_ctx->m_drawingFBO, GL_FRAMEBUFFER_EXT );

	m_bFBODirty = false;
}

void COpenGLDevice9::ResetFBOMap()
{
	if ( !m_pFBOs )
		return;

	FOR_EACH_MAP_FAST( (*m_pFBOs), i )
	{
		const RenderTargetState_t &rtState = m_pFBOs->Key( i ); (void)rtState;
		CGLMFBO *pFBO = (*m_pFBOs)[i];
				
		m_ctx->DelFBO( pFBO );
	}

	m_pFBOs->Purge();

	m_bFBODirty = true;
}

void COpenGLDevice9::ScrubFBOMap( CGLMTex *pTex )
{
	Assert( pTex );

	if ( !m_pFBOs )
		return;
				
	CUtlVectorFixed< RenderTargetState_t, 128 > fbosToRemove;
	
	FOR_EACH_MAP_FAST( (*m_pFBOs), i )
	{
		const RenderTargetState_t &rtState = m_pFBOs->Key( i );
		CGLMFBO *pFBO = (*m_pFBOs)[i]; (void)pFBO;

		if ( rtState.RefersTo( pTex ) )
		{
			fbosToRemove.AddToTail( rtState );
		}
	}

	for ( int i = 0; i < fbosToRemove.Count(); ++i )
	{
		const RenderTargetState_t &rtState = fbosToRemove[i];

		CUtlMap < RenderTargetState_t, CGLMFBO * >::IndexType_t index = m_pFBOs->Find( rtState );

		if ( !m_pFBOs->IsValidIndex( index ) )
		{
			Assert( 0 );
			continue;
		}
		
		CGLMFBO *pFBO = (*m_pFBOs)[index];
						
		m_ctx->DelFBO( pFBO );

		m_pFBOs->RemoveAt( index );

		m_bFBODirty = true;
	}

	//GLMDebugPrintf( "IDirect3DDevice9::ScrubFBOMap: Removed %u entries\n", fbosToRemove.Count() );
}

void COpenGLDevice9::ReleasedVertexDeclaration( IDirect3DVertexDeclaration9 *pDecl )
{
	m_ctx->ClearCurAttribs();

	Assert( m_ObjectStats.m_nTotalVertexDecls >= 1 );
	m_ObjectStats.m_nTotalVertexDecls--;
}

void COpenGLDevice9::ReleasedTexture( IDirect3DBaseTexture9 *baseTex )
{
	GL_BATCH_PERF_CALL_TIMER;
	TOGL_NULL_DEVICE_CHECK_RET_VOID;

	// see if this texture is referenced in any of the texture units and scrub it if so.
	for( int i=0; i< GLM_SAMPLER_COUNT; i++)
	{
		if (m_textures[i] == baseTex)
		{
			m_textures[i] = NULL;
			m_ctx->SetSamplerTex( i, NULL );	// texture sets go straight through to GLM, no dirty bit
		}
	}
}

void COpenGLDevice9::ReleasedCGLMTex( CGLMTex *pTex)
{
	GL_BATCH_PERF_CALL_TIMER;
	TOGL_NULL_DEVICE_CHECK_RET_VOID;

	ScrubFBOMap( pTex );
	if ( pTex->m_layout )
	{
		if ( pTex->m_layout->m_key.m_texFlags & kGLMTexRenderable )
		{
			Assert( m_ObjectStats.m_nTotalRenderTargets >= 1 );
			m_ObjectStats.m_nTotalRenderTargets--;
		}
	}
}
void COpenGLDevice9::ReleasedSurface( IDirect3DSurface9 *pSurface )
{
	for( int i = 0; i < 4; i++ )
	{
		if ( m_pRenderTargets[i] == pSurface )
		{
			// this was a surprise release... scrub it
			m_pRenderTargets[i] = NULL;
			m_bFBODirty = true;
			GLMPRINTF(( "-A- Scrubbed pSurface %08x from m_pRenderTargets[%d]", pSurface, i ));
		}
	}

	if ( m_pDepthStencil == pSurface )
	{
		m_pDepthStencil = NULL;
		m_bFBODirty = true;
		GLMPRINTF(( "-A- Scrubbed pSurface %08x from m_pDepthStencil", pSurface ));
	}
	
	if ( m_pDefaultColorSurface == pSurface )
	{
		m_pDefaultColorSurface = NULL;
		GLMPRINTF(( "-A- Scrubbed pSurface %08x from m_pDefaultColorSurface", pSurface ));
	}
	
	if ( m_pDefaultDepthStencilSurface == pSurface )
	{
		m_pDefaultDepthStencilSurface = NULL;
		GLMPRINTF(( "-A- Scrubbed pSurface %08x from m_pDefaultDepthStencilSurface", pSurface ));
	}

	Assert( m_ObjectStats.m_nTotalSurfaces >= 1 );
	m_ObjectStats.m_nTotalSurfaces--;
}

void COpenGLDevice9::ReleasedPixelShader( IDirect3DPixelShader9 *pixelShader )
{
	if ( m_pixelShader == pixelShader )
	{
		m_pixelShader = NULL;
		GLMPRINTF(( "-A- Scrubbed pixel shader %08x from m_pixelShader", pixelShader ));
	}
	m_ctx->ReleasedShader();
	
	Assert( m_ObjectStats.m_nTotalPixelShaders >= 1 );
	m_ObjectStats.m_nTotalPixelShaders--;
}

void COpenGLDevice9::ReleasedVertexShader( IDirect3DVertexShader9 *vertexShader )
{
	if ( m_vertexShader == vertexShader )
	{
		m_vertexShader = NULL;
		GLMPRINTF(( "-A- Scrubbed vertex shader %08x from m_vertexShader", vertexShader ));
	}
	m_ctx->ClearCurAttribs();
	m_ctx->ReleasedShader();
	
	Assert( m_ObjectStats.m_nTotalVertexShaders >= 1 );
	m_ObjectStats.m_nTotalVertexShaders--;
}

void COpenGLDevice9::ReleasedVertexBuffer( IDirect3DVertexBuffer9 *vertexBuffer )
{
	for (int i=0; i< D3D_MAX_STREAMS; i++)
	{
		if ( m_streams[i].m_vtxBuffer == vertexBuffer )
		{
			m_streams[i].m_vtxBuffer = NULL;
			m_vtx_buffers[i] = m_pDummy_vtx_buffer;

			GLMPRINTF(( "-A- Scrubbed vertex buffer %08x from m_streams[%d]", vertexBuffer, i ));
		}
	}
	m_ctx->ClearCurAttribs();

	Assert( m_ObjectStats.m_nTotalVertexBuffers >= 1 );
	m_ObjectStats.m_nTotalVertexBuffers--;
}

void COpenGLDevice9::ReleasedIndexBuffer( IDirect3DIndexBuffer9 *indexBuffer )
{
	if ( m_indices.m_idxBuffer == indexBuffer )
	{
		m_indices.m_idxBuffer = NULL;
		GLMPRINTF(( "-A- Scrubbed index buffer %08x from m_indices", indexBuffer ));
	}
	
	Assert( m_ObjectStats.m_nTotalIndexBuffers >= 1 );
	m_ObjectStats.m_nTotalIndexBuffers--;
}

void COpenGLDevice9::ReleasedQuery( IDirect3DQuery9 *query )
{
	Assert( m_ObjectStats.m_nTotalQueries >= 1 );
	m_ObjectStats.m_nTotalQueries--;
}

void COpenGLDevice9::FlushClipPlaneEquation()
{
	for( int x=0; x<kGLMUserClipPlanes; x++)
	{
		GLClipPlaneEquation_t temp1;	// Antonio's way
		GLClipPlaneEquation_t temp2;	// our way

		// if we don't have native clip vertex support. then munge the plane coeffs
		// this should engage on ALL ATI PARTS < 10.6.4
		// and should continue to engage on R5xx forever.
			
		if ( !m_ctx->Caps().m_hasNativeClipVertexMode )
		{
			// hacked coeffs = { src->x, -src->y, 0.5f * src->z, src->w + (0.5f * src->z) };
			// Antonio's trick - so we can use gl_Position as the clippee, not gl_ClipVertex.

			GLClipPlaneEquation_t *equ = &gl.m_ClipPlaneEquation[x];

			///////////////// temp1
			temp1.x	=	equ->x;
			temp1.y	=	equ->y * -1.0;
			temp1.z	=	equ->z * 0.5;
			temp1.w	=	equ->w + (equ->z * 0.5);

				
			//////////////// temp2
			VMatrix mat1(	1,	0,	0,	0,
							0,	-1,	0,	0,
							0,	0,	2,	-1,
							0,	0,	0,	1
							);
			//mat1 = mat1.Transpose();
								
			VMatrix mat2;
			bool success = mat1.InverseGeneral( mat2 );
				
			if (success)
			{
				VMatrix mat3;
				mat3 = mat2.Transpose();

				VPlane origPlane( Vector( equ->x, equ->y, equ->z ), equ->w );
				VPlane newPlane;
					
				newPlane = mat3 * origPlane /* * mat3 */;
					
				VPlane finalPlane = newPlane;
					
				temp2.x = newPlane.m_Normal.x;
				temp2.y = newPlane.m_Normal.y;
				temp2.z = newPlane.m_Normal.z;
				temp2.w = newPlane.m_Dist;
			}
			else
			{
				temp2.x = 0;
				temp2.y = 0;
				temp2.z = 0;
				temp2.w = 0;
			}
		}
		else
		{
			temp1 = temp2 = gl.m_ClipPlaneEquation[x];
		}

		if (1)	//GLMKnob("caps-key",NULL)==0.0)
		{
			m_ctx->WriteClipPlaneEquation( &temp1, x );		// no caps lock = Antonio or classic
				
			/*
			if (x<1)
			{
				GLMPRINTF(( " plane %d  √vers1[ %5.2f %5.2f %5.2f %5.2f ]    vers2[ %5.2f %5.2f %5.2f %5.2f ]",
					x,
					temp1.x,temp1.y,temp1.z,temp1.w,
					temp2.x,temp2.y,temp2.z,temp2.w
				));
			}
			*/
		}
		else
		{
			m_ctx->WriteClipPlaneEquation( &temp2, x );		// caps = our way or classic

			/*
			if (x<1)
			{
				GLMPRINTF(( " plane %d   vers1[ %5.2f %5.2f %5.2f %5.2f ]    √vers2[ %5.2f %5.2f %5.2f %5.2f ]",
					x,
					temp1.x,temp1.y,temp1.z,temp1.w,
					temp2.x,temp2.y,temp2.z,temp2.w
				));
			}
			*/
		}
	}
}

void COpenGLDevice9::InitStates()
{
	m_ctx->m_AlphaTestEnable.Read( &gl.m_AlphaTestEnable, 0 );
	m_ctx->m_AlphaTestFunc.Read( &gl.m_AlphaTestFunc, 0 );
	m_ctx->m_CullFaceEnable.Read( &gl.m_CullFaceEnable, 0 );
	m_ctx->m_DepthBias.Read( &gl.m_DepthBias, 0 );
	m_ctx->m_ScissorEnable.Read( &gl.m_ScissorEnable, 0 );
	m_ctx->m_ScissorBox.Read( &gl.m_ScissorBox, 0 );
	m_ctx->m_ViewportBox.Read( &gl.m_ViewportBox, 0 );
	m_ctx->m_ViewportDepthRange.Read( &gl.m_ViewportDepthRange, 0 );

	for( int x=0; x<kGLMUserClipPlanes; x++)
		m_ctx->m_ClipPlaneEnable.ReadIndex( &gl.m_ClipPlaneEnable[x], x, 0 );

	m_ctx->m_PolygonMode.Read( &gl.m_PolygonMode, 0 );
	m_ctx->m_CullFrontFace.Read( &gl.m_CullFrontFace, 0 );
	m_ctx->m_AlphaToCoverageEnable.Read( &gl.m_AlphaToCoverageEnable, 0 );
	m_ctx->m_BlendEquation.Read( &gl.m_BlendEquation, 0 );
	m_ctx->m_BlendColor.Read( &gl.m_BlendColor, 0 );
	
	for( int x=0; x<kGLMUserClipPlanes; x++)
		m_ctx->m_ClipPlaneEquation.ReadIndex( &gl.m_ClipPlaneEquation[x], x, 0 );

	m_ctx->m_ColorMaskSingle.Read( &gl.m_ColorMaskSingle, 0 );
	
	m_ctx->m_BlendEnable.Read( &gl.m_BlendEnable, 0 );
	m_ctx->m_BlendFactor.Read( &gl.m_BlendFactor, 0 );
	m_ctx->m_BlendEnableSRGB.Read( &gl.m_BlendEnableSRGB, 0 );
	m_ctx->m_DepthTestEnable.Read( &gl.m_DepthTestEnable, 0 );
	m_ctx->m_DepthFunc.Read( &gl.m_DepthFunc, 0 );
	m_ctx->m_DepthMask.Read( &gl.m_DepthMask, 0 );
	m_ctx->m_StencilTestEnable.Read( &gl.m_StencilTestEnable, 0 );
	m_ctx->m_StencilFunc.Read( &gl.m_StencilFunc, 0 );

	m_ctx->m_StencilOp.ReadIndex( &gl.m_StencilOp, 0, 0 );
	m_ctx->m_StencilOp.ReadIndex( &gl.m_StencilOp, 1, 0 );

	m_ctx->m_StencilWriteMask.Read( &gl.m_StencilWriteMask, 0 );
	m_ctx->m_ClearColor.Read( &gl.m_ClearColor, 0 );
	m_ctx->m_ClearDepth.Read( &gl.m_ClearDepth, 0 );
	m_ctx->m_ClearStencil.Read( &gl.m_ClearStencil, 0 );
}

void COpenGLDevice9::FullFlushStates()
{
	m_ctx->WriteAlphaTestEnable( &gl.m_AlphaTestEnable );
	m_ctx->WriteAlphaTestFunc( &gl.m_AlphaTestFunc );
	m_ctx->WriteCullFaceEnable( &gl.m_CullFaceEnable );
	m_ctx->WriteDepthBias( &gl.m_DepthBias );
	m_ctx->WriteScissorEnable( &gl.m_ScissorEnable );
	m_ctx->WriteScissorBox( &gl.m_ScissorBox );
	m_ctx->WriteViewportBox( &gl.m_ViewportBox );
	m_ctx->WriteViewportDepthRange( &gl.m_ViewportDepthRange );

	for( int x=0; x<kGLMUserClipPlanes; x++)
		m_ctx->WriteClipPlaneEnable( &gl.m_ClipPlaneEnable[x], x );
	
	m_ctx->WritePolygonMode( &gl.m_PolygonMode );
	m_ctx->WriteCullFrontFace( &gl.m_CullFrontFace );
	m_ctx->WriteAlphaToCoverageEnable( &gl.m_AlphaToCoverageEnable );
	m_ctx->WriteBlendEquation( &gl.m_BlendEquation );
	m_ctx->WriteBlendColor( &gl.m_BlendColor );
	FlushClipPlaneEquation();
	m_ctx->WriteColorMaskSingle( &gl.m_ColorMaskSingle );

	m_ctx->WriteBlendEnable( &gl.m_BlendEnable );
	m_ctx->WriteBlendFactor( &gl.m_BlendFactor );
	m_ctx->WriteBlendEnableSRGB( &gl.m_BlendEnableSRGB );
	m_ctx->WriteDepthTestEnable( &gl.m_DepthTestEnable );
	m_ctx->WriteDepthFunc( &gl.m_DepthFunc );
	m_ctx->WriteDepthMask( &gl.m_DepthMask );
	m_ctx->WriteStencilTestEnable( &gl.m_StencilTestEnable );
	m_ctx->WriteStencilFunc( &gl.m_StencilFunc );

	m_ctx->WriteStencilOp( &gl.m_StencilOp,0 );
	m_ctx->WriteStencilOp( &gl.m_StencilOp,1 );		// ********* need to recheck this
	
	m_ctx->WriteStencilWriteMask( &gl.m_StencilWriteMask );
	m_ctx->WriteClearColor( &gl.m_ClearColor );
	m_ctx->WriteClearDepth( &gl.m_ClearDepth );
	m_ctx->WriteClearStencil( &gl.m_ClearStencil );
}

void COpenGLDevice9::SaveGLState()
{
}

void COpenGLDevice9::RestoreGLState()
{
	m_ctx->ForceFlushStates();

	m_bFBODirty = true;
}

void COpenGLDevice9::AcquireThreadOwnership( )
{
	GL_BATCH_PERF_CALL_TIMER;
	m_ctx->MakeCurrent( true );
}


void COpenGLDevice9::ReleaseThreadOwnership( )
{
	GL_BATCH_PERF_CALL_TIMER;
	m_ctx->ReleaseCurrent( true );
}

void COpenGLDevice9::SetMaxUsedVertexShaderConstantsHintNonInline( unsigned int nMaxReg )
{
	GL_BATCH_PERF_CALL_TIMER;
	m_ctx->SetMaxUsedVertexShaderConstantsHint( nMaxReg );
}

void COpenGLDevice9::SetSamplerStatesNonInline(
	DWORD Sampler, DWORD AddressU, DWORD AddressV, DWORD AddressW,
	DWORD MinFilter, DWORD MagFilter, DWORD MipFilter )
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
		
	Assert( Sampler < GLM_SAMPLER_COUNT);

	m_ctx->SetSamplerDirty( Sampler );

	m_ctx->SetSamplerStates( Sampler, AddressU, AddressV, AddressW, MinFilter, MagFilter, MipFilter );
}

HRESULT COpenGLDevice9::BeginScene()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	m_ctx->BeginFrame();

	return S_OK;		
}
	
HRESULT COpenGLDevice9::BeginStateBlock(){}

HRESULT COpenGLDevice9::Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	GL_BATCH_PERF_CALL_TIMER;

	if ( m_bFBODirty )
	{
		UpdateBoundFBO();
	}
		
	g_nTotalDrawsOrClears++;

	m_ctx->FlushDrawStatesNoShaders();

	
	//debugging Color = (rand() | 0xFF0000FF) & 0xFF3F3FFF;
	if (!Count)
	{
		// run clear with no added rectangle
		m_ctx->Clear(	(Flags&D3DCLEAR_TARGET)!=0, Color,
						(Flags&D3DCLEAR_ZBUFFER)!=0, Z,
						(Flags&D3DCLEAR_STENCIL)!=0, Stencil,
						NULL
					);
	}
	else
	{
		GLScissorBox_t	tempbox;
		
		// do the rects one by one and convert each one to GL form
		for( unsigned int i=0; i<Count; i++)
		{
			D3DRECT d3dtempbox = pRects[i];
			d3drect_to_glmbox( &d3dtempbox, &tempbox );

			m_ctx->Clear(	(Flags&D3DCLEAR_TARGET)!=0, Color,
							(Flags&D3DCLEAR_ZBUFFER)!=0, Z,
							(Flags&D3DCLEAR_STENCIL)!=0, Stencil,
							&tempbox
						);
		}
	}

	return S_OK;	
}

HRESULT COpenGLDevice9::ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color){}
HRESULT COpenGLDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain){}

HRESULT COpenGLDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	Assert( m_ctx->m_nCurOwnerThreadId == ThreadGetCurrentId() );
	GLMPRINTF((">-A-  IDirect3DDevice9::CreateCubeTexture"));

	Assert( m_ctx->m_nCurOwnerThreadId == ThreadGetCurrentId() );

	m_ObjectStats.m_nTotalTextures++;

	IDirect3DCubeTexture9	*dxtex = new IDirect3DCubeTexture9;
	dxtex->m_restype = D3DRTYPE_CUBETEXTURE;
	
	dxtex->m_device			= this;

	dxtex->m_descZero.Format	= Format;
	dxtex->m_descZero.Type		= D3DRTYPE_CUBETEXTURE;
	dxtex->m_descZero.Usage		= Usage;
	dxtex->m_descZero.Pool		= Pool;

	dxtex->m_descZero.MultiSampleType		= D3DMULTISAMPLE_NONE;
	dxtex->m_descZero.MultiSampleQuality	= 0;
	dxtex->m_descZero.Width		= EdgeLength;
	dxtex->m_descZero.Height	= EdgeLength;
	
	GLMTexLayoutKey key;
	memset( &key, 0, sizeof(key) );
	
	key.m_texGLTarget	= GL_TEXTURE_CUBE_MAP;
	key.m_texFormat		= Format;

	if (Levels>1)
	{
		key.m_texFlags |= kGLMTexMipped;
	}

	// http://msdn.microsoft.com/en-us/library/bb172625(VS.85).aspx	
	// complain if any usage bits come down that I don't know.
	uint knownUsageBits = (D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC | D3DUSAGE_TEXTURE_SRGB);
	if ( (Usage & knownUsageBits) != Usage )
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}
	
	if (Usage & D3DUSAGE_AUTOGENMIPMAP)
	{
		key.m_texFlags |= kGLMTexMipped | kGLMTexMippedAuto;
	}
	
	if (Usage & D3DUSAGE_RENDERTARGET)
	{
		key.m_texFlags |= kGLMTexRenderable;
		
		m_ObjectStats.m_nTotalRenderTargets++;
	}
		
	if (Usage & D3DUSAGE_DYNAMIC)
	{
		//GLMPRINTF(("-X- DYNAMIC tex usage ignored.."));	//FIXME
	}
	
	if (Usage & D3DUSAGE_TEXTURE_SRGB)
	{
		key.m_texFlags |= kGLMTexSRGB;
	}
	
	key.m_xSize = EdgeLength;
	key.m_ySize = EdgeLength;
	key.m_zSize = 1;
	
	CGLMTex *tex = m_ctx->NewTex( &key, pDebugLabel );
	if (!tex)
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}
	dxtex->m_tex = tex;
	
	dxtex->m_tex->m_srgbFlipCount = 0;

	for( int face = 0; face < 6; face ++)
	{
		m_ObjectStats.m_nTotalSurfaces++;

		dxtex->m_surfZero[face] = new IDirect3DSurface9;
		dxtex->m_surfZero[face]->m_restype = (D3DRESOURCETYPE)0;	// 0 is special and means this 'surface' does not own its m_tex
		// do not do an AddRef here.	
		
		dxtex->m_surfZero[face]->m_device = this;
		
		dxtex->m_surfZero[face]->m_desc	=	dxtex->m_descZero;
		dxtex->m_surfZero[face]->m_tex	=	tex;
		dxtex->m_surfZero[face]->m_face	=	face;
		dxtex->m_surfZero[face]->m_mip	=	0;
	}
	
	GLMPRINTF(("-A- IDirect3DDevice9::CreateCubeTexture created '%s' @ %08x (GLM %08x)",tex->m_layout->m_layoutSummary, dxtex, tex ));
	
	*ppCubeTexture = dxtex;
	
	GLMPRINTF(("<-A- IDirect3DDevice9::CreateCubeTexture"));

	return S_OK;	
}

HRESULT COpenGLDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	Assert( ( Format == D3DFMT_D16 ) || ( Format == D3DFMT_D24X8 ) || ( Format == D3DFMT_D24S8 ) );
	HRESULT result = S_OK;
	
	m_ObjectStats.m_nTotalSurfaces++;
	m_ObjectStats.m_nTotalRenderTargets++;

	IDirect3DSurface9 *surf = new IDirect3DSurface9;
	surf->m_restype = D3DRTYPE_SURFACE;

	surf->m_device = this;				// always set device on creations!
	
	GLMTexLayoutKey depthkey;
	memset( &depthkey, 0, sizeof(depthkey) );

	depthkey.m_texGLTarget	=	GL_TEXTURE_2D;
	depthkey.m_xSize		=	Width;
	depthkey.m_ySize		=	Height;
	depthkey.m_zSize		=	1;

	depthkey.m_texFormat	=	Format;
	depthkey.m_texFlags		=	kGLMTexRenderable | kGLMTexIsDepth;
		
	if ( Format == D3DFMT_D24S8 )
	{
		depthkey.m_texFlags |= kGLMTexIsStencil;
	}

	if ( (MultiSample !=0) && (!m_ctx->Caps().m_nvG7x) )
	{
		depthkey.m_texFlags |= kGLMTexMultisampled;
		depthkey.m_texSamples = MultiSample;
		// FIXME no support for "MS quality" yet
	}

	surf->m_tex				= m_ctx->NewTex( &depthkey, "depth-stencil surface" );
	surf->m_face			= 0;
	surf->m_mip				= 0;

	//desc

	surf->m_desc.Format				=	Format;
    surf->m_desc.Type				=	D3DRTYPE_SURFACE;
    surf->m_desc.Usage				=	0;					//FIXME ???????????
    surf->m_desc.Pool				=	D3DPOOL_DEFAULT;	//FIXME ???????????
	surf->m_desc.MultiSampleType	=	MultiSample;
    surf->m_desc.MultiSampleQuality	=	MultisampleQuality;
    surf->m_desc.Width				=	Width;
    surf->m_desc.Height				=	Height;

	*ppSurface = (result==S_OK) ? surf : NULL;
	
	return result;	
}

HRESULT COpenGLDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	GLMPRINTF(( ">-A- IDirect3DDevice9::CreateIndexBuffer" ));

	// it is important to save all the create info, since GetDesc could get called later to query it
	
	m_ObjectStats.m_nTotalIndexBuffers++;

	IDirect3DIndexBuffer9 *newbuff = new IDirect3DIndexBuffer9;

	newbuff->m_device = this;

	newbuff->m_restype = D3DRTYPE_INDEXBUFFER;		//	hmmmmmmm why are we not derived from d3dresource..
		
	newbuff->m_ctx = m_ctx;
	
		// FIXME need to find home or use for the Usage, Format, Pool values passed in
	uint options = 0;
	
	if (Usage&D3DUSAGE_DYNAMIC)
	{
		options |= GLMBufferOptionDynamic;
	}

	newbuff->m_idxBuffer = m_ctx->NewBuffer( kGLMIndexBuffer, Length, options ) ;
	
	newbuff->m_idxDesc.Format	= Format;
	newbuff->m_idxDesc.Type		= D3DRTYPE_INDEXBUFFER;
	newbuff->m_idxDesc.Usage	= Usage;
	newbuff->m_idxDesc.Pool		= Pool;
	newbuff->m_idxDesc.Size		= Length;
		
	*ppIndexBuffer = newbuff;

	GLMPRINTF(( "<-A- IDirect3DDevice9::CreateIndexBuffer" ));

	return S_OK;	
}

HRESULT COpenGLDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	// set surf->m_restype to D3DRTYPE_SURFACE...

	// this is almost identical to CreateRenderTarget..
	
	HRESULT result = S_OK;
	
	m_ObjectStats.m_nTotalSurfaces++;
	m_ObjectStats.m_nTotalRenderTargets++;

	IDirect3DSurface9 *surf = new IDirect3DSurface9;
	surf->m_restype = D3DRTYPE_SURFACE;

	surf->m_device		= this;				// always set device on creations!

	GLMTexLayoutKey rtkey;
	memset( &rtkey, 0, sizeof(rtkey) );
	
	rtkey.m_texGLTarget	=	GL_TEXTURE_2D;
	rtkey.m_xSize		=	Width;
	rtkey.m_ySize		=	Height;
	rtkey.m_zSize		=	1;

	rtkey.m_texFormat	=	Format;
	rtkey.m_texFlags	=	kGLMTexRenderable;

	surf->m_tex			=	m_ctx->NewTex( &rtkey, "offscreen plain surface" );
	surf->m_face		=	0;
	surf->m_mip			=	0;
	
	//desc
	surf->m_desc.Format				=	Format;
    surf->m_desc.Type				=	D3DRTYPE_SURFACE;
    surf->m_desc.Usage				=	0;
    surf->m_desc.Pool				=	D3DPOOL_DEFAULT;
	surf->m_desc.MultiSampleType	=	D3DMULTISAMPLE_NONE;
    surf->m_desc.MultiSampleQuality	=	0;
    surf->m_desc.Width				=	Width;
    surf->m_desc.Height				=	Height;

	*ppSurface = (result==S_OK) ? surf : NULL;
	
	return result;	
}

HRESULT COpenGLDevice9::CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	HRESULT	result = D3DERR_INVALIDCALL;
	*ppShader = NULL;
	
	int nShadowDepthSamplerMask = ShadowDepthSamplerMaskFromName( pShaderName );
	uint nCentroidMask = CentroidMaskFromName( true, pShaderName );

	if ( pCentroidMask )
	{
		if ( *pCentroidMask != nCentroidMask )
		{
			char buf[256];
			V_snprintf( buf, sizeof( buf ), "IDirect3DDevice9::CreatePixelShader: shaderapi's centroid mask (0x%08X) differs from mask derived from shader name (0x%08X) for shader %s\n", *pCentroidMask, nCentroidMask, pDebugLabel );
			Plat_DebugString( buf );
		}
		// It would be great if we could use these centroid masks passed in from shaderapi - but unfortunately they're only available for pixel shaders, and we also need to compute matching masks for vertex shaders!
		//nCentroidMask = *pCentroidMask;
	}

	{
		int numTranslations = 1;
		
		bool bVertexShader = false;

		// we can do one or two translated forms. they go together in a single buffer with some markers to allow GLM to break it up.
		// this also lets us mirror each set of translations to disk with a single file making it easier to view and edit side by side.
		
		int maxTranslationSize = 50000;	// size of any one translation
		
		CUtlBuffer transbuf( 3000, numTranslations * maxTranslationSize, CUtlBuffer::TEXT_BUFFER );
		CUtlBuffer tempbuf( 3000, maxTranslationSize, CUtlBuffer::TEXT_BUFFER );

		transbuf.PutString( "//GLSLfp\n" );		// this is required so GLM can crack the text apart

		// note the GLSL translator wants its own buffer
		tempbuf.EnsureCapacity( maxTranslationSize );
			
		uint glslPixelShaderOptions = D3DToGL_OptionUseEnvParams;// | D3DToGL_OptionAllowStaticControlFlow;
			

		// Fake SRGB mode - needed on R500, probably indefinitely.
		// Do this stuff if caps show m_needsFakeSRGB=true and the sRGBWrite state is true
		// (but not if it's engine_post which is special)

		if (!m_ctx->Caps().m_hasGammaWrites)
		{
			if ( pShaderName )
			{
				if ( !V_stristr( pShaderName, "engine_post" ) )
				{
					glslPixelShaderOptions |= D3DToGL_OptionSRGBWriteSuffix;
				}
			}
		}

		g_D3DToOpenGLTranslatorGLSL.TranslateShader( (uint32 *) pFunction, &tempbuf, &bVertexShader, glslPixelShaderOptions, nShadowDepthSamplerMask, nCentroidMask, pDebugLabel );
			
		transbuf.PutString( (char*)tempbuf.Base() );
		transbuf.PutString( "\n\n" );	// whitespace
				
		if ( bVertexShader )
		{
			// don't cross the streams
			Assert(!"Can't accept vertex shader in CreatePixelShader");
			result = D3DERR_INVALIDCALL;
		}
		else
		{
			m_ObjectStats.m_nTotalPixelShaders++;

			IDirect3DPixelShader9 *newprog = new IDirect3DPixelShader9;

			newprog->m_pixHighWater = 0;
			newprog->m_pixSamplerMask = 0;
			newprog->m_pixSamplerTypes = 0;
					
			newprog->m_pixProgram = m_ctx->NewProgram( kGLMFragmentProgram, (char *)transbuf.Base(), pShaderName ? pShaderName : "?" ) ;
			newprog->m_pixProgram->m_nCentroidMask = nCentroidMask;
			newprog->m_pixProgram->m_nShadowDepthSamplerMask = nShadowDepthSamplerMask;
			
			newprog->m_pixProgram->m_bTranslatedProgram = true;
			newprog->m_pixProgram->m_maxVertexAttrs = 0;

			newprog->m_device = this;
			
			//------ find the frag program metadata and extract it..
						
			
			{
				// find the highwater mark
				char *highWaterPrefix = "//HIGHWATER-";		// try to arrange this so it can work with pure GLSL if needed
				char *highWaterStr = strstr( (char *)transbuf.Base(), highWaterPrefix );
				if (highWaterStr)
				{
					char *highWaterActualData = highWaterStr + strlen( highWaterPrefix );
				
					int value = -1;
					sscanf( highWaterActualData, "%d", &value );

					newprog->m_pixHighWater = value;
					newprog->m_pixProgram->m_descs[kGLMGLSL].m_highWater = value;
				}
				else
				{
					Assert(!"couldn't find sampler map in pixel shader");
				}
			}

			{
				// find the sampler map
				char *samplerMaskPrefix = "//SAMPLERMASK-";		// try to arrange this so it can work with pure GLSL if needed
			
				char *samplerMaskStr = strstr( (char *)transbuf.Base(), samplerMaskPrefix );
				if (samplerMaskStr)
				{
					char *samplerMaskActualData = samplerMaskStr + strlen( samplerMaskPrefix );
				
					int value = -1;
					sscanf( samplerMaskActualData, "%04x", &value );

					newprog->m_pixSamplerMask = value;
					newprog->m_pixProgram->m_samplerMask = value;	// helps GLM maintain a better linked pair cache even when SRGB sampler state changes
										
					int nMaxReg;
					for ( nMaxReg = 31; nMaxReg >= 0; --nMaxReg )
						if ( value & ( 1 << nMaxReg ) )
							break;

					newprog->m_pixProgram->m_maxSamplers = nMaxReg + 1;

					int nNumUsedSamplers = 0;
					for ( int i = 31; i >= 0; --i)
						if ( value & ( 1 << i ) )
							nNumUsedSamplers++;
					newprog->m_pixProgram->m_nNumUsedSamplers = nNumUsedSamplers;
				}
				else
				{
					Assert(!"couldn't find sampler map in pixel shader");
				}
			}

			{
				// find the sampler map
				char *samplerTypesPrefix = "//SAMPLERTYPES-";		// try to arrange this so it can work with pure GLSL if needed

				char *samplerTypesStr = strstr( (char *)transbuf.Base(), samplerTypesPrefix );
				if (samplerTypesStr)
				{
					char *samplerTypesActualData = samplerTypesStr + strlen( samplerTypesPrefix );

					int value = -1;
					sscanf( samplerTypesActualData, "%08x", &value );

					newprog->m_pixSamplerTypes = value;
					newprog->m_pixProgram->m_samplerTypes = value;	// helps GLM maintain a better linked pair cache even when SRGB sampler state changes
				}
				else
				{
					Assert(!"couldn't find sampler types in pixel shader");
				}
			}

			{
				// find the fb outputs used by this shader/combo
				const GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };

				char *fragDataMaskPrefix = "//FRAGDATAMASK-";		

				char *fragDataMaskStr = strstr( (char *)transbuf.Base(), fragDataMaskPrefix );
				if ( fragDataMaskStr )
				{
					char *fragDataActualData = fragDataMaskStr + strlen( fragDataMaskPrefix );

					int value = -1;
					sscanf( fragDataActualData, "%04x", &value );

					newprog->m_pixFragDataMask = value;
					newprog->m_pixProgram->m_fragDataMask = value;

					newprog->m_pixProgram->m_numDrawBuffers = 0;
					for( int i = 0; i < 4; i++ )
					{
						if( newprog->m_pixProgram->m_fragDataMask & ( 1 << i ) )
						{
							newprog->m_pixProgram->m_drawBuffers[ newprog->m_pixProgram->m_numDrawBuffers ] = buffers[ i ];
							newprog->m_pixProgram->m_numDrawBuffers++;
						}
					}

					if( newprog->m_pixProgram->m_numDrawBuffers ==  0 )
					{
						Assert(!"couldn't find fragment output in pixel shader");
						newprog->m_pixProgram->m_drawBuffers[ 0 ] = buffers[ 0 ];
						newprog->m_pixProgram->m_numDrawBuffers = 1;
					}
				}
				else
				{
					newprog->m_pixFragDataMask = 0;
					newprog->m_pixProgram->m_fragDataMask = 0;
				}

			}
									
			*ppShader = newprog;
			
			result = S_OK;
		}
	}

	return result;	
}

HRESULT COpenGLDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	if (m_ctx->Caps().m_hasOcclusionQuery)
	{
		m_ObjectStats.m_nTotalQueries++;

		IDirect3DQuery9	*newquery = new IDirect3DQuery9;
		
		newquery->m_device = this;
		
		newquery->m_type = Type;
		newquery->m_ctx = m_ctx;
		newquery->m_nIssueStartThreadID = 0;
		newquery->m_nIssueEndThreadID = 0;
		newquery->m_nIssueStartDrawCallIndex = 0;
		newquery->m_nIssueEndDrawCallIndex = 0;

		GLMQueryParams	params;
		memset( &params, 0, sizeof(params) );
		
		//bool known = false;
		switch(newquery->m_type)
		{
			case	D3DQUERYTYPE_OCCLUSION:				/* D3DISSUE_BEGIN, D3DISSUE_END */
				// create an occlusion query
				params.m_type = EOcclusion;
			break;
			
			case	D3DQUERYTYPE_EVENT:					/* D3DISSUE_END */
				params.m_type = EFence;
			break;
			
			case	D3DQUERYTYPE_RESOURCEMANAGER:		/* D3DISSUE_END */
			case	D3DQUERYTYPE_TIMESTAMP:				/* D3DISSUE_END */
			case	D3DQUERYTYPE_TIMESTAMPFREQ:			/* D3DISSUE_END */
			case	D3DQUERYTYPE_INTERFACETIMINGS:		/* D3DISSUE_BEGIN, D3DISSUE_END */
			case	D3DQUERYTYPE_PIXELTIMINGS:			/* D3DISSUE_BEGIN, D3DISSUE_END */
			case	D3DQUERYTYPE_CACHEUTILIZATION:		/* D3DISSUE_BEGIN, D3DISSUE_END */
				Assert( !"Un-implemented query type" );
			break;
			
			default:
				Assert( !"Unknown query type" );
			break;
		}
		newquery->m_query = m_ctx->NewQuery( &params );
		
		*ppQuery = newquery;
		return S_OK;
	}
	else
	{
		*ppQuery = NULL;
		return -1;	// failed
	}	
}

HRESULT COpenGLDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	HRESULT result = S_OK;
	
	m_ObjectStats.m_nTotalSurfaces++;
	m_ObjectStats.m_nTotalRenderTargets++;

	IDirect3DSurface9 *surf = new IDirect3DSurface9;
	surf->m_restype = D3DRTYPE_SURFACE;

	surf->m_device		= this;				// always set device on creations!
	
	GLMTexLayoutKey rtkey;
	memset( &rtkey, 0, sizeof(rtkey) );
	
	rtkey.m_texGLTarget	=	GL_TEXTURE_2D;
	rtkey.m_xSize		=	Width;
	rtkey.m_ySize		=	Height;
	rtkey.m_zSize		=	1;

	rtkey.m_texFormat	=	Format;
	rtkey.m_texFlags	=	kGLMTexRenderable;

	rtkey.m_texFlags |= kGLMTexSRGB;	// all render target tex are SRGB mode
	if (m_ctx->Caps().m_cantAttachSRGB)
	{
		// this config can't support SRGB render targets.  quietly turn off the sRGB bit.
		rtkey.m_texFlags &= ~kGLMTexSRGB;
	}
	
	if ( (MultiSample !=0) && (!m_ctx->Caps().m_nvG7x) )
	{
		rtkey.m_texFlags |= kGLMTexMultisampled;
		rtkey.m_texSamples = MultiSample;
		// FIXME no support for "MS quality" yet
	}

	surf->m_tex			= m_ctx->NewTex( &rtkey, pDebugLabel );
	surf->m_face		= 0;
	surf->m_mip			= 0;
	
	//desc
	surf->m_desc.Format				=	Format;
    surf->m_desc.Type				=	D3DRTYPE_SURFACE;
    surf->m_desc.Usage				=	0;					//FIXME ???????????
    surf->m_desc.Pool				=	D3DPOOL_DEFAULT;	//FIXME ???????????
	surf->m_desc.MultiSampleType	=	MultiSample;
    surf->m_desc.MultiSampleQuality	=	MultisampleQuality;
    surf->m_desc.Width				=	Width;
    surf->m_desc.Height				=	Height;

	*ppSurface = (result==S_OK) ? surf : NULL;

	#if IUNKNOWN_ALLOC_SPEW
		char scratch[1024];
		sprintf(scratch,"RT %s", surf->m_tex->m_layout->m_layoutSummary );
		surf->SetMark( true, scratch ); 
	#endif
	
	
	return result;	
}

HRESULT COpenGLDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB){}

HRESULT COpenGLDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );

	m_ObjectStats.m_nTotalTextures++;

	GLMPRINTF((">-A-IDirect3DDevice9::CreateTexture"));
	IDirect3DTexture9	*dxtex = new IDirect3DTexture9;
	dxtex->m_restype = D3DRTYPE_TEXTURE;
	
	dxtex->m_device		= this;

	dxtex->m_descZero.Format	= Format;
	dxtex->m_descZero.Type		= D3DRTYPE_TEXTURE;
	dxtex->m_descZero.Usage		= Usage;
	dxtex->m_descZero.Pool		= Pool;

	dxtex->m_descZero.MultiSampleType		= D3DMULTISAMPLE_NONE;
	dxtex->m_descZero.MultiSampleQuality	= 0;
	dxtex->m_descZero.Width		= Width;
	dxtex->m_descZero.Height	= Height;
	
	GLMTexLayoutKey key;
	memset( &key, 0, sizeof(key) );
	
	key.m_texGLTarget	= GL_TEXTURE_2D;
	key.m_texFormat		= Format;

	if (Levels>1)
	{
		key.m_texFlags |= kGLMTexMipped;
	}

	// http://msdn.microsoft.com/en-us/library/bb172625(VS.85).aspx
	
	// complain if any usage bits come down that I don't know.
	uint knownUsageBits = (D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC | D3DUSAGE_TEXTURE_SRGB | D3DUSAGE_DEPTHSTENCIL);
	if ( (Usage & knownUsageBits) != Usage )
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}
	
	if (Usage & D3DUSAGE_AUTOGENMIPMAP)
	{
		key.m_texFlags |= kGLMTexMipped | kGLMTexMippedAuto;
	}
	
	if (Usage & D3DUSAGE_DYNAMIC)
	{
		// GLMPRINTF(("-X- DYNAMIC tex usage ignored.."));	//FIXME
	}
	
	if (Usage & D3DUSAGE_TEXTURE_SRGB)
	{
		key.m_texFlags |= kGLMTexSRGB;
	}
	
	if (Usage & D3DUSAGE_RENDERTARGET)
	{
		Assert( !(Usage & D3DUSAGE_DEPTHSTENCIL) );
		
		m_ObjectStats.m_nTotalRenderTargets++;
						
		key.m_texFlags |= kGLMTexRenderable;
		
		const GLMTexFormatDesc *pFmtDesc = GetFormatDesc( key.m_texFormat );
		if ( pFmtDesc->m_glIntFormatSRGB != 0 )
		{
			key.m_texFlags |= kGLMTexSRGB;			// this catches callers of CreateTexture who set the "renderable" option - they get an SRGB tex
		}
		
		if (m_ctx->Caps().m_cantAttachSRGB)
		{
			// this config can't support SRGB render targets.  quietly turn off the sRGB bit.
			key.m_texFlags &= ~kGLMTexSRGB;
		}
	}

	if ( ( Format == D3DFMT_D16 ) || ( Format == D3DFMT_D24X8 ) || ( Format == D3DFMT_D24S8 ) )
	{
		key.m_texFlags |= kGLMTexIsDepth;
	}
	if ( Format == D3DFMT_D24S8 )
	{
		key.m_texFlags |= kGLMTexIsStencil;
	}
			
	key.m_xSize = Width;
	key.m_ySize = Height;
	key.m_zSize = 1;
	
	CGLMTex *tex = m_ctx->NewTex( &key, pDebugLabel );
	if (!tex)
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}
	dxtex->m_tex = tex;

	dxtex->m_tex->m_srgbFlipCount = 0;

	m_ObjectStats.m_nTotalSurfaces++;

	dxtex->m_surfZero = new IDirect3DSurface9;
	dxtex->m_surfZero->m_restype = (D3DRESOURCETYPE)0;	// this is a ref to a tex, not the owner... 
	
	// do not do an AddRef here.	
	
	dxtex->m_surfZero->m_device = this;

	dxtex->m_surfZero->m_desc	=	dxtex->m_descZero;
	dxtex->m_surfZero->m_tex	=	tex;
	dxtex->m_surfZero->m_face	=	0;
	dxtex->m_surfZero->m_mip	=	0;
	
	GLMPRINTF(("-A- IDirect3DDevice9::CreateTexture created '%s' @ %08x (GLM %08x) %s",tex->m_layout->m_layoutSummary, dxtex, tex, pDebugLabel ? pDebugLabel : "" ));
	
	*ppTexture = dxtex;
	
	GLMPRINTF(("<-A-IDirect3DDevice9::CreateTexture"));
	return S_OK;	
}

HRESULT COpenGLDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF(( ">-A- IDirect3DDevice9::CreateVertexBuffer" ));
	Assert( m_ctx->m_nCurOwnerThreadId == ThreadGetCurrentId() );
	
	m_ObjectStats.m_nTotalVertexBuffers++;

	IDirect3DVertexBuffer9 *newbuff = new IDirect3DVertexBuffer9;
	
	newbuff->m_device = this;
	
	newbuff->m_ctx = m_ctx;

		// FIXME need to find home or use for the Usage, FVF, Pool values passed in
	uint options = 0;
	
	if (Usage&D3DUSAGE_DYNAMIC)
	{
		options |= GLMBufferOptionDynamic;
	}
	
	newbuff->m_vtxBuffer = m_ctx->NewBuffer( kGLMVertexBuffer, Length, options  ) ;
	
	newbuff->m_vtxDesc.Type		= D3DRTYPE_VERTEXBUFFER;
	newbuff->m_vtxDesc.Usage	= Usage;
	newbuff->m_vtxDesc.Pool		= Pool;
	newbuff->m_vtxDesc.Size		= Length;

	*ppVertexBuffer = newbuff;
	
	GLMPRINTF(( "<-A- IDirect3DDevice9::CreateVertexBuffer" ));

	return S_OK;	
}

HRESULT COpenGLDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	*ppDecl = NULL;
	
	// the goal here is to arrive at something which lets us quickly generate GLMVertexSetups.

	// the information we don't have, that must be inferred from the decls, is:
	// -> how many unique streams (buffers) are used - pure curiosity
	// -> what the stride and offset is for each decl.  Size you can figure out on the spot, stride requires surveying all the components in each stream first.
	//	so init an array of per-stream offsets to 0.
	//	each one is a cursor that gets bumped by decls.
	uint	streamOffsets[ D3D_MAX_STREAMS ];
	uint	streamCount = 0;
	
	uint	attribMap[16];
	uint	attribMapIndex = 0;
	memset( attribMap, 0xFF, sizeof( attribMap ) );
	
	memset( streamOffsets, 0, sizeof( streamOffsets ) );

	m_ObjectStats.m_nTotalVertexDecls++;

	IDirect3DVertexDeclaration9 *decl9 = new IDirect3DVertexDeclaration9;
	decl9->m_device = this;
	
	decl9->m_elemCount = 0;
	
	for (const D3DVERTEXELEMENT9 *src = pVertexElements; (src->Stream != 0xFF); src++)
	{
		// element
		D3DVERTEXELEMENT9_GL *elem = &decl9->m_elements[ decl9->m_elemCount++ ];

		// copy the D3D decl wholesale.
		elem->m_dxdecl = *src;
		
		// latch current offset in this stream.
		elem->m_gldecl.m_offset = streamOffsets[ elem->m_dxdecl.Stream ];
		
		// figure out size of this attr and move the cursor
		// if cursor was on zero, bump the active stream count
		
		if (!streamOffsets[ elem->m_dxdecl.Stream ])
			streamCount++;
		
		int bytes = 0;
		switch( elem->m_dxdecl.Type )
		{
			case D3DDECLTYPE_FLOAT1:	elem->m_gldecl.m_nCompCount = 1; elem->m_gldecl.m_datatype = GL_FLOAT; elem->m_gldecl.m_normalized=0; bytes = 4; break;
			case D3DDECLTYPE_FLOAT2:	elem->m_gldecl.m_nCompCount = 2; elem->m_gldecl.m_datatype = GL_FLOAT; elem->m_gldecl.m_normalized=0; bytes = 8; break;

			//case D3DVSDT_FLOAT3:
			case D3DDECLTYPE_FLOAT3:	elem->m_gldecl.m_nCompCount = 3; elem->m_gldecl.m_datatype = GL_FLOAT; elem->m_gldecl.m_normalized=0; bytes = 12; break;
			
			//case D3DVSDT_FLOAT4:
			case D3DDECLTYPE_FLOAT4:	elem->m_gldecl.m_nCompCount = 4; elem->m_gldecl.m_datatype = GL_FLOAT; elem->m_gldecl.m_normalized=0; bytes = 16; break;
			
			// case D3DVSDT_UBYTE4:		
			case D3DDECLTYPE_D3DCOLOR:
			case D3DDECLTYPE_UBYTE4:
			case D3DDECLTYPE_UBYTE4N:
				
				// Force this path since we're on 10.6.2 and can't rely on EXT_vertex_array_bgra
				if ( 1 )
				{
					// pass 4 UB's but we know this is out of order compared to D3DCOLOR data
					elem->m_gldecl.m_nCompCount = 4; elem->m_gldecl.m_datatype = GL_UNSIGNED_BYTE;
				}
				else
				{
					// pass a GL BGRA color courtesy of http://www.opengl.org/registry/specs/ARB/vertex_array_bgra.txt
					elem->m_gldecl.m_nCompCount = GL_BGRA; elem->m_gldecl.m_datatype = GL_UNSIGNED_BYTE;
				}

				elem->m_gldecl.m_normalized = ( (elem->m_dxdecl.Type == D3DDECLTYPE_D3DCOLOR) ||
												(elem->m_dxdecl.Type == D3DDECLTYPE_UBYTE4N) );
				
				bytes = 4;
			break;
			
			case D3DDECLTYPE_SHORT2:
				// pass 2 US's but we know this is out of order compared to D3DCOLOR data
				elem->m_gldecl.m_nCompCount = 2; elem->m_gldecl.m_datatype = GL_SHORT;

				elem->m_gldecl.m_normalized = 0;
				
				bytes = 4;
			break;
			
			default:	DXABSTRACT_BREAK_ON_ERROR(); return D3DERR_INVALIDCALL; break;

			/*
				typedef enum _D3DDECLTYPE
				{
					D3DDECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
					D3DDECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
					D3DDECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
					D3DDECLTYPE_FLOAT4    =  3,  // 4D float
					D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
												 // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
					D3DDECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
					D3DDECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 1.)
					D3DDECLTYPE_SHORT4    =  7,  // 4D signed short

				// The following types are valid only with vertex shaders >= 2.0


					D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 255.0
					D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
					D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
					D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
					D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
					D3DDECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
					D3DDECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
					D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
					D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
					D3DDECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.
				} D3DDECLTYPE;
			*/
		}
		
		// write the offset and move the cursor
		elem->m_gldecl.m_offset = streamOffsets[elem->m_dxdecl.Stream];
		streamOffsets[ elem->m_dxdecl.Stream ] += bytes;
		
		// cannot write m_stride yet, so zero it
		elem->m_gldecl.m_stride = 0;
		
		elem->m_gldecl.m_pBuffer = NULL;	// must be filled in at draw time..
		
		// elem count was already bumped.
		
		// update attrib map
		attribMap[ attribMapIndex++ ] = (elem->m_dxdecl.Usage << 4) | (elem->m_dxdecl.UsageIndex);
	}
	// the loop is done, we now know how many active streams there are, how many atribs are active in the declaration,
	// and how big each one is in terms of stride.

	// all that is left is to go back and write the strides - the stride comes from the stream offset cursors accumulated earlier.
	for( unsigned int j=0; j< decl9->m_elemCount; j++)
	{
		D3DVERTEXELEMENT9_GL *elem = &decl9->m_elements[ j ];
		
		elem->m_gldecl.m_stride = streamOffsets[ elem->m_dxdecl.Stream ];
	}
		
	memset( decl9->m_VertexAttribDescToStreamIndex, 0xFF, sizeof( decl9->m_VertexAttribDescToStreamIndex ) );
	D3DVERTEXELEMENT9_GL *pDeclElem = decl9->m_elements;
	for( unsigned int j = 0; j < decl9->m_elemCount; j++, pDeclElem++)
	{
		uint nPackedVertexAttribDesc = ( pDeclElem->m_dxdecl.Usage << 4 ) | pDeclElem->m_dxdecl.UsageIndex;
		if ( nPackedVertexAttribDesc == 0xBB )
		{
			// 0xBB is a reserved packed vertex attrib value - shouldn't encounter in practice
			DXABSTRACT_BREAK_ON_ERROR();
		}
		decl9->m_VertexAttribDescToStreamIndex[ nPackedVertexAttribDesc ] = j;
	}
	
	*ppDecl = decl9;
	
	return S_OK;	
}

HRESULT COpenGLDevice9::CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	HRESULT	result = D3DERR_INVALIDCALL;
	*ppShader = NULL;

	uint32 nCentroidMask = CentroidMaskFromName( false, pShaderName );
			
	{
		int numTranslations = 1;
		
		bool bVertexShader = false;

		// we can do one or two translated forms. they go together in a single buffer with some markers to allow GLM to break it up.
		// this also lets us mirror each set of translations to disk with a single file making it easier to view and edit side by side.
		
		int maxTranslationSize = 500000;	// size of any one translation

		CUtlBuffer transbuf( 1000, numTranslations * maxTranslationSize, CUtlBuffer::TEXT_BUFFER );
		CUtlBuffer tempbuf( 1000, maxTranslationSize, CUtlBuffer::TEXT_BUFFER );

		transbuf.PutString( "//GLSLvp\n" );		// this is required so GLM can crack the text apart

		// note the GLSL translator wants its own buffer
		tempbuf.EnsureCapacity( maxTranslationSize );
			
		uint glslVertexShaderOptions = D3DToGL_OptionUseEnvParams | D3DToGL_OptionDoFixupZ | D3DToGL_OptionDoFixupY;

		if ( m_ctx->Caps().m_hasNativeClipVertexMode )
		{
			// note the matched trickery over in IDirect3DDevice9::FlushStates - 
			// if on a chipset that does no have native gl_ClipVertex support, then
			// omit writes to gl_ClipVertex, and instead submit plane equations that have been altered,
			// and clipping will take place in GL space using gl_Position instead of gl_ClipVertex.
				
			// note that this is very much a hack to mate up with ATI R5xx hardware constraints, and with older
			// drivers even for later ATI parts like r6xx/r7xx.   And it doesn't work on NV parts, so you really
			// do have to choose the right way to go.
				
			glslVertexShaderOptions |= D3DToGL_OptionDoUserClipPlanes; 
		}
			
		if ( !CommandLine()->CheckParm("-disableboneuniformbuffers") )
		{
			// If using GLSL, enabling a uniform buffer specifically for bone registers. (Not currently supported with ARB shaders, which are not optimized at all anyway.)
			glslVertexShaderOptions |= D3DToGL_OptionGenerateBoneUniformBuffer;
		}

		g_D3DToOpenGLTranslatorGLSL.TranslateShader( (uint32 *) pFunction, &tempbuf, &bVertexShader, glslVertexShaderOptions, -1, nCentroidMask, pDebugLabel );
			
		transbuf.PutString( (char*)tempbuf.Base() );
		transbuf.PutString( "\n\n" );	// whitespace
				
		if ( !bVertexShader )
		{
			// don't cross the streams
			Assert(!"Can't accept pixel shader in CreateVertexShader");
			result = D3DERR_INVALIDCALL;
		}
		else
		{
			m_ObjectStats.m_nTotalVertexShaders++;

			IDirect3DVertexShader9 *newprog = new IDirect3DVertexShader9;

			newprog->m_device = this;
					
			newprog->m_vtxProgram = m_ctx->NewProgram( kGLMVertexProgram, (char *)transbuf.Base(), pShaderName ? pShaderName : "?" ) ;
			newprog->m_vtxProgram->m_nCentroidMask = nCentroidMask;

			newprog->m_vtxProgram->m_bTranslatedProgram = true;
			newprog->m_vtxProgram->m_maxVertexAttrs = 0;
			newprog->m_maxVertexAttrs = 0;
						
			// find the highwater mark..
						
			char *highWaterPrefix = "//HIGHWATER-";		// try to arrange this so it can work with pure GLSL if needed
			char *highWaterStr = strstr( (char *)transbuf.Base(), highWaterPrefix );
			if (highWaterStr)
			{
				char *highWaterActualData = highWaterStr + strlen( highWaterPrefix );
				
				int value = -1;
				sscanf( highWaterActualData, "%d", &value );

				newprog->m_vtxHighWater = value;
				newprog->m_vtxProgram->m_descs[kGLMGLSL].m_highWater = value;
			}
			else
			{
				Assert(!"couldn't find highwater mark in vertex shader");
			}

			char *highWaterBonePrefix = "//HIGHWATERBONE-";		// try to arrange this so it can work with pure GLSL if needed
			char *highWaterBoneStr = strstr( (char *)transbuf.Base(), highWaterBonePrefix );
			if (highWaterBoneStr)
			{
				char *highWaterActualData = highWaterBoneStr + strlen( highWaterBonePrefix );

				int value = -1;
				sscanf( highWaterActualData, "%d", &value );

				newprog->m_vtxHighWaterBone = value;
				newprog->m_vtxProgram->m_descs[kGLMGLSL].m_VSHighWaterBone = value;
			}
			else
			{
				newprog->m_vtxHighWaterBone = 0;
				newprog->m_vtxProgram->m_descs[kGLMGLSL].m_VSHighWaterBone = 0;
			}
									
			// find the attrib map..
			char *attribMapPrefix = "//ATTRIBMAP-";		// try to arrange this so it can work with pure GLSL if needed
			char *attribMapStr = strstr( (char *)transbuf.Base(), attribMapPrefix );
			
			if (attribMapStr)
			{
				char *attribMapActualData = attribMapStr + strlen( attribMapPrefix );
				uint nMaxVertexAttribs = 0;
				for( int i=0; i<16; i++)
				{
					int value = -1;
					char *dataItem = attribMapActualData + (i*3);
					sscanf( dataItem, "%02x", &value );
					if (value >=0)
					{
						// make sure it's not a terminator
						if (value == 0xBB)
						{
							DXABSTRACT_BREAK_ON_ERROR();
						}
					}
					else
					{
						// probably an 'xx'... check
						if ( (dataItem[0] != 'x') || (dataItem[1] != 'x') )
						{
							DXABSTRACT_BREAK_ON_ERROR();	// bad news
						}
						else
						{
							value = 0xBB;		// not likely to see one of these... "fog with usage index 11"
						}
					}

					if ( value != 0xBB )
						nMaxVertexAttribs = i;

					newprog->m_vtxAttribMap[i] = value;
				}

				newprog->m_vtxProgram->m_maxVertexAttrs = nMaxVertexAttribs + 1;
				newprog->m_maxVertexAttrs = nMaxVertexAttribs + 1;
			}
			else
			{
				DXABSTRACT_BREAK_ON_ERROR();	// that's bad...
			}
									
			*ppShader = newprog;
			
			result = S_OK;
		}
	}

	return result;	
}

HRESULT COpenGLDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle)
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF((">-A-  IDirect3DDevice9::CreateVolumeTexture"));
	// set dxtex->m_restype to D3DRTYPE_VOLUMETEXTURE...

	Assert( m_ctx->m_nCurOwnerThreadId == ThreadGetCurrentId() );

	m_ObjectStats.m_nTotalTextures++;

	IDirect3DVolumeTexture9	*dxtex = new IDirect3DVolumeTexture9;
	dxtex->m_restype = D3DRTYPE_VOLUMETEXTURE;
	
	dxtex->m_device			= this;

	dxtex->m_descZero.Format	= Format;
	dxtex->m_descZero.Type		= D3DRTYPE_VOLUMETEXTURE;
	dxtex->m_descZero.Usage		= Usage;
	dxtex->m_descZero.Pool		= Pool;

	dxtex->m_descZero.MultiSampleType		= D3DMULTISAMPLE_NONE;
	dxtex->m_descZero.MultiSampleQuality	= 0;
	dxtex->m_descZero.Width		= Width;
	dxtex->m_descZero.Height	= Height;

	// also a volume specific desc
	dxtex->m_volDescZero.Format		= Format;
	dxtex->m_volDescZero.Type		= D3DRTYPE_VOLUMETEXTURE;
	dxtex->m_volDescZero.Usage		= Usage;
	dxtex->m_volDescZero.Pool		= Pool;

	dxtex->m_volDescZero.Width		= Width;
	dxtex->m_volDescZero.Height		= Height;
	dxtex->m_volDescZero.Depth		= Depth;
	
	GLMTexLayoutKey key;
	memset( &key, 0, sizeof(key) );
	
	key.m_texGLTarget	= GL_TEXTURE_3D;
	key.m_texFormat		= Format;

	if (Levels>1)
	{
		key.m_texFlags |= kGLMTexMipped;
	}

	// http://msdn.microsoft.com/en-us/library/bb172625(VS.85).aspx	
	// complain if any usage bits come down that I don't know.
	uint knownUsageBits = (D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC | D3DUSAGE_TEXTURE_SRGB);
	if ( (Usage & knownUsageBits) != Usage )
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}
	
	if (Usage & D3DUSAGE_AUTOGENMIPMAP)
	{
		key.m_texFlags |= kGLMTexMipped | kGLMTexMippedAuto;
	}
	
	if (Usage & D3DUSAGE_RENDERTARGET)
	{
		key.m_texFlags |= kGLMTexRenderable;
		
		m_ObjectStats.m_nTotalRenderTargets++;
	}
	
	if (Usage & D3DUSAGE_DYNAMIC)
	{
		GLMPRINTF(("-X- DYNAMIC tex usage ignored.."));	//FIXME
	}
	
	if (Usage & D3DUSAGE_TEXTURE_SRGB)
	{
		key.m_texFlags |= kGLMTexSRGB;
	}
	
	key.m_xSize = Width;
	key.m_ySize = Height;
	key.m_zSize = Depth;
	
	CGLMTex *tex = m_ctx->NewTex( &key, pDebugLabel );
	if (!tex)
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}
	dxtex->m_tex = tex;
	
	dxtex->m_tex->m_srgbFlipCount = 0;

	m_ObjectStats.m_nTotalSurfaces++;

	dxtex->m_surfZero = new IDirect3DSurface9;
	dxtex->m_surfZero->m_restype = (D3DRESOURCETYPE)0;	// this is a ref to a tex, not the owner... 
	// do not do an AddRef here.	
	
	dxtex->m_surfZero->m_device = this;
	
	dxtex->m_surfZero->m_desc	=	dxtex->m_descZero;
	dxtex->m_surfZero->m_tex	=	tex;
	dxtex->m_surfZero->m_face	=	0;
	dxtex->m_surfZero->m_mip	=	0;
	
	GLMPRINTF(("-A- IDirect3DDevice9::CreateVolumeTexture created '%s' @ %08x (GLM %08x)",tex->m_layout->m_layoutSummary, dxtex, tex ));
	
	*ppVolumeTexture = dxtex;

	GLMPRINTF(("<-A-  IDirect3DDevice9::CreateVolumeTexture"));

	return S_OK;	
}

HRESULT COpenGLDevice9::DeletePatch(UINT Handle){}

HRESULT COpenGLDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	tmZone( TELEMETRY_LEVEL2, TMZF_NONE, "%s", __FUNCTION__ );
	Assert( m_ctx->m_nCurOwnerThreadId == ThreadGetCurrentId() );
		
	TOGL_NULL_DEVICE_CHECK;
	if ( m_bFBODirty )
	{
		UpdateBoundFBO();
	}

	if ( m_bFBODirty )
	{
		UpdateBoundFBO();
	}
		
	g_nTotalDrawsOrClears++;

#if GL_BATCH_PERF_ANALYSIS
	m_nTotalPrims += primCount;
	CFastTimer tm;
	CFlushDrawStatesStats& flushStats = m_ctx->m_FlushStats;
	tm.Start();
	flushStats.Clear();
#endif

#if GLMDEBUG
	if ( gl.m_FogEnable )
	{
		GLMPRINTF(("-D- IDirect3DDevice9::DrawIndexedPrimitive is seeing enabled fog..."));
	}
#endif

	if ( ( !m_indices.m_idxBuffer ) || ( !m_vertexShader ) )
		goto draw_failed;
	
	{
		GL_BATCH_PERF_CALL_TIMER;
								
		m_ctx->FlushDrawStates( MinVertexIndex, MinVertexIndex + NumVertices - 1, BaseVertexIndex );

		{
#if !GL_TELEMETRY_ZONES && GL_BATCH_TELEMETRY_ZONES
			tmZone( TELEMETRY_LEVEL2, TMZF_NONE, "glDrawRangeElements %u", primCount );
#endif
			Assert( ( D3DPT_LINELIST == 2 ) && ( D3DPT_TRIANGLELIST == 4 ) && ( D3DPT_TRIANGLESTRIP == 5 ) );

			static const struct prim_t
			{
				GLenum m_nType;
				uint m_nPrimMul;
				uint m_nPrimAdd;
			} s_primTypes[6] = 
			{ 
				{ 0, 0, 0 },				// 0
				{ 0, 0, 0 },				// 1
				{ GL_LINES, 2, 0 },			// 2 D3DPT_LINELIST
				{ 0, 0, 0 },				// 3 
				{ GL_TRIANGLES, 3, 0 },		// 4 D3DPT_TRIANGLELIST
				{ GL_TRIANGLE_STRIP, 1, 2 }	// 5 D3DPT_TRIANGLESTRIP
			};

			if ( Type <= D3DPT_TRIANGLESTRIP )	
			{
				const prim_t& p = s_primTypes[Type];
				Assert( p.m_nType );
				Assert( NumVertices >= 1 );

				m_ctx->DrawRangeElements( p.m_nType, (GLuint)MinVertexIndex, (GLuint)( MinVertexIndex + NumVertices - 1 ), (GLsizei)p.m_nPrimAdd + primCount * p.m_nPrimMul, (GLenum)GL_UNSIGNED_SHORT, (const GLvoid *)( startIndex * sizeof(short) ), BaseVertexIndex, m_indices.m_idxBuffer->m_idxBuffer );
			}
		}
	}

#if GL_BATCH_PERF_ANALYSIS
	if ( s_rdtsc_to_ms == 0.0f )
	{
		TmU64 t0 = Plat_Rdtsc();
		double d0 = Plat_FloatTime();

		ThreadSleep( 1000 );

		TmU64 t1 = Plat_Rdtsc();
		double d1 = Plat_FloatTime();

		s_rdtsc_to_ms = ( 1000.0f * ( d1 - d0 ) ) / ( t1 - t0 );
	}

#if GL_BATCH_PERF_ANALYSIS_WRITE_PNGS
	if ( m_pBatch_vis_bitmap && m_pBatch_vis_bitmap->is_valid() )
	{
		double t = tm.GetDurationInProgress().GetMillisecondsF();

		uint h = 1;
		if ( gl_batch_vis_y_scale.GetFloat() > 0.0f)
		{
			h = ceil( t / gl_batch_vis_y_scale.GetFloat() );
			h = MAX(h, 1);
		}

		// Total time spent inside any and all our "D3D9" calls
		double flTotalD3DTime = g_nTotalD3DCycles * s_rdtsc_to_ms;
		m_pBatch_vis_bitmap->fill_box(0, m_nBatchVisY, (uint)(.5f + flTotalD3DTime / gl_batch_vis_abs_scale.GetFloat() * m_pBatch_vis_bitmap->width()), h, 150, 150, 150);

		// Total total spent processing just DrawIndexedPrimitive() for this batch.
		m_pBatch_vis_bitmap->fill_box(0, m_nBatchVisY, (uint)(.5f + t / gl_batch_vis_abs_scale.GetFloat() * m_pBatch_vis_bitmap->width()), h, 70, 70, 70);
		
		double flTotalGLMS = gGL->m_nTotalGLCycles * s_rdtsc_to_ms;
		
		// Total time spent inside of all OpenGL calls
		m_pBatch_vis_bitmap->additive_fill_box(0, m_nBatchVisY, (uint)(.5f + flTotalGLMS / gl_batch_vis_abs_scale.GetFloat() * m_pBatch_vis_bitmap->width()), h, 0, 0, 64);
								
		if (flushStats.m_nNewVS) m_pBatch_vis_bitmap->additive_fill_box(80-16, m_nBatchVisY, 8, h, 0, 110, 0);
		if (flushStats.m_nNewPS) m_pBatch_vis_bitmap->additive_fill_box(80-8, m_nBatchVisY, 8, h, 110, 0, 110);
		
		int lm = 80;
		m_pBatch_vis_bitmap->fill_box(lm+0+flushStats.m_nFirstVSConstant, m_nBatchVisY, flushStats.m_nNumVSConstants, h, 64, 255, 255);
		m_pBatch_vis_bitmap->fill_box(lm+64, m_nBatchVisY, flushStats.m_nNumVSBoneConstants, h, 255, 64, 64);
		m_pBatch_vis_bitmap->fill_box(lm+64+256+flushStats.m_nFirstPSConstant, m_nBatchVisY, flushStats.m_nNumPSConstants, h, 64, 64, 255);

		m_pBatch_vis_bitmap->fill_box(lm+64+256+32, m_nBatchVisY, flushStats.m_nNumChangedSamplers, h, 255, 255, 255);
		m_pBatch_vis_bitmap->fill_box(lm+64+256+32+16, m_nBatchVisY, flushStats.m_nNumSamplingParamsChanged, h, 92, 128, 255);

		if ( flushStats.m_nVertexBufferChanged ) m_pBatch_vis_bitmap->fill_box(lm+64+256+32+16+64, m_nBatchVisY, 16, h, 128, 128, 128);
		if ( flushStats.m_nIndexBufferChanged ) m_pBatch_vis_bitmap->fill_box(lm+64+256+32+16+64+16, m_nBatchVisY, 16, h, 128, 128, 255);

		m_pBatch_vis_bitmap->fill_box(lm+64+256+32+16+64+16+16, m_nBatchVisY, ( ( g_nTotalVBLockBytes + g_nTotalIBLockBytes ) * 64 + 2047 ) / 2048, h, 120, 120, 120 );
		m_pBatch_vis_bitmap->additive_fill_box(lm+64+256+32+16+64+16+16, m_nBatchVisY, ( g_nTotalVBLockBytes * 64 + 2047 ) / 2048, h, 120, 0, 0);

		m_nBatchVisY += h;
	}
#endif
	
	m_nNumProgramChanges += ((flushStats.m_nNewVS + flushStats.m_nNewPS) != 0);

	m_flTotalD3DTime += g_nTotalD3DCycles * s_rdtsc_to_ms;
	m_nTotalD3DCalls += g_nTotalD3DCalls;
	g_nTotalD3DCycles = 0;
	g_nTotalD3DCalls = 0;

	m_flTotalGLTime += gGL->m_nTotalGLCycles * s_rdtsc_to_ms;
	m_nTotalGLCalls += gGL->m_nTotalGLCalls;
	gGL->m_nTotalGLCycles = 0;
	gGL->m_nTotalGLCalls = 0; 

	g_nTotalVBLockBytes = 0;
	g_nTotalIBLockBytes = 0;
#endif

	return S_OK;

draw_failed:
	Assert( 0 );
	return E_FAIL;	
}

HRESULT COpenGLDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo){}
HRESULT COpenGLDevice9::DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo){}

HRESULT COpenGLDevice9::EndScene()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	m_ctx->EndFrame();
	return S_OK;		
}
	
HRESULT COpenGLDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB){}

HRESULT COpenGLDevice9::EvictManagedResources()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	GLMPRINTF(("-X- IDirect3DDevice9::EvictManagedResources --> IGNORED"));
	return S_OK;	
}

UINT COpenGLDevice9::GetAvailableTextureMem(){}
HRESULT COpenGLDevice9::GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer){}
HRESULT COpenGLDevice9::GetClipPlane(DWORD Index,float *pPlane){}
HRESULT COpenGLDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus){}
HRESULT COpenGLDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters){}
HRESULT COpenGLDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber){}

HRESULT COpenGLDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	if ( !ppZStencilSurface )
	{
		return D3DERR_INVALIDCALL;		
	}
	
	if ( !m_pDepthStencil )
	{
		*ppZStencilSurface = NULL;
		return D3DERR_NOTFOUND;
	}

	m_pDepthStencil->AddRef(0, "+B  GetDepthStencilSurface public addref");			// per http://msdn.microsoft.com/en-us/library/bb174384(VS.85).aspx

	*ppZStencilSurface = m_pDepthStencil;

	return S_OK;	
}

HRESULT COpenGLDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	
	// "Adapter" is used to index amongst the set of fake-adapters maintained in the display DB
	GLMDisplayDB *db = GetDisplayDB();
	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;

	GLMRendererInfoFields	glmRendererInfo;
	GLMDisplayInfoFields	glmDisplayInfo;

	bool result = db->GetFakeAdapterInfo( m_params.m_adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo ); (void)result;
	Assert (!result);
	// just leave glmRendererInfo filled out for subsequent code to look at as needed.

	FillD3DCaps9( glmRendererInfo, pCaps );

	return S_OK;	
}

HRESULT COpenGLDevice9::GetDirect3D(IDirect3D9 **ppD3D9){}
HRESULT COpenGLDevice9::GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode){}

HRESULT COpenGLDevice9::GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::GetFVF(DWORD *pFVF)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

void COpenGLDevice9::GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp){}
HRESULT COpenGLDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData,UINT *pBaseVertexIndex){}

HRESULT COpenGLDevice9::GetLight(DWORD Index,D3DLIGHT9 *pLight){} 

HRESULT COpenGLDevice9::GetLightEnable(DWORD Index,BOOL *pEnable)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;		
}

HRESULT COpenGLDevice9::GetMaterial(D3DMATERIAL9 *pMaterial){}
FLOAT COpenGLDevice9::GetNPatchMode(){}
UINT COpenGLDevice9::GetNumberOfSwapChains(){}
HRESULT COpenGLDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries){}
HRESULT COpenGLDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader){}
HRESULT COpenGLDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount){}
HRESULT COpenGLDevice9::GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount){}
HRESULT COpenGLDevice9::GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount){}
HRESULT COpenGLDevice9::GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus){}
HRESULT COpenGLDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue){}

HRESULT COpenGLDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	if ( !m_pRenderTargets[ RenderTargetIndex ] )
		return D3DERR_NOTFOUND;
	
	if ( ( RenderTargetIndex > 4 ) || !ppRenderTarget )
		return D3DERR_INVALIDCALL;

	// safe because of early exit on NULL above
	m_pRenderTargets[ RenderTargetIndex ]->AddRef(0, "+B GetRenderTarget public addref");	// per http://msdn.microsoft.com/en-us/library/bb174404(VS.85).aspx
	
	*ppRenderTarget = m_pRenderTargets[ RenderTargetIndex ];
	
	return S_OK;	
}

HRESULT COpenGLDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	// is it just a blit ?

	this->StretchRect( pRenderTarget, NULL, pDestSurface, NULL, D3DTEXF_NONE ); // is this good enough ???

	return S_OK;	
}

HRESULT COpenGLDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue){}
HRESULT COpenGLDevice9::GetScissorRect(RECT *pRect){}
BOOL COpenGLDevice9::GetSoftwareVertexProcessing(){}
HRESULT COpenGLDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride){}
HRESULT COpenGLDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider){}
HRESULT COpenGLDevice9::GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain){}

HRESULT COpenGLDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	// if implemented, should it increase the ref count ??
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue){}
HRESULT COpenGLDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix){}
HRESULT COpenGLDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl){}
HRESULT COpenGLDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader){}
HRESULT COpenGLDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount){}
HRESULT COpenGLDevice9::GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount){}
HRESULT COpenGLDevice9::GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount){}

HRESULT COpenGLDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	// 7LS - unfinished, used in scaleformuirenderimpl.cpp (only width and height required)
	GL_BATCH_PERF_CALL_TIMER;
	Assert( GetCurrentOwnerThreadId() == ThreadGetCurrentId() );
	GLMPRINTF(("-X- IDirect3DDevice9::GetViewport " ));

	pViewport->X = gl.m_ViewportBox.x;
	pViewport->Width = gl.m_ViewportBox.width;

	pViewport->Y = gl.m_ViewportBox.y;
	pViewport->Height = gl.m_ViewportBox.height;	

	pViewport->MinZ = gl.m_ViewportDepthRange.flNear;
	pViewport->MaxZ = gl.m_ViewportDepthRange.flFar;

	return S_OK;		
}
	
HRESULT COpenGLDevice9::LightEnable(DWORD LightIndex,BOOL bEnable){}
HRESULT COpenGLDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix){}

HRESULT COpenGLDevice9::Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion)
{
	GL_BATCH_PERF( g_nTotalD3DCalls++; )
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
			
	TOGL_NULL_DEVICE_CHECK;

	if ( m_bFBODirty )
	{
		UpdateBoundFBO();
	}
		
	// before attempting to present a tex, make sure it's been resolved if it was MSAA.
		// if we push that responsibility down to m_ctx->Present, it could probably do it without an extra copy.
		// i.e. anticipate the blit from the resolvedtex to GL_BACK, and just do that instead.

	// no explicit ResolveTex call first - that got pushed down into GLMContext::Present
		
#if GL_BATCH_PERF_ANALYSIS
	uint64 nStartGLCycles = gGL->m_nTotalGLCycles;
	nStartGLCycles;
	
	CFastTimer tm;
	tm.Start();
#endif

	m_ctx->Present( m_pDefaultColorSurface->m_tex );
		
#if GL_BATCH_PERF_ANALYSIS
	double flPresentTime = tm.GetDurationInProgress().GetMillisecondsF();
	double flGLSwapWindowTime = g_pLauncherMgr->GetPrevGLSwapWindowTime();

	m_flOverallPresentTime += flPresentTime;
	m_flOverallPresentTimeSquared += flPresentTime * flPresentTime;
	m_flOverallSwapWindowTime += flGLSwapWindowTime;
	m_flOverallSwapWindowTimeSquared += flGLSwapWindowTime * flGLSwapWindowTime;
	m_nOverallPresents++;

	uint64 nEndGLCycles = gGL->m_nTotalGLCycles;
	nEndGLCycles;

	m_flTotalD3DTime += flPresentTime + g_nTotalD3DCycles * s_rdtsc_to_ms;
	m_nTotalD3DCalls += g_nTotalD3DCalls;
		
	m_flTotalGLTime += gGL->m_nTotalGLCycles * s_rdtsc_to_ms;
	m_nTotalGLCalls += gGL->m_nTotalGLCalls;

	m_nOverallProgramChanges += m_nNumProgramChanges;
	m_nOverallDraws += g_nTotalDrawsOrClears;
	m_nOverallPrims += m_nTotalPrims;
	m_nOverallD3DCalls += m_nTotalD3DCalls;
	m_flOverallD3DTime += m_flTotalD3DTime;
	m_nOverallGLCalls += m_nTotalGLCalls;
	m_flOverallGLTime += m_flTotalGLTime;
					
	static int nPrevBatchVis = -1;

#if GL_BATCH_PERF_ANALYSIS_WRITE_PNGS
	if ((nPrevBatchVis == 1) && m_pBatch_vis_bitmap && m_pBatch_vis_bitmap->is_valid())
	{
		double flTotalGLPresentTime = ( nEndGLCycles - nStartGLCycles ) * s_rdtsc_to_ms;

		m_pBatch_vis_bitmap->fill_box(0, m_nBatchVisY, (uint)(.5f + flPresentTime / gl_present_vis_abs_scale.GetFloat() * m_pBatch_vis_bitmap->width()), 10, 255, 16, 128);
		m_pBatch_vis_bitmap->additive_fill_box(0, m_nBatchVisY, (uint)(.5f + flTotalGLPresentTime / gl_present_vis_abs_scale.GetFloat() * m_pBatch_vis_bitmap->width()), 10, 0, 255, 128);
		m_nBatchVisY += 10;

		uint y = MAX(m_nBatchVisY + 20, 600), l = 0;
		m_pBatch_vis_bitmap->draw_formatted_text(0, y+8*(l++), 1, 255, 255, 255, "OpenGL Frame: %u, Batches+Clears: %u, Prims: %u, Program Changes: %u", m_nOverallPresents, g_nTotalDrawsOrClears, m_nTotalPrims, m_nNumProgramChanges );
		m_pBatch_vis_bitmap->draw_formatted_text(0, y+8*(l++), 1, 255, 255, 255, "Frame: D3D Calls: %u, D3D Time: %3.3fms", m_nTotalD3DCalls, m_flTotalD3DTime);
		m_pBatch_vis_bitmap->draw_formatted_text(0, y+8*(l++), 1, 255, 255, 255, "Frame:  GL Calls: %u,  GL Time: %3.3fms", m_nTotalGLCalls, m_flTotalGLTime);
		l++;
		m_pBatch_vis_bitmap->draw_formatted_text(0, y+8*(l++), 1, 255, 255, 255, "Overall: Batches: %u, Prims: %u, Program Changes: %u", m_nOverallDraws, m_nOverallPrims, m_nOverallProgramChanges  );
		m_pBatch_vis_bitmap->draw_formatted_text(0, y+8*(l++), 1, 255, 255, 255, "Overall: D3D Calls: %u D3D Time: %4.3fms", m_nOverallD3DCalls, m_flOverallD3DTime );
		m_pBatch_vis_bitmap->draw_formatted_text(0, y+8*(l++), 1, 255, 255, 255, "Overall:  GL Calls: %u  GL Time: %4.3fms", m_nOverallGLCalls, m_flOverallGLTime );
		
		size_t png_size = 0;
		void *pPNG_data = tdefl_write_image_to_png_file_in_memory(m_pBatch_vis_bitmap->get_ptr(), m_pBatch_vis_bitmap->width(), m_pBatch_vis_bitmap->height(), 3, &png_size, true);
		if (pPNG_data)
		{
			char filename[256];
			V_snprintf(filename, sizeof(filename), "left4dead2/batchvis_%u_%u.png", m_nBatchVisFileIdx, m_nBatchVisFrameIndex);
			FILE* pFile = fopen(filename, "wb");
			if (pFile)
			{
				fwrite(pPNG_data, png_size, 1, pFile);
				fclose(pFile);
			}
			free(pPNG_data);
		}
		m_nBatchVisFrameIndex++;
		m_nBatchVisY = 0;
		m_pBatch_vis_bitmap->cls();
	}
#endif

	if (nPrevBatchVis != (int)gl_batch_vis.GetBool())
	{
		if ( !m_pBatch_vis_bitmap )
			m_pBatch_vis_bitmap = new simple_bitmap;

		nPrevBatchVis = gl_batch_vis.GetBool();
		if (!nPrevBatchVis)
		{
			DumpStatsToConsole( NULL );
			m_pBatch_vis_bitmap->clear();
		}
		else
		{
			m_pBatch_vis_bitmap->init(768, 1024);
		}
		m_nBatchVisY = 0;
		m_nBatchVisFrameIndex = 0;
		m_nBatchVisFileIdx = (uint)time(NULL); //rand();

		m_nOverallProgramChanges  = 0;
		m_nOverallDraws = 0;
		m_nOverallD3DCalls = 0;
		m_flOverallD3DTime = 0;
		m_nOverallGLCalls = 0;
		m_flOverallGLTime  = 0;
		m_flOverallPresentTime = 0;
		m_flOverallPresentTimeSquared = 0;
		m_flOverallSwapWindowTime = 0;
		m_flOverallSwapWindowTimeSquared = 0;
		m_nOverallPresents = 0;
	}
	
	g_nTotalD3DCycles = 0;
	g_nTotalD3DCalls = 0;
	gGL->m_nTotalGLCycles = 0;
	gGL->m_nTotalGLCalls = 0;
		
	m_nNumProgramChanges = 0;
	m_flTotalD3DTime = 0;
	m_nTotalD3DCalls = 0;
	m_flTotalGLTime = 0;
	m_nTotalGLCalls = 0;
	m_nTotalPrims = 0;
#else
	if ( gl_batch_vis.GetBool() )
	{
		gl_batch_vis.SetValue( false );
		
		ConMsg( "Must define GL_BATCH_PERF_ANALYSIS to use this feature" );
	}
#endif

	g_nTotalDrawsOrClears = 0;
				
#if GL_TELEMETRY_GPU_ZONES
	g_TelemetryGPUStats.Clear();
#endif

	return S_OK;	
}

HRESULT COpenGLDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags){}

HRESULT COpenGLDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	Assert( m_nValidMarker == D3D_DEVICE_VALID_MARKER );
	HRESULT result = S_OK;

	// define the task of reset as:
	// provide new drawable RT's for the backbuffer (color and depthstencil).
	// fix up viewport / scissor..
	// then pass the new presentation parameters through to GLM.
	// (it will in turn notify appframework on the next present... which may be very soon, as mode changes are usually spotted inside Present() ).
	
	// so some of this looks a lot like Create - we're just a subset of what it does.
	// with a little work you could refactor this to be common code.

	GLMDebugPrintf( "IDirect3DDevice9::Reset: BackBufWidth: %u, BackBufHeight: %u, D3DFMT: %u, BackBufCount: %u, MultisampleType: %u, MultisampleQuality: %u\n",
		pPresentationParameters->BackBufferWidth,
		pPresentationParameters->BackBufferHeight,
		pPresentationParameters->BackBufferFormat,
		pPresentationParameters->BackBufferCount,
		pPresentationParameters->MultiSampleType,
		pPresentationParameters->MultiSampleQuality );

	//------------------------------------------------------------------------------- absorb new presentation params..
	
	m_params.m_presentationParameters = *pPresentationParameters;
	
	//------------------------------------------------------------------------------- color buffer..
	// release old color surface if it's there..
	if ( m_pDefaultColorSurface )
	{
		ULONG refc = m_pDefaultColorSurface->Release( 0, "IDirect3DDevice9::Reset public release color surface" ); (void)refc;
		Assert( !refc );
		m_pDefaultColorSurface = NULL;
	}
	
	GLMPRINTF(("-X- IDirect3DDevice9::Reset making new color render target..."));

	// color surface
	result = this->CreateRenderTarget( 
		m_params.m_presentationParameters.BackBufferWidth,			// width
		m_params.m_presentationParameters.BackBufferHeight,			// height
		m_params.m_presentationParameters.BackBufferFormat,			// format
		m_params.m_presentationParameters.MultiSampleType,			// MSAA depth
		m_params.m_presentationParameters.MultiSampleQuality,		// MSAA quality
		true,														// lockable
		&m_pDefaultColorSurface,										// ppSurface
		NULL														// shared handle
		);

	if ( result != S_OK )
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Reset (error out)"));
		return result;
	}
	// do not do an AddRef here..

	GLMPRINTF(("-X- IDirect3DDevice9::Reset making color render target complete -> %08x", m_pDefaultColorSurface ));

	GLMPRINTF(("-X- IDirect3DDevice9::Reset setting color render target..."));
	
	result = this->SetDepthStencilSurface( NULL );

	result = this->SetRenderTarget( 0, m_pDefaultColorSurface );
	if (result != S_OK)
	{
		GLMPRINTF(("< IDirect3DDevice9::Reset (error out)"));
		return result;
	}
	GLMPRINTF(("-X- IDirect3DDevice9::Reset setting color render target complete."));


	//-------------------------------------------------------------------------------depth stencil buffer
	// release old depthstencil surface if it's there..
	if ( m_pDefaultDepthStencilSurface )
	{
		ULONG refc = m_pDefaultDepthStencilSurface->Release( 0, "IDirect3DDevice9::Reset public release depthstencil surface" ); (void)refc;
		Assert(!refc);
		m_pDefaultDepthStencilSurface = NULL;
	}
	
	Assert (m_params.m_presentationParameters.EnableAutoDepthStencil);

	GLMPRINTF(("-X- IDirect3DDevice9::Reset making depth-stencil..."));
    result = CreateDepthStencilSurface(
		m_params.m_presentationParameters.BackBufferWidth,			// width
		m_params.m_presentationParameters.BackBufferHeight,			// height
		m_params.m_presentationParameters.AutoDepthStencilFormat,	// format
		m_params.m_presentationParameters.MultiSampleType,			// MSAA depth
		m_params.m_presentationParameters.MultiSampleQuality,		// MSAA quality
		TRUE,														// enable z-buffer discard ????
		&m_pDefaultDepthStencilSurface,								// ppSurface
		NULL														// shared handle
		);
	if (result != S_OK)
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Reset (error out)"));
		return result;
	}
		// do not do an AddRef here..

	GLMPRINTF(("-X- IDirect3DDevice9::Reset making depth-stencil complete -> %08x", m_pDefaultDepthStencilSurface));

	GLMPRINTF(("-X- IDirect3DDevice9::Reset setting depth-stencil render target..."));
	result = this->SetDepthStencilSurface(m_pDefaultDepthStencilSurface);
	if (result != S_OK)
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Reset (error out)"));
		return result;
	}
	GLMPRINTF(("-X- IDirect3DDevice9::Reset setting depth-stencil render target complete."));

	UpdateBoundFBO();

	bool ready = m_ctx->m_drawingFBO->IsReady();
	if (!ready)
	{
		GLMPRINTF(("<-X- IDirect3DDevice9::Reset (error out)"));
		return D3DERR_DEVICELOST;
	}

	//-------------------------------------------------------------------------------zap viewport and scissor to new backbuffer size

	InitStates();

	GLScissorEnable_t		defScissorEnable		= { true };
	GLScissorBox_t			defScissorBox			= { 0,0, m_params.m_presentationParameters.BackBufferWidth,m_params.m_presentationParameters.BackBufferHeight };
	GLViewportBox_t			defViewportBox			= { 0,0, m_params.m_presentationParameters.BackBufferWidth,m_params.m_presentationParameters.BackBufferHeight, m_params.m_presentationParameters.BackBufferWidth | ( m_params.m_presentationParameters.BackBufferHeight << 16 ) };
	GLViewportDepthRange_t	defViewportDepthRange	= { 0.1, 1000.0 };
	GLCullFaceEnable_t		defCullFaceEnable		= { true };
	GLCullFrontFace_t		defCullFrontFace		= { GL_CCW };
	
	gl.m_ScissorEnable		=	defScissorEnable;
	gl.m_ScissorBox			=	defScissorBox;
	gl.m_ViewportBox		=	defViewportBox;
	gl.m_ViewportDepthRange	=	defViewportDepthRange;
	gl.m_CullFaceEnable		=	defCullFaceEnable;
	gl.m_CullFrontFace		=	defCullFrontFace;
			
	FullFlushStates();
	
	//-------------------------------------------------------------------------------finally, propagate new display params to GLM context
	GLMDisplayParams	glmParams;	
	ConvertPresentationParamsToGLMDisplayParams( pPresentationParameters, &glmParams );

	// steal back previously sent focus window...
	glmParams.m_focusWindow = m_ctx->m_displayParams.m_focusWindow;
	Assert( glmParams.m_focusWindow != NULL );

	// so GetClientRect can return sane answers
	//uint width, height;		
	RenderedSize( pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight, true );	// true = set

	m_ctx->Reset();
		
	m_ctx->SetDisplayParams( &glmParams );
	
	return S_OK;		
}

HRESULT COpenGLDevice9::SetClipPlane(DWORD Index,const float *pPlane)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	Assert(Index<2);

	// We actually push the clip plane coeffs to two places
	// - into a shader param for ARB mode
	// - and into the API defined clip plane slots for GLSL mode.
	
	// if ARB mode... THIS NEEDS TO GO... it's messing up the dirty ranges..
	{
	//	this->SetVertexShaderConstantF( DXABSTRACT_VS_CLIP_PLANE_BASE+Index, pPlane, 1 );	// stash the clip plane values into shader param - translator knows where to look
	}
	
	// if GLSL mode... latch it and let FlushStates push it out
	{
		GLClipPlaneEquation_t	peq;
		peq.x = pPlane[0];
		peq.y = pPlane[1];
		peq.z = pPlane[2];
		peq.w = pPlane[3];

		gl.m_ClipPlaneEquation[ Index ] = peq;
		FlushClipPlaneEquation();

		// m_ctx->WriteClipPlaneEquation( &peq, Index );
	}

	return S_OK;	
}

HRESULT COpenGLDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus){}
HRESULT COpenGLDevice9::SetCurrentTexturePalette(UINT PaletteNumber){}
void COpenGLDevice9::SetCursorPosition(INT X,INT Y,DWORD Flags){}
HRESULT COpenGLDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap){}

HRESULT COpenGLDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	HRESULT	result = S_OK;

	GLMPRINTF(("-F- SetDepthStencilSurface, surface=%8x (tex=%8x %s)",
		pNewZStencil,
		pNewZStencil ? pNewZStencil->m_tex : NULL,
		pNewZStencil ? pNewZStencil->m_tex->m_layout->m_layoutSummary : ""
	));

	if ( pNewZStencil == m_pDepthStencil )
	{
		GLMPRINTF(("-F-             --> no change"));
		return S_OK;
	}

	if ( pNewZStencil )
	{
		pNewZStencil->AddRef(1, "+A  SetDepthStencilSurface private addref");
	}

	if ( m_pDepthStencil )
	{
		// Note this Release() could cause the surface to be deleted!
		m_pDepthStencil->Release(1, "-A  SetDepthStencilSurface private release");
	}
	
	m_pDepthStencil = pNewZStencil;

	m_bFBODirty = true;
		
	return result;	
}

HRESULT COpenGLDevice9::SetDialogBoxMode(BOOL bEnableDialogs){}

HRESULT COpenGLDevice9::SetFVF(DWORD FVF)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

void COpenGLDevice9::SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp)
{
	GL_BATCH_PERF_CALL_TIMER;
	Assert( GetCurrentOwnerThreadId() == ThreadGetCurrentId() );
	
	if ( g_pLauncherMgr )

	{
		g_pLauncherMgr->SetGammaRamp( pRamp->red, pRamp->green, pRamp->blue );
	}	
}

HRESULT COpenGLDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	// just latch it.
	m_indices.m_idxBuffer = pIndexData;

	return S_OK;
}

HRESULT COpenGLDevice9::SetLight(DWORD Index,const D3DLIGHT9 *pLight)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	GLMPRINTF(("-X- IDirect3DDevice9::SetMaterial - ignored."));
//	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::SetNPatchMode(float nSegments){}
HRESULT COpenGLDevice9::SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries){}

HRESULT COpenGLDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	
	m_ctx->SetFragmentProgram( pShader ? pShader->m_pixProgram : NULL );
	m_pixelShader = pShader;

	return S_OK;	
}

HRESULT COpenGLDevice9::SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	TOGL_NULL_DEVICE_CHECK;
	m_ctx->SetProgramParametersB( kGLMFragmentProgram, StartRegister, (int *)pConstantData, BoolCount );
	return S_OK;	
}

HRESULT COpenGLDevice9::SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	TOGL_NULL_DEVICE_CHECK;
#if 0
	const unsigned int nRegToWatch = 3;
	if ( ( ( StartRegister + Vector4fCount ) > nRegToWatch ) && ( StartRegister <= nRegToWatch ) )
	{
		char buf[256];
		V_snprintf( buf, sizeof(buf ), "-- %f %f %f %f\n", pConstantData[(nRegToWatch - StartRegister)*4+0], pConstantData[(nRegToWatch - StartRegister)*4+1], pConstantData[(nRegToWatch - StartRegister)*4+2], pConstantData[(nRegToWatch - StartRegister)*4+3] );
		Plat_DebugString( buf );
	}
#endif
	m_ctx->SetProgramParametersF( kGLMFragmentProgram, StartRegister, (float *)pConstantData, Vector4fCount );
	return S_OK;	
}

HRESULT COpenGLDevice9::SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	TOGL_NULL_DEVICE_CHECK;
	GLMPRINTF(("-X- Ignoring IDirect3DDevice9::SetPixelShaderConstantI call, count was %d", Vector4iCount ));
//	m_ctx->SetProgramParametersI( kGLMFragmentProgram, StartRegister, pConstantData, Vector4iCount );
	return S_OK;	
}

HRESULT COpenGLDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	TOGL_NULL_DEVICE_CHECK;

#if GLMDEBUG
	// None of this is needed normally.
	char	rsSpew = 1;
	char	ignored = 0;

	if (State >= D3DRS_VALUE_LIMIT)
	{
		DXABSTRACT_BREAK_ON_ERROR();		// bad
		return S_OK;
	}

	const D3D_RSINFO *info = &g_D3DRS_INFO_unpacked[ State ];
	if (info->m_state != State)
	{
		DXABSTRACT_BREAK_ON_ERROR();	// bad - we never set up that state in our list
		return S_OK;
	}

	if (rsSpew)
	{
		GLMPRINTF(("-X- IDirect3DDevice9::SetRenderState: set %s(%d) to %d(0x%08x) ( class %d, defval is %d(0x%08x) )", GLMDecode( eD3D_RSTATE,State),State, Value,Value, info->m_class, info->m_defval,info->m_defval ));
	}

	switch( info->m_class )
	{
		case 0:		// just ignore quietly. example: D3DRS_LIGHTING
			ignored = 1;
			break;

		case 1:
		{
			// no GL response - and no error as long as the write value matches the default
			if (Value != info->m_defval)
			{
				static char stop_here_1 = 0;
				if (stop_here_1)
					DXABSTRACT_BREAK_ON_ERROR();
			}
			break;
		}

		case 2:
		{
			// provide GL response, but only support known default value
			if (Value != info->m_defval)
			{
				static char stop_here_2 = 0;
				if (stop_here_2)
					DXABSTRACT_BREAK_ON_ERROR();
			}
			// fall through to mode 3
		}
		case 3:
			// normal case - the switch statement below will handle this
			break;
	}
#endif

	switch (State)
	{
		case D3DRS_ZENABLE:				// kGLDepthTestEnable
		{
			gl.m_DepthTestEnable.enable = Value;
			m_ctx->WriteDepthTestEnable( &gl.m_DepthTestEnable );
			break;
		}
		case D3DRS_ZWRITEENABLE:			// kGLDepthMask
		{
			gl.m_DepthMask.mask = Value;
			m_ctx->WriteDepthMask( &gl.m_DepthMask );
			break;
		}
		case D3DRS_ZFUNC:	
		{
			// kGLDepthFunc
			GLenum func = D3DCompareFuncToGL( Value );
			gl.m_DepthFunc.func = func;
			m_ctx->WriteDepthFunc( &gl.m_DepthFunc );
			break;
		}

		case D3DRS_COLORWRITEENABLE:		// kGLColorMaskSingle
		{
			gl.m_ColorMaskSingle.r	=	((Value & D3DCOLORWRITEENABLE_RED)  != 0) ? 0xFF : 0x00;
			gl.m_ColorMaskSingle.g	=	((Value & D3DCOLORWRITEENABLE_GREEN)!= 0) ? 0xFF : 0x00;	
			gl.m_ColorMaskSingle.b	=	((Value & D3DCOLORWRITEENABLE_BLUE) != 0) ? 0xFF : 0x00;
			gl.m_ColorMaskSingle.a	=	((Value & D3DCOLORWRITEENABLE_ALPHA)!= 0) ? 0xFF : 0x00;
			m_ctx->WriteColorMaskSingle( &gl.m_ColorMaskSingle );
			break;
		}

		case D3DRS_CULLMODE:				// kGLCullFaceEnable / kGLCullFrontFace
		{
			switch (Value)
			{
				case D3DCULL_NONE:
				{
					gl.m_CullFaceEnable.enable = false;
					gl.m_CullFrontFace.value = GL_CCW;	//doesn't matter																

					m_ctx->WriteCullFaceEnable( &gl.m_CullFaceEnable );
					m_ctx->WriteCullFrontFace( &gl.m_CullFrontFace );
					break;
				}
					
				case D3DCULL_CW:
				{
					gl.m_CullFaceEnable.enable = true;
					gl.m_CullFrontFace.value = GL_CW;	//origGL_CCW;

					m_ctx->WriteCullFaceEnable( &gl.m_CullFaceEnable );
					m_ctx->WriteCullFrontFace( &gl.m_CullFrontFace );
					break;
				}
				case D3DCULL_CCW:
				{
					gl.m_CullFaceEnable.enable = true;
					gl.m_CullFrontFace.value = GL_CCW;	//origGL_CW;

					m_ctx->WriteCullFaceEnable( &gl.m_CullFaceEnable );
					m_ctx->WriteCullFrontFace( &gl.m_CullFrontFace );
					break;
				}
				default:	
				{
					DXABSTRACT_BREAK_ON_ERROR();	
					break;
				}
			}
			break;
		}

		//-------------------------------------------------------------------------------------------- alphablend stuff

		case D3DRS_ALPHABLENDENABLE:		// kGLBlendEnable
		{
			gl.m_BlendEnable.enable = Value;
			m_ctx->WriteBlendEnable( &gl.m_BlendEnable );
			break;
		}

		case D3DRS_BLENDOP:				// kGLBlendEquation				// D3D blend-op ==> GL blend equation
		{
			GLenum	equation = D3DBlendOperationToGL( Value );
			gl.m_BlendEquation.equation = equation;
			m_ctx->WriteBlendEquation( &gl.m_BlendEquation );
			break;
		}

		case D3DRS_SRCBLEND:				// kGLBlendFactor				// D3D blend-factor ==> GL blend factor
		case D3DRS_DESTBLEND:			// kGLBlendFactor
		{
			GLenum	factor = D3DBlendFactorToGL( Value );

			if (State==D3DRS_SRCBLEND)
			{
				gl.m_BlendFactor.srcfactor = factor;
			}
			else
			{
				gl.m_BlendFactor.dstfactor = factor;
			}
			m_ctx->WriteBlendFactor( &gl.m_BlendFactor );
			break;
		}

		case D3DRS_SRGBWRITEENABLE:			// kGLBlendEnableSRGB
		{
			gl.m_BlendEnableSRGB.enable = Value;
			m_ctx->WriteBlendEnableSRGB( &gl.m_BlendEnableSRGB );
			break;					
		}

		//-------------------------------------------------------------------------------------------- alphatest stuff

		case D3DRS_ALPHATESTENABLE:
		{
			gl.m_AlphaTestEnable.enable = Value;
			m_ctx->WriteAlphaTestEnable( &gl.m_AlphaTestEnable );
			break;
		}

		case D3DRS_ALPHAREF:
		{
			gl.m_AlphaTestFunc.ref = Value / 255.0f;
			m_ctx->WriteAlphaTestFunc( &gl.m_AlphaTestFunc );
			break;
		}

		case D3DRS_ALPHAFUNC:
		{
			GLenum func = D3DCompareFuncToGL( Value );;
			gl.m_AlphaTestFunc.func = func;
			m_ctx->WriteAlphaTestFunc( &gl.m_AlphaTestFunc );
			break;
		}

		//-------------------------------------------------------------------------------------------- stencil stuff

		case D3DRS_STENCILENABLE:		// GLStencilTestEnable_t
		{
			gl.m_StencilTestEnable.enable = Value;
			m_ctx->WriteStencilTestEnable( &gl.m_StencilTestEnable );
			break;
		}

		case D3DRS_STENCILFAIL:			// GLStencilOp_t		"what do you do if stencil test fails"
		{
			GLenum stencilop = D3DStencilOpToGL( Value );
			gl.m_StencilOp.sfail = stencilop;

			m_ctx->WriteStencilOp( &gl.m_StencilOp,0 );
			m_ctx->WriteStencilOp( &gl.m_StencilOp,1 );		// ********* need to recheck this
			break;
		}

		case D3DRS_STENCILZFAIL:			// GLStencilOp_t		"what do you do if stencil test passes *but* depth test fails, if depth test happened"
		{
			GLenum stencilop = D3DStencilOpToGL( Value );
			gl.m_StencilOp.dpfail = stencilop;

			m_ctx->WriteStencilOp( &gl.m_StencilOp,0 );
			m_ctx->WriteStencilOp( &gl.m_StencilOp,1 );		// ********* need to recheck this
			break;
		}

		case D3DRS_STENCILPASS:			// GLStencilOp_t		"what do you do if stencil test and depth test both pass"
		{
			GLenum stencilop = D3DStencilOpToGL( Value );
			gl.m_StencilOp.dppass = stencilop;

			m_ctx->WriteStencilOp( &gl.m_StencilOp,0 );
			m_ctx->WriteStencilOp( &gl.m_StencilOp,1 );		// ********* need to recheck this
			break;
		}

		case D3DRS_STENCILFUNC:			// GLStencilFunc_t
		{
			GLenum stencilfunc = D3DCompareFuncToGL( Value );
			gl.m_StencilFunc.frontfunc = gl.m_StencilFunc.backfunc = stencilfunc;

			m_ctx->WriteStencilFunc( &gl.m_StencilFunc );
			break;
		}

		case D3DRS_STENCILREF:			// GLStencilFunc_t
		{
			gl.m_StencilFunc.ref = Value;
			m_ctx->WriteStencilFunc( &gl.m_StencilFunc );
			break;
		}

		case D3DRS_STENCILMASK:			// GLStencilFunc_t
		{
			gl.m_StencilFunc.mask = Value;
			m_ctx->WriteStencilFunc( &gl.m_StencilFunc );
			break;
		}

		case D3DRS_STENCILWRITEMASK:		// GLStencilWriteMask_t
		{
			gl.m_StencilWriteMask.mask = Value;
			m_ctx->WriteStencilWriteMask( &gl.m_StencilWriteMask );
			break;
		}

		case D3DRS_FOGENABLE:			// none of these are implemented yet... erk
		{
			gl.m_FogEnable = (Value != 0);
			GLMPRINTF(("-D- fogenable = %d",Value ));
			break;
		}
		
		case D3DRS_SCISSORTESTENABLE:	// kGLScissorEnable
		{
			gl.m_ScissorEnable.enable = Value;
			m_ctx->WriteScissorEnable( &gl.m_ScissorEnable );
			break;
		}

		case D3DRS_DEPTHBIAS:			// kGLDepthBias
		{
			// the value in the dword is actually a float
			float	fvalue = *(float*)&Value;
			gl.m_DepthBias.units = fvalue;

			m_ctx->WriteDepthBias( &gl.m_DepthBias );
			break;
		}

		// good ref on these: http://aras-p.info/blog/2008/06/12/depth-bias-and-the-power-of-deceiving-yourself/
		case D3DRS_SLOPESCALEDEPTHBIAS:
		{
			// the value in the dword is actually a float
			float	fvalue = *(float*)&Value;
			gl.m_DepthBias.factor = fvalue;

			m_ctx->WriteDepthBias( &gl.m_DepthBias );
			break;
		}

		// Alpha to coverage
		case D3DRS_ADAPTIVETESS_Y:
		{
			gl.m_AlphaToCoverageEnable.enable = Value;
			m_ctx->WriteAlphaToCoverageEnable( &gl.m_AlphaToCoverageEnable );
			break;
		}

		case D3DRS_CLIPPLANEENABLE:		// kGLClipPlaneEnable
		{
			// d3d packs all the enables into one word.
			// we break that out so we don't do N glEnable calls to sync - 
			// GLM is tracking one unique enable per plane.
			for( int i=0; i<kGLMUserClipPlanes; i++)
			{
				gl.m_ClipPlaneEnable[i].enable = (Value & (1<<i)) != 0;
			}

			for( int x=0; x<kGLMUserClipPlanes; x++)
				m_ctx->WriteClipPlaneEnable( &gl.m_ClipPlaneEnable[x], x );
			break;
		}

		//-------------------------------------------------------------------------------------------- polygon/fill mode

		case D3DRS_FILLMODE:
		{
			GLuint mode = 0;
			switch(Value)
			{
				case D3DFILL_POINT:			mode = GL_POINT; break;
				case D3DFILL_WIREFRAME:		mode = GL_LINE; break;
				case D3DFILL_SOLID:			mode = GL_FILL; break;
				default:					DXABSTRACT_BREAK_ON_ERROR(); break;
			}
			gl.m_PolygonMode.values[0] = gl.m_PolygonMode.values[1] = mode;						
			m_ctx->WritePolygonMode( &gl.m_PolygonMode );
			break;
		}

#if GLMDEBUG					
		case D3DRS_MULTISAMPLEANTIALIAS:
		case D3DRS_MULTISAMPLEMASK:
		case D3DRS_FOGCOLOR:
		case D3DRS_FOGTABLEMODE:
		case D3DRS_FOGSTART:
		case D3DRS_FOGEND:
		case D3DRS_FOGDENSITY:
		case D3DRS_RANGEFOGENABLE:
		case D3DRS_FOGVERTEXMODE:
		case D3DRS_COLORWRITEENABLE1:	// kGLColorMaskMultiple
		case D3DRS_COLORWRITEENABLE2:	// kGLColorMaskMultiple
		case D3DRS_COLORWRITEENABLE3:	// kGLColorMaskMultiple
		case D3DRS_SEPARATEALPHABLENDENABLE:
		case D3DRS_BLENDOPALPHA:
		case D3DRS_SRCBLENDALPHA:
		case D3DRS_DESTBLENDALPHA:
		case D3DRS_TWOSIDEDSTENCILMODE:	// -> GL_STENCIL_TEST_TWO_SIDE_EXT... not yet implemented ?
		case D3DRS_CCW_STENCILFAIL:		// GLStencilOp_t
		case D3DRS_CCW_STENCILZFAIL:		// GLStencilOp_t
		case D3DRS_CCW_STENCILPASS:		// GLStencilOp_t
		case D3DRS_CCW_STENCILFUNC:		// GLStencilFunc_t
		case D3DRS_CLIPPING:				// ???? is clipping ever turned off ??
		case D3DRS_LASTPIXEL:
		case D3DRS_DITHERENABLE:
		case D3DRS_SHADEMODE:
		default:
			ignored = 1;
			break;
#endif
	}

#if GLMDEBUG
	if (rsSpew && ignored)
	{
		GLMPRINTF(("-X-  (ignored)"));
	}
#endif

	return S_OK;	
}

HRESULT COpenGLDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	tmZone( TELEMETRY_LEVEL2, TMZF_NONE, "%s", __FUNCTION__ );
	
	Assert( RenderTargetIndex < 4 );

	HRESULT result = S_OK;

	GLMPRINTF(("-F- SetRenderTarget index=%d, surface=%8x (tex=%8x %s)",
		RenderTargetIndex,
		pRenderTarget,
		pRenderTarget ? pRenderTarget->m_tex : NULL,
		pRenderTarget ? pRenderTarget->m_tex->m_layout->m_layoutSummary : ""
	));

	// note that it is OK to pass NULL for pRenderTarget, it implies that you would like to detach any color buffer from that target index
	
	// behaviors...
	// if new surf is same as old surf, no change in refcount, in fact, it's early exit
	IDirect3DSurface9 *oldTarget = m_pRenderTargets[RenderTargetIndex];

	if (pRenderTarget == oldTarget)
	{
		GLMPRINTF(("-F-             --> no change",RenderTargetIndex));
		return S_OK;
	}
			

	// Fix this if porting to x86_64
	if ( m_pRenderTargets[RenderTargetIndex] )


	{











	// we now know that the new surf is not the same as the old surf.
	// you can't assume either one is non NULL here though.
	
		m_pRenderTargets[RenderTargetIndex]->Release( 1, "-A  SetRenderTarget private release" );
	}

	if (pRenderTarget)
	{
		pRenderTarget->AddRef( 1, "+A  SetRenderTarget private addref"  );						// again, private refcount being raised
	}
	m_pRenderTargets[RenderTargetIndex] = pRenderTarget;	
	
	m_bFBODirty = true;

/*
	if (!pRenderTarget)
	{		
		GLMPRINTF(("-F-             --> Setting NULL render target on index=%d ",RenderTargetIndex));
	}
	else
	{
		GLMPRINTF(("-F-             --> attaching index=%d on drawing FBO (%8x)",RenderTargetIndex, m_drawableFBO));
		// attach color to FBO
		GLMFBOTexAttachParams	rtParams;
		memset( &rtParams, 0, sizeof(rtParams) );
		
		rtParams.m_tex		= pRenderTarget->m_tex;
		rtParams.m_face		= pRenderTarget->m_face;
		rtParams.m_mip		= pRenderTarget->m_mip;
		rtParams.m_zslice	= 0;	// FIXME if you ever want to be able to render to slices of a 3D tex..
		
		m_drawableFBO->TexAttach( &rtParams, (EGLMFBOAttachment)(kAttColor0 + RenderTargetIndex) );
	}
*/

#if GL_BATCH_PERF_ANALYSIS && GL_BATCH_PERF_ANALYSIS_WRITE_PNGS
	if ( m_pBatch_vis_bitmap && m_pBatch_vis_bitmap->is_valid() && !RenderTargetIndex )
	{
		m_pBatch_vis_bitmap->fill_box(0, m_nBatchVisY, m_pBatch_vis_bitmap->width(), 1, 30, 20, 20);
		m_nBatchVisY += 1;
	}
#endif

	return result;	
}

HRESULT COpenGLDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
		
	Assert( Sampler < GLM_SAMPLER_COUNT );

	m_ctx->SetSamplerDirty( Sampler );

	switch( Type )
	{
	case D3DSAMP_ADDRESSU:
		m_ctx->SetSamplerAddressU( Sampler, Value );
		break;
	case D3DSAMP_ADDRESSV:
		m_ctx->SetSamplerAddressV( Sampler, Value );
		break;
	case D3DSAMP_ADDRESSW:
		m_ctx->SetSamplerAddressW( Sampler, Value );
		break;
	case D3DSAMP_BORDERCOLOR:
		m_ctx->SetSamplerBorderColor( Sampler, Value );
		break;
	case D3DSAMP_MAGFILTER:
		m_ctx->SetSamplerMagFilter( Sampler, Value );
		break;
	case D3DSAMP_MIPFILTER:	
		m_ctx->SetSamplerMipFilter( Sampler, Value );
		break;
	case D3DSAMP_MINFILTER:	
		m_ctx->SetSamplerMinFilter( Sampler, Value );
		break;
	case D3DSAMP_MIPMAPLODBIAS: 
		m_ctx->SetSamplerMipMapLODBias( Sampler, Value );
		break;		
	case D3DSAMP_MAXMIPLEVEL: 
		m_ctx->SetSamplerMaxMipLevel( Sampler, Value);
		break;
	case D3DSAMP_MAXANISOTROPY: 
		m_ctx->SetSamplerMaxAnisotropy( Sampler, Value);
		break;
	case D3DSAMP_SRGBTEXTURE: 
		//m_samplers[ Sampler ].m_srgb = Value;
		m_ctx->SetSamplerSRGBTexture(Sampler, Value);
		break;
	case D3DSAMP_SHADOWFILTER: 
		m_ctx->SetShadowFilter(Sampler, Value);
		break;

	default: DXABSTRACT_BREAK_ON_ERROR(); break;
	}

	return S_OK;
}

HRESULT COpenGLDevice9::SetScissorRect(const RECT *pRect)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	//int nSurfaceHeight = m_ctx->m_drawingFBO->m_attach[ kAttColor0 ].m_tex->m_layout->m_key.m_ySize;
	
	GLScissorBox_t newScissorBox = { pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top };
	gl.m_ScissorBox	= newScissorBox;
	m_ctx->WriteScissorBox( &gl.m_ScissorBox );
	return S_OK;	
}

HRESULT COpenGLDevice9::SetSoftwareVertexProcessing(BOOL bSoftware){}

HRESULT COpenGLDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	Assert( StreamNumber < D3D_MAX_STREAMS );
	Assert( ( Stride & 3 ) == 0 ); // we support non-DWORD aligned strides, but on some drivers (like AMD's) perf goes off a cliff 

	// perfectly legal to see a vertex buffer of NULL get passed in here.
	// so we need an array to track these.
	// OK, we are being given the stride, we don't need to calc it..
	
	GLMPRINTF(("-X- IDirect3DDevice9::SetStreamSource setting stream #%d to D3D buf %p (GL name %d); offset %d, stride %d", StreamNumber, pStreamData, (pStreamData) ? pStreamData->m_vtxBuffer->m_nHandle: -1, OffsetInBytes, Stride));

	if ( !pStreamData )
	{
		OffsetInBytes = 0;
		Stride = 0;
		
		m_vtx_buffers[ StreamNumber ] = m_pDummy_vtx_buffer;
	}
	else
	{
		// We do not support strides of 0
		Assert( Stride > 0 );
		m_vtx_buffers[ StreamNumber ] = pStreamData->m_vtxBuffer;
	}

	m_streams[ StreamNumber ].m_vtxBuffer = pStreamData;
	m_streams[ StreamNumber ].m_offset	= OffsetInBytes;
	m_streams[ StreamNumber ].m_stride	= Stride;
		
	return S_OK;
}

HRESULT COpenGLDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter){}

HRESULT COpenGLDevice9::SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	Assert( Stage < GLM_SAMPLER_COUNT );
	m_textures[Stage] = pTexture;
	m_ctx->SetSamplerTex( Stage, pTexture ? pTexture->m_tex : NULL );
	return S_OK;	
}

HRESULT COpenGLDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}

HRESULT COpenGLDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	m_pVertDecl = pDecl;
	return S_OK;	
}

HRESULT COpenGLDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	m_ctx->SetVertexProgram( pShader ? pShader->m_vtxProgram : NULL );
	m_vertexShader = pShader;
	return S_OK;
}

HRESULT COpenGLDevice9::SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	TOGL_NULL_DEVICE_CHECK;
	m_ctx->SetProgramParametersB( kGLMVertexProgram, StartRegister, (int *)pConstantData, BoolCount );
	return S_OK;	
}

HRESULT COpenGLDevice9::SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	TOGL_NULL_DEVICE_CHECK;
	m_ctx->SetProgramParametersF( kGLMVertexProgram, StartRegister, (float *)pConstantData, Vector4fCount );
	return S_OK;
}

HRESULT COpenGLDevice9::SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	TOGL_NULL_DEVICE_CHECK;
	m_ctx->SetProgramParametersI( kGLMVertexProgram, StartRegister, (int *)pConstantData, Vector4iCount );
	return S_OK;	
}

HRESULT COpenGLDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	GLMPRINTF(("-X- IDirect3DDevice9::SetViewport : minZ %f, maxZ %f",pViewport->MinZ, pViewport->MaxZ ));
	
	gl.m_ViewportBox.x		= pViewport->X;
	gl.m_ViewportBox.width	= pViewport->Width;

	gl.m_ViewportBox.y		= pViewport->Y;
	gl.m_ViewportBox.height	= pViewport->Height;	

	gl.m_ViewportBox.widthheight = pViewport->Width | ( pViewport->Height << 16 );

	m_ctx->WriteViewportBox( &gl.m_ViewportBox );

	gl.m_ViewportDepthRange.flNear	=	pViewport->MinZ;
	gl.m_ViewportDepthRange.flFar	=	pViewport->MaxZ;
	m_ctx->WriteViewportDepthRange( &gl.m_ViewportDepthRange );

	return S_OK;	
}

BOOL COpenGLDevice9::ShowCursor(BOOL bShow)
{
	return TRUE;	
}

HRESULT COpenGLDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	// find relevant slices in GLM tex

	if ( m_bFBODirty )
	{
		UpdateBoundFBO();
	}
		
	CGLMTex	*srcTex = pSourceSurface->m_tex;
	int srcSliceIndex = srcTex->CalcSliceIndex( pSourceSurface->m_face, pSourceSurface->m_mip );
	GLMTexLayoutSlice *srcSlice = &srcTex->m_layout->m_slices[ srcSliceIndex ];

	CGLMTex	*dstTex = pDestSurface->m_tex;
	int dstSliceIndex = dstTex->CalcSliceIndex( pDestSurface->m_face, pDestSurface->m_mip );
	GLMTexLayoutSlice *dstSlice = &dstTex->m_layout->m_slices[ dstSliceIndex ];

	if ( dstTex->m_rboName != 0 )
	{
		Assert(!"No path yet for blitting into an MSAA tex");
		return S_OK;
	}

	bool useFastBlit = (gl_blitmode.GetInt() != 0);
	
	if ( !useFastBlit && (srcTex->m_rboName !=0))		// old way, we do a resolve to scratch tex first (necessitating two step blit)
	{
		m_ctx->ResolveTex( srcTex, true );
	}

	// set up source/dest rect in GLM form
	GLMRect	srcRect, dstRect;

	// d3d nomenclature:
	// Y=0 is the visual top and also aligned with V=0.

	srcRect.xmin	=	pSourceRect		?	pSourceRect->left	:	0;
	srcRect.xmax	=	pSourceRect		?	pSourceRect->right	:	srcSlice->m_xSize;
	srcRect.ymin	=	pSourceRect		?	pSourceRect->top	:	0;
	srcRect.ymax	=	pSourceRect		?	pSourceRect->bottom	:	srcSlice->m_ySize;

	dstRect.xmin	=	pDestRect		?	pDestRect->left		:	0;
	dstRect.xmax	=	pDestRect		?	pDestRect->right	:	dstSlice->m_xSize;
	dstRect.ymin	=	pDestRect		?	pDestRect->top		:	0;
	dstRect.ymax	=	pDestRect		?	pDestRect->bottom	:	dstSlice->m_ySize;

	GLenum filterGL = 0;
	switch(Filter)
	{
		case	D3DTEXF_NONE:
		case	D3DTEXF_POINT:
			filterGL = GL_NEAREST;
		break;
		
		case	D3DTEXF_LINEAR:
			filterGL = GL_LINEAR;
		break;
		
		default:			// D3DTEXF_ANISOTROPIC
			Assert(!"Impl aniso stretch");
		break;
	}
	
	if (useFastBlit)
	{
		m_ctx->Blit2(		srcTex, &srcRect, pSourceSurface->m_face, pSourceSurface->m_mip, 
							dstTex, &dstRect, pDestSurface->m_face, pDestSurface->m_mip, 
							filterGL
					);
	}
	else
	{
		m_ctx->BlitTex(		srcTex, &srcRect, pSourceSurface->m_face, pSourceSurface->m_mip, 
							dstTex, &dstRect, pDestSurface->m_face, pDestSurface->m_mip, 
							filterGL
					);
	}
						
	return S_OK;	
}

HRESULT COpenGLDevice9::TestCooperativeLevel()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	// game calls this to see if device was lost.
	// last I checked the device was still attached to the computer.
	// so, return OK.

	return S_OK;	
}

HRESULT COpenGLDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint){}

HRESULT COpenGLDevice9::ValidateDevice(DWORD *pNumPasses)
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( this );
	DXABSTRACT_BREAK_ON_ERROR();
	return S_OK;	
}
