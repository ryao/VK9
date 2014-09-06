//========= Copyright Christopher Joseph Dean Schaefer, All rights reserved. ============//
//                       SchaeferGL CODE LICENSE
//
//  Copyright 2014 Christopher Joseph Dean Schaefer
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
//
//==================================================================================================
 
#ifndef TYPES_H
#define TYPES_H

#include <windows.h>

typedef __int64 int64_t;
typedef __int32 int32_t;
typedef __int16 int16_t;
typedef __int8 int8_t;

#if ! (defined _GUID_DEFINED || defined GUID_DEFINED) /* also defined in winnt.h */
#define GUID_DEFINED
typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID,*REFGUID,*LPGUID;
#endif /* GUID_DEFINED */

typedef unsigned long ULONG;
typedef signed long LONG;
typedef unsigned int UINT;
typedef int INT;
typedef unsigned char BYTE;
typedef int BOOL;
typedef float FLOAT;
#define VOID void
#define VOID void

typedef LONG HRESULT;
typedef GUID IID;
//typedef IID *REFIID;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef DWORD D3DCOLOR;
typedef void *PVOID;
typedef PVOID HANDLE;
//typedef HANDLE HDC;
//typedef HANDLE HWND;

#define MAKEFOURCC(c0,c1,c2,c3) ((DWORD)(BYTE)(c0)|((DWORD)(BYTE)(c1)<<8)|((DWORD)(BYTE)(c2)<<16)|((DWORD)(BYTE)(c3)<<24))
#define MAKE_HRESULT(sev,fac,code) ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )

/*
typedef struct tagPOINT {
  LONG x;
  LONG y;
} POINT, *PPOINT;
*/

/*
typedef struct _RECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT;
*/

typedef struct D3DRECT {
  LONG x1;
  LONG y1;
  LONG x2;
  LONG y2;
} D3DRECT;

typedef struct D3DLOCKED_RECT {
  INT  Pitch;
  void *pBits;
} D3DLOCKED_RECT, *LPD3DLOCKED_RECT;

typedef enum D3DRESOURCETYPE { 
  D3DRTYPE_SURFACE        = 1,
  D3DRTYPE_VOLUME         = 2,
  D3DRTYPE_TEXTURE        = 3,
  D3DRTYPE_VOLUMETEXTURE  = 4,
  D3DRTYPE_CubeTexture    = 5,
  D3DRTYPE_VERTEXBUFFER   = 6,
  D3DRTYPE_INDEXBUFFER    = 7,
  D3DRTYPE_FORCE_DWORD    = 0x7fffffff
} D3DRESOURCETYPE, *LPD3DRESOURCETYPE;

typedef enum _D3DFORMAT {
    D3DFMT_UNKNOWN              =  0,
    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,
    D3DFMT_A2B10G10R10          = 31,
    D3DFMT_A8B8G8R8             = 32,
    D3DFMT_X8B8G8R8             = 33,
    D3DFMT_G16R16               = 34,
    D3DFMT_A2R10G10B10          = 35,
    D3DFMT_A16B16G16R16         = 36,
    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,
    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,
    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_A2W10V10U10          = 67,
    D3DFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
    D3DFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
    D3DFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),
    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_D16                  = 80,
    D3DFMT_D32F_LOCKABLE        = 82,
    D3DFMT_D24FS8               = 83,
    D3DFMT_D32_LOCKABLE         = 84,
    D3DFMT_S8_LOCKABLE          = 85,
    D3DFMT_L16                  = 81,
    D3DFMT_VERTEXDATA           =100,
    D3DFMT_INDEX16              =101,
    D3DFMT_INDEX32              =102,
    D3DFMT_Q16W16V16U16         =110,
    D3DFMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),
    D3DFMT_R16F                 = 111,
    D3DFMT_G16R16F              = 112,
    D3DFMT_A16B16G16R16F        = 113,
    D3DFMT_R32F                 = 114,
    D3DFMT_G32R32F              = 115,
    D3DFMT_A32B32G32R32F        = 116,
    D3DFMT_CxV8U8               = 117,
    D3DFMT_A1                   = 118,
    D3DFMT_A2B10G10R10_XR_BIAS  = 119,
    D3DFMT_BINARYBUFFER         = 199,
    D3DFMT_FORCE_DWORD          =0x7fffffff,

		D3DFMT_NV_INTZ		= 0x5a544e49,	// MAKEFOURCC('I','N','T','Z')
		D3DFMT_NV_RAWZ		= 0x5a574152,	// MAKEFOURCC('R','A','W','Z')

		// NV null tex
		D3DFMT_NV_NULL		= 0x4c4c554e,	// MAKEFOURCC('N','U','L','L')

		// ATI shadow depth tex
		D3DFMT_ATI_D16		= 0x36314644,	// MAKEFOURCC('D','F','1','6')
		D3DFMT_ATI_D24S8	= 0x34324644,	// MAKEFOURCC('D','F','2','4')

		// ATI 1N and 2N compressed tex
		D3DFMT_ATI_2N		= 0x32495441,	// MAKEFOURCC('A', 'T', 'I', '2')
		D3DFMT_ATI_1N		= 0x31495441,	// MAKEFOURCC('A', 'T', 'I', '1')

} D3DFORMAT;

typedef enum D3DPOOL { 
  D3DPOOL_DEFAULT      = 0,
  D3DPOOL_MANAGED      = 1,
  D3DPOOL_SYSTEMMEM    = 2,
  D3DPOOL_SCRATCH      = 3,
  D3DPOOL_FORCE_DWORD  = 0x7fffffff
} D3DPOOL, *LPD3DPOOL;

typedef enum D3DMULTISAMPLE_TYPE { 
  D3DMULTISAMPLE_NONE          = 0,
  D3DMULTISAMPLE_NONMASKABLE   = 1,
  D3DMULTISAMPLE_2_SAMPLES     = 2,
  D3DMULTISAMPLE_3_SAMPLES     = 3,
  D3DMULTISAMPLE_4_SAMPLES     = 4,
  D3DMULTISAMPLE_5_SAMPLES     = 5,
  D3DMULTISAMPLE_6_SAMPLES     = 6,
  D3DMULTISAMPLE_7_SAMPLES     = 7,
  D3DMULTISAMPLE_8_SAMPLES     = 8,
  D3DMULTISAMPLE_9_SAMPLES     = 9,
  D3DMULTISAMPLE_10_SAMPLES    = 10,
  D3DMULTISAMPLE_11_SAMPLES    = 11,
  D3DMULTISAMPLE_12_SAMPLES    = 12,
  D3DMULTISAMPLE_13_SAMPLES    = 13,
  D3DMULTISAMPLE_14_SAMPLES    = 14,
  D3DMULTISAMPLE_15_SAMPLES    = 15,
  D3DMULTISAMPLE_16_SAMPLES    = 16,
  D3DMULTISAMPLE_FORCE_DWORD   = 0xffffffff
} D3DMULTISAMPLE_TYPE, *LPD3DMULTISAMPLE_TYPE;

