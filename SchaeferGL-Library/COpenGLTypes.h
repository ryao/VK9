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

#ifndef COPENGLTYPES_H
#define COPENGLTYPES_H

class GLMContext;
struct GLMRect;
struct GLMShaderPairInfo;
class CGLMBuffer;
class CGLMQuery;
class CGLMTex;
class CGLMProgram;
class CGLMFBO;

class COpenGLIndexBuffer9;

#define NDEBUG 

#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <assert.h>
#include <map>
#include <functional>
#include <bitset>
#include <algorithm>    // std::find
#include <vector>      
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include "D3D9.h"

#include <boost/foreach.hpp>

enum
{
	// Standard vertex shader constants
	VERTEX_SHADER_MATH_CONSTANTS0 = 0,
	VERTEX_SHADER_MATH_CONSTANTS1 = 1,
	VERTEX_SHADER_CAMERA_POS = 2,
	VERTEX_SHADER_FLEXSCALE = 3,		// used by DX9 only!
	VERTEX_SHADER_LIGHT_INDEX = 3,		// used by DX8 only!
	VERTEX_SHADER_MODELVIEWPROJ = 4,
	VERTEX_SHADER_VIEWPROJ = 8,
	VERTEX_SHADER_MODELVIEWPROJ_THIRD_ROW = 12,
	VERTEX_SHADER_VIEWPROJ_THIRD_ROW = 13,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_10 = 14,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_11 = 15,
	VERTEX_SHADER_FOG_PARAMS = 16,
	VERTEX_SHADER_VIEWMODEL = 17,
	VERTEX_SHADER_AMBIENT_LIGHT = 21,
	VERTEX_SHADER_LIGHTS = 27,
	VERTEX_SHADER_LIGHT0_POSITION = 29,
	VERTEX_SHADER_MODULATION_COLOR = 47,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_0 = 48,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_1 = 49,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_2 = 50,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_3 = 51,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_4 = 52,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_5 = 53,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_6 = 54,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_7 = 55,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_8 = 56,
	VERTEX_SHADER_SHADER_SPECIFIC_CONST_9 = 57,
	VERTEX_SHADER_MODEL = 58,

	//
	// We reserve up through 216 for the 53 bones
	//

	// 219		ClipPlane0				|------ OpenGL will jam clip planes into these two
	// 220		ClipPlane1				|	

	VERTEX_SHADER_FLEX_WEIGHTS = 1024,
	VERTEX_SHADER_MAX_FLEX_WEIGHT_COUNT = 512,
};

#define tmZone(...) 
#define TELEMETRY_LEVEL2 NULL
#define	D3D_MAX_STREAMS	5 //9
#define MAX_D3DVERTEXELEMENTS	16
#define	DXABSTRACT_VS_PARAM_SLOTS	219
#define DXABSTRACT_VS_FIRST_BONE_SLOT VERTEX_SHADER_MODEL
//TODO: DXABSTRACT_VS_LAST_BONE_SLOT needs to be verified.
#define DXABSTRACT_VS_LAST_BONE_SLOT 216
#define DebuggerBreak() __debugbreak()
#define DXABSTRACT_BREAK_ON_ERROR() __debugbreak()
#define TOGLMETHODCALLTYPE       __stdcall
#define D3DXINLINE inline
#define MD5_DIGEST_LENGTH    16
#define GL_BATCH_PERF_CALL_TIMER
#define D3DLOCK_READONLY           0x00000010L
#define D3DLOCK_DISCARD            0x00002000L
#define D3DLOCK_NOOVERWRITE        0x00001000L
#define D3DLOCK_NOSYSLOCK          0x00000800L
#define D3DLOCK_NO_DIRTY_UPDATE     0x00008000L
#define D3DISSUE_END (1 << 0)  
#define D3DISSUE_BEGIN (1 << 1)  
#define D3DUSAGE_TEXTURE_SRGB (0x80000000L)
#define	IUNKNOWN_ALLOC_SPEW 0
#define	IUNKNOWN_ALLOC_SPEW_MARK_ALL 0	

// FP conversions to hex courtesy of http://babbage.cs.qc.cuny.edu/IEEE-754/Decimal.html
#define	CONST_DZERO		0x00000000
#define	CONST_DONE		0x3F800000
#define	CONST_D64		0x42800000
#define	DONT_KNOW_YET	0x31415926

