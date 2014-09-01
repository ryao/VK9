

#ifndef COPENGLTYPES_H
#define COPENGLTYPES_H

#include "togl/rendermechanism.h"

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
		//GLMTexSamplingParams		m_samplers[GLM_SAMPLER_COUNT];
	} gl;

#endif // COPENGLTYPES_H