typedef struct D3DSURFACE_DESC {
  D3DFORMAT           Format;
  D3DRESOURCETYPE     Type;
  DWORD               Usage;
  D3DPOOL             Pool;
  D3DMULTISAMPLE_TYPE MultiSampleType;
  DWORD               MultiSampleQuality;
  UINT                Width;
  UINT                Height;
} D3DSURFACE_DESC, *LPD3DSURFACE_DESC;

typedef enum D3DSWAPEFFECT { 
  D3DSWAPEFFECT_DISCARD      = 1,
  D3DSWAPEFFECT_FLIP         = 2,
  D3DSWAPEFFECT_COPY         = 3,
  D3DSWAPEFFECT_OVERLAY      = 4,
  D3DSWAPEFFECT_FLIPEX       = 5,
  D3DSWAPEFFECT_FORCE_DWORD  = 0xFFFFFFFF
} D3DSWAPEFFECT, *LPD3DSWAPEFFECT;

typedef struct D3DPRESENT_PARAMETERS {
  UINT                BackBufferWidth;
  UINT                BackBufferHeight;
  D3DFORMAT           BackBufferFormat;
  UINT                BackBufferCount;
  D3DMULTISAMPLE_TYPE MultiSampleType;
  DWORD               MultiSampleQuality;
  D3DSWAPEFFECT       SwapEffect;
  HWND                hDeviceWindow;
  BOOL                Windowed;
  BOOL                EnableAutoDepthStencil;
  D3DFORMAT           AutoDepthStencilFormat;
  DWORD               Flags;
  UINT                FullScreen_RefreshRateInHz;
  UINT                PresentationInterval;
} D3DPRESENT_PARAMETERS, *LPD3DPRESENT_PARAMETERS;

typedef enum D3DBACKBUFFER_TYPE { 
  D3DBACKBUFFER_TYPE_MONO         = 0,
  D3DBACKBUFFER_TYPE_LEFT         = 1,
  D3DBACKBUFFER_TYPE_RIGHT        = 2,
  D3DBACKBUFFER_TYPE_FORCE_DWORD  = 0x7fffffff
} D3DBACKBUFFER_TYPE, *LPD3DBACKBUFFER_TYPE;

typedef struct D3DDISPLAYMODE {
  UINT      Width;
  UINT      Height;
  UINT      RefreshRate;
  D3DFORMAT Format;
} D3DDISPLAYMODE, *LPD3DDISPLAYMODE;

typedef struct D3DRASTER_STATUS {
  BOOL InVBlank;
  UINT ScanLine;
} D3DRASTER_STATUS, *LPD3DRASTER_STATUS;

/*
typedef struct _RGNDATAHEADER {
  DWORD dwSize;
  DWORD iType;
  DWORD nCount;
  DWORD nRgnSize;
  RECT  rcBound;
} RGNDATAHEADER, *PRGNDATAHEADER;
*/

/*
typedef struct _RGNDATA {
  RGNDATAHEADER rdh;
  char          Buffer[1];
} RGNDATA, *PRGNDATA;
*/

typedef enum D3DTEXTUREFILTERTYPE { 
  D3DTEXF_NONE             = 0,
  D3DTEXF_POINT            = 1,
  D3DTEXF_LINEAR           = 2,
  D3DTEXF_ANISOTROPIC      = 3,
  D3DTEXF_PYRAMIDALQUAD    = 6,
  D3DTEXF_GAUSSIANQUAD     = 7,
  D3DTEXF_CONVOLUTIONMONO  = 8,
  D3DTEXF_FORCE_DWORD      = 0x7fffffff
} D3DTEXTUREFILTERTYPE, *LPD3DTEXTUREFILTERTYPE;

typedef enum D3DCUBEMAP_FACES { 
  D3DCUBEMAP_FACE_POSITIVE_X   = 0,
  D3DCUBEMAP_FACE_NEGATIVE_X   = 1,
  D3DCUBEMAP_FACE_POSITIVE_Y   = 2,
  D3DCUBEMAP_FACE_NEGATIVE_Y   = 3,
  D3DCUBEMAP_FACE_POSITIVE_Z   = 4,
  D3DCUBEMAP_FACE_NEGATIVE_Z   = 5,
  D3DCUBEMAP_FACE_FORCE_DWORD  = 0xffffffff
} D3DCUBEMAP_FACES, *LPD3DCUBEMAP_FACES;

typedef struct D3DINDEXBUFFER_DESC {
  D3DFORMAT       Format;
  D3DRESOURCETYPE Type;
  DWORD           Usage;
  D3DPOOL         Pool;
  UINT            Size;
} D3DINDEXBUFFER_DESC, *LPD3DINDEXBUFFER_DESC;

typedef enum D3DQUERYTYPE { 
  D3DQUERYTYPE_VCACHE             = 4,
  D3DQUERYTYPE_ResourceManager    = 5,
  D3DQUERYTYPE_VERTEXSTATS        = 6,
  D3DQUERYTYPE_EVENT              = 8,
  D3DQUERYTYPE_OCCLUSION          = 9,
  D3DQUERYTYPE_TIMESTAMP          = 10,
  D3DQUERYTYPE_TIMESTAMPDISJOINT  = 11,
  D3DQUERYTYPE_TIMESTAMPFREQ      = 12,
  D3DQUERYTYPE_PIPELINETIMINGS    = 13,
  D3DQUERYTYPE_INTERFACETIMINGS   = 14,
  D3DQUERYTYPE_VERTEXTIMINGS      = 15,
  D3DQUERYTYPE_PIXELTIMINGS       = 16,
  D3DQUERYTYPE_BANDWIDTHTIMINGS   = 17,
  D3DQUERYTYPE_CACHEUTILIZATION   = 18,
  D3DQUERYTYPE_MEMORYPRESSURE     = 19
} D3DQUERYTYPE, *LPD3DQUERYTYPE;

typedef enum _D3DSTATEBLOCKTYPE { 
  D3DSBT_ALL          = 1,
  D3DSBT_PIXELSTATE   = 2,
  D3DSBT_VERTEXSTATE  = 3,
  D3DSBT_FORCE_DWORD  = 0x7fffffff
} D3DSTATEBLOCKTYPE;

typedef struct D3DVERTEXBUFFER_DESC {
  D3DFORMAT       Format;
  D3DRESOURCETYPE Type;
  DWORD           Usage;
  D3DPOOL         Pool;
  UINT            Size;
  DWORD           FVF;
} D3DVERTEXBUFFER_DESC, *LPD3DVERTEXBUFFER_DESC;

typedef struct D3DVERTEXELEMENT9 {
  WORD Stream;
  WORD Offset;
  BYTE Type;
  BYTE Method;
  BYTE Usage;
  BYTE UsageIndex;
} D3DVERTEXELEMENT9, *LPD3DVERTEXELEMENT9;

typedef struct D3DBOX {
  UINT Left;
  UINT Top;
  UINT Right;
  UINT Bottom;
  UINT Front;
  UINT Back;
} D3DBOX, *LPD3DBOX;