#ifdef D3D_RSI
	#error macro collision... rename this
#else
	#define D3D_RSI(nclass,nstate,ndefval)	{ nclass, nstate, ndefval }
#endif

typedef void* VD3DHWND;
typedef void* VD3DHANDLE;
#define CUtlMap std::map
//#define CUtlHash std::hash
#define CBitVec std::bitset
#define Q_memset memset
#define V_memset memset
#define V_strncpy strncpy
#define Q_strncpy strncpy
#define V_snprintf printf
#define Q_snprintf printf
#define V_stristr strstr
#define V_stricmp strcmp
#define V_strcmp strcmp
#define V_strcpy strcpy
#define V_strlen strlen
#define Plat_DebugString printf
#define Warning printf
#define Error printf
#define ConMsg printf
#define Msg printf
#define CUtlString std::string
#define TmU64 unsigned __int64
//TODO: intp - not sure about this one.
typedef int intp;

#define COMPILE_TIME_ASSERT(expr) char constraint[expr]

#define D3D_DEVICE_VALID_MARKER 0x12EBC845
#define GL_PUBLIC_ENTRYPOINT_CHECKS( dev ) Assert( dev->GetCurrentOwnerThreadId() == ThreadGetCurrentId() ); Assert( dev->m_nValidMarker == D3D_DEVICE_VALID_MARKER );

#define SAMPLER_TYPE_2D		0
#define SAMPLER_TYPE_CUBE	1
#define SAMPLER_TYPE_3D		2
#define SAMPLER_TYPE_UNUSED	3

//Not sure what they are doing with these, something like __declspec( align( 16 ) ) maybe?
#define ALIGN16
#define ALIGN16_POST

#define Assert assert

//typedef enum _D3DTEXTUREADDRESS 
//{
//    D3DTADDRESS_WRAP            = 0,
//    D3DTADDRESS_CLAMP           = 1,
//    D3DTADDRESS_BORDER          = 2,
//    D3DTADDRESS_FORCE_DWORD     = 0x7fffffff,  
//} D3DTEXTUREADDRESS;

struct D3DStreamDesc
{
	IDirect3DVertexBuffer9	*m_vtxBuffer;
	unsigned int					m_offset;
	unsigned int					m_stride;
};

struct GLMVertexAttributeDesc			// all the info you need to do vertex setup for one attribute
{
	CGLMBuffer				*m_pBuffer;	// NULL allowed in which case m_offset is the full 32-bit pointer.. so you can draw from plain RAM if desired
	GLuint					m_nCompCount;	// comp count of the attribute (1-4)
	GLenum					m_datatype;	// data type of the attribute (GL_FLOAT, GL_UNSIGNED_BYTE, etc)
	GLuint					m_stride;
	GLuint					m_offset;	// net offset to attribute 'zero' within the buffer.
	GLuint					m_streamOffset;	// net offset to attribute 'zero' within the buffer.
	GLboolean				m_normalized;	// apply to any fixed point data that needs normalizing, esp color bytes

	inline unsigned int GetDataTypeSizeInBytes() const 
	{
		switch ( m_datatype )
		{
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
				return 1;
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			case GL_HALF_FLOAT:
				return 2;
			case GL_INT:
			case GL_FLOAT:
				return 4;
			default:
				Assert( 0 );
				break;
		}
		return 0;
	}

	inline unsigned int GetTotalAttributeSizeInBytes() const { Assert( m_nCompCount ); return m_nCompCount * GetDataTypeSizeInBytes(); }

	// may need a seed value at some point to be able to disambiguate re-lifed buffers holding same pointer
	// simpler alternative is to do shoot-down inside the vertex/index buffer free calls.
	// I'd rather not have to have each attribute fiddling a ref count on the buffer to which it refers..

//#define	EQ(fff) ( (src.fff) == (fff) )
	// test in decreasing order of likelihood of difference, but do not include the buffer revision as caller is not supplying it..
	//inline bool operator== ( const GLMVertexAttributeDesc& src ) const { return EQ( m_pBuffer ) && EQ( m_offset ) && EQ( m_stride ) && EQ( m_datatype ) && EQ( m_normalized ) && EQ( m_nCompCount ); }
//#undef EQ

