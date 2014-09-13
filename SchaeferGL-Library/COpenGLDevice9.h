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
 
#ifndef COPENGLDEVICE9_H
#define COPENGLDEVICE9_H

#include "COpenGLTypes.h"

#include "d3d9.h" // Base class: IDirect3DDevice9
#include "COpenGLUnknown.h"

#include "COpenGLVertexDeclaration9.h"
#include "COpenGLSurface9.h"

#include "COpenGLPixelShader9.h"
#include "COpenGLVertexShader9.h"
#include "COpenGLVertexBuffer9.h"
#include "COpenGLIndexBuffer9.h"
#include "COpenGLQuery9.h"
#include "COpenGLBaseTexture9.h"

void	d3drect_to_glmbox( D3DRECT *src, GLScissorBox_t *dst );

void	UnpackD3DRSITable( void );

	struct ObjectStats_t
	{
		int						m_nTotalFBOs;
		int						m_nTotalVertexShaders;
		int						m_nTotalPixelShaders;
		int						m_nTotalVertexDecls;
		int						m_nTotalIndexBuffers;
		int						m_nTotalVertexBuffers;
		int						m_nTotalRenderTargets;
		int						m_nTotalTextures;
		int						m_nTotalSurfaces;
		int						m_nTotalQueries;

		void clear() { V_memset( this, 0, sizeof(* this ) ); }

		ObjectStats_t &operator -= ( const ObjectStats_t &rhs ) 
		{
			m_nTotalFBOs -= rhs.m_nTotalFBOs;
			m_nTotalVertexShaders -= rhs.m_nTotalVertexShaders;
			m_nTotalPixelShaders -= rhs.m_nTotalPixelShaders;
			m_nTotalVertexDecls -= rhs.m_nTotalVertexDecls;
			m_nTotalIndexBuffers -= rhs.m_nTotalIndexBuffers;
			m_nTotalVertexBuffers -= rhs.m_nTotalVertexBuffers;
			m_nTotalRenderTargets -= rhs.m_nTotalRenderTargets;
			m_nTotalTextures -= rhs.m_nTotalTextures;
			m_nTotalSurfaces -= rhs.m_nTotalSurfaces;
			m_nTotalQueries -= m_nTotalQueries;
			return *this;
		}
	};