typedef struct D3DLOCKED_BOX {
  int  RowPitch;
  int  SlicePitch;
  void *pBits;
} D3DLOCKED_BOX, *LPD3DLOCKED_BOX;

typedef struct D3DVOLUME_DESC {
  D3DFORMAT       Format;
  D3DRESOURCETYPE Type;
  DWORD           Usage;
  D3DPOOL         Pool;
  UINT            Width;
  UINT            Height;
  UINT            Depth;
} D3DVOLUME_DESC, *LPD3DVOLUME_DESC;

typedef enum D3DPRIMITIVETYPE { 
  D3DPT_POINTLIST      = 1,
  D3DPT_LINELIST       = 2,
  D3DPT_LINESTRIP      = 3,
  D3DPT_TRIANGLELIST   = 4,
  D3DPT_TRIANGLESTRIP  = 5,
  D3DPT_TRIANGLEFAN    = 6,
  D3DPT_FORCE_DWORD    = 0x7fffffff
} D3DPRIMITIVETYPE, *LPD3DPRIMITIVETYPE;

typedef enum D3DBASISTYPE { 
  D3DBASIS_BEZIER       = 0,
  D3DBASIS_BSPLINE      = 1,
  D3DBASIS_CATMULL_ROM  = 2,
  D3DBASIS_FORCE_DWORD  = 0x7fffffff
} D3DBASISTYPE, *LPD3DBASISTYPE;

typedef enum D3DDEGREETYPE { 
  D3DDEGREE_LINEAR     = 1,
  D3DDEGREE_QUADRATIC  = 2,
  D3DDEGREE_CUBIC      = 3,
  D3DDEGREE_QUINTIC    = 5,
  D3DCULL_FORCE_DWORD  = 0x7fffffff
} D3DDEGREETYPE, *LPD3DDEGREETYPE;

typedef struct D3DRECTPATCH_INFO {
  UINT          StartVertexOffsetWidth;
  UINT          StartVertexOffsetHeight;
  UINT          Width;
  UINT          Height;
  UINT          Stride;
  D3DBASISTYPE  Basis;
  D3DDEGREETYPE Degree;
} D3DRECTPATCH_INFO, *LPD3DRECTPATCH_INFO;

typedef struct D3DTRIPATCH_INFO {
  UINT          StartVertexOffset;
  UINT          NumVertices;
  D3DBASISTYPE  Basis;
  D3DDEGREETYPE Degree;
} D3DTRIPATCH_INFO, *LPD3DTRIPATCH_INFO;

typedef struct D3DCLIPSTATUS9 {
  DWORD ClipUnion;
  DWORD ClipIntersection;
} D3DCLIPSTATUS9, *LPD3DCLIPSTATUS9;

typedef enum D3DDEVTYPE { 
  D3DDEVTYPE_HAL          = 1,
  D3DDEVTYPE_NULLREF      = 4,
  D3DDEVTYPE_REF          = 2,
  D3DDEVTYPE_SW           = 3,
  D3DDEVTYPE_FORCE_DWORD  = 0xffffffff
} D3DDEVTYPE, *LPD3DDEVTYPE;

typedef struct D3DDEVICE_CREATION_PARAMETERS {
  UINT       AdapterOrdinal;
  D3DDEVTYPE DeviceType;
  HWND       hFocusWindow;
  DWORD      BehaviorFlags;
} D3DDEVICE_CREATION_PARAMETERS, *LPD3DDEVICE_CREATION_PARAMETERS;

typedef struct D3DVSHADERCAPS2_0 {
  DWORD Caps;
  INT   DynamicFlowControlDepth;
  INT   NumTemps;
  INT   StaticFlowControlDepth;
} D3DVSHADERCAPS2_0, *LPD3DVSHADERCAPS2_0;

typedef struct D3DPSHADERCAPS2_0 {
  DWORD Caps;
  INT   DynamicFlowControlDepth;
  INT   NumTemps;
  INT   StaticFlowControlDepth;
  INT   NumInstructionSlots;
} D3DPSHADERCAPS2_0, *LPD3DPSHADERCAPS2_0;

typedef struct D3DCAPS9 {
  D3DDEVTYPE        DeviceType;
  UINT              AdapterOrdinal;
  DWORD             Caps;
  DWORD             Caps2;
  DWORD             Caps3;
  DWORD             PresentationIntervals;
  DWORD             CursorCaps;
  DWORD             DevCaps;
  DWORD             PrimitiveMiscCaps;
  DWORD             RasterCaps;
  DWORD             ZCmpCaps;
  DWORD             SrcBlendCaps;
  DWORD             DestBlendCaps;
  DWORD             AlphaCmpCaps;
  DWORD             ShadeCaps;
  DWORD             TextureCaps;
  DWORD             TextureFilterCaps;
  DWORD             CubeTextureFilterCaps;
  DWORD             VolumeTextureFilterCaps;
  DWORD             TextureAddressCaps;
  DWORD             VolumeTextureAddressCaps;
  DWORD             LineCaps;
  DWORD             MaxTextureWidth;
  DWORD             MaxTextureHeight;
  DWORD             MaxVolumeExtent;
  DWORD             MaxTextureRepeat;
  DWORD             MaxTextureAspectRatio;
  DWORD             MaxAnisotropy;
  float             MaxVertexW;
  float             GuardBandLeft;
  float             GuardBandTop;
  float             GuardBandRight;
  float             GuardBandBottom;
  float             ExtentsAdjust;
  DWORD             StencilCaps;
  DWORD             FVFCaps;
  DWORD             TextureOpCaps;
  DWORD             MaxTextureBlendStages;
  DWORD             MaxSimultaneousTextures;
  DWORD             VertexProcessingCaps;
  DWORD             MaxActiveLights;
  DWORD             MaxUserClipPlanes;
  DWORD             MaxVertexBlendMatrices;
  DWORD             MaxVertexBlendMatrixIndex;
  float             MaxPointSize;
  DWORD             MaxPrimitiveCount;
  DWORD             MaxVertexIndex;
  DWORD             MaxStreams;
  DWORD             MaxStreamStride;
  DWORD             VertexShaderVersion;
  DWORD             MaxVertexShaderConst;
  DWORD             PixelShaderVersion;
  float             PixelShader1xMaxValue;
  DWORD             DevCaps2;
  UINT              MasterAdapterOrdinal;
  UINT              AdapterOrdinalInGroup;
  UINT              NumberOfAdaptersInGroup;
  DWORD             DeclTypes;
  DWORD             NumSimultaneousRTs;
  DWORD             StretchRectFilterCaps;
  D3DVSHADERCAPS2_0 VS20Caps;
  D3DPSHADERCAPS2_0 PS20Caps;
  DWORD             VertexTextureFilterCaps;
  DWORD             MaxVShaderInstructionsExecuted;
  DWORD             MaxPShaderInstructionsExecuted;
  DWORD             MaxVertexShader30InstructionSlots;
  DWORD             MaxPixelShader30InstructionSlots;
  DWORD             Reserved2;
  DWORD             Reserved3;
} D3DCAPS9, *LPD3DCAPS9;