	unsigned int					m_bufferRevision;	// only set in GLM context's copy, to disambiguate references that are same offset / same buffer but cross an orphan event
};

enum	GLMVertexAttributeIndex
{
	kGLMGenericAttr00 = 0,
	kGLMGenericAttr01,
	kGLMGenericAttr02,
	kGLMGenericAttr03,
	kGLMGenericAttr04,
	kGLMGenericAttr05,
	kGLMGenericAttr06,
	kGLMGenericAttr07,
	kGLMGenericAttr08,
	kGLMGenericAttr09,
	kGLMGenericAttr10,
	kGLMGenericAttr11,
	kGLMGenericAttr12,
	kGLMGenericAttr13,
	kGLMGenericAttr14,
	kGLMGenericAttr15,

	kGLMVertexAttributeIndexMax			// ideally < 32
};

struct D3DIndexDesc
{
	COpenGLIndexBuffer9	*m_idxBuffer;
};

struct IDirect3DDevice9Params
{
	UINT					m_adapter;
	D3DDEVTYPE				m_deviceType;
	VD3DHWND				m_focusWindow;
	DWORD					m_behaviorFlags;
	D3DPRESENT_PARAMETERS	m_presentationParameters;
};

//typedef enum _D3DCMPFUNC 
//{
//    D3DCMP_NEVER                = 1,
//    D3DCMP_LESS                 = 2,
//    D3DCMP_EQUAL                = 3,
//    D3DCMP_LESSEQUAL            = 4,
//    D3DCMP_GREATER              = 5,
//    D3DCMP_NOTEQUAL             = 6,
//    D3DCMP_GREATEREQUAL         = 7,
//    D3DCMP_ALWAYS               = 8,
//    D3DCMP_FORCE_DWORD          = 0x7fffffff,  
//} D3DCMPFUNC;

#define D3DCOLORWRITEENABLE_RED     (1L<<0)
#define D3DCOLORWRITEENABLE_GREEN   (1L<<1)
#define D3DCOLORWRITEENABLE_BLUE    (1L<<2)
#define D3DCOLORWRITEENABLE_ALPHA   (1L<<3)

//typedef enum _D3DCULL 
//{
//    D3DCULL_NONE                = 1,
//    D3DCULL_CW                  = 2,
//    D3DCULL_CCW                 = 3,
//    /*D3DCULL_FORCE_DWORD         = 0x7fffffff,  */
//} D3DCULL;

//typedef enum _D3DFILLMODE 
//{
//    D3DFILL_POINT               = 1,
//    D3DFILL_WIREFRAME           = 2,
//    D3DFILL_SOLID               = 3,
//    D3DFILL_FORCE_DWORD         = 0x7fffffff,  
//} D3DFILLMODE;

//typedef enum _D3DBLEND 
//{
//    D3DBLEND_ZERO               = 1,
//    D3DBLEND_ONE                = 2,
//    D3DBLEND_SRCCOLOR           = 3,
//    D3DBLEND_INVSRCCOLOR        = 4,
//    D3DBLEND_SRCALPHA           = 5,
//    D3DBLEND_INVSRCALPHA        = 6,
//    D3DBLEND_DESTALPHA          = 7,
//    D3DBLEND_INVDESTALPHA       = 8,
//    D3DBLEND_DESTCOLOR          = 9,
//    D3DBLEND_INVDESTCOLOR       = 10,
//    D3DBLEND_SRCALPHASAT        = 11,
//    D3DBLEND_BOTHSRCALPHA       = 12,
//    D3DBLEND_BOTHINVSRCALPHA    = 13,
//    D3DBLEND_BLENDFACTOR        = 14,  
//    D3DBLEND_FORCE_DWORD        = 0x7fffffff,  
//} D3DBLEND;