D3D_RSINFO	g_D3DRS_INFO_packed[] = 
{
	// these do not have to be in any particular order.  they get unpacked into the empty array above for direct indexing.

	D3D_RSI(	3,	D3DRS_ZENABLE,						DONT_KNOW_YET			),	// enable Z test (or W buffering)
	D3D_RSI(	3,	D3DRS_ZWRITEENABLE,					DONT_KNOW_YET			),	// enable Z write
	D3D_RSI(	3,	D3DRS_ZFUNC,						DONT_KNOW_YET			),	// select Z func

	D3D_RSI(	3,	D3DRS_COLORWRITEENABLE,				TRUE					),	// see transitiontable.cpp "APPLY_RENDER_STATE_FUNC( D3DRS_COLORWRITEENABLE, ColorWriteEnable )"

	D3D_RSI(	3,	D3DRS_CULLMODE,						D3DCULL_CCW				),	// backface cull control

	D3D_RSI(	3,	D3DRS_ALPHABLENDENABLE,				DONT_KNOW_YET			),	// ->CTransitionTable::ApplySeparateAlphaBlend and ApplyAlphaBlend
	D3D_RSI(	3,	D3DRS_BLENDOP,						D3DBLENDOP_ADD			),
	D3D_RSI(	3,	D3DRS_SRCBLEND,						DONT_KNOW_YET			),
	D3D_RSI(	3,	D3DRS_DESTBLEND,					DONT_KNOW_YET			),

	D3D_RSI(	1,	D3DRS_SEPARATEALPHABLENDENABLE,		FALSE					),	// hit in CTransitionTable::ApplySeparateAlphaBlend
	D3D_RSI(	1,	D3DRS_SRCBLENDALPHA,				D3DBLEND_ONE			),	// going to demote these to class 1 until I figure out if they are implementable
	D3D_RSI(	1,	D3DRS_DESTBLENDALPHA,				D3DBLEND_ZERO			),
	D3D_RSI(	1,	D3DRS_BLENDOPALPHA,					D3DBLENDOP_ADD			),

	// what is the deal with alpha test... looks like it is inited to off.
	D3D_RSI(	3,	D3DRS_ALPHATESTENABLE,				0						),
	D3D_RSI(	3,	D3DRS_ALPHAREF,						0						),
	D3D_RSI(	3,	D3DRS_ALPHAFUNC,					D3DCMP_GREATEREQUAL		),

	D3D_RSI(	3,	D3DRS_STENCILENABLE,				FALSE					),
	D3D_RSI(	3,	D3DRS_STENCILFAIL,					D3DSTENCILOP_KEEP		),
	D3D_RSI(	3,	D3DRS_STENCILZFAIL,					D3DSTENCILOP_KEEP		),
	D3D_RSI(	3,	D3DRS_STENCILPASS,					D3DSTENCILOP_KEEP		),
	D3D_RSI(	3,	D3DRS_STENCILFUNC,					D3DCMP_ALWAYS			),
	D3D_RSI(	3,	D3DRS_STENCILREF,					0						),
	D3D_RSI(	3,	D3DRS_STENCILMASK,					0xFFFFFFFF				),
	D3D_RSI(	3,	D3DRS_STENCILWRITEMASK,				0xFFFFFFFF				),

	D3D_RSI(	3,	D3DRS_TWOSIDEDSTENCILMODE,			FALSE					),
	D3D_RSI(	3,	D3DRS_CCW_STENCILFAIL,				D3DSTENCILOP_KEEP		),
	D3D_RSI(	3,	D3DRS_CCW_STENCILZFAIL,				D3DSTENCILOP_KEEP		),
	D3D_RSI(	3,	D3DRS_CCW_STENCILPASS,				D3DSTENCILOP_KEEP		),
	D3D_RSI(	3,	D3DRS_CCW_STENCILFUNC,				D3DCMP_ALWAYS 			),

	D3D_RSI(	3,	D3DRS_FOGENABLE,					FALSE					),	// see CShaderAPIDx8::FogMode and friends - be ready to do the ARB fog linear option madness
	D3D_RSI(	3,	D3DRS_FOGCOLOR,						0						),
	D3D_RSI(	3,	D3DRS_FOGTABLEMODE,					D3DFOG_NONE				),
	D3D_RSI(	3,	D3DRS_FOGSTART,						CONST_DZERO				),
	D3D_RSI(	3,	D3DRS_FOGEND,						CONST_DONE				),
	D3D_RSI(	3,	D3DRS_FOGDENSITY,					CONST_DZERO				),
	D3D_RSI(	3,	D3DRS_RANGEFOGENABLE,				FALSE					),
	D3D_RSI(	3,	D3DRS_FOGVERTEXMODE,				D3DFOG_NONE				),	// watch out for CShaderAPIDx8::CommitPerPassFogMode....

	D3D_RSI(	3,	D3DRS_MULTISAMPLEANTIALIAS,			TRUE					),
	D3D_RSI(	3,	D3DRS_MULTISAMPLEMASK,				0xFFFFFFFF				),

	D3D_RSI(	3,	D3DRS_SCISSORTESTENABLE,			FALSE					),	// heed IDirect3DDevice9::SetScissorRect

	D3D_RSI(	3,	D3DRS_DEPTHBIAS,					CONST_DZERO				),
	D3D_RSI(	3,	D3DRS_SLOPESCALEDEPTHBIAS,			CONST_DZERO				),

	D3D_RSI(	3,	D3DRS_COLORWRITEENABLE1,			0x0000000f				),
	D3D_RSI(	3,	D3DRS_COLORWRITEENABLE2,			0x0000000f				),
	D3D_RSI(	3,	D3DRS_COLORWRITEENABLE3,			0x0000000f				),

	D3D_RSI(	3,	D3DRS_SRGBWRITEENABLE,				0						),	// heeded but ignored..

	D3D_RSI(	2,	D3DRS_CLIPPING,						TRUE					),	// um, yeah, clipping is enabled (?)
	D3D_RSI(	3,	D3DRS_CLIPPLANEENABLE,				0						),	// mask 1<<n of active user clip planes.

	D3D_RSI(	0,	D3DRS_LIGHTING,						0						),	// strange, someone turns it on then off again. move to class 0 and just ignore it (lie)?

	D3D_RSI(	3,	D3DRS_FILLMODE,						D3DFILL_SOLID			),

	D3D_RSI(	1,	D3DRS_SHADEMODE,					D3DSHADE_GOURAUD		),
	D3D_RSI(	1,	D3DRS_LASTPIXEL,					TRUE					),
	D3D_RSI(	1,	D3DRS_DITHERENABLE,					0						),	//set to false by game, no one sets it to true
	D3D_RSI(	1,	D3DRS_SPECULARENABLE,				FALSE					),
	D3D_RSI(	1,	D3DRS_TEXTUREFACTOR,				0xFFFFFFFF				),	// watch out for CShaderAPIDx8::Color3f et al.
	D3D_RSI(	1,	D3DRS_WRAP0,						0						),
	D3D_RSI(	1,	D3DRS_WRAP1,						0						),
	D3D_RSI(	1,	D3DRS_WRAP2,						0						),
	D3D_RSI(	1,	D3DRS_WRAP3,						0						),
	D3D_RSI(	1,	D3DRS_WRAP4,						0						),
	D3D_RSI(	1,	D3DRS_WRAP5,						0						),
	D3D_RSI(	1,	D3DRS_WRAP6,						0						),
	D3D_RSI(	1,	D3DRS_WRAP7,						0						),
	D3D_RSI(	1,	D3DRS_AMBIENT,						0						),	// FF lighting, no
	D3D_RSI(	1,	D3DRS_COLORVERTEX,					TRUE					),	// FF lighing again
	D3D_RSI(	1,	D3DRS_LOCALVIEWER,					TRUE					),	// FF lighting
	D3D_RSI(	1,	D3DRS_NORMALIZENORMALS,				FALSE					),	// FF mode I think.  CShaderAPIDx8::SetVertexBlendState says it might switch this on when skinning is in play
	D3D_RSI(	1,	D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_MATERIAL			),	// hit only in CShaderAPIDx8::ResetRenderState
	D3D_RSI(	1,	D3DRS_SPECULARMATERIALSOURCE,		D3DMCS_COLOR2			),
	D3D_RSI(	1,	D3DRS_AMBIENTMATERIALSOURCE,		D3DMCS_MATERIAL			),
	D3D_RSI(	1,	D3DRS_EMISSIVEMATERIALSOURCE,		D3DMCS_MATERIAL			),
	D3D_RSI(	1,	D3DRS_VERTEXBLEND,					D3DVBF_DISABLE			),	// also being set by CShaderAPIDx8::SetVertexBlendState, so might be FF
	D3D_RSI(	1,	D3DRS_POINTSIZE,					CONST_DONE				),
	D3D_RSI(	1,	D3DRS_POINTSIZE_MIN,				CONST_DONE				),
	D3D_RSI(	1,	D3DRS_POINTSPRITEENABLE,			FALSE					),
	D3D_RSI(	1,	D3DRS_POINTSCALEENABLE,				FALSE					),
	D3D_RSI(	1,	D3DRS_POINTSCALE_A,					CONST_DONE				),
	D3D_RSI(	1,	D3DRS_POINTSCALE_B,					CONST_DZERO				),
	D3D_RSI(	1,	D3DRS_POINTSCALE_C,					CONST_DZERO				),
	D3D_RSI(	1,	D3DRS_PATCHEDGESTYLE,				D3DPATCHEDGE_DISCRETE	),
	D3D_RSI(	1,	D3DRS_DEBUGMONITORTOKEN,			D3DDMT_ENABLE			),
	D3D_RSI(	1,	D3DRS_POINTSIZE_MAX,				CONST_D64				),
	D3D_RSI(	1,	D3DRS_INDEXEDVERTEXBLENDENABLE,		FALSE					),
	D3D_RSI(	1,	D3DRS_TWEENFACTOR,					CONST_DZERO				),
	D3D_RSI(	1,	D3DRS_POSITIONDEGREE,				D3DDEGREE_CUBIC			),
	D3D_RSI(	1,	D3DRS_NORMALDEGREE,					D3DDEGREE_LINEAR		),
	D3D_RSI(	1,	D3DRS_ANTIALIASEDLINEENABLE,		FALSE					),	// just ignore it
	D3D_RSI(	1,	D3DRS_MINTESSELLATIONLEVEL,			CONST_DONE				),
	D3D_RSI(	1,	D3DRS_MAXTESSELLATIONLEVEL,			CONST_DONE				),
	D3D_RSI(	1,	D3DRS_ADAPTIVETESS_X,				CONST_DZERO				),
	D3D_RSI(	1,	D3DRS_ADAPTIVETESS_Y,				CONST_DZERO				), // Overridden as Alpha-to-coverage contrl
	D3D_RSI(	1,	D3DRS_ADAPTIVETESS_Z,				CONST_DONE				),
	D3D_RSI(	1,	D3DRS_ADAPTIVETESS_W,				CONST_DZERO				),
	D3D_RSI(	1,	D3DRS_ENABLEADAPTIVETESSELLATION,	FALSE					),
	D3D_RSI(	1,	D3DRS_BLENDFACTOR,					0xffffffff				),
	D3D_RSI(	1,	D3DRS_WRAP8,						0						),
	D3D_RSI(	1,	D3DRS_WRAP9,						0						),
	D3D_RSI(	1,	D3DRS_WRAP10,						0						),
	D3D_RSI(	1,	D3DRS_WRAP11,						0						),
	D3D_RSI(	1,	D3DRS_WRAP12,						0						),
	D3D_RSI(	1,	D3DRS_WRAP13,						0						),
	D3D_RSI(	1,	D3DRS_WRAP14,						0						),
	D3D_RSI(	1,	D3DRS_WRAP15,						0						),
	D3D_RSI(	-1,	(D3DRENDERSTATETYPE)0,				0						)	// terminator
};