typedef struct D3DGAMMARAMP {
  WORD red[256];
  WORD green[256];
  WORD blue[256];
} D3DGAMMARAMP, *LPD3DGAMMARAMP;

typedef enum D3DLIGHTTYPE { 
  D3DLIGHT_POINT        = 1,
  D3DLIGHT_SPOT         = 2,
  D3DLIGHT_DIRECTIONAL  = 3,
  D3DLIGHT_FORCE_DWORD  = 0x7fffffff
} D3DLIGHTTYPE, *LPD3DLIGHTTYPE;

typedef struct _D3DCOLORVALUE {
  float r;
  float g;
  float b;
  float a;
} D3DCOLORVALUE;

typedef struct D3DVECTOR {
  float x;
  float y;
  float z;
} D3DVECTOR;

typedef struct D3DLIGHT9 {
  D3DLIGHTTYPE  Type;
  D3DCOLORVALUE Diffuse;
  D3DCOLORVALUE Specular;
  D3DCOLORVALUE Ambient;
  D3DVECTOR     Position;
  D3DVECTOR     Direction;
  float         Range;
  float         Falloff;
  float         Attenuation0;
  float         Attenuation1;
  float         Attenuation2;
  float         Theta;
  float         Phi;
} D3DLIGHT9, *LPD3DLIGHT;

typedef struct D3DMATERIAL9 {
  D3DCOLORVALUE Diffuse;
  D3DCOLORVALUE Ambient;
  D3DCOLORVALUE Specular;
  D3DCOLORVALUE Emissive;
  float         Power;
} D3DMATERIAL9, *LPD3DMATERIAL9;

/*
typedef struct PALETTEENTRY {
  BYTE peRed;
  BYTE peGreen;
  BYTE peBlue;
  BYTE peFlags;
} PALETTEENTRY, *LPPALETTEENTRY;
*/

typedef enum D3DRENDERSTATETYPE { 
  D3DRS_ZENABLE                     = 7,
  D3DRS_FILLMODE                    = 8,
  D3DRS_SHADEMODE                   = 9,
  D3DRS_ZWRITEENABLE                = 14,
  D3DRS_ALPHATESTENABLE             = 15,
  D3DRS_LASTPIXEL                   = 16,
  D3DRS_SRCBLEND                    = 19,
  D3DRS_DESTBLEND                   = 20,
  D3DRS_CULLMODE                    = 22,
  D3DRS_ZFUNC                       = 23,
  D3DRS_ALPHAREF                    = 24,
  D3DRS_ALPHAFUNC                   = 25,
  D3DRS_DITHERENABLE                = 26,
  D3DRS_ALPHABLENDENABLE            = 27,
  D3DRS_FOGENABLE                   = 28,
  D3DRS_SPECULARENABLE              = 29,
  D3DRS_FOGCOLOR                    = 34,
  D3DRS_FOGTABLEMODE                = 35,
  D3DRS_FOGSTART                    = 36,
  D3DRS_FOGEND                      = 37,
  D3DRS_FOGDENSITY                  = 38,
  D3DRS_RANGEFOGENABLE              = 48,
  D3DRS_STENCILENABLE               = 52,
  D3DRS_STENCILFAIL                 = 53,
  D3DRS_STENCILZFAIL                = 54,
  D3DRS_STENCILPASS                 = 55,
  D3DRS_STENCILFUNC                 = 56,
  D3DRS_STENCILREF                  = 57,
  D3DRS_STENCILMASK                 = 58,
  D3DRS_STENCILWRITEMASK            = 59,
  D3DRS_TEXTUREFACTOR               = 60,
  D3DRS_WRAP0                       = 128,
  D3DRS_WRAP1                       = 129,
  D3DRS_WRAP2                       = 130,
  D3DRS_WRAP3                       = 131,
  D3DRS_WRAP4                       = 132,
  D3DRS_WRAP5                       = 133,
  D3DRS_WRAP6                       = 134,
  D3DRS_WRAP7                       = 135,
  D3DRS_CLIPPING                    = 136,
  D3DRS_LIGHTING                    = 137,
  D3DRS_AMBIENT                     = 139,
  D3DRS_FOGVERTEXMODE               = 140,
  D3DRS_COLORVERTEX                 = 141,
  D3DRS_LOCALVIEWER                 = 142,
  D3DRS_NORMALIZENORMALS            = 143,
  D3DRS_DIFFUSEMATERIALSOURCE       = 145,
  D3DRS_SPECULARMATERIALSOURCE      = 146,
  D3DRS_AMBIENTMATERIALSOURCE       = 147,
  D3DRS_EMISSIVEMATERIALSOURCE      = 148,
  D3DRS_VERTEXBLEND                 = 151,
  D3DRS_CLIPPLANEENABLE             = 152,
  D3DRS_POINTSIZE                   = 154,
  D3DRS_POINTSIZE_MIN               = 155,
  D3DRS_POINTSPRITEENABLE           = 156,
  D3DRS_POINTSCALEENABLE            = 157,
  D3DRS_POINTSCALE_A                = 158,
  D3DRS_POINTSCALE_B                = 159,
  D3DRS_POINTSCALE_C                = 160,
  D3DRS_MULTISAMPLEANTIALIAS        = 161,
  D3DRS_MULTISAMPLEMASK             = 162,
  D3DRS_PATCHEDGESTYLE              = 163,
  D3DRS_DEBUGMONITORTOKEN           = 165,
  D3DRS_POINTSIZE_MAX               = 166,
  D3DRS_INDEXEDVERTEXBLENDENABLE    = 167,
  D3DRS_COLORWRITEENABLE            = 168,
  D3DRS_TWEENFACTOR                 = 170,
  D3DRS_BLENDOP                     = 171,
  D3DRS_POSITIONDEGREE              = 172,
  D3DRS_NORMALDEGREE                = 173,
  D3DRS_SCISSORTESTENABLE           = 174,
  D3DRS_SLOPESCALEDEPTHBIAS         = 175,
  D3DRS_ANTIALIASEDLINEENABLE       = 176,
  D3DRS_MINTESSELLATIONLEVEL        = 178,
  D3DRS_MAXTESSELLATIONLEVEL        = 179,
  D3DRS_ADAPTIVETESS_X              = 180,
  D3DRS_ADAPTIVETESS_Y              = 181,
  D3DRS_ADAPTIVETESS_Z              = 182,
  D3DRS_ADAPTIVETESS_W              = 183,
  D3DRS_ENABLEADAPTIVETESSELLATION  = 184,
  D3DRS_TWOSIDEDSTENCILMODE         = 185,
  D3DRS_CCW_STENCILFAIL             = 186,
  D3DRS_CCW_STENCILZFAIL            = 187,
  D3DRS_CCW_STENCILPASS             = 188,
  D3DRS_CCW_STENCILFUNC             = 189,
  D3DRS_COLORWRITEENABLE1           = 190,
  D3DRS_COLORWRITEENABLE2           = 191,
  D3DRS_COLORWRITEENABLE3           = 192,
  D3DRS_BLENDFACTOR                 = 193,
  D3DRS_SRGBWRITEENABLE             = 194,
  D3DRS_DEPTHBIAS                   = 195,
  D3DRS_WRAP8                       = 198,
  D3DRS_WRAP9                       = 199,
  D3DRS_WRAP10                      = 200,
  D3DRS_WRAP11                      = 201,
  D3DRS_WRAP12                      = 202,
  D3DRS_WRAP13                      = 203,
  D3DRS_WRAP14                      = 204,
  D3DRS_WRAP15                      = 205,
  D3DRS_SEPARATEALPHABLENDENABLE    = 206,
  D3DRS_SRCBLENDALPHA               = 207,
  D3DRS_DESTBLENDALPHA              = 208,
  D3DRS_BLENDOPALPHA                = 209,
  D3DRS_FORCE_DWORD                 = 0x7fffffff
} D3DRENDERSTATETYPE, *LPD3DRENDERSTATETYPE;

