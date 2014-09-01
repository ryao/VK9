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
 
#include "COpenGL9.h"

static void FillD3DCaps9( const GLMRendererInfoFields &glmRendererInfo, D3DCAPS9* pCaps )
{
	// fill in the pCaps record for adapter... we zero most of it and just fill in the fields that we think the caller wants.
	Q_memset( pCaps, 0, sizeof(*pCaps) );


	/* Device Info */
	pCaps->DeviceType					=	D3DDEVTYPE_HAL;

	/* Caps from DX7 Draw */
	pCaps->Caps							=	0;									// does anyone look at this ?

	pCaps->Caps2						=	D3DCAPS2_DYNAMICTEXTURES;    
	/* Cursor Caps */
	pCaps->CursorCaps					=	0;									// nobody looks at this

	/* 3D Device Caps */
	pCaps->DevCaps						=	D3DDEVCAPS_HWTRANSFORMANDLIGHT;

	pCaps->TextureCaps					=	D3DPTEXTURECAPS_CUBEMAP | D3DPTEXTURECAPS_MIPCUBEMAP | D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_PROJECTED;
	// D3DPTEXTURECAPS_NOPROJECTEDBUMPENV ?
	// D3DPTEXTURECAPS_POW2 ? 
	// caller looks at POT support like this:
	//		pCaps->m_SupportsNonPow2Textures = 
	//			( !( caps.TextureCaps & D3DPTEXTURECAPS_POW2 ) || 
	//			( caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL ) );
	// so we should set D3DPTEXTURECAPS_NONPOW2CONDITIONAL bit ?


	pCaps->PrimitiveMiscCaps			=	0;									//only the HDR setup looks at this for D3DPMISCCAPS_SEPARATEALPHABLEND.
	// ? D3DPMISCCAPS_SEPARATEALPHABLEND 
	// ? D3DPMISCCAPS_BLENDOP
	// ? D3DPMISCCAPS_CLIPPLANESCALEDPOINTS
	// ? D3DPMISCCAPS_CLIPTLVERTS D3DPMISCCAPS_COLORWRITEENABLE D3DPMISCCAPS_MASKZ D3DPMISCCAPS_TSSARGTEMP


	pCaps->RasterCaps					=	D3DPRASTERCAPS_SCISSORTEST
		|	D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS	// ref'd in CShaderDeviceMgrDx8::ComputeCapsFromD3D
		|	D3DPRASTERCAPS_DEPTHBIAS			// ref'd in CShaderDeviceMgrDx8::ComputeCapsFromD3D
		;

	pCaps->TextureFilterCaps			=	D3DPTFILTERCAPS_MINFANISOTROPIC | D3DPTFILTERCAPS_MAGFANISOTROPIC;

	pCaps->MaxTextureWidth				=	4096;
	pCaps->MaxTextureHeight				=	4096;
	pCaps->MaxVolumeExtent				=	1024;	//guesses

	pCaps->MaxTextureAspectRatio		=	0;		// imply no limit on AR

	pCaps->MaxAnisotropy				=	glmRendererInfo.m_maxAniso;

	pCaps->TextureOpCaps				=	D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_MODULATE2X;	//guess
	//DWORD   MaxTextureBlendStages;
	//DWORD   MaxSimultaneousTextures;

	pCaps->VertexProcessingCaps			=	D3DVTXPCAPS_TEXGEN_SPHEREMAP;

	pCaps->MaxActiveLights				=	8;		// guess


	// MaxUserClipPlanes.  A bit complicated..
	// it's difficult to make this fluid without teaching the engine about a cap that could change during run.

	// start it out set to '2'.
	// turn it off, if we're in GLSL mode but do not have native clip plane capability.
	pCaps->MaxUserClipPlanes			=	2;		// assume good news

	// is user asking for it to be off ?
	if ( CommandLine()->CheckParm( "-nouserclip" ) )
	{
		pCaps->MaxUserClipPlanes		=	0;
	}

	pCaps->MaxVertexBlendMatrices		=	0;		// see if anyone cares
	pCaps->MaxVertexBlendMatrixIndex	=	0;		// see if anyone cares

	pCaps->MaxPrimitiveCount			=	32768;	// guess
	pCaps->MaxStreams					=	D3D_MAX_STREAMS;		// guess

	pCaps->VertexShaderVersion			=	0x300;	// model 3.0
	pCaps->MaxVertexShaderConst			=	DXABSTRACT_VS_PARAM_SLOTS;	// number of vertex shader constant registers

	pCaps->PixelShaderVersion			=	0x300;	// model 3.0

	// Here are the DX9 specific ones
	pCaps->DevCaps2						=	D3DDEVCAPS2_STREAMOFFSET;

	pCaps->PS20Caps.NumInstructionSlots	=	512;	// guess
	// only examined once:
	// pCaps->m_SupportsPixelShaders_2_b = ( ( caps.PixelShaderVersion & 0xffff ) >= 0x0200) && (caps.PS20Caps.NumInstructionSlots >= 512);
	//pCaps->m_SupportsPixelShaders_2_b = 1;

	pCaps->NumSimultaneousRTs					=	1;         // Will be at least 1
	pCaps->MaxVertexShader30InstructionSlots	=	0; 
	pCaps->MaxPixelShader30InstructionSlots		=	0;

#if DX_TO_GL_ABSTRACTION
	pCaps->FakeSRGBWrite			=	!glmRendererInfo.m_hasGammaWrites;
	pCaps->CanDoSRGBReadFromRTs		=	!glmRendererInfo.m_cantAttachSRGB;
	pCaps->MixedSizeTargets			=	glmRendererInfo.m_hasMixedAttachmentSizes;
#endif
}