//typedef enum _D3DSTENCILOP 
//{
//    D3DSTENCILOP_KEEP           = 1,
//    D3DSTENCILOP_ZERO           = 2,
//	D3DSTENCILOP_REPLACE		= 3,
//    D3DSTENCILOP_INCRSAT        = 4,
//    D3DSTENCILOP_DECRSAT        = 5,
//    D3DSTENCILOP_INVERT         = 6,
//    D3DSTENCILOP_INCR           = 7,
//    D3DSTENCILOP_DECR           = 8,
//    D3DSTENCILOP_FORCE_DWORD    = 0x7fffffff,  
//} D3DSTENCILOP;

struct D3DVERTEXELEMENT9_GL
{
	// fields right out of the original decl element (copied)
	D3DVERTEXELEMENT9		m_dxdecl;	// d3d info
	//		WORD    Stream;     // Stream index
	//		WORD    Offset;     // Offset in the stream in bytes
	//		BYTE    Type;       // Data type
	//		BYTE    Method;     // Processing method
	//		BYTE    Usage;      // Semantics
	//		BYTE    UsageIndex; // Semantic index

	GLMVertexAttributeDesc	m_gldecl;
	// CGLMBuffer				*m_buffer;		// late-dropped from selected stream desc (left NULL, will replace with stream source buffer at sync time)
	// GLuint					m_datasize;		// component count (1,2,3,4) of the attrib
	// GLenum					m_datatype;		// data type of the attribute (GL_FLOAT et al)
	// GLuint					m_stride;		// late-dropped from stream desc
	// GLuint					m_offset;		// net offset to attribute 'zero' within the stream data.  Add the stream offset before passing to GL. 
	// GLuint					m_normalized;	// net offset to attribute 'zero' within the stream data.  Add the stream offset before passing to GL. 
};

//typedef enum _D3DBLENDOP 
//{
//    D3DBLENDOP_ADD              = 1,
//    D3DBLENDOP_SUBTRACT         = 2,
//    D3DBLENDOP_REVSUBTRACT      = 3,
//    D3DBLENDOP_MIN              = 4,
//    D3DBLENDOP_MAX              = 5,
//    D3DBLENDOP_FORCE_DWORD      = 0x7fffffff,  
//} D3DBLENDOP;

//typedef enum _D3DDECLUSAGE
//{
//    D3DDECLUSAGE_POSITION		= 0,
//    D3DDECLUSAGE_BLENDWEIGHT	= 1,
//    D3DDECLUSAGE_BLENDINDICES	= 2,
//    D3DDECLUSAGE_NORMAL			= 3,
//    D3DDECLUSAGE_PSIZE			= 4,
//    D3DDECLUSAGE_TEXCOORD		= 5,
//    D3DDECLUSAGE_TANGENT		= 6,
//    D3DDECLUSAGE_BINORMAL		= 7,
//    D3DDECLUSAGE_TESSFACTOR		= 8,
//    D3DDECLUSAGE_PLUGH			= 9,	 
//    D3DDECLUSAGE_COLOR			= 10,
//    D3DDECLUSAGE_FOG			= 11,
//    D3DDECLUSAGE_DEPTH			= 12,
//    D3DDECLUSAGE_SAMPLE			= 13,
//} D3DDECLUSAGE;