typedef enum D3DSAMPLERSTATETYPE { 
  D3DSAMP_ADDRESSU       = 1,
  D3DSAMP_ADDRESSV       = 2,
  D3DSAMP_ADDRESSW       = 3,
  D3DSAMP_BORDERCOLOR    = 4,
  D3DSAMP_MAGFILTER      = 5,
  D3DSAMP_MINFILTER      = 6,
  D3DSAMP_MIPFILTER      = 7,
  D3DSAMP_MIPMAPLODBIAS  = 8,
  D3DSAMP_MAXMIPLEVEL    = 9,
  D3DSAMP_MAXANISOTROPY  = 10,
  D3DSAMP_SRGBTEXTURE    = 11,
  D3DSAMP_ELEMENTINDEX   = 12,
  D3DSAMP_DMAPOFFSET     = 13,
  D3DSAMP_FORCE_DWORD    = 0x7fffffff
} D3DSAMPLERSTATETYPE, *LPD3DSAMPLERSTATETYPE;

typedef enum D3DTEXTURESTAGESTATETYPE { 
  D3DTSS_COLOROP                = 1,
  D3DTSS_COLORARG1              = 2,
  D3DTSS_COLORARG2              = 3,
  D3DTSS_ALPHAOP                = 4,
  D3DTSS_ALPHAARG1              = 5,
  D3DTSS_ALPHAARG2              = 6,
  D3DTSS_BUMPENVMAT00           = 7,
  D3DTSS_BUMPENVMAT01           = 8,
  D3DTSS_BUMPENVMAT10           = 9,
  D3DTSS_BUMPENVMAT11           = 10,
  D3DTSS_TEXCOORDINDEX          = 11,
  D3DTSS_BUMPENVLSCALE          = 22,
  D3DTSS_BUMPENVLOFFSET         = 23,
  D3DTSS_TEXTURETRANSFORMFLAGS  = 24,
  D3DTSS_COLORARG0              = 26,
  D3DTSS_ALPHAARG0              = 27,
  D3DTSS_RESULTARG              = 28,
  D3DTSS_CONSTANT               = 32,
  D3DTSS_FORCE_DWORD            = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE, *LPD3DTEXTURESTAGESTATETYPE;

typedef enum D3DTRANSFORMSTATETYPE { 
  D3DTS_VIEW         = 2,
  D3DTS_PROJECTION   = 3,
  D3DTS_TEXTURE0     = 16,
  D3DTS_TEXTURE1     = 17,
  D3DTS_TEXTURE2     = 18,
  D3DTS_TEXTURE3     = 19,
  D3DTS_TEXTURE4     = 20,
  D3DTS_TEXTURE5     = 21,
  D3DTS_TEXTURE6     = 22,
  D3DTS_TEXTURE7     = 23,
  D3DTS_FORCE_DWORD  = 0x7fffffff
} D3DTRANSFORMSTATETYPE, *LPD3DTRANSFORMSTATETYPE;

typedef struct _D3DMATRIX {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
    };
} D3DMATRIX;

typedef struct D3DVIEWPORT9 {
  DWORD X;
  DWORD Y;
  DWORD Width;
  DWORD Height;
  float MinZ;
  float MaxZ;
} D3DVIEWPORT9, *LPD3DVIEWPORT9;

typedef int64_t LONGLONG; 

/*
typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  };
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
*/

#define MAX_DEVICE_IDENTIFIER_STRING        512

typedef struct D3DADAPTER_IDENTIFIER9 {
  char          Driver[MAX_DEVICE_IDENTIFIER_STRING];
  char          Description[MAX_DEVICE_IDENTIFIER_STRING];
  char          DeviceName[32];
  LARGE_INTEGER DriverVersion;
  DWORD         DriverVersionLowPart;
  DWORD         DriverVersionHighPart;
  DWORD         VendorId;
  DWORD         DeviceId;
  DWORD         SubSysId;
  DWORD         Revision;
  GUID          DeviceIdentifier;
  DWORD         WHQLLevel;
} D3DADAPTER_IDENTIFIER9, *LPD3DADAPTER_IDENTIFIER9;

/*
typedef HANDLE HMONITOR;
*/

