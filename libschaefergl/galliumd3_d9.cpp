/*
 * Copyright (C) 2014 Christopher Joseph Dean Schaefer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
 
#include "galliumd3_d9.h"
#include "galliumd3d_device9.h"

#include "util/u_memory.h"
#include "util/u_inlines.h"
#include "util/u_math.h"
#include "util/u_format.h"
#include "util/u_dump.h"

#include <stdlib.h> //for getenv
#include <new>  //for nothrow

static const char* GetLibrarySearchPath()
{
	const char* search_path = NULL;
   
   /*
	* I've seen several examples with this check indicating that it is to prevent "set uid apps" from using GBM_BACKENDS_PATH.
	* TODO determine reason for check and if possible add alternate (non-static) method for finding pipe path.
	*/
	if (geteuid() == getuid())
	{
		search_path = getenv("GBM_BACKENDS_PATH");		  
	}

	if (search_path == NULL)
	{
		#ifdef PIPE_SEARCH_DIR 
		search_path = PIPE_SEARCH_DIR; //This would normally be set by the build scripts.
		#endif
	}
    
	return search_path;	
}

GalliumD3D9::GalliumD3D9()
{
	/*
	 * This will try to load up to 20 DRM pipe devices and populate the array with the devices that were found.
	 * My guess is that 90% of the time we only care about the first device unless Gallium doesn't return the primary device first.
	 * If that is the case the devices may need to be sorted to allow D3D caps and other functionality checks to work correctly.
	 */
	this->mPipeDeviceCount = pipe_loader_drm_probe(&mPipeDevices,20);
	
	/*
	 * We only need one screen per device so we can pull caps.
	 * All of the entries should be set to NULL so that we can tell if they are not valid screens later.
	 */
	mPipeScreens = new pipe_screen*[mPipeDeviceCount]();
	for(int i=0; i<mPipeDeviceCount; i++)
	{
		mPipeScreens[i]=NULL;
	}
}

GalliumD3D9::~GalliumD3D9()
{
	pipe_loader_release(&mPipeDevices,mPipeDeviceCount);
	delete[] mPipeScreens;
}

UINT GalliumD3D9::GetAdapterCount()
{
	return this->mPipeDeviceCount;
}

HRESULT GalliumD3D9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	int returnValue = 0;
	pipe_screen* screen = NULL;
	pipe_loader_device* pipeDevice = NULL;
	GalliumD3DDevice9* device = NULL;
	IDirect3DDevice9* deviceInterface = NULL;
	D3DCAPS9 capabilities;
	D3DDEVICE_CREATION_PARAMETERS parameters;
	
	if(Adapter<mPipeDeviceCount)
	{
		pipeDevice = &mPipeDevices[Adapter];
	}
	else
	{
		return D3DERR_INVALIDCALL;
	}
	
	switch(DeviceType)
	{
        case D3DDEVTYPE_HAL:
				screen = pipe_loader_create_screen(pipeDevice, GetLibrarySearchPath());
				mPipeScreens[Adapter] = screen;
            break;
        case D3DDEVTYPE_REF:
			return D3DERR_NOTAVAILABLE;
        case D3DDEVTYPE_NULLREF:
				return D3DERR_NOTAVAILABLE;
        case D3DDEVTYPE_SW:
            return D3DERR_NOTAVAILABLE;
        default:
            return D3DERR_INVALIDCALL;		
	}
	
	if(screen==NULL)
	{
		return D3DERR_NOTAVAILABLE;
	}
	
	if(this->GetDeviceCaps(Adapter,DeviceType,&capabilities)!=D3D_OK)
	{
		return E_FAIL;
	}
	
	parameters.AdapterOrdinal = Adapter;
    parameters.DeviceType = DeviceType;
    parameters.hFocusWindow = hFocusWindow;
    parameters.BehaviorFlags = BehaviorFlags;
	
	device = new (std::nothrow) GalliumD3DDevice9(screen,&parameters,&capabilities);
	
	if(device==NULL)
	{
		return E_FAIL;
	}
	
	deviceInterface = (IDirect3DDevice9*)device;
	ppReturnedDeviceInterface = &deviceInterface;
	
	return D3D_OK;
}