COpenGL9::COpenGL9()
{
}

COpenGL9::~COpenGL9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF(("-A-  ~IDirect3D9 - signpost"));	
}

HRESULT COpenGL9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF(("-X- IDirect3D9::CheckDepthStencilMatch:    Adapter=%d || DevType=%d:%s || AdapterFormat=%d:%s || RenderTargetFormat=%d:%s || DepthStencilFormat=%d:%s",
		Adapter,
		DeviceType, GLMDecode(eD3D_DEVTYPE,DeviceType),
		AdapterFormat, GLMDecode(eD3D_FORMAT, AdapterFormat),
		RenderTargetFormat, GLMDecode(eD3D_FORMAT, RenderTargetFormat),
		DepthStencilFormat, GLMDecode(eD3D_FORMAT, DepthStencilFormat) ));
	
	// one known request looks like this:
	// AdapterFormat=5:D3DFMT_X8R8G8B8 || RenderTargetFormat=3:D3DFMT_A8R8G8B8 || DepthStencilFormat=2:D3DFMT_D24S8
	
	// return S_OK for that one combo, DXABSTRACT_BREAK_ON_ERROR() on anything else
	HRESULT result = D3DERR_NOTAVAILABLE;	// failure
	
	switch( AdapterFormat )
	{
		case	D3DFMT_X8R8G8B8:
		{
			if ( (RenderTargetFormat == D3DFMT_A8R8G8B8) && (DepthStencilFormat == D3DFMT_D24S8) )
			{
				result = S_OK;
			}
		}
		break;
	}
	
	Assert( result == S_OK );

	return result;	
}