#define D3D_SDK_VERSION    31
#define D3DCREATE_FPU_PRESERVE    0x02
#define D3DCREATE_MULTITHREADED    0x04
#define D3DCREATE_PUREDEVICE    0x10
#define D3DCREATE_SOFTWARE_VERTEXPROCESSING    0x20
#define D3DCREATE_HARDWARE_VERTEXPROCESSING    0x40
#define D3DCREATE_MIXED_VERTEXPROCESSING    0x80
#define D3DSPD_IUNKNOWN    1
#define D3DSGR_NO_CALIBRATION    0
#define D3DSGR_CALIBRATE    1
#define MAKE_D3DHRESULT(code)    MAKE_HRESULT(1,0x876,code)
#define MAKE_D3DSTATUS(code)    MAKE_HRESULT(0,0x876,code)
#define D3D_OK    0
#define D3DOK_NOAUTOGEN    MAKE_D3DSTATUS(2159)
#define D3DERR_WRONGTEXTUREFORMAT    MAKE_D3DHRESULT(2072)
#define D3DERR_UNSUPPORTEDCOLOROPERATION    MAKE_D3DHRESULT(2073)
#define D3DERR_UNSUPPORTEDCOLORARG    MAKE_D3DHRESULT(2074)
#define D3DERR_UNSUPPORTEDALPHAOPERATION    MAKE_D3DHRESULT(2075)
#define D3DERR_UNSUPPORTEDALPHAARG    MAKE_D3DHRESULT(2076)
#define D3DERR_TOOMANYOPERATIONS    MAKE_D3DHRESULT(2077)
#define D3DERR_CONFLICTINGTEXTUREFILTER    MAKE_D3DHRESULT(2078)
#define D3DERR_UNSUPPORTEDFACTORVALUE    MAKE_D3DHRESULT(2079)
#define D3DERR_CONFLICTINGRENDERSTATE    MAKE_D3DHRESULT(2081)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER    MAKE_D3DHRESULT(2082)
#define D3DERR_CONFLICTINGTEXTUREPALETTE    MAKE_D3DHRESULT(2086)
#define D3DERR_DRIVERINTERNALERROR    MAKE_D3DHRESULT(2087)
#define D3DERR_NOTFOUND    MAKE_D3DHRESULT(2150)
#define D3DERR_MOREDATA    MAKE_D3DHRESULT(2151)
#define D3DERR_DEVICELOST    MAKE_D3DHRESULT(2152)
#define D3DERR_DEVICENOTRESET    MAKE_D3DHRESULT(2153)
#define D3DERR_NOTAVAILABLE    MAKE_D3DHRESULT(2154)
#define D3DERR_OUTOFVIDEOMEMORY    MAKE_D3DHRESULT(380)
#define D3DERR_INVALIDDEVICE    MAKE_D3DHRESULT(2155)
#define D3DERR_INVALIDCALL    MAKE_D3DHRESULT(2156)
#define D3DERR_DRIVERINVALIDCALL    MAKE_D3DHRESULT(2157)
#define D3DERR_WASSTILLDRAWING    MAKE_D3DHRESULT(540)
#define D3DADAPTER_DEFAULT    0
#define D3DCURSOR_IMMEDIATE_UPDATE    1
#define D3DENUM_HOST_ADAPTER    1
#define D3DPRESENTFLAG_LOCKABLE_BACKBUFFER    1
#define D3DPV_DONOTCOPYDATA    1
#define D3DENUM_NO_WHQL_LEVEL    2
#define D3DPRESENT_BACK_BUFFERS_MAX    3
#define VALID_D3DENUM_FLAGS    3
#define D3DMAXNUMPRIMITIVES    0xFFFF
#define D3DMAXNUMVERTICES    0xFFFF
#define D3DCURRENT_DISPLAY_MODE    0xEFFFFF

#define S_OK 0x00000000
#define E_ABORT 0x80004004
#define E_ACCESSDENIED 0x80070005
#define E_FAIL 0x80004005
#define E_HANDLE 0x80070006
#define E_INVALIDARG 0x80070057
#define E_NOINTERFACE 0x80004002
#define E_NOTIMPL 0x80004001
#define E_OUTOFMEMORY 0x8007000E
#define E_POINTER 0x80004003
#define E_UNEXPECTED 0x8000FFFF