class COpenGLDevice9 : public IDirect3DDevice9
{	
public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//Device
	virtual HRESULT STDMETHODCALLTYPE BeginScene();
	virtual HRESULT STDMETHODCALLTYPE BeginStateBlock();
	virtual HRESULT STDMETHODCALLTYPE Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	virtual HRESULT STDMETHODCALLTYPE ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color);
	virtual HRESULT STDMETHODCALLTYPE CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain);
	virtual HRESULT STDMETHODCALLTYPE CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader);
	virtual HRESULT STDMETHODCALLTYPE CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery);
	virtual HRESULT STDMETHODCALLTYPE CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB);
	virtual HRESULT STDMETHODCALLTYPE CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl);
	virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader);
	virtual HRESULT STDMETHODCALLTYPE CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE DeletePatch(UINT Handle);
	virtual HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount);
	virtual HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT STDMETHODCALLTYPE DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	virtual HRESULT STDMETHODCALLTYPE DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT STDMETHODCALLTYPE DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo);
	virtual HRESULT STDMETHODCALLTYPE DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo);
	virtual HRESULT STDMETHODCALLTYPE EndScene();
	virtual HRESULT STDMETHODCALLTYPE EndStateBlock(IDirect3DStateBlock9 **ppSB);
	virtual HRESULT STDMETHODCALLTYPE EvictManagedResources();
	virtual UINT STDMETHODCALLTYPE GetAvailableTextureMem();
	virtual HRESULT STDMETHODCALLTYPE GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer);
	virtual HRESULT STDMETHODCALLTYPE GetClipPlane(DWORD Index,float *pPlane);
	virtual HRESULT STDMETHODCALLTYPE GetClipStatus(D3DCLIPSTATUS9 *pClipStatus);
	virtual HRESULT STDMETHODCALLTYPE GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
	virtual HRESULT STDMETHODCALLTYPE GetCurrentTexturePalette(UINT *pPaletteNumber);
	virtual HRESULT STDMETHODCALLTYPE GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface);
	virtual HRESULT STDMETHODCALLTYPE GetDeviceCaps(D3DCAPS9 *pCaps);
	virtual HRESULT STDMETHODCALLTYPE GetDirect3D(IDirect3D9 **ppD3D9);
	virtual HRESULT STDMETHODCALLTYPE GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode);
	virtual HRESULT STDMETHODCALLTYPE GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface);
	virtual HRESULT STDMETHODCALLTYPE GetFVF(DWORD *pFVF);
	virtual void STDMETHODCALLTYPE GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp);
	virtual HRESULT STDMETHODCALLTYPE GetIndices(IDirect3DIndexBuffer9 **ppIndexData); //,UINT *pBaseVertexIndex  ?
	virtual HRESULT STDMETHODCALLTYPE GetLight(DWORD Index,D3DLIGHT9 *pLight); /*?documentation lists D3DLight9*/
	virtual HRESULT STDMETHODCALLTYPE GetLightEnable(DWORD Index,BOOL *pEnable);
	virtual HRESULT STDMETHODCALLTYPE GetMaterial(D3DMATERIAL9 *pMaterial);
	virtual FLOAT STDMETHODCALLTYPE GetNPatchMode();
	virtual UINT STDMETHODCALLTYPE GetNumberOfSwapChains();
	virtual HRESULT STDMETHODCALLTYPE GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries);
	virtual HRESULT STDMETHODCALLTYPE GetPixelShader(IDirect3DPixelShader9 **ppShader);
	virtual HRESULT STDMETHODCALLTYPE GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount);
	virtual HRESULT STDMETHODCALLTYPE GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount);
	virtual HRESULT STDMETHODCALLTYPE GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount);
	virtual HRESULT STDMETHODCALLTYPE GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus);
	virtual HRESULT STDMETHODCALLTYPE GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue);
	virtual HRESULT STDMETHODCALLTYPE GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget);
	virtual HRESULT STDMETHODCALLTYPE GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface);
	virtual HRESULT STDMETHODCALLTYPE GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue);
	virtual HRESULT STDMETHODCALLTYPE GetScissorRect(RECT *pRect);
	virtual BOOL STDMETHODCALLTYPE GetSoftwareVertexProcessing();
	virtual HRESULT STDMETHODCALLTYPE GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride);
	virtual HRESULT STDMETHODCALLTYPE GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider);
	virtual HRESULT STDMETHODCALLTYPE GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain);
	virtual HRESULT STDMETHODCALLTYPE GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture);
	virtual HRESULT STDMETHODCALLTYPE GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue);
	virtual HRESULT STDMETHODCALLTYPE GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix);
	virtual HRESULT STDMETHODCALLTYPE GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl);
	virtual HRESULT STDMETHODCALLTYPE GetVertexShader(IDirect3DVertexShader9 **ppShader);
	virtual HRESULT STDMETHODCALLTYPE GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount);
	virtual HRESULT STDMETHODCALLTYPE GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount);
	virtual HRESULT STDMETHODCALLTYPE GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount);
	virtual HRESULT STDMETHODCALLTYPE GetViewport(D3DVIEWPORT9 *pViewport);
	virtual HRESULT STDMETHODCALLTYPE LightEnable(DWORD LightIndex,BOOL bEnable);
	virtual HRESULT STDMETHODCALLTYPE MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix);
	virtual HRESULT STDMETHODCALLTYPE Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion);
	virtual HRESULT STDMETHODCALLTYPE ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE Reset(D3DPRESENT_PARAMETERS *pPresentationParameters);
	virtual HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD Index,const float *pPlane);
	virtual HRESULT STDMETHODCALLTYPE SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus);
	virtual HRESULT STDMETHODCALLTYPE SetCurrentTexturePalette(UINT PaletteNumber);
	virtual void STDMETHODCALLTYPE SetCursorPosition(INT X,INT Y,DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap);
	virtual HRESULT STDMETHODCALLTYPE SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil);
	virtual HRESULT STDMETHODCALLTYPE SetDialogBoxMode(BOOL bEnableDialogs);
	virtual HRESULT STDMETHODCALLTYPE SetFVF(DWORD FVF);
	virtual void STDMETHODCALLTYPE SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp);
	virtual HRESULT STDMETHODCALLTYPE SetIndices(IDirect3DIndexBuffer9 *pIndexData);
	virtual HRESULT STDMETHODCALLTYPE SetLight(DWORD Index,const D3DLIGHT9 *pLight);
	virtual HRESULT STDMETHODCALLTYPE SetMaterial(const D3DMATERIAL9 *pMaterial);
	virtual HRESULT STDMETHODCALLTYPE SetNPatchMode(float nSegments);
	virtual HRESULT STDMETHODCALLTYPE SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries);
	virtual HRESULT STDMETHODCALLTYPE SetPixelShader(IDirect3DPixelShader9 *pShader);
	virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount);
	virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount);
	virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount);
	virtual HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
	virtual HRESULT STDMETHODCALLTYPE SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget);
	virtual HRESULT STDMETHODCALLTYPE SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
	virtual HRESULT STDMETHODCALLTYPE SetScissorRect(const RECT *pRect);
	virtual HRESULT STDMETHODCALLTYPE SetSoftwareVertexProcessing(BOOL bSoftware);
	virtual HRESULT STDMETHODCALLTYPE SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride);
	virtual HRESULT STDMETHODCALLTYPE SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter);
	virtual HRESULT STDMETHODCALLTYPE SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture);
	virtual HRESULT STDMETHODCALLTYPE SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	virtual HRESULT STDMETHODCALLTYPE SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix);
	virtual HRESULT STDMETHODCALLTYPE SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl);
	virtual HRESULT STDMETHODCALLTYPE SetVertexShader(IDirect3DVertexShader9 *pShader);
	virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount);
	virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount);
	virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount);
	virtual HRESULT STDMETHODCALLTYPE SetViewport(const D3DVIEWPORT9 *pViewport);
	virtual BOOL STDMETHODCALLTYPE ShowCursor(BOOL bShow);
	virtual HRESULT STDMETHODCALLTYPE StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter);
	virtual HRESULT STDMETHODCALLTYPE TestCooperativeLevel();
	virtual HRESULT STDMETHODCALLTYPE UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint);
	virtual HRESULT STDMETHODCALLTYPE UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
	virtual HRESULT STDMETHODCALLTYPE ValidateDevice(DWORD *pNumPasses);
	