HRESULT COpenGL9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	GL_BATCH_PERF_CALL_TIMER;
	if (0)	// hush for now, less spew
	{
		GLMPRINTF(("-X- ** IDirect3D9::CheckDeviceFormat:  \n -- Adapter=%d || DeviceType=%4x:%s || AdapterFormat=%8x:%s\n -- RType       %8x: %s\n -- CheckFormat %8x: %s\n -- Usage       %8x: %s",
			Adapter,													
			DeviceType,		GLMDecode(eD3D_DEVTYPE, DeviceType),				
			AdapterFormat,	GLMDecode(eD3D_FORMAT, AdapterFormat),
			RType,			GLMDecode(eD3D_RTYPE, RType),							
			CheckFormat,	GLMDecode(eD3D_FORMAT, CheckFormat),
			Usage,			GLMDecodeMask( eD3D_USAGE, Usage ) ));			
	}

	HRESULT result = D3DERR_NOTAVAILABLE;	// failure
	
	DWORD	knownUsageMask =	D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP
							|	D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_FILTER | D3DUSAGE_QUERY_SRGBWRITE | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING
							|	D3DUSAGE_QUERY_VERTEXTEXTURE;
	(void)knownUsageMask;

	//	FramebufferSRGB stuff.
	//	basically a format is only allowed to have SRGB usage for writing, if you have the framebuffer SRGB extension.
	//	so, check for that capability with GLM adapter db, and if it's not there, don't mark that bit as usable in any of our formats.
	GLMDisplayDB *db = GetDisplayDB();
	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;
	
	GLMRendererInfoFields	glmRendererInfo;
	GLMDisplayInfoFields	glmDisplayInfo;
	
	bool dbresult = db->GetFakeAdapterInfo( Adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo ); (void)dbresult;
	Assert (!dbresult);

	Assert ((Usage & knownUsageMask) == Usage);

	DWORD	legalUsage = 0;
	switch( AdapterFormat )
	{
		case	D3DFMT_X8R8G8B8:
			switch( RType )
			{
				case	D3DRTYPE_TEXTURE:
					switch( CheckFormat )
					{
						case D3DFMT_DXT1:
						case D3DFMT_DXT3:
						case D3DFMT_DXT5:
													legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage	|=	D3DUSAGE_QUERY_SRGBREAD;
													
													//open question: is auto gen of mipmaps is allowed or attempted on any DXT textures.
						break;

						case D3DFMT_A8R8G8B8:		legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage |=	D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING;
						break;

						case D3DFMT_A2R10G10B10:	legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage |=	D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING;
						break;

						case D3DFMT_A2B10G10R10:	legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage |=	D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING;
						break;

						case D3DFMT_R32F:			legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage |=	D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING;
						break;

						case D3DFMT_A16B16G16R16:
													legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage |=	D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING;
						break;

						case D3DFMT_A16B16G16R16F:	legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE;

													if ( !glmRendererInfo.m_atiR5xx )
													{
														legalUsage |= D3DUSAGE_QUERY_FILTER | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING;
													}
						break;

						case D3DFMT_A32B32G32R32F:	legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE;

													if ( !glmRendererInfo.m_atiR5xx && !glmRendererInfo.m_nvG7x )
													{
														legalUsage |= D3DUSAGE_QUERY_FILTER | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING;
													}
						break;

						case D3DFMT_R5G6B5:			legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;

						//-----------------------------------------------------------
						// these come in from TestTextureFormat in ColorFormatDX8.cpp which is being driven by InitializeColorInformation...
						// which is going to try all 8 combinations of (vertex texturable / render targetable / filterable ) on every image format it knows.

						case D3DFMT_R8G8B8:			legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage	|=	D3DUSAGE_QUERY_SRGBREAD;
						break;
												
						case D3DFMT_X8R8G8B8:		legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
													legalUsage	|=	D3DUSAGE_QUERY_SRGBREAD | D3DUSAGE_QUERY_SRGBWRITE;
						break;

							// one and two channel textures... we'll have to fake these as four channel tex if we want to support them
						case D3DFMT_L8:				legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;

						case D3DFMT_A8L8:			legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;

						case D3DFMT_A8:				legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;
						
							// going to need to go back and double check all of these..
						case D3DFMT_X1R5G5B5:		legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;
						
						case D3DFMT_A4R4G4B4:		legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;

						case D3DFMT_A1R5G5B5:		legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;
						
						case D3DFMT_V8U8:			legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
						break;

						case D3DFMT_Q8W8V8U8:		legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
							// what the heck is QWVU8 ... ?
						break;

						case D3DFMT_X8L8V8U8:		legalUsage	=	D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_QUERY_FILTER;
							// what the heck is XLVU8 ... ?
						break;

							// formats with depth...
							
						case	D3DFMT_D16:			legalUsage =	D3DUSAGE_DYNAMIC | D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL;
							// just a guess on the legal usages
						break;

						case	D3DFMT_D24S8:		legalUsage =	D3DUSAGE_DYNAMIC | D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL;
							// just a guess on the legal usages
						break;

							// vendor formats... try marking these all invalid for now
						case	D3DFMT_NV_INTZ:
						case	D3DFMT_NV_RAWZ:
						case	D3DFMT_NV_NULL:
						case	D3DFMT_ATI_D16:
						case	D3DFMT_ATI_D24S8:
						case	D3DFMT_ATI_2N:
						case	D3DFMT_ATI_1N:
							legalUsage = 0;
						break;

						//-----------------------------------------------------------
												
						default:
							Assert(!"Unknown check format");
							result = D3DERR_NOTAVAILABLE;
						break;
					}

					if ((Usage & legalUsage) == Usage)
					{
						result = S_OK;
					}
					else
					{
						DWORD unsatBits = Usage & (~legalUsage);	// clear the bits of the req that were legal, leaving the illegal ones
						unsatBits;
						GLMPRINTF(( "-X- --> NOT OK: flags %8x:%s", unsatBits,GLMDecodeMask( eD3D_USAGE, unsatBits ) ));
						result = D3DERR_NOTAVAILABLE;
					}
				break;				
				
				case	D3DRTYPE_SURFACE:
					switch( static_cast<uint>(CheckFormat) )
					{
						case	0x434f5441:
						case	0x41415353:
							result = D3DERR_NOTAVAILABLE;
						break;
							
						case	D3DFMT_D24S8:
							result = S_OK;
						break;
						//** IDirect3D9::CheckDeviceFormat  adapter=0, DeviceType=   1:D3DDEVTYPE_HAL, AdapterFormat=       5:D3DFMT_X8R8G8B8, RType=   1:D3DRTYPE_SURFACE, CheckFormat=434f5441:UNKNOWN
						//** IDirect3D9::CheckDeviceFormat  adapter=0, DeviceType=   1:D3DDEVTYPE_HAL, AdapterFormat=       5:D3DFMT_X8R8G8B8, RType=   1:D3DRTYPE_SURFACE, CheckFormat=41415353:UNKNOWN
						//** IDirect3D9::CheckDeviceFormat  adapter=0, DeviceType=   1:D3DDEVTYPE_HAL, AdapterFormat=       5:D3DFMT_X8R8G8B8, RType=   1:D3DRTYPE_SURFACE, CheckFormat=434f5441:UNKNOWN
						//** IDirect3D9::CheckDeviceFormat  adapter=0, DeviceType=   1:D3DDEVTYPE_HAL, AdapterFormat=       5:D3DFMT_X8R8G8B8, RType=   1:D3DRTYPE_SURFACE, CheckFormat=41415353:UNKNOWN
					}
				break;
				
				default:
					Assert(!"Unknown resource type");
					result = D3DERR_NOTAVAILABLE;
				break;
			}
		break;
		
		default:
			Assert(!"Unknown adapter format");
			result = D3DERR_NOTAVAILABLE;
		break;
	}
	
	return result;	
}