//caps 
#define D3DCAPS_READ_SCANLINE                           0x00020000
#define D3DCAPS2_FULLSCREENGAMMA                        0x00020000
#define D3DCAPS2_CANCALIBRATEGAMMA                      0x00100000
#define D3DCAPS2_RESERVED                               0x02000000
#define D3DCAPS2_CANMANAGERESOURCE                      0x10000000
#define D3DCAPS2_DYNAMICTEXTURES                        0x20000000
#define D3DCAPS2_CANAUTOGENMIPMAP                       0x40000000
#define D3DCAPS3_RESERVED                               0x8000001F
#define D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD       0x00000020
#define D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION            0x00000080
#define D3DCAPS3_COPY_TO_VIDMEM                         0x00000100
#define D3DCAPS3_COPY_TO_SYSTEMMEM                      0x00000200
#define D3DPRESENT_INTERVAL_DEFAULT                     0x00000000
#define D3DPRESENT_INTERVAL_ONE                         0x00000001
#define D3DPRESENT_INTERVAL_TWO                         0x00000002
#define D3DPRESENT_INTERVAL_THREE                       0x00000004
#define D3DPRESENT_INTERVAL_FOUR                        0x00000008
#define D3DPRESENT_INTERVAL_IMMEDIATE                   0x80000000
#define D3DCURSORCAPS_COLOR                             0x00000001
#define D3DCURSORCAPS_LOWRES                            0x00000002
#define D3DDEVCAPS_EXECUTESYSTEMMEMORY                  0x00000010
#define D3DDEVCAPS_EXECUTEVIDEOMEMORY                   0x00000020
#define D3DDEVCAPS_TLVERTEXSYSTEMMEMORY                 0x00000040
#define D3DDEVCAPS_TLVERTEXVIDEOMEMORY                  0x00000080
#define D3DDEVCAPS_TEXTURESYSTEMMEMORY                  0x00000100
#define D3DDEVCAPS_TEXTUREVIDEOMEMORY                   0x00000200
#define D3DDEVCAPS_DRAWPRIMTLVERTEX                     0x00000400
#define D3DDEVCAPS_CANRENDERAFTERFLIP                   0x00000800
#define D3DDEVCAPS_TEXTURENONLOCALVIDMEM                0x00001000
#define D3DDEVCAPS_DRAWPRIMITIVES2                      0x00002000
#define D3DDEVCAPS_SEPARATETEXTUREMEMORIES              0x00004000
#define D3DDEVCAPS_DRAWPRIMITIVES2EX                    0x00008000
#define D3DDEVCAPS_HWTRANSFORMANDLIGHT                  0x00010000
#define D3DDEVCAPS_CANBLTSYSTONONLOCAL                  0x00020000
#define D3DDEVCAPS_HWRASTERIZATION                      0x00080000
#define D3DDEVCAPS_PUREDEVICE                           0x00100000
#define D3DDEVCAPS_QUINTICRTPATCHES                     0x00200000
#define D3DDEVCAPS_RTPATCHES                            0x00400000
#define D3DDEVCAPS_RTPATCHHANDLEZERO                    0x00800000
#define D3DDEVCAPS_NPATCHES                             0x01000000
#define D3DPMISCCAPS_MASKZ                              0x00000002
#define D3DPMISCCAPS_CULLNONE                           0x00000010
#define D3DPMISCCAPS_CULLCW                             0x00000020
#define D3DPMISCCAPS_CULLCCW                            0x00000040
#define D3DPMISCCAPS_COLORWRITEENABLE                   0x00000080
#define D3DPMISCCAPS_CLIPPLANESCALEDPOINTS              0x00000100
#define D3DPMISCCAPS_CLIPTLVERTS                        0x00000200
#define D3DPMISCCAPS_TSSARGTEMP                         0x00000400
#define D3DPMISCCAPS_BLENDOP                            0x00000800
#define D3DPMISCCAPS_NULLREFERENCE                      0x00001000
#define D3DPMISCCAPS_INDEPENDENTWRITEMASKS              0x00004000
#define D3DPMISCCAPS_PERSTAGECONSTANT                   0x00008000
#define D3DPMISCCAPS_FOGANDSPECULARALPHA                0x00010000
#define D3DPMISCCAPS_SEPARATEALPHABLEND                 0x00020000
#define D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS            0x00040000
#define D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING         0x00080000
#define D3DPMISCCAPS_FOGVERTEXCLAMPED                   0x00100000
#define D3DLINECAPS_TEXTURE                             0x00000001
#define D3DLINECAPS_ZTEST                               0x00000002
#define D3DLINECAPS_BLEND                               0x00000004
#define D3DLINECAPS_ALPHACMP                            0x00000008
#define D3DLINECAPS_FOG                                 0x00000010
#define D3DLINECAPS_ANTIALIAS                           0x00000020
#define D3DPRASTERCAPS_DITHER                           0x00000001
#define D3DPRASTERCAPS_ZTEST                            0x00000010
#define D3DPRASTERCAPS_FOGVERTEX                        0x00000080
#define D3DPRASTERCAPS_FOGTABLE                         0x00000100
#define D3DPRASTERCAPS_MIPMAPLODBIAS                    0x00002000
#define D3DPRASTERCAPS_ZBUFFERLESSHSR                   0x00008000
#define D3DPRASTERCAPS_FOGRANGE                         0x00010000
#define D3DPRASTERCAPS_ANISOTROPY                       0x00020000
#define D3DPRASTERCAPS_WBUFFER                          0x00040000
#define D3DPRASTERCAPS_WFOG                             0x00100000
#define D3DPRASTERCAPS_ZFOG                             0x00200000
#define D3DPRASTERCAPS_COLORPERSPECTIVE                 0x00400000
#define D3DPRASTERCAPS_SCISSORTEST                      0x01000000
#define D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS              0x02000000
#define D3DPRASTERCAPS_DEPTHBIAS                        0x04000000
#define D3DPRASTERCAPS_MULTISAMPLE_TOGGLE               0x08000000
#define D3DPCMPCAPS_NEVER                               0x00000001
#define D3DPCMPCAPS_LESS                                0x00000002
#define D3DPCMPCAPS_EQUAL                               0x00000004
#define D3DPCMPCAPS_LESSEQUAL                           0x00000008
#define D3DPCMPCAPS_GREATER                             0x00000010
#define D3DPCMPCAPS_NOTEQUAL                            0x00000020
#define D3DPCMPCAPS_GREATEREQUAL                        0x00000040
#define D3DPCMPCAPS_ALWAYS                              0x00000080
#define D3DPBLENDCAPS_ZERO                              0x00000001
#define D3DPBLENDCAPS_ONE                               0x00000002
#define D3DPBLENDCAPS_SRCCOLOR                          0x00000004
#define D3DPBLENDCAPS_INVSRCCOLOR                       0x00000008
#define D3DPBLENDCAPS_SRCALPHA                          0x00000010
#define D3DPBLENDCAPS_INVSRCALPHA                       0x00000020
#define D3DPBLENDCAPS_DESTALPHA                         0x00000040
#define D3DPBLENDCAPS_INVDESTALPHA                      0x00000080
#define D3DPBLENDCAPS_DESTCOLOR                         0x00000100
#define D3DPBLENDCAPS_INVDESTCOLOR                      0x00000200
#define D3DPBLENDCAPS_SRCALPHASAT                       0x00000400
#define D3DPBLENDCAPS_BOTHSRCALPHA                      0x00000800
#define D3DPBLENDCAPS_BOTHINVSRCALPHA                   0x00001000
#define D3DPBLENDCAPS_BLENDFACTOR                       0x00002000
#define D3DPSHADECAPS_COLORGOURAUDRGB                   0x00000008
#define D3DPSHADECAPS_SPECULARGOURAUDRGB                0x00000200
#define D3DPSHADECAPS_ALPHAGOURAUDBLEND                 0x00004000
#define D3DPSHADECAPS_FOGGOURAUD                        0x00080000
#define D3DPTEXTURECAPS_PERSPECTIVE                     0x00000001
#define D3DPTEXTURECAPS_POW2                            0x00000002
#define D3DPTEXTURECAPS_ALPHA                           0x00000004
#define D3DPTEXTURECAPS_SQUAREONLY                      0x00000020
#define D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE        0x00000040
#define D3DPTEXTURECAPS_ALPHAPALETTE                    0x00000080
#define D3DPTEXTURECAPS_NONPOW2CONDITIONAL              0x00000100
#define D3DPTEXTURECAPS_PROJECTED                       0x00000400
#define D3DPTEXTURECAPS_CUBEMAP                         0x00000800
#define D3DPTEXTURECAPS_VOLUMEMAP                       0x00002000
#define D3DPTEXTURECAPS_MIPMAP                          0x00004000
#define D3DPTEXTURECAPS_MIPVOLUMEMAP                    0x00008000
#define D3DPTEXTURECAPS_MIPCUBEMAP                      0x00010000
#define D3DPTEXTURECAPS_CUBEMAP_POW2                    0x00020000
#define D3DPTEXTURECAPS_VOLUMEMAP_POW2                  0x00040000
#define D3DPTEXTURECAPS_NOPROJECTEDBUMPENV              0x00200000
#define D3DPTFILTERCAPS_MINFPOINT                       0x00000100
#define D3DPTFILTERCAPS_MINFLINEAR                      0x00000200
#define D3DPTFILTERCAPS_MINFANISOTROPIC                 0x00000400
#define D3DPTFILTERCAPS_MINFPYRAMIDALQUAD               0x00000800
#define D3DPTFILTERCAPS_MINFGAUSSIANQUAD                0x00001000
#define D3DPTFILTERCAPS_MIPFPOINT                       0x00010000
#define D3DPTFILTERCAPS_MIPFLINEAR                      0x00020000
#define D3DPTFILTERCAPS_MAGFPOINT                       0x01000000
#define D3DPTFILTERCAPS_MAGFLINEAR                      0x02000000
#define D3DPTFILTERCAPS_MAGFANISOTROPIC                 0x04000000
#define D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD               0x08000000
#define D3DPTFILTERCAPS_MAGFGAUSSIANQUAD                0x10000000
#define D3DPTADDRESSCAPS_WRAP                           0x00000001
#define D3DPTADDRESSCAPS_MIRROR                         0x00000002
#define D3DPTADDRESSCAPS_CLAMP                          0x00000004
#define D3DPTADDRESSCAPS_BORDER                         0x00000008
#define D3DPTADDRESSCAPS_INDEPENDENTUV                  0x00000010
#define D3DPTADDRESSCAPS_MIRRORONCE                     0x00000020
#define D3DSTENCILCAPS_KEEP                             0x00000001
#define D3DSTENCILCAPS_ZERO                             0x00000002
#define D3DSTENCILCAPS_REPLACE                          0x00000004
#define D3DSTENCILCAPS_INCRSAT                          0x00000008
#define D3DSTENCILCAPS_DECRSAT                          0x00000010
#define D3DSTENCILCAPS_INVERT                           0x00000020
#define D3DSTENCILCAPS_INCR                             0x00000040
#define D3DSTENCILCAPS_DECR                             0x00000080
#define D3DSTENCILCAPS_TWOSIDED                         0x00000100
#define D3DTEXOPCAPS_DISABLE                            0x00000001
#define D3DTEXOPCAPS_SELECTARG1                         0x00000002
#define D3DTEXOPCAPS_SELECTARG2                         0x00000004
#define D3DTEXOPCAPS_MODULATE                           0x00000008
#define D3DTEXOPCAPS_MODULATE2X                         0x00000010
#define D3DTEXOPCAPS_MODULATE4X                         0x00000020
#define D3DTEXOPCAPS_ADD                                0x00000040
#define D3DTEXOPCAPS_ADDSIGNED                          0x00000080
#define D3DTEXOPCAPS_ADDSIGNED2X                        0x00000100
#define D3DTEXOPCAPS_SUBTRACT                           0x00000200
#define D3DTEXOPCAPS_ADDSMOOTH                          0x00000400
#define D3DTEXOPCAPS_BLENDDIFFUSEALPHA                  0x00000800
#define D3DTEXOPCAPS_BLENDTEXTUREALPHA                  0x00001000
#define D3DTEXOPCAPS_BLENDFACTORALPHA                   0x00002000
#define D3DTEXOPCAPS_BLENDTEXTUREALPHAPM                0x00004000
#define D3DTEXOPCAPS_BLENDCURRENTALPHA                  0x00008000
#define D3DTEXOPCAPS_PREMODULATE                        0x00010000
#define D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR             0x00020000
#define D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA             0x00040000
#define D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR          0x00080000
#define D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA          0x00100000
#define D3DTEXOPCAPS_BUMPENVMAP                         0x00200000
#define D3DTEXOPCAPS_BUMPENVMAPLUMINANCE                0x00400000
#define D3DTEXOPCAPS_DOTPRODUCT3                        0x00800000
#define D3DTEXOPCAPS_MULTIPLYADD                        0x01000000
#define D3DTEXOPCAPS_LERP                               0x02000000
#define D3DFVFCAPS_TEXCOORDCOUNTMASK                    0x0000FFFF
#define D3DFVFCAPS_DONOTSTRIPELEMENTS                   0x00080000
#define D3DFVFCAPS_PSIZE                                0x00100000
#define D3DVTXPCAPS_TEXGEN                              0x00000001
#define D3DVTXPCAPS_MATERIALSOURCE7                     0x00000002
#define D3DVTXPCAPS_DIRECTIONALLIGHTS                   0x00000008
#define D3DVTXPCAPS_POSITIONALLIGHTS                    0x00000010
#define D3DVTXPCAPS_LOCALVIEWER                         0x00000020
#define D3DVTXPCAPS_TWEENING                            0x00000040
#define D3DVTXPCAPS_TEXGEN_SPHEREMAP                    0x00000100
#define D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER            0x00000200
#define D3DDEVCAPS2_STREAMOFFSET                        0x00000001
#define D3DDEVCAPS2_DMAPNPATCH                          0x00000002
#define D3DDEVCAPS2_ADAPTIVETESSRTPATCH                 0x00000004
#define D3DDEVCAPS2_ADAPTIVETESSNPATCH                  0x00000008
#define D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES       0x00000010
#define D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH                0x00000020
#define D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET  0x00000040
#define D3DDTCAPS_UBYTE4                                0x00000001
#define D3DDTCAPS_UBYTE4N                               0x00000002
#define D3DDTCAPS_SHORT2N                               0x00000004
#define D3DDTCAPS_SHORT4N                               0x00000008
#define D3DDTCAPS_USHORT2N                              0x00000010
#define D3DDTCAPS_USHORT4N                              0x00000020
#define D3DDTCAPS_UDEC3                                 0x00000040
#define D3DDTCAPS_DEC3N                                 0x00000080
#define D3DDTCAPS_FLOAT16_2                             0x00000100
#define D3DDTCAPS_FLOAT16_4                             0x00000200
#define D3DVS20CAPS_PREDICATION                         (1<<0)
#define D3DVS20_MAX_DYNAMICFLOWCONTROLDEPTH             24
#define D3DVS20_MIN_DYNAMICFLOWCONTROLDEPTH             0
#define D3DVS20_MAX_NUMTEMPS                            32
#define D3DVS20_MIN_NUMTEMPS                            12
#define D3DVS20_MAX_STATICFLOWCONTROLDEPTH              4
#define D3DVS20_MIN_STATICFLOWCONTROLDEPTH              1
#define D3DPS20CAPS_ARBITRARYSWIZZLE                    (1<<0)
#define D3DPS20CAPS_GRADIENTINSTRUCTIONS                (1<<1)
#define D3DPS20CAPS_PREDICATION                         (1<<2)
#define D3DPS20CAPS_NODEPENDENTREADLIMIT                (1<<3)
#define D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT               (1<<4)
#define D3DPS20_MAX_DYNAMICFLOWCONTROLDEPTH             24
#define D3DPS20_MIN_DYNAMICFLOWCONTROLDEPTH             0
#define D3DPS20_MAX_NUMTEMPS                            32
#define D3DPS20_MIN_NUMTEMPS                            12
#define D3DPS20_MAX_STATICFLOWCONTROLDEPTH              4
#define D3DPS20_MIN_STATICFLOWCONTROLDEPTH              0
#define D3DPS20_MAX_NUMINSTRUCTIONSLOTS                 512
#define D3DPS20_MIN_NUMINSTRUCTIONSLOTS                 96
#define D3DMIN30SHADERINSTRUCTIONS                      512
#define D3DMAX30SHADERINSTRUCTIONS                      32768