//typedef enum _D3DSHADER_INSTRUCTION_OPCODE_TYPE
//{
//    D3DSIO_NOP          = 0,
//    D3DSIO_MOV          ,
//    D3DSIO_ADD          ,
//    D3DSIO_SUB          ,
//    D3DSIO_MAD          ,
//    D3DSIO_MUL          ,
//    D3DSIO_RCP          ,
//    D3DSIO_RSQ          ,
//    D3DSIO_DP3          ,
//    D3DSIO_DP4          ,
//    D3DSIO_MIN          ,	 
//    D3DSIO_MAX          ,
//    D3DSIO_SLT          ,
//    D3DSIO_SGE          ,
//    D3DSIO_EXP          ,
//    D3DSIO_LOG          ,
//    D3DSIO_LIT          ,
//    D3DSIO_DST          ,
//    D3DSIO_LRP          ,
//    D3DSIO_FRC          ,
//    D3DSIO_M4x4         ,	 
//    D3DSIO_M4x3         ,
//    D3DSIO_M3x4         ,
//    D3DSIO_M3x3         ,
//    D3DSIO_M3x2         ,
//    D3DSIO_CALL         ,
//    D3DSIO_CALLNZ       ,
//    D3DSIO_LOOP         ,
//    D3DSIO_RET          ,
//    D3DSIO_ENDLOOP      ,
//    D3DSIO_LABEL        ,	 
//    D3DSIO_DCL          ,
//    D3DSIO_POW          ,
//    D3DSIO_CRS          ,
//    D3DSIO_SGN          ,
//    D3DSIO_ABS          ,
//    D3DSIO_NRM          ,
//    D3DSIO_SINCOS       ,
//    D3DSIO_REP          ,
//    D3DSIO_ENDREP       ,
//    D3DSIO_IF           ,	 
//    D3DSIO_IFC          ,
//    D3DSIO_ELSE         ,
//    D3DSIO_ENDIF        ,
//    D3DSIO_BREAK        ,
//    D3DSIO_BREAKC       ,
//    D3DSIO_MOVA         ,
//    D3DSIO_DEFB         ,
//    D3DSIO_DEFI         ,
//
//    D3DSIO_TEXCOORD     = 64,
//    D3DSIO_TEXKILL      ,
//    D3DSIO_TEX          ,
//    D3DSIO_TEXBEM       ,
//    D3DSIO_TEXBEML      ,
//    D3DSIO_TEXREG2AR    ,
//    D3DSIO_TEXREG2GB    ,
//    D3DSIO_TEXM3x2PAD   ,
//    D3DSIO_TEXM3x2TEX   ,
//    D3DSIO_TEXM3x3PAD   ,
//    D3DSIO_TEXM3x3TEX   ,
//    D3DSIO_RESERVED0    ,
//    D3DSIO_TEXM3x3SPEC  ,
//    D3DSIO_TEXM3x3VSPEC ,
//    D3DSIO_EXPP         ,
//    D3DSIO_LOGP         ,
//    D3DSIO_CND          ,
//    D3DSIO_DEF          ,
//    D3DSIO_TEXREG2RGB   ,
//    D3DSIO_TEXDP3TEX    ,
//    D3DSIO_TEXM3x2DEPTH ,
//    D3DSIO_TEXDP3       ,
//    D3DSIO_TEXM3x3      ,
//    D3DSIO_TEXDEPTH     ,
//    D3DSIO_CMP          ,
//    D3DSIO_BEM          ,
//    D3DSIO_DP2ADD       ,
//    D3DSIO_DSX          ,
//    D3DSIO_DSY          ,
//    D3DSIO_TEXLDD       ,
//    D3DSIO_SETP         ,
//    D3DSIO_TEXLDL       ,
//    D3DSIO_BREAKP       ,
//
//    D3DSIO_PHASE        = 0xFFFD,
//    D3DSIO_COMMENT      = 0xFFFE,
//    D3DSIO_END          = 0xFFFF,
//
//    D3DSIO_FORCE_DWORD  = 0x7fffffff,    
//} D3DSHADER_INSTRUCTION_OPCODE_TYPE;

//typedef enum _D3DMATERIALCOLORSOURCE
//{
//    D3DMCS_MATERIAL         = 0,             
//    D3DMCS_COLOR1           = 1,             
//    D3DMCS_COLOR2           = 2,             
//    D3DMCS_FORCE_DWORD      = 0x7fffffff,    
//} D3DMATERIALCOLORSOURCE;

//typedef enum _D3DVERTEXBLENDFLAGS
//{
//    D3DVBF_DISABLE  = 0,      
//    D3DVBF_1WEIGHTS = 1,      
//    D3DVBF_2WEIGHTS = 2,      
//    D3DVBF_3WEIGHTS = 3,      
//    D3DVBF_TWEENING = 255,    
//    D3DVBF_0WEIGHTS = 256,    
//    D3DVBF_FORCE_DWORD = 0x7fffffff,  
//} D3DVERTEXBLENDFLAGS;

//typedef enum _D3DPATCHEDGESTYLE
//{
//   D3DPATCHEDGE_DISCRETE    = 0,
//   D3DPATCHEDGE_CONTINUOUS  = 1,
//   D3DPATCHEDGE_FORCE_DWORD = 0x7fffffff,
//} D3DPATCHEDGESTYLE;

