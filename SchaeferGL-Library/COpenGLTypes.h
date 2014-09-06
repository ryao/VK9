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
#include <assert.h>
#include <map>
#include <functional>
#include <bitset>
#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include "D3D9.h"

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
typedef void* VD3DHWND;
typedef void* VD3DHANDLE;
#define CUtlMap std::map
//#define CUtlHash std::hash
#define CBitVec std::bitset

//TODO: intp - not sure about this one.
typedef int intp;

#define D3D_DEVICE_VALID_MARKER 0x12EBC845
#define GL_PUBLIC_ENTRYPOINT_CHECKS( dev ) Assert( dev->GetCurrentOwnerThreadId() == ThreadGetCurrentId() ); Assert( dev->m_nValidMarker == D3D_DEVICE_VALID_MARKER );

#define SAMPLER_TYPE_2D		0
#define SAMPLER_TYPE_CUBE	1
#define SAMPLER_TYPE_3D		2
#define SAMPLER_TYPE_UNUSED	3

//#define V_snprintf vsnprintf
#define V_strncpy strncpy
#define Q_strncpy strncpy

//Not sure what they are doing with these, something like __declspec( align( 16 ) ) maybe?
#define ALIGN16
#define ALIGN16_POST

#define Assert assert

typedef enum _D3DTEXTUREADDRESS 
{
    D3DTADDRESS_WRAP            = 0,
    D3DTADDRESS_CLAMP           = 1,
    D3DTADDRESS_BORDER          = 2,
    D3DTADDRESS_FORCE_DWORD     = 0x7fffffff,  
} D3DTEXTUREADDRESS;

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

typedef enum _D3DCMPFUNC 
{
    D3DCMP_NEVER                = 1,
    D3DCMP_LESS                 = 2,
    D3DCMP_EQUAL                = 3,
    D3DCMP_LESSEQUAL            = 4,
    D3DCMP_GREATER              = 5,
    D3DCMP_NOTEQUAL             = 6,
    D3DCMP_GREATEREQUAL         = 7,
    D3DCMP_ALWAYS               = 8,
    D3DCMP_FORCE_DWORD          = 0x7fffffff,  
} D3DCMPFUNC;

#define D3DCOLORWRITEENABLE_RED     (1L<<0)
#define D3DCOLORWRITEENABLE_GREEN   (1L<<1)
#define D3DCOLORWRITEENABLE_BLUE    (1L<<2)
#define D3DCOLORWRITEENABLE_ALPHA   (1L<<3)

typedef enum _D3DCULL 
{
    D3DCULL_NONE                = 1,
    D3DCULL_CW                  = 2,
    D3DCULL_CCW                 = 3,
    /*D3DCULL_FORCE_DWORD         = 0x7fffffff,  */
} D3DCULL;

typedef enum _D3DFILLMODE 
{
    D3DFILL_POINT               = 1,
    D3DFILL_WIREFRAME           = 2,
    D3DFILL_SOLID               = 3,
    D3DFILL_FORCE_DWORD         = 0x7fffffff,  
} D3DFILLMODE;

typedef enum _D3DBLEND 
{
    D3DBLEND_ZERO               = 1,
    D3DBLEND_ONE                = 2,
    D3DBLEND_SRCCOLOR           = 3,
    D3DBLEND_INVSRCCOLOR        = 4,
    D3DBLEND_SRCALPHA           = 5,
    D3DBLEND_INVSRCALPHA        = 6,
    D3DBLEND_DESTALPHA          = 7,
    D3DBLEND_INVDESTALPHA       = 8,
    D3DBLEND_DESTCOLOR          = 9,
    D3DBLEND_INVDESTCOLOR       = 10,
    D3DBLEND_SRCALPHASAT        = 11,
    D3DBLEND_BOTHSRCALPHA       = 12,
    D3DBLEND_BOTHINVSRCALPHA    = 13,
    D3DBLEND_BLENDFACTOR        = 14,  
    D3DBLEND_FORCE_DWORD        = 0x7fffffff,  
} D3DBLEND;

typedef enum _D3DSTENCILOP 
{
    D3DSTENCILOP_KEEP           = 1,
    D3DSTENCILOP_ZERO           = 2,
	D3DSTENCILOP_REPLACE		= 3,
    D3DSTENCILOP_INCRSAT        = 4,
    D3DSTENCILOP_DECRSAT        = 5,
    D3DSTENCILOP_INVERT         = 6,
    D3DSTENCILOP_INCR           = 7,
    D3DSTENCILOP_DECR           = 8,
    D3DSTENCILOP_FORCE_DWORD    = 0x7fffffff,  
} D3DSTENCILOP;

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

typedef enum _D3DBLENDOP 
{
    D3DBLENDOP_ADD              = 1,
    D3DBLENDOP_SUBTRACT         = 2,
    D3DBLENDOP_REVSUBTRACT      = 3,
    D3DBLENDOP_MIN              = 4,
    D3DBLENDOP_MAX              = 5,
    D3DBLENDOP_FORCE_DWORD      = 0x7fffffff,  
} D3DBLENDOP;

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

typedef CUtlMap< RenderTargetState_t, CGLMFBO *> CGLMFBOMap;

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

#endif // COPENGLTYPES_H