HRESULT GalliumD3D9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	struct pipe_screen *screen;
	
	/*
	 * Pull the screen so we can use it to test teh caps. If that screen does not exist then bail out.
	 * (May need to create screen on the fly instead of just failing)
	 */
	if(this->mPipeDeviceCount>Adapter)
	{
		screen = this->mPipeScreens[Adapter];
	}
	if(screen==NULL)
	{
		return E_FAIL;
	}
	
	//Misc setup.
	pCaps->DeviceType = DeviceType;
	pCaps->AdapterOrdinal = Adapter;
	
	#define D3DPIPECAP(pcap, d3dcap) (screen->get_param(screen, PIPE_CAP_##pcap) ? (d3dcap) : 0)
	#define D3DNPIPECAP(pcap, d3dcap) (screen->get_param(screen, PIPE_CAP_##pcap) ? 0 : (d3dcap))
	
	//Assumed capabilities.
	pCaps->Caps = 0;
	
	pCaps->Caps2 = 
		D3DCAPS2_CANMANAGERESOURCE |
		/* D3DCAPS2_CANSHARERESOURCE | */
		/* D3DCAPS2_CANCALIBRATEGAMMA | */
		D3DCAPS2_DYNAMICTEXTURES |
		D3DCAPS2_FULLSCREENGAMMA |
		D3DCAPS2_CANAUTOGENMIPMAP;
			   
	pCaps->Caps3 =
		/* D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD | */
		D3DCAPS3_COPY_TO_VIDMEM |
		D3DCAPS3_COPY_TO_SYSTEMMEM |
		D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION |
		D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD;
		
	/*
	 * Bit mask of values representing what presentation swap intervals are available.
	 */
    pCaps->PresentationIntervals = 
		D3DPRESENT_INTERVAL_DEFAULT |
		D3DPRESENT_INTERVAL_ONE |
		/*D3DPRESENT_INTERVAL_TWO |*/
		/*D3DPRESENT_INTERVAL_THREE |*/
		/*D3DPRESENT_INTERVAL_FOUR |*/
		D3DPRESENT_INTERVAL_IMMEDIATE;
	
	/*
	 * Bit mask indicating what hardware support is available for cursors. 
	 * Direct3D 9 does not define alpha-blending cursor capabilities.
	 */
	pCaps->CursorCaps = 
		D3DCURSORCAPS_COLOR | D3DCURSORCAPS_LOWRES;
	
	pCaps->DevCaps = 
		D3DDEVCAPS_CANBLTSYSTONONLOCAL |
		D3DDEVCAPS_CANRENDERAFTERFLIP |
		D3DDEVCAPS_DRAWPRIMITIVES2 |
		D3DDEVCAPS_DRAWPRIMITIVES2EX |
		D3DDEVCAPS_DRAWPRIMTLVERTEX |
		D3DDEVCAPS_EXECUTESYSTEMMEMORY |
		D3DDEVCAPS_EXECUTEVIDEOMEMORY |
		D3DDEVCAPS_HWRASTERIZATION |
		D3DDEVCAPS_HWTRANSFORMANDLIGHT |
		/*D3DDEVCAPS_NPATCHES |*/
		D3DDEVCAPS_PUREDEVICE |
		/*D3DDEVCAPS_QUINTICRTPATCHES |*/
		/*D3DDEVCAPS_RTPATCHES |*/
		/*D3DDEVCAPS_RTPATCHHANDLEZERO |*/
		/*D3DDEVCAPS_SEPARATETEXTUREMEMORIES |*/
		/*D3DDEVCAPS_TEXTURENONLOCALVIDMEM |*/
		D3DDEVCAPS_TEXTURESYSTEMMEMORY |
		D3DDEVCAPS_TEXTUREVIDEOMEMORY |
		D3DDEVCAPS_TLVERTEXSYSTEMMEMORY |
		D3DDEVCAPS_TLVERTEXVIDEOMEMORY;
	
	//Miscellaneous driver primitive capabilities.
    pCaps->PrimitiveMiscCaps = 
		D3DPMISCCAPS_MASKZ |
		D3DPMISCCAPS_CULLNONE | /* XXX */
		D3DPMISCCAPS_CULLCW |
		D3DPMISCCAPS_CULLCCW |
		D3DPMISCCAPS_COLORWRITEENABLE |
		D3DPMISCCAPS_CLIPPLANESCALEDPOINTS |
		D3DPMISCCAPS_CLIPTLVERTS |
		D3DPMISCCAPS_TSSARGTEMP |
		D3DPMISCCAPS_BLENDOP |
		D3DPIPECAP(INDEP_BLEND_ENABLE, D3DPMISCCAPS_INDEPENDENTWRITEMASKS) |
		/*D3DPMISCCAPS_PERSTAGECONSTANT |*/
		/*D3DPMISCCAPS_POSTBLENDSRGBCONVERT |*/ /* TODO */
		D3DPMISCCAPS_FOGANDSPECULARALPHA |
		D3DPIPECAP(BLEND_EQUATION_SEPARATE, D3DPMISCCAPS_SEPARATEALPHABLEND) |
		D3DPIPECAP(MIXED_COLORBUFFER_FORMATS, D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) |
		D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING |
		/*D3DPMISCCAPS_FOGVERTEXCLAMPED*/0;	
	
	/*
	 * Information on raster-drawing capabilities. 
	 */
	pCaps->RasterCaps =
        D3DPIPECAP(ANISOTROPIC_FILTER, D3DPRASTERCAPS_ANISOTROPY) |
        /*D3DPRASTERCAPS_COLORPERSPECTIVE |*/
        D3DPRASTERCAPS_DITHER |
        D3DPRASTERCAPS_DEPTHBIAS |
        /*D3DPRASTERCAPS_FOGRANGE |*/
        /*D3DPRASTERCAPS_FOGTABLE |*/
        /*D3DPRASTERCAPS_FOGVERTEX |*/
        D3DPRASTERCAPS_MIPMAPLODBIAS |
        D3DPRASTERCAPS_MULTISAMPLE_TOGGLE |
        D3DPRASTERCAPS_SCISSORTEST |
        D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS |
        /*D3DPRASTERCAPS_WBUFFER |*/
        /*D3DPRASTERCAPS_WFOG |*/
        /*D3DPRASTERCAPS_ZBUFFERLESSHSR |*/
        /*D3DPRASTERCAPS_ZFOG |*/
        D3DPRASTERCAPS_ZTEST;
		
	/*
	 * Z-buffer comparison capabilities.
	 */
    pCaps->ZCmpCaps = 
		D3DPCMPCAPS_NEVER |
		D3DPCMPCAPS_LESS |
		D3DPCMPCAPS_EQUAL |
		D3DPCMPCAPS_LESSEQUAL |
		D3DPCMPCAPS_GREATER |
		D3DPCMPCAPS_NOTEQUAL |
		D3DPCMPCAPS_GREATEREQUAL |
		D3DPCMPCAPS_ALWAYS;	
	
	/*
	 * Source-blending capabilities.
	 */
	pCaps->SrcBlendCaps = 
		D3DPBLENDCAPS_ZERO |
		D3DPBLENDCAPS_ONE |
		D3DPBLENDCAPS_SRCCOLOR |
		D3DPBLENDCAPS_INVSRCCOLOR |
		D3DPBLENDCAPS_SRCALPHA |
		D3DPBLENDCAPS_INVSRCALPHA |
		D3DPBLENDCAPS_DESTALPHA |
		D3DPBLENDCAPS_INVDESTALPHA |
		D3DPBLENDCAPS_DESTCOLOR |
		D3DPBLENDCAPS_INVDESTCOLOR |
		D3DPBLENDCAPS_SRCALPHASAT |
		D3DPBLENDCAPS_BOTHSRCALPHA |
		D3DPBLENDCAPS_BOTHINVSRCALPHA |
		D3DPBLENDCAPS_BLENDFACTOR |
		D3DPIPECAP(MAX_DUAL_SOURCE_RENDER_TARGETS,
		D3DPBLENDCAPS_INVSRCCOLOR2 |
		D3DPBLENDCAPS_SRCCOLOR2);
	
	/*
	 * Destination-blending capabilities.
	 * Assuming source and destination are the same.
	 */
	pCaps->DestBlendCaps = 
		pCaps->SrcBlendCaps;
	
	/*
	 * Alpha-test comparison capabilities. 
	 * This member can include the same capability flags defined for the ZCmpCaps member. 
	 * If this member contains only the D3DPCMPCAPS_ALWAYS capability or only the D3DPCMPCAPS_NEVER capability, the driver does not support alpha tests. 
	 * Otherwise, the flags identify the individual comparisons that are supported for alpha testing.
	 */
	pCaps->AlphaCmpCaps = 
		D3DPCMPCAPS_LESS |
		D3DPCMPCAPS_EQUAL |
		D3DPCMPCAPS_LESSEQUAL |
		D3DPCMPCAPS_GREATER |
		D3DPCMPCAPS_NOTEQUAL |
		D3DPCMPCAPS_GREATEREQUAL |
		D3DPCMPCAPS_ALWAYS;
	
	/*
	 * Shading operations capabilities.
	 * This flag specifies whether the driver can also support Gouraud shading and whether alpha color components are supported.
	 */
	pCaps->ShadeCaps = 
		D3DPSHADECAPS_COLORGOURAUDRGB |
		D3DPSHADECAPS_SPECULARGOURAUDRGB |
		D3DPSHADECAPS_ALPHAGOURAUDBLEND |
		D3DPSHADECAPS_FOGGOURAUD;
	
	/*
	 * Miscellaneous texture-mapping capabilities.
	 */
    pCaps->TextureCaps =
        D3DPTEXTURECAPS_ALPHA |
        D3DPTEXTURECAPS_ALPHAPALETTE |
        D3DPTEXTURECAPS_PERSPECTIVE |
        D3DPTEXTURECAPS_PROJECTED |
        /*D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE |*/
        D3DPTEXTURECAPS_CUBEMAP |
        D3DPTEXTURECAPS_VOLUMEMAP |
        D3DNPIPECAP(NPOT_TEXTURES, D3DPTEXTURECAPS_POW2) |
        D3DNPIPECAP(NPOT_TEXTURES, D3DPTEXTURECAPS_NONPOW2CONDITIONAL) |
        D3DNPIPECAP(NPOT_TEXTURES, D3DPTEXTURECAPS_CUBEMAP_POW2) |
        D3DNPIPECAP(NPOT_TEXTURES, D3DPTEXTURECAPS_VOLUMEMAP_POW2) |
        D3DPIPECAP(MAX_TEXTURE_2D_LEVELS, D3DPTEXTURECAPS_MIPMAP) |
        D3DPIPECAP(MAX_TEXTURE_3D_LEVELS, D3DPTEXTURECAPS_MIPVOLUMEMAP) |
        D3DPIPECAP(MAX_TEXTURE_CUBE_LEVELS, D3DPTEXTURECAPS_MIPCUBEMAP);

	/*
	 * Texture-filtering capabilities for a texture. 
	 * Per-stage filtering capabilities reflect which filtering modes are supported for texture stages when performing multiple-texture blending.
	 */
    pCaps->TextureFilterCaps =
        D3DPTFILTERCAPS_MINFPOINT |
        D3DPTFILTERCAPS_MINFLINEAR |
        D3DPIPECAP(ANISOTROPIC_FILTER, D3DPTFILTERCAPS_MINFANISOTROPIC) |
        /*D3DPTFILTERCAPS_MINFPYRAMIDALQUAD |*/
        /*D3DPTFILTERCAPS_MINFGAUSSIANQUAD |*/
        D3DPTFILTERCAPS_MIPFPOINT |
        D3DPTFILTERCAPS_MIPFLINEAR |
        D3DPTFILTERCAPS_MAGFPOINT |
        D3DPTFILTERCAPS_MAGFLINEAR |
        D3DPIPECAP(ANISOTROPIC_FILTER, D3DPTFILTERCAPS_MAGFANISOTROPIC) |
        /*D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD |*/
        /*D3DPTFILTERCAPS_MAGFGAUSSIANQUAD*/0;
	
	/*
	 * Texture-filtering capabilities for a cube texture. 
	 * Per-stage filtering capabilities reflect which filtering modes are supported for texture stages when performing multiple-texture blending.
	 */
	pCaps->CubeTextureFilterCaps = 
		pCaps->TextureFilterCaps;
	
	/*
	 * Texture-filtering capabilities for a volume texture. 
	 * Per-stage filtering capabilities reflect which filtering modes are supported for texture stages when performing multiple-texture blending.
	 */
    pCaps->VolumeTextureFilterCaps = 
		pCaps->TextureFilterCaps;
	
	/*
	 * Texture-addressing capabilities for texture objects.
	 */
	pCaps->TextureAddressCaps =
        D3DPTADDRESSCAPS_BORDER |
        D3DPTADDRESSCAPS_INDEPENDENTUV |
        D3DPTADDRESSCAPS_WRAP |
        D3DPTADDRESSCAPS_MIRROR |
        D3DPTADDRESSCAPS_CLAMP |
        D3DPIPECAP(TEXTURE_MIRROR_CLAMP, D3DPTADDRESSCAPS_MIRRORONCE);
	
	/*
	 * Texture-addressing capabilities for a volume texture.
	 */
	pCaps->VolumeTextureAddressCaps = 
		pCaps->TextureAddressCaps;
	
	/*
	 * Defines the capabilities for line-drawing primitives.
	 */
	pCaps->LineCaps =
        D3DLINECAPS_ALPHACMP |
        D3DLINECAPS_BLEND |
        D3DLINECAPS_TEXTURE |
        D3DLINECAPS_ZTEST |
        D3DLINECAPS_FOG;
	
	/*
	 * Flags specifying supported stencil-buffer operations. 
	 * Stencil operations are assumed to be valid for all three stencil-buffer operation render states (D3DRS_STENCILFAIL, D3DRS_STENCILPASS, and D3DRS_STENCILZFAIL).
	 */
	pCaps->StencilCaps =
        D3DSTENCILCAPS_KEEP |
        D3DSTENCILCAPS_ZERO |
        D3DSTENCILCAPS_REPLACE |
        D3DSTENCILCAPS_INCRSAT |
        D3DSTENCILCAPS_DECRSAT |
        D3DSTENCILCAPS_INVERT |
        D3DSTENCILCAPS_INCR |
        D3DSTENCILCAPS_DECR |
        D3DPIPECAP(TWO_SIDED_STENCIL, D3DSTENCILCAPS_TWOSIDED);
	
	/*
	 * Flexible vertex format capabilities.
	 */
	pCaps->FVFCaps =
		(D3DFVFCAPS_TEXCOORDCOUNTMASK & 0xff) |
		/*D3DFVFCAPS_DONOTSTRIPELEMENTS |*/
		D3DFVFCAPS_PSIZE;
	
	/*
	 * Combination of flags describing the texture operations supported by this device.
	 */
	pCaps->TextureOpCaps = 
		D3DTEXOPCAPS_DISABLE |
		D3DTEXOPCAPS_SELECTARG1 |
		D3DTEXOPCAPS_SELECTARG2 |
		D3DTEXOPCAPS_MODULATE |
		D3DTEXOPCAPS_MODULATE2X |
		D3DTEXOPCAPS_MODULATE4X |
		D3DTEXOPCAPS_ADD |
		D3DTEXOPCAPS_ADDSIGNED |
		D3DTEXOPCAPS_ADDSIGNED2X |
		D3DTEXOPCAPS_SUBTRACT |
		D3DTEXOPCAPS_ADDSMOOTH |
		D3DTEXOPCAPS_BLENDDIFFUSEALPHA |
		D3DTEXOPCAPS_BLENDTEXTUREALPHA |
		D3DTEXOPCAPS_BLENDFACTORALPHA |
		D3DTEXOPCAPS_BLENDTEXTUREALPHAPM |
		D3DTEXOPCAPS_BLENDCURRENTALPHA |
		D3DTEXOPCAPS_PREMODULATE |
		D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR |
		D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA |
		D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR |
		D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA |
		D3DTEXOPCAPS_BUMPENVMAP |
		D3DTEXOPCAPS_BUMPENVMAPLUMINANCE |
		D3DTEXOPCAPS_DOTPRODUCT3 |
		D3DTEXOPCAPS_MULTIPLYADD |
		D3DTEXOPCAPS_LERP;	
	
	/*
	 * This number represents the maximum range of the integer bits of the post-normalized texture coordinates.
	 * A texture coordinate is stored as a 32-bit signed integer using 27 bits to store the integer part and 5 bits for the floating point fraction.
	 * The maximum integer index, 2^27, is used to determine the maximum texture coordinate, depending on how the hardware does texture-coordinate scaling.
	 */
	pCaps->MaxTextureRepeat = 32768; //32k
	
	/*
	 * Maximum W-based depth value that the device supports.
	 * (W-based depth buffers are an alternative to z-based depth buffers.)
	 */
	pCaps->MaxVertexW = 1.0f;	
	
	/*
	 * Number of pixels to adjust the extents rectangle outward to accommodate antialiasing kernels.
	 */
	pCaps->ExtentsAdjust = 0.0f;	
	
	/*
	 * Maximum number of texture-blending stages supported in the fixed function pipeline. 
	 * This value is the number of blenders available. 
	 * In the programmable pixel pipeline, this corresponds to the number of unique texture registers used by pixel shader instructions.
	 */
	pCaps->MaxTextureBlendStages = 8;
	
	/*
	 * Vertex processing capabilities.
	 */
	pCaps->VertexProcessingCaps = 
		D3DVTXPCAPS_TEXGEN |
		/*D3DVTXPCAPS_TEXGEN_SPHEREMAP |*/
		D3DVTXPCAPS_MATERIALSOURCE7 |
		D3DVTXPCAPS_DIRECTIONALLIGHTS |
		D3DVTXPCAPS_POSITIONALLIGHTS |
		D3DVTXPCAPS_LOCALVIEWER |
		D3DVTXPCAPS_TWEENING |
		/*D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER*/0;
	
	/*
	 * Maximum number of lights that can be active simultaneously.
	 */
	pCaps->MaxActiveLights = 8; //This may need to be changed to poll the hardware in some way.
    
	/*
	 * Maximum number of user-defined clipping planes supported. This member can be 0.
	 */
	pCaps->MaxUserClipPlanes = PIPE_MAX_CLIP_PLANES;
	
	/*
	 * Maximum number of matrices that this device can apply when performing multimatrix vertex blending.
	 */
    pCaps->MaxVertexBlendMatrices = 4;
	
	/*
	 * DWORD value that specifies the maximum matrix index that can be indexed into using the per-vertex indices.
	 * The number of matrices is MaxVertexBlendMatrixIndex + 1, which is the size of the matrix palette.
	 */
    pCaps->MaxVertexBlendMatrixIndex = 7; /* D3DTS_WORLDMATRIX(0..7) */
	
	/*
	 * Maximum number of primitives for each DrawPrimitive call.
	 * If MaxPrimitiveCount is not equal to 0xffff, you can draw at most MaxPrimitiveCount primitives with each draw call.
	 * However, if MaxPrimitiveCount equals 0xffff, you can still draw at most MaxPrimitiveCount primitive, but you may also use no more than MaxPrimitiveCount unique vertices.
	 */
	pCaps->MaxPrimitiveCount = 0xFFFFF;
	
	/*
	 * Maximum size of indices supported for hardware vertex processing.
	 */
	pCaps->MaxVertexIndex = 0xFFFFF;
	
	/*
	 * Maximum stride for SetStreamSource.
	 */
	pCaps->MaxStreamStride = 0xFFFF;
	
	/*
	 * Maximum value of pixel shader arithmetic component. 
	 * This value indicates the internal range of values supported for pixel color blending operations.
	 */
	pCaps->PixelShader1xMaxValue = 8.0f; /* XXX: wine */

	/*
	 * Device driver capabilities for adaptive tessellation.
	 */
    pCaps->DevCaps2 = 
		D3DDEVCAPS2_STREAMOFFSET |
		D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET |
		D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES |
		/*D3DDEVCAPS2_DMAPNPATCH |*/
		/*D3DDEVCAPS2_ADAPTIVETESSRTPATCH |*/
		/*D3DDEVCAPS2_ADAPTIVETESSNPATCH |*/
		/*D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH*/0;

	/*
	 * This number indicates which device is the master for this subordinate.
	 * (Seems like we should poll Gallium for this information.)
	 */
    pCaps->MasterAdapterOrdinal = 0;
	
	/*
	 * This number indicates the order in which heads are referenced by the API. The value for the master adapter is always 0.
	 * (Seems like we should poll Gallium for this information.)
	 */
    pCaps->AdapterOrdinalInGroup = 0;
	
	/*
	 * Number of adapters in this adapter group (only if master). 
	 * This will be 1 for conventional adapters.
	 */
    pCaps->NumberOfAdaptersInGroup = 1;
	
	/*
	 * A combination of one or more data types contained in a vertex declaration.
	 */
	pCaps->DeclTypes = 
		D3DDTCAPS_UBYTE4 |
		D3DDTCAPS_UBYTE4N |
		D3DDTCAPS_SHORT2N |
		D3DDTCAPS_SHORT4N |
		D3DDTCAPS_USHORT2N |
		D3DDTCAPS_USHORT4N |
		D3DDTCAPS_UDEC3 |
		D3DDTCAPS_DEC3N |
		D3DDTCAPS_FLOAT16_2 |
		D3DDTCAPS_FLOAT16_4;
		
	/*
	 * Combination of constants that describe the operations supported by StretchRect.
	 */	
	pCaps->StretchRectFilterCaps = 
		D3DPTFILTERCAPS_MINFPOINT |
		D3DPTFILTERCAPS_MINFLINEAR |
		D3DPTFILTERCAPS_MAGFPOINT |
		D3DPTFILTERCAPS_MAGFLINEAR;
	
	/*
	 * Instruction predication is supported if this value is nonzero.
	 */
	pCaps->PS20Caps.Caps = 
		D3DPS20CAPS_ARBITRARYSWIZZLE |
		D3DPS20CAPS_GRADIENTINSTRUCTIONS |
		D3DPS20CAPS_PREDICATION;
	
	//capability checks.
	
	/*
	 * PIPE_CAPF_MAX_LINE_WIDTH_AA: The maximum width of a smoothed line.
	 * If the smoothed line max is greater than 0.0 then antialiased lines are supported.
	 */
	if (screen->get_paramf(screen, PIPE_CAPF_MAX_LINE_WIDTH_AA) > 0.0) 
	{
        pCaps->LineCaps |= D3DLINECAPS_ANTIALIAS;
    }
	
	/*
	 * PIPE_CAP_MAX_TEXTURE_2D_LEVELS: The maximum number of mipmap levels available for a 2D texture.
	 */
	pCaps->MaxTextureWidth = 1 << (screen->get_param(screen, PIPE_CAP_MAX_TEXTURE_2D_LEVELS) - 1);
    pCaps->MaxTextureHeight = pCaps->MaxTextureWidth;
	
	/*
	 * PIPE_CAP_MAX_TEXTURE_3D_LEVELS: The maximum number of mipmap levels available for a 3D texture.
	 */
	pCaps->MaxVolumeExtent = 1 << (screen->get_param(screen, PIPE_CAP_MAX_TEXTURE_3D_LEVELS) - 1);
		 
	/*
	 * Maximum texture aspect ratio supported by the hardware, typically a power of 2.
	 */
	pCaps->MaxTextureAspectRatio = pCaps->MaxTextureWidth;
	 
	/*
	 * PIPE_CAPF_MAX_TEXTURE_ANISOTROPY: The maximum level of anisotropy that can be applied to anisotropically filtered textures.
	 * Maximum valid value for the D3DSAMP_MAXANISOTROPY texture-stage state.
	 */
	pCaps->MaxAnisotropy = (DWORD)screen->get_paramf(screen, PIPE_CAPF_MAX_TEXTURE_ANISOTROPY);
	 	 
	/*
	 * Screen-space coordinate of the guard-band clipping region. 
	 * Coordinates inside this rectangle but outside the viewport rectangle are automatically clipped.
	 * A guard band is a rectangle that is potentially larger than the viewport (and even the render target), to which vertices can be clipped automatically by the driver.
	 */
	pCaps->GuardBandLeft = screen->get_paramf(screen,PIPE_CAPF_GUARD_BAND_LEFT);
	pCaps->GuardBandTop = screen->get_paramf(screen,PIPE_CAPF_GUARD_BAND_TOP);
	pCaps->GuardBandRight = screen->get_paramf(screen,PIPE_CAPF_GUARD_BAND_RIGHT);
	pCaps->GuardBandBottom = screen->get_paramf(screen,PIPE_CAPF_GUARD_BAND_BOTTOM);
	 
	/*
	 * PIPE_SHADER_CAP_MAX_TEXTURE_SAMPLERS: The maximum number of texture samplers.
	 * Maximum number of textures that can be simultaneously bound to the fixed-function pipeline sampler stages.
	 */
    pCaps->MaxSimultaneousTextures = screen->get_shader_param(screen,PIPE_SHADER_FRAGMENT, PIPE_SHADER_CAP_MAX_TEXTURE_SAMPLERS);
	
	/*
	 * PIPE_CAPF_MAX_POINT_WIDTH: The maximum width and height of a point.
	 * Maximum size of a point primitive. 
	 * If set to 1.0f then device does not support point size control. 
	 * The range is greater than or equal to 1.0f.
	 */
	pCaps->MaxPointSize = screen->get_paramf(screen, PIPE_CAPF_MAX_POINT_WIDTH);
	
	/*
	 * PIPE_SHADER_CAP_MAX_INPUTS: The maximum number of input registers.
	 * Maximum number of concurrent data streams for SetStreamSource. The valid range is 1 to 16.
	 */
	pCaps->MaxStreams = screen->get_shader_param(screen,PIPE_SHADER_VERTEX, PIPE_SHADER_CAP_MAX_INPUTS);
	if (pCaps->MaxStreams > 16)
	{
		pCaps->MaxStreams = 16;
	}
	
	/*
	 * PIPE_CAP_SM3: Whether the vertex shader and fragment shader support equivalent opcodes to the Shader Model 3 specification.
	 * Two numbers that represent the vertex shader main and sub versions.
	 */
	bool supportsSM3 = screen->get_param(screen, PIPE_CAP_SM3);
	pCaps->VertexShaderVersion = supportsSM3 ? D3DVS_VERSION(3,0) : D3DVS_VERSION(2,0);
	
	/*
	 * The number of vertex shader Vertex Shader Registers that are reserved for constants.
	 */
	if (!!(screen->get_shader_param(screen, PIPE_SHADER_VERTEX,PIPE_SHADER_CAP_MAX_INSTRUCTIONS))) 
	{
        pCaps->MaxVertexShaderConst = screen->get_shader_param(screen, PIPE_SHADER_VERTEX,PIPE_SHADER_CAP_MAX_CONST_BUFFERS) - 20;		
	}
	else
	{
		pCaps->MaxVertexShaderConst = 0;
	}
	
	/*
	 * Two numbers that represent the pixel shader main and sub versions.
	 */
	pCaps->PixelShaderVersion = supportsSM3 ? D3DPS_VERSION(3,0) : D3DPS_VERSION(2,0);
	
	/*
	 * PIPE_CAP_MAX_RENDER_TARGETS: The maximum number of render targets that may be bound.
	 * Number of simultaneous render targets. This number must be at least one.
	 */
	pCaps->NumSimultaneousRTs = screen->get_param(screen, PIPE_CAP_MAX_RENDER_TARGETS);
	
	
	/*
	 * Instruction predication is supported if this value is nonzero.
	 */
	pCaps->VS20Caps.Caps = D3DVS20CAPS_PREDICATION;
	
	/*
	 * PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH: The maximum nested control flow depth.
	 * Either 0 or 24, which represents the depth of the dynamic flow control instruction nesting. 
	 */
    pCaps->VS20Caps.DynamicFlowControlDepth = screen->get_shader_param(screen, PIPE_SHADER_VERTEX,PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH);
	if (pCaps->VS20Caps.DynamicFlowControlDepth > D3DVS20_MAX_DYNAMICFLOWCONTROLDEPTH)
	{
        pCaps->VS20Caps.DynamicFlowControlDepth = D3DVS20_MAX_DYNAMICFLOWCONTROLDEPTH;
	}
	else if(pCaps->VS20Caps.DynamicFlowControlDepth < D3DVS20_MIN_DYNAMICFLOWCONTROLDEPTH)
	{
		return E_FAIL;
	}
	
	/*
	 * PIPE_SHADER_CAP_MAX_TEMPS: The maximum number of temporary registers.
	 * The number of temporary registers supported.
	 */
    pCaps->VS20Caps.NumTemps =screen->get_shader_param(screen, PIPE_SHADER_VERTEX,PIPE_SHADER_CAP_MAX_TEMPS);
    if (pCaps->VS20Caps.NumTemps > D3DVS20_MAX_NUMTEMPS)
	{
        pCaps->VS20Caps.NumTemps = D3DVS20_MAX_NUMTEMPS;
	}
	else if(pCaps->VS20Caps.NumTemps < D3DVS20_MIN_NUMTEMPS)
	{
		return E_FAIL;
	}
	
	/*
	 * PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH: The maximum nested control flow depth.
	 * The depth of nesting of the loop - vs/rep - vs and call - vs/callnz bool - vs instructions.
	 */
    pCaps->VS20Caps.StaticFlowControlDepth = screen->get_shader_param(screen, PIPE_SHADER_VERTEX,PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH);
    if (pCaps->VS20Caps.StaticFlowControlDepth > D3DVS20_MAX_STATICFLOWCONTROLDEPTH)
	{
        pCaps->VS20Caps.StaticFlowControlDepth = D3DVS20_MAX_STATICFLOWCONTROLDEPTH;
	}
    else if(pCaps->VS20Caps.StaticFlowControlDepth < D3DVS20_MIN_STATICFLOWCONTROLDEPTH)
	{
		return E_FAIL;
	}

	/*
	 * PIPE_SHADER_CAP_MAX_TEX_INSTRUCTIONS: The maximum number of texture instructions.
	 * PIPE_SHADER_CAP_MAX_INSTRUCTIONS: The maximum number of instructions.
	 */
    if (screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_TEX_INSTRUCTIONS) == 
		screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_INSTRUCTIONS))
	{
		pCaps->PS20Caps.Caps |= D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT;
	}
	
	/*
	 * PIPE_SHADER_CAP_MAX_TEX_INSTRUCTIONS: The maximum number of texture instructions.
	 * PIPE_SHADER_CAP_MAX_TEX_INDIRECTIONS: The maximum number of texture indirections.
	 */	
    if (screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_TEX_INSTRUCTIONS) ==
        screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_TEX_INDIRECTIONS))
	{
        pCaps->PS20Caps.Caps |= D3DPS20CAPS_NODEPENDENTREADLIMIT;
	}

	/*
	 * PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH: The maximum nested control flow depth.
	 * Either 0 or 24, which represents the depth of the dynamic flow control instruction nesting. 
	 */	
    pCaps->PS20Caps.DynamicFlowControlDepth = screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH);
    if (pCaps->PS20Caps.DynamicFlowControlDepth > D3DPS20_MAX_DYNAMICFLOWCONTROLDEPTH)
	{
        pCaps->PS20Caps.DynamicFlowControlDepth = D3DPS20_MAX_DYNAMICFLOWCONTROLDEPTH;
	}
	else if(pCaps->PS20Caps.DynamicFlowControlDepth < D3DPS20_MIN_DYNAMICFLOWCONTROLDEPTH)
	{
		return E_FAIL;
	}
	
	/*
	 * PIPE_SHADER_CAP_MAX_TEMPS: The maximum number of temporary registers.
	 * The number of temporary registers supported.
	 */	
    pCaps->PS20Caps.NumTemps = screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_TEMPS);
    if (pCaps->PS20Caps.NumTemps > D3DPS20_MAX_NUMTEMPS)
	{
        pCaps->PS20Caps.NumTemps = D3DPS20_MAX_NUMTEMPS;
		
	}
	else if(pCaps->PS20Caps.NumTemps < D3DPS20_MIN_NUMTEMPS)
	{
		return E_FAIL;
	}
	
	/*
	 * PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH: The maximum nested control flow depth.
	 * The depth of nesting of the loop - vs/rep - vs and call - vs/callnz bool - vs instructions.
	 */	
    pCaps->PS20Caps.StaticFlowControlDepth = screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH);
    if (pCaps->PS20Caps.StaticFlowControlDepth > D3DPS20_MAX_STATICFLOWCONTROLDEPTH)
	{
        pCaps->PS20Caps.StaticFlowControlDepth = D3DPS20_MAX_STATICFLOWCONTROLDEPTH;
	}
	else if(pCaps->PS20Caps.StaticFlowControlDepth < D3DPS20_MIN_STATICFLOWCONTROLDEPTH)
	{
		return E_FAIL;
	}
	
	/*
	 * PIPE_SHADER_CAP_MAX_INSTRUCTIONS: The maximum number of instructions.
	 * The number of instruction slots supported.
	 */	
    pCaps->PS20Caps.NumInstructionSlots = screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_INSTRUCTIONS);
    if (pCaps->PS20Caps.NumInstructionSlots > D3DPS20_MAX_NUMINSTRUCTIONSLOTS)
	{
        pCaps->PS20Caps.NumInstructionSlots = D3DPS20_MAX_NUMINSTRUCTIONSLOTS;
	}
	else if(pCaps->PS20Caps.NumInstructionSlots < D3DPS20_MIN_NUMINSTRUCTIONSLOTS)
	{
		return E_FAIL;
	}		
	
	/*
	 * PIPE_SHADER_CAP_MAX_TEXTURE_SAMPLERS: The maximum number of texture samplers.
	 * Device supports vertex shader texture filter capability.
	 */
	if (screen->get_shader_param(screen, PIPE_SHADER_VERTEX,PIPE_SHADER_CAP_MAX_TEXTURE_SAMPLERS))
	{
		pCaps->VertexTextureFilterCaps = pCaps->TextureFilterCaps & ~(D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFPOINT);
	}
    else
	{
        pCaps->VertexTextureFilterCaps = 0;
	}

	/*
	 * PIPE_SHADER_CAP_MAX_INSTRUCTIONS: The maximum number of instructions.
	 * Maximum number of vertex shader instruction slots supported. The maximum value that can be set on this cap is 32768.
	 */
    pCaps->MaxVertexShader30InstructionSlots = supportsSM3 ? screen->get_shader_param(screen, PIPE_SHADER_VERTEX,PIPE_SHADER_CAP_MAX_INSTRUCTIONS) : 0;
	if (pCaps->MaxVertexShader30InstructionSlots > D3DMAX30SHADERINSTRUCTIONS)
	{
		pCaps->MaxVertexShader30InstructionSlots = D3DMAX30SHADERINSTRUCTIONS;
	}
	else if(pCaps->MaxVertexShader30InstructionSlots < D3DMIN30SHADERINSTRUCTIONS)
	{
		return E_FAIL;
	}
	
	/*
	 * PIPE_SHADER_CAP_MAX_INSTRUCTIONS: The maximum number of instructions.
	 * Maximum number of pixel shader instruction slots supported. The maximum value that can be set on this cap is 32768.
	 */
	pCaps->MaxPixelShader30InstructionSlots = supportsSM3 ? screen->get_shader_param(screen, PIPE_SHADER_FRAGMENT,PIPE_SHADER_CAP_MAX_INSTRUCTIONS) : 0;
    if (pCaps->MaxPixelShader30InstructionSlots > D3DMAX30SHADERINSTRUCTIONS)
	{
        pCaps->MaxPixelShader30InstructionSlots = D3DMAX30SHADERINSTRUCTIONS;
	}
	else if(pCaps->MaxPixelShader30InstructionSlots < D3DMIN30SHADERINSTRUCTIONS)
	{
		return E_FAIL;
	}
	
	/*
	 * Maximum number of vertex shader instructions that can be run when using flow control. 
	 * (May need some way to calculate true limit and handle cases where limit is less than 65535)
	 */
	pCaps->MaxVShaderInstructionsExecuted = MAX2(65535, pCaps->MaxVertexShader30InstructionSlots * 32);
	
	/*
	 * Maximum number of pixel shader instructions that can be run when using flow control.
	 * (May need some way to calculate true limit and handle cases where limit is less than 65535)
	 */
    pCaps->MaxPShaderInstructionsExecuted = MAX2(65535, pCaps->MaxPixelShader30InstructionSlots * 32);
	
	return D3D_OK;
}