public:

	int	m_refcount[2];
	bool m_mark;

	// GL state 
	struct 
	{
		// render state buckets
		GLAlphaTestEnable_t			m_AlphaTestEnable;
		GLAlphaTestFunc_t			m_AlphaTestFunc;

		GLAlphaToCoverageEnable_t	m_AlphaToCoverageEnable;

		GLDepthTestEnable_t			m_DepthTestEnable;
		GLDepthMask_t				m_DepthMask;
		GLDepthFunc_t				m_DepthFunc;

		GLClipPlaneEnable_t			m_ClipPlaneEnable[kGLMUserClipPlanes];
		GLClipPlaneEquation_t		m_ClipPlaneEquation[kGLMUserClipPlanes];

		GLColorMaskSingle_t			m_ColorMaskSingle;
		GLColorMaskMultiple_t		m_ColorMaskMultiple;

		GLCullFaceEnable_t			m_CullFaceEnable;
		GLCullFrontFace_t			m_CullFrontFace;
		GLPolygonMode_t				m_PolygonMode;
		GLDepthBias_t				m_DepthBias;
		GLScissorEnable_t			m_ScissorEnable;
		GLScissorBox_t				m_ScissorBox;
		GLViewportBox_t				m_ViewportBox;
		GLViewportDepthRange_t		m_ViewportDepthRange;

		GLBlendEnable_t				m_BlendEnable;
		GLBlendFactor_t				m_BlendFactor;
		GLBlendEquation_t			m_BlendEquation;
		GLBlendColor_t				m_BlendColor;
		GLBlendEnableSRGB_t			m_BlendEnableSRGB;

		GLStencilTestEnable_t		m_StencilTestEnable;
		GLStencilFunc_t				m_StencilFunc;
		GLStencilOp_t				m_StencilOp;
		GLStencilWriteMask_t		m_StencilWriteMask;

		GLClearColor_t				m_ClearColor;
		GLClearDepth_t				m_ClearDepth;
		GLClearStencil_t			m_ClearStencil;

		bool						m_FogEnable;			// not really pushed to GL, just latched here

		// samplers
		GLMTexSamplingParams		m_samplers[GLM_SAMPLER_COUNT];
	} gl;

	// D3D flavor stuff
	COpenGLSurface9			*m_pRenderTargets[4];
	COpenGLSurface9			*m_pDepthStencil;

	COpenGLSurface9			*m_pDefaultColorSurface;			// default color surface.
	COpenGLSurface9			*m_pDefaultDepthStencilSurface;	// queried by GetDepthStencilSurface.

	COpenGLVertexDeclaration9	*m_pVertDecl;					// Set by SetVertexDeclaration...
	D3DStreamDesc				m_streams[ D3D_MAX_STREAMS ];	// Set by SetStreamSource..
	CGLMBuffer					*m_vtx_buffers[ D3D_MAX_STREAMS ];
	CGLMBuffer					*m_pDummy_vtx_buffer;
	D3DIndexDesc				m_indices;						// Set by SetIndices..

	COpenGLVertexShader9		*m_vertexShader;				// Set by SetVertexShader...
	COpenGLPixelShader9		*m_pixelShader;					// Set by SetPixelShader...

	COpenGLBaseTexture9		*m_textures[GLM_SAMPLER_COUNT];				// set by SetTexture... NULL if stage inactive
	
	// GLM flavor stuff
	GLMContext					*m_ctx;
	CGLMFBOMap					*m_pFBOs;
	bool						m_bFBODirty;
	DWORD						m_nValidMarker;

	ObjectStats_t					m_ObjectStats;
	ObjectStats_t					m_PrevObjectStats;
	