/*
HRESULT COpenGL9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	
}
*/

HRESULT COpenGL9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels)
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMDisplayDB *db = GetDisplayDB();

	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;
	
	GLMRendererInfoFields		glmRendererInfo;
	GLMDisplayInfoFields		glmDisplayInfo;
	//GLMDisplayModeInfoFields	glmModeInfo;
	
	// the D3D "Adapter" number feeds the fake adapter index
	bool result = db->GetFakeAdapterInfo( Adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo );
	Assert( !result );
	if ( result )
		return D3DERR_INVALIDCALL;

	
	if ( !CommandLine()->FindParm("-glmenabletrustmsaa") )
	{
		// These ghetto drivers don't get MSAA
		if ( ( glmRendererInfo.m_nvG7x || glmRendererInfo.m_atiR5xx ) && ( MultiSampleType > D3DMULTISAMPLE_NONE ) )
		{
			if ( pQualityLevels )
			{
				*pQualityLevels = 0;
			}
			return D3DERR_NOTAVAILABLE;
		}
	}

	switch ( MultiSampleType )
	{
		case D3DMULTISAMPLE_NONE:		// always return true
			if ( pQualityLevels )
			{
				*pQualityLevels = 1;
			}
			return S_OK;
		break;

		case D3DMULTISAMPLE_2_SAMPLES:
		case D3DMULTISAMPLE_4_SAMPLES:
		case D3DMULTISAMPLE_6_SAMPLES:
		case D3DMULTISAMPLE_8_SAMPLES:
			// note the fact that the d3d enums for 2, 4, 6, 8 samples are equal to 2,4,6,8...
			if (glmRendererInfo.m_maxSamples >= (int)MultiSampleType )
			{
				if ( pQualityLevels )
				{
					*pQualityLevels = 1;
				}
				return S_OK;
			}
			else
			{
				return D3DERR_NOTAVAILABLE;
			}
		break;
		
		default:
			if ( pQualityLevels )
			{
				*pQualityLevels = 0;
			}
			return D3DERR_NOTAVAILABLE;
		break;
	}
	return D3DERR_NOTAVAILABLE;	
}