//typedef enum _D3DFOGMODE 
//{
//    D3DFOG_NONE                 = 0,
//    D3DFOG_LINEAR               = 3,
//    D3DFOG_FORCE_DWORD          = 0x7fffffff,  
//} D3DFOGMODE;

//typedef enum _D3DSHADEMODE 
//{
//    D3DSHADE_FLAT               = 1,
//    D3DSHADE_GOURAUD            = 2,
//    D3DSHADE_PHONG              = 3,
//    D3DSHADE_FORCE_DWORD        = 0x7fffffff,  
//} D3DSHADEMODE;

//typedef enum _D3DDEBUGMONITORTOKENS 
//{
//    D3DDMT_ENABLE            = 0,     
//} D3DDEBUGMONITORTOKENS;

struct RenderTargetState_t
{
	void clear() { V_memset( this, 0, sizeof( *this ) ); }

	CGLMTex *m_pRenderTargets[4];
	CGLMTex *m_pDepthStencil;

	inline bool RefersTo( CGLMTex * pSurf ) const
	{
		for ( unsigned int i = 0; i < 4; i++ )
			if ( m_pRenderTargets[i] == pSurf )
				return true;

		if ( m_pDepthStencil == pSurf )
			return true;

		return false;
	}

	static inline bool LessFunc( const RenderTargetState_t &lhs, const RenderTargetState_t &rhs ) 
	{
		COMPILE_TIME_ASSERT( sizeof( lhs.m_pRenderTargets[0] ) == sizeof( unsigned __int32 ) );
		unsigned __int64 lhs0 = reinterpret_cast<const unsigned __int64 *>(lhs.m_pRenderTargets)[0];
		unsigned __int64 rhs0 = reinterpret_cast<const unsigned __int64 *>(rhs.m_pRenderTargets)[0];
		if ( lhs0 < rhs0 )
			return true;
		else if ( lhs0 == rhs0 )
		{
			unsigned __int64 lhs1 = reinterpret_cast<const unsigned __int64 *>(lhs.m_pRenderTargets)[1];
			unsigned __int64 rhs1 = reinterpret_cast<const unsigned __int64 *>(rhs.m_pRenderTargets)[1];
			if ( lhs1 < rhs1 )
				return true;
			else if ( lhs1 == rhs1 )
			{
				return lhs.m_pDepthStencil < rhs.m_pDepthStencil;
			}
		}
		return false;
	}

	inline bool operator < ( const RenderTargetState_t &rhs ) const
	{
		return LessFunc( *this, rhs );
	}
};

struct RenderTargetStateCompare
{
  bool operator()(const RenderTargetState_t &lhs, const RenderTargetState_t &rhs) const
  {
	  return RenderTargetState_t::LessFunc(lhs, rhs);
  }
};

typedef CUtlMap< RenderTargetState_t, CGLMFBO *, RenderTargetStateCompare> CGLMFBOMap;

#define	D3DRS_VALUE_LIMIT 210

struct	D3D_RSINFO
{
	int					m_class;
	D3DRENDERSTATETYPE	m_state;
	DWORD				m_defval;
	// m_class runs 0-3.
	// 3 = must implement - fully general - "obey"
	// 2 = implement setup to the default value (it has a GL effect but does not change later) "obey once"
	// 1 = "fake implement" setup to the default value no GL effect, debug break if anything but default value comes through - "ignore"
	// 0 = game never ever sets this one, break if someone even tries. "complain"
};

D3D_RSINFO g_D3DRS_INFO_unpacked[ D3DRS_VALUE_LIMIT+1 ];

inline int MIN(int a,int b)
{
	return b<a?b:a;
}

inline int MAX(int a,int b)
{
	return b<a?a:b;
}

class ILauncherMgr
{

} _g_pLauncherMgr;

ILauncherMgr* g_pLauncherMgr = &_g_pLauncherMgr;

//use boost implementation for iteration. item and container are reversed in BOOST_FOREACH.
//#define FOR_EACH_MAP BOOST_FOREACH
//#define FOR_EACH_MAP_FAST BOOST_FOREACH

//This method includes a length to compare. (presumably as an optimization)
inline int V_strncmp(const char *s1, const char *s2, int count)
{
	return memcmp(s1,s2,count); //Using memcmp because the length is known.
}

#endif // COPENGLTYPES_H