#if GL_BATCH_PERF_ANALYSIS
	simple_bitmap *m_pBatch_vis_bitmap;
	uint m_nBatchVisY;
	uint m_nBatchVisFrameIndex, m_nBatchVisFileIdx;
	uint m_nNumProgramChanges;
		
	uint m_nTotalD3DCalls;
	double m_flTotalD3DTime;
	uint m_nTotalGLCalls;
	double m_flTotalGLTime;
	uint m_nTotalPrims;
		
	uint m_nOverallProgramChanges;
	uint m_nOverallDraws;
	uint m_nOverallPrims;
	uint m_nOverallD3DCalls;
	double m_flOverallD3DTime;
	uint m_nOverallGLCalls;
	double m_flOverallGLTime;

	double m_flOverallPresentTime;
	double m_flOverallPresentTimeSquared;
	double m_flOverallSwapWindowTime;
	double m_flOverallSwapWindowTimeSquared;
	uint m_nOverallPresents;
#endif
public:
	friend class GLMContext;

	COpenGLDevice9();
	~COpenGLDevice9();

	IDirect3DDevice9Params	m_params;						// mirror of the creation inputs

	HRESULT	Create( IDirect3DDevice9Params *params );
	void UpdateBoundFBO();
	void ResetFBOMap();
	void ScrubFBOMap( CGLMTex *pTex );
	void ReleasedVertexDeclaration( COpenGLVertexDeclaration9 *pDecl );
	void ReleasedTexture( COpenGLBaseTexture9 *baseTex );
	void ReleasedCGLMTex( CGLMTex *pTex);
	void ReleasedSurface( COpenGLSurface9 *pSurface );
	void ReleasedPixelShader( COpenGLPixelShader9 *pixelShader );
	void ReleasedVertexShader( COpenGLVertexShader9 *vertexShader );
	void ReleasedVertexBuffer( COpenGLVertexBuffer9 *vertexBuffer );
	void ReleasedIndexBuffer( COpenGLIndexBuffer9 *indexBuffer );
	void ReleasedQuery( COpenGLQuery9 *query );
	void FlushClipPlaneEquation();
	void InitStates();
	void FullFlushStates();
	void SaveGLState();
	void RestoreGLState();
	void AcquireThreadOwnership( );
	void ReleaseThreadOwnership( );
	void SetMaxUsedVertexShaderConstantsHintNonInline( unsigned int nMaxReg );
	void SetSamplerStatesNonInline(
	DWORD Sampler, DWORD AddressU, DWORD AddressV, DWORD AddressW,
	DWORD MinFilter, DWORD MagFilter, DWORD MipFilter );

	FORCEINLINE void TOGLMETHODCALLTYPE SetSamplerStates(
	DWORD Sampler, DWORD AddressU, DWORD AddressV, DWORD AddressW,
	DWORD MinFilter, DWORD MagFilter, DWORD MipFilter )
	{
	#if GLMDEBUG || GL_BATCH_PERF_ANALYSIS
		SetSamplerStatesNonInline( Sampler, AddressU, AddressV, AddressW, MinFilter, MagFilter, MipFilter );
	#else
		Assert( GetCurrentOwnerThreadId() == ThreadGetCurrentId() );
		Assert( Sampler < GLM_SAMPLER_COUNT);
			
		m_ctx->SetSamplerDirty( Sampler );
			
		m_ctx->SetSamplerStates( Sampler, AddressU, AddressV, AddressW, MinFilter, MagFilter, MipFilter );
	#endif
	}

	inline GLenum D3DCompareFuncToGL( DWORD function )
	{
		switch ( function )
		{
			case D3DCMP_NEVER		: return GL_NEVER;				// Always fail the test.
			case D3DCMP_LESS		: return GL_LESS;				// Accept the new pixel if its value is less than the value of the current pixel.
			case D3DCMP_EQUAL		: return GL_EQUAL;				// Accept the new pixel if its value equals the value of the current pixel.
			case D3DCMP_LESSEQUAL	: return GL_LEQUAL;				// Accept the new pixel if its value is less than or equal to the value of the current pixel. **
			case D3DCMP_GREATER		: return GL_GREATER;			// Accept the new pixel if its value is greater than the value of the current pixel.
			case D3DCMP_NOTEQUAL	: return GL_NOTEQUAL;			// Accept the new pixel if its value does not equal the value of the current pixel.
			case D3DCMP_GREATEREQUAL: return GL_GEQUAL;				// Accept the new pixel if its value is greater than or equal to the value of the current pixel.
			case D3DCMP_ALWAYS		: return GL_ALWAYS;				// Always pass the test.
			default					: DXABSTRACT_BREAK_ON_ERROR(); return 0xFFFFFFFF;
		}
	}

	FORCEINLINE GLenum D3DBlendOperationToGL( DWORD operation )
	{
		switch (operation)
		{
			case	D3DBLENDOP_ADD			: return GL_FUNC_ADD;				// The result is the destination added to the source. Result = Source + Destination
			case	D3DBLENDOP_SUBTRACT		: return GL_FUNC_SUBTRACT;			// The result is the destination subtracted from to the source. Result = Source - Destination
			case	D3DBLENDOP_REVSUBTRACT	: return GL_FUNC_REVERSE_SUBTRACT;	// The result is the source subtracted from the destination. Result = Destination - Source
			case	D3DBLENDOP_MIN			: return GL_MIN;					// The result is the minimum of the source and destination. Result = MIN(Source, Destination)
			case	D3DBLENDOP_MAX			: return GL_MAX;					// The result is the maximum of the source and destination. Result = MAX(Source, Destination)
			default:
				DXABSTRACT_BREAK_ON_ERROR();
				return 0xFFFFFFFF;
			break;
		}
	}

	FORCEINLINE GLenum D3DBlendFactorToGL( DWORD equation )
	{
		switch (equation)
		{
			case	D3DBLEND_ZERO			: return GL_ZERO;					// Blend factor is (0, 0, 0, 0).
			case	D3DBLEND_ONE			: return GL_ONE;					// Blend factor is (1, 1, 1, 1).
			case	D3DBLEND_SRCCOLOR		: return GL_SRC_COLOR;				// Blend factor is (Rs, Gs, Bs, As).
			case	D3DBLEND_INVSRCCOLOR	: return GL_ONE_MINUS_SRC_COLOR;	// Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As).
			case	D3DBLEND_SRCALPHA		: return GL_SRC_ALPHA;				// Blend factor is (As, As, As, As).
			case	D3DBLEND_INVSRCALPHA	: return GL_ONE_MINUS_SRC_ALPHA;	// Blend factor is ( 1 - As, 1 - As, 1 - As, 1 - As).
			case	D3DBLEND_DESTALPHA		: return GL_DST_ALPHA;				// Blend factor is (Ad Ad Ad Ad).
			case	D3DBLEND_INVDESTALPHA	: return GL_ONE_MINUS_DST_ALPHA;	// Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad).
			case	D3DBLEND_DESTCOLOR		: return GL_DST_COLOR;				// Blend factor is (Rd, Gd, Bd, Ad).
			case	D3DBLEND_INVDESTCOLOR	: return GL_ONE_MINUS_DST_COLOR;	// Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad).
			case	D3DBLEND_SRCALPHASAT	: return GL_SRC_ALPHA_SATURATE;		// Blend factor is (f, f, f, 1); where f = min(As, 1 - Ad).

			/*
				// these are weird.... break if we hit them
				case	D3DBLEND_BOTHSRCALPHA	: Assert(0); return GL_ZERO;		// Obsolete. Starting with DirectX 6, you can achieve the same effect by setting the source and destination blend factors to D3DBLEND_SRCALPHA and D3DBLEND_INVSRCALPHA in separate calls.
				case	D3DBLEND_BOTHINVSRCALPHA: Assert(0); return GL_ZERO;		// Source blend factor is (1 - As, 1 - As, 1 - As, 1 - As), and destination blend factor is (As, As, As, As); the destination blend selection is overridden. This blend mode is supported only for the D3DRS_SRCBLEND render state.
				case	D3DBLEND_BLENDFACTOR	: Assert(0); return GL_ZERO;		// Constant color blending factor used by the frame-buffer blender. This blend mode is supported only if D3DPBLENDCAPS_BLENDFACTOR is set in the SrcBlendCaps or DestBlendCaps members of D3DCAPS9.
			
			dxabstract.h has not heard of these, so let them hit the debugger if they come through
				case	D3DBLEND_INVBLENDFACTOR:	//Inverted constant color-blending factor used by the frame-buffer blender. This blend mode is supported only if the D3DPBLENDCAPS_BLENDFACTOR bit is set in the SrcBlendCaps or DestBlendCaps members of D3DCAPS9.
				case	D3DBLEND_SRCCOLOR2:		// Blend factor is (PSOutColor[1]r, PSOutColor[1]g, PSOutColor[1]b, not used).	This flag is available in Direct3D 9Ex only.
				case	D3DBLEND_INVSRCCOLOR2:	// Blend factor is (1 - PSOutColor[1]r, 1 - PSOutColor[1]g, 1 - PSOutColor[1]b, not used)). This flag is available in Direct3D 9Ex only.
			*/
			default:
				DXABSTRACT_BREAK_ON_ERROR();
				return 0xFFFFFFFF;
			break;
		}
	}

	FORCEINLINE GLenum D3DStencilOpToGL( DWORD operation )
	{
		switch( operation )
		{
			case D3DSTENCILOP_KEEP		: return GL_KEEP;
			case D3DSTENCILOP_ZERO		: return GL_ZERO;
			case D3DSTENCILOP_REPLACE	: return GL_REPLACE;
			case D3DSTENCILOP_INCRSAT	: return GL_INCR;
			case D3DSTENCILOP_DECRSAT	: return GL_DECR;
			case D3DSTENCILOP_INVERT	: return GL_INVERT;
			case D3DSTENCILOP_INCR		: return GL_INCR_WRAP_EXT;
			case D3DSTENCILOP_DECR		: return GL_DECR_WRAP_EXT;
			default						: DXABSTRACT_BREAK_ON_ERROR(); return 0xFFFFFFFF;
		}
	}

	FORCEINLINE HRESULT TOGLMETHODCALLTYPE SetRenderStateInline( D3DRENDERSTATETYPE State, DWORD Value )
	{
	#if GLMDEBUG || GL_BATCH_PERF_ANALYSIS
		return SetRenderState( State, Value );
	#else
		TOGL_NULL_DEVICE_CHECK;
		Assert( GetCurrentOwnerThreadId() == ThreadGetCurrentId() );

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
		}
			
		return S_OK;
	#endif
	}

	FORCEINLINE HRESULT TOGLMETHODCALLTYPE SetRenderStateConstInline( D3DRENDERSTATETYPE State, DWORD Value )
	{
		// State is a compile time constant - luckily no need to do anything special to get the compiler to optimize this case.
		return SetRenderStateInline( State, Value );
	}

	FORCEINLINE void SetMaxUsedVertexShaderConstantsHint( unsigned int nMaxReg )
	{
	#if GLMDEBUG || GL_BATCH_PERF_ANALYSIS
		return SetMaxUsedVertexShaderConstantsHintNonInline( nMaxReg );
	#else
		Assert( GetCurrentOwnerThreadId() == ThreadGetCurrentId() );
		m_ctx->SetMaxUsedVertexShaderConstantsHint( nMaxReg );
	#endif
	}
};

#endif // COPENGLDEVICE9_H