HRESULT COpenGL9::CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)
{
	GL_BATCH_PERF_CALL_TIMER;
	//FIXME: we just say "OK" on any query

	GLMPRINTF(( "-X- IDirect3D9::CheckDeviceType:    Adapter=%d || DevType=%d:%s || AdapterFormat=%d:%s || BackBufferFormat=%d:%s || bWindowed=%d",
		Adapter,
		DevType, GLMDecode(eD3D_DEVTYPE,DevType),
		AdapterFormat, GLMDecode(eD3D_FORMAT, AdapterFormat),
		BackBufferFormat, GLMDecode(eD3D_FORMAT, BackBufferFormat),
		(int) bWindowed ));
		
	return S_OK;	
}

HRESULT COpenGL9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	GL_BATCH_PERF_CALL_TIMER;

#if GLMDEBUG
	Plat_DebugString( "WARNING: GLMEBUG is 1, perf. is going to be low!");
	Warning( "WARNING: GLMEBUG is 1, perf. is going to be low!");
#endif
#if !TOGL_SUPPORT_NULL_DEVICE	
	if (DeviceType == D3DDEVTYPE_NULLREF)
	{
		Error("Must define TOGL_SUPPORT_NULL_DEVICE	to use the NULL device");
		DebuggerBreak();
		return E_FAIL;
	}
#endif
	
	// constrain these inputs for the time being
	// BackBufferFormat			-> A8R8G8B8
	// BackBufferCount			-> 1;
	// MultiSampleType			-> D3DMULTISAMPLE_NONE
	// AutoDepthStencilFormat	-> D3DFMT_D24S8
	
	// NULL out the return pointer so if we exit early it is not set
	*ppReturnedDeviceInterface = NULL;
	
	// assume success unless something is sour
	HRESULT result = S_OK;
	
	// relax this check for now
	//if (pPresentationParameters->BackBufferFormat != D3DFMT_A8R8G8B8)
	//{
	//	DXABSTRACT_BREAK_ON_ERROR();
	//	result = -1;
	//}
	
	//rbarris 24Aug10 - relaxing this check - we don't care if the game asks for two backbuffers, it's moot
	//if ( pPresentationParameters->BackBufferCount != 1 )
	//{
	//	DXABSTRACT_BREAK_ON_ERROR();
	//	result = D3DERR_NOTAVAILABLE;
	//}
		
	if ( pPresentationParameters->AutoDepthStencilFormat != D3DFMT_D24S8 )
	{
		DXABSTRACT_BREAK_ON_ERROR();
		result = D3DERR_NOTAVAILABLE;
	}

	if ( result == S_OK )
	{
		// create an IDirect3DDevice9
		// it will make a GLMContext and set up some drawables

		IDirect3DDevice9Params	devparams;
		memset( &devparams, 0, sizeof(devparams) );
		
		devparams.m_adapter					= Adapter;
		devparams.m_deviceType				= DeviceType;
		devparams.m_focusWindow				= hFocusWindow;				// is this meaningful?  is this a WindowRef ?  follow it up the chain..
		devparams.m_behaviorFlags			= BehaviorFlags;
		devparams.m_presentationParameters	= *pPresentationParameters;

		IDirect3DDevice9 *dev = new IDirect3DDevice9;
		
		result = dev->Create( &devparams );
		
		if ( result == S_OK )
		{
			*ppReturnedDeviceInterface = dev;
		}
		
		g_bNullD3DDevice = ( DeviceType == D3DDEVTYPE_NULLREF );
	}
	return result;	
}