//9ex caps
#define D3DPBLENDCAPS_SRCCOLOR2         0x00004000L
#define D3DPBLENDCAPS_INVSRCCOLOR2      0x00008000L

#define D3DPS_VERSION(major, minor) (0xFFFF0000 | ((major) << 8) | (minor))
#define D3DVS_VERSION(major, minor) (0xFFFE0000 | ((major) << 8) | (minor))
#define D3DSHADER_VERSION_MAJOR(version) (((version) >> 8) & 0xFF)
#define D3DSHADER_VERSION_MINOR(version) (((version) >> 0) & 0xFF)

//more
#define D3DUSAGE_RENDERTARGET	0x01
#define D3DUSAGE_DEPTHSTENCIL	0x02
#define D3DUSAGE_WRITEONLY	0x08
#define D3DUSAGE_SOFTWAREPROCESSING	0x10
#define D3DUSAGE_DONOTCLIP	0x20
#define D3DUSAGE_POINTS	0x40
#define D3DUSAGE_RTPATCHES	0x80
#define D3DUSAGE_NPATCHES	0x100
#define D3DUSAGE_DYNAMIC	0x200
#define D3DUSAGE_AUTOGENMIPMAP	0x400
#define D3DUSAGE_DMAP	0x4000
#define D3DUSAGE_QUERY_LEGACYBUMPMAP	0x8000
#define D3DUSAGE_QUERY_SRGBREAD	0x10000
#define D3DUSAGE_QUERY_FILTER	0x20000
#define D3DUSAGE_QUERY_SRGBWRITE	0x40000
#define D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING	0x80000
#define D3DUSAGE_QUERY_VERTEXTEXTURE	0x100000

#endif // TYPES