HRESULT COpenGL9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode)
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF(( "-X- IDirect3D9::EnumAdapterModes:    Adapter=%d || Format=%8x:%s || Mode=%d", Adapter, Format, GLMDecode(eD3D_FORMAT, Format), Mode ));

	Assert(Format==D3DFMT_X8R8G8B8);

	GLMDisplayDB *db = GetDisplayDB();
	
	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;
	
	GLMRendererInfoFields		glmRendererInfo;
	GLMDisplayInfoFields		glmDisplayInfo;
	GLMDisplayModeInfoFields	glmModeInfo;
	
	// the D3D "Adapter" number feeds the fake adapter index
	bool result = db->GetFakeAdapterInfo( Adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo );
	Assert (!result);
		if (result) return D3DERR_NOTAVAILABLE;

	bool result2 = db->GetModeInfo( glmRendererIndex, glmDisplayIndex, Mode, &glmModeInfo );
	Assert( !result2 );
		if (result2) return D3DERR_NOTAVAILABLE;

	pMode->Width		= glmModeInfo.m_modePixelWidth;
	pMode->Height		= glmModeInfo.m_modePixelHeight;
	pMode->RefreshRate	= glmModeInfo.m_modeRefreshHz;		// "adapter default"
	pMode->Format		= Format;							// whatever you asked for ?
	
	GLMPRINTF(( "-X- IDirect3D9::EnumAdapterModes returning mode size (%d,%d) and D3DFMT_X8R8G8B8",pMode->Width,pMode->Height ));
	return S_OK;		
}

UINT COpenGL9::GetAdapterCount()
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMgr::NewGLMgr();				// init GL manager

	GLMDisplayDB *db = GetDisplayDB();
	int dxAdapterCount = db->GetFakeAdapterCount();

	return dxAdapterCount;	
}

HRESULT COpenGL9::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode)
{
	GL_BATCH_PERF_CALL_TIMER;
	// asking what the current mode is
	GLMPRINTF(("-X- IDirect3D9::GetAdapterDisplayMode: Adapter=%d", Adapter ));

	GLMDisplayDB *db = GetDisplayDB();

	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;
	
	GLMRendererInfoFields		glmRendererInfo;
	GLMDisplayInfoFields		glmDisplayInfo;
	GLMDisplayModeInfoFields	glmModeInfo;
	
	// the D3D "Adapter" number feeds the fake adapter index
	bool result = db->GetFakeAdapterInfo( Adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo );
	Assert(!result);
		if (result)	return D3DERR_INVALIDCALL;

	int modeIndex = -1;	// pass -1 as a mode index to find out about whatever the current mode is on the selected display

	bool modeResult = db->GetModeInfo( glmRendererIndex, glmDisplayIndex, modeIndex, &glmModeInfo );
	Assert (!modeResult);
		if (modeResult)	return D3DERR_INVALIDCALL;

	pMode->Width		= glmModeInfo.m_modePixelWidth;
	pMode->Height		= glmModeInfo.m_modePixelHeight;
	pMode->RefreshRate	= glmModeInfo.m_modeRefreshHz;		// "adapter default"
	pMode->Format		= D3DFMT_X8R8G8B8;					//FIXME, this is a SWAG

	return S_OK;	
}

HRESULT COpenGL9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
	GL_BATCH_PERF_CALL_TIMER;
	// Generally called from "CShaderDeviceMgrDx8::ComputeCapsFromD3D" in ShaderDeviceDX8.cpp

	Assert( Flags == D3DENUM_WHQL_LEVEL );	// we're not handling any other queries than this yet
	
	Q_memset( pIdentifier, 0, sizeof(*pIdentifier) );

	GLMDisplayDB *db = GetDisplayDB();
	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;
	
	GLMRendererInfoFields	glmRendererInfo;
	GLMDisplayInfoFields	glmDisplayInfo;
	
	// the D3D "Adapter" number feeds the fake adapter index
	bool result = db->GetFakeAdapterInfo( Adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo ); (void)result;
	Assert (!result);

	if( glmRendererInfo.m_rendererID )
	{
		const char *pRenderer = GLMDecode( eGL_RENDERER, glmRendererInfo.m_rendererID & 0x00FFFF00 );

		Q_snprintf( pIdentifier->Driver, sizeof(pIdentifier->Driver), "OpenGL %s (%08x)",
			pRenderer, glmRendererInfo.m_rendererID	);

		Q_snprintf( pIdentifier->Description, sizeof(pIdentifier->Description), "%s - %dx%d - %dMB VRAM",
			pRenderer,
			glmDisplayInfo.m_displayPixelWidth, glmDisplayInfo.m_displayPixelHeight,
			glmRendererInfo.m_vidMemory >> 20 );
	}
	else
	{
		static CDynamicFunctionOpenGL< true, const GLubyte *( APIENTRY *)(GLenum name), const GLubyte * > glGetString("glGetString");

		const char *pszStringVendor = ( const char * )glGetString( GL_VENDOR );		// NVIDIA Corporation
		const char *pszStringRenderer = ( const char * )glGetString( GL_RENDERER );   // GeForce GTX 680/PCIe/SSE2
		const char *pszStringVersion = ( const char * )glGetString( GL_VERSION );     // 4.2.0 NVIDIA 304.22

		Q_snprintf( pIdentifier->Driver, sizeof( pIdentifier->Driver ), "OpenGL %s (%s)",
			pszStringVendor, pszStringRenderer );
		Q_snprintf( pIdentifier->Description, sizeof( pIdentifier->Description ), "%s (%s) %s - %dx%d",
			pszStringVendor, pszStringRenderer, pszStringVersion,
			glmDisplayInfo.m_displayPixelWidth, glmDisplayInfo.m_displayPixelHeight );
	}

	pIdentifier->VendorId				= glmRendererInfo.m_pciVendorID;	// 4318;
	pIdentifier->DeviceId				= glmRendererInfo.m_pciDeviceID;	// 401;
	pIdentifier->SubSysId				= 0;								// 3358668866;
	pIdentifier->Revision				= 0;								// 162;
	pIdentifier->VideoMemory			= glmRendererInfo.m_vidMemory;		// amount of video memory in bytes

	#if 0
		// this came from the shaderapigl effort	
		Q_strncpy( pIdentifier->Driver, "Fake-Video-Card", MAX_DEVICE_IDENTIFIER_STRING );
		Q_strncpy( pIdentifier->Description, "Fake-Video-Card", MAX_DEVICE_IDENTIFIER_STRING );
		pIdentifier->VendorId				= 4318;
		pIdentifier->DeviceId				= 401;
		pIdentifier->SubSysId				= 3358668866;
		pIdentifier->Revision				= 162;
	#endif
	
	return S_OK;	
}

UINT COpenGL9::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format)
{
	GL_BATCH_PERF_CALL_TIMER;
	GLMPRINTF(( "-X- IDirect3D9::GetAdapterModeCount: Adapter=%d || Format=%8x:%s", Adapter, Format, GLMDecode(eD3D_FORMAT, Format) ));

	uint modeCount=0;
	
	GLMDisplayDB *db = GetDisplayDB();
	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;
	
	GLMRendererInfoFields	glmRendererInfo;
	GLMDisplayInfoFields	glmDisplayInfo;
	
	// the D3D "Adapter" number feeds the fake adapter index
	bool result = db->GetFakeAdapterInfo( Adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo ); (void)result;
	Assert (!result);

	modeCount = db->GetModeCount( glmRendererIndex, glmDisplayIndex );
	GLMPRINTF(( "-X-   --> result is %d", modeCount ));		
	
	return modeCount;	
}

/*
HMONITOR COpenGL9::GetAdapterMonitor(UINT Adapter)
{
	
}
*/

HRESULT COpenGL9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	GL_BATCH_PERF_CALL_TIMER;
	// Generally called from "CShaderDeviceMgrDx8::ComputeCapsFromD3D" in ShaderDeviceDX8.cpp

	// "Adapter" is used to index amongst the set of fake-adapters maintained in the display DB
	GLMDisplayDB *db = GetDisplayDB();
	int glmRendererIndex = -1;
	int glmDisplayIndex = -1;
	
	GLMRendererInfoFields	glmRendererInfo;
	GLMDisplayInfoFields	glmDisplayInfo;
	
	bool result = db->GetFakeAdapterInfo( Adapter, &glmRendererIndex, &glmDisplayIndex, &glmRendererInfo, &glmDisplayInfo ); (void)result;
	Assert (!result);
	// just leave glmRendererInfo filled out for subsequent code to look at as needed.

	FillD3DCaps9( glmRendererInfo, pCaps );

	return S_OK;	
}

/*
HRESULT COpenGL9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	
}
*/