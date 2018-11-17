/*
Copyright(c) 2016 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef CTYPES_H
#define CTYPES_H

#include "d3d9.h"
#include "d3d9types.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.hpp>

#include <vector>
#include <unordered_map>

#include <Eigen/Dense>

class CVertexBuffer9;
class CVertexDeclaration9;
class CIndexBuffer9;
class CPixelShader9;
class CVertexShader9;
class CTexture9;

struct Vertex
{
	float x, y, z; // Position of vertex in 3D space
	DWORD color;   // Color of vertex
};

/*
The structure below is using vec4 for position and direction because some implementations handle vec3 strange.
The alignment requirements are different for vec4 versus int/float so I moved "Type" after the vec4 variables to prevent strange padding in the middle of the structure.
In addition if the structure isn't a multiple of 16 the stride between elements in an array can be different. That is why the filler variables are included.
This structure is exactly 128 bytes.
*/
struct Light
{
	float                 Diffuse[4] = {};         /* Diffuse color of light */
	float                 Specular[4] = {};        /* Specular color of light */
	float                 Ambient[4] = {};         /* Ambient color of light */
	float                 Position[4] = {};        /* Position in world space */
	float                 Direction[4] = {};       /* Direction in world space */
	uint32_t              Type = 0;            /* Type of light source */
	float                 Range = 0;           /* Cutoff range */
	float                 Falloff = 0;         /* Falloff */
	float                 Attenuation0 = 0;    /* Constant attenuation */
	float                 Attenuation1 = 0;    /* Linear attenuation */
	float                 Attenuation2 = 0;    /* Quadratic attenuation */
	float                 Theta = 0;           /* Inner angle of spotlight cone */
	float                 Phi = 0;             /* Outer angle of spotlight cone */
	uint32_t              IsEnabled = 0;       /*Replaces separate enable structure.*/
	uint32_t              Filler1 = 0;
	uint32_t              Filler2 = 0;
	uint32_t              Filler3 = 0;
};

class StreamSource
{
public:

	UINT StreamNumber;
	CVertexBuffer9* StreamData;
	VkDeviceSize OffsetInBytes;
	UINT Stride;

	StreamSource();
	StreamSource(const StreamSource& value);
	StreamSource& operator =(const StreamSource& value);

	StreamSource(UINT streamNumber, CVertexBuffer9* streamData, VkDeviceSize offsetInBytes, UINT stride);
	~StreamSource();
};

struct ShaderConstantSlots
{
	uint32_t IntegerConstants[16 * 4]; //= { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	BOOL BooleanConstants[16]; //= { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	float FloatConstants[256 * 4]; //= { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
};

struct TextureStage
{
	D3DMATRIX textureTransformationMatrix;
	uint32_t Constant = 0;
	uint32_t Result = D3DTA_CURRENT;
	uint32_t textureTransformationFlags = D3DTTFF_DISABLE;
	uint32_t texureCoordinateIndex = 0;
	uint32_t colorOperation = D3DTOP_MODULATE;
	uint32_t colorArgument0 = D3DTA_CURRENT;
	uint32_t colorArgument1 = D3DTA_TEXTURE;
	uint32_t colorArgument2 = D3DTA_CURRENT;
	uint32_t alphaOperation = D3DTOP_SELECTARG1;
	uint32_t alphaArgument0 = D3DTA_CURRENT;
	uint32_t alphaArgument1 = D3DTA_TEXTURE;
	uint32_t alphaArgument2 = D3DTA_CURRENT;
	float bumpMapMatrix00 = 0.0f;
	float bumpMapMatrix01 = 0.0f;
	float bumpMapMatrix10 = 0.0f;
	float bumpMapMatrix11 = 0.0f;
	float bumpMapScale = 0.0f;
	float bumpMapOffset = 0.0f;
	uint32_t filler1 = 0;
	uint32_t filler2 = 0;
};

struct RenderState
{
	uint32_t textureCount = 1;

	//Render State
	uint32_t zEnable = D3DZB_TRUE;
	uint32_t fillMode = D3DFILL_SOLID;
	uint32_t shadeMode = D3DSHADE_GOURAUD;
	uint32_t zWriteEnable = TRUE;
	uint32_t alphaTestEnable = FALSE;
	uint32_t lastPixel = TRUE;
	uint32_t sourceBlend = D3DBLEND_ONE;
	uint32_t destinationBlend = D3DBLEND_ZERO;
	uint32_t cullMode = D3DCULL_CCW; // D3DCULL_CW;
	uint32_t zFunction = D3DCMP_LESSEQUAL;
	uint32_t alphaReference = 0;
	uint32_t alphaFunction = D3DCMP_ALWAYS;
	uint32_t ditherEnable = FALSE;
	uint32_t alphaBlendEnable = FALSE;
	uint32_t fogEnable = FALSE;
	uint32_t specularEnable = FALSE;
	uint32_t fogColor = 0;
	uint32_t fogTableMode = D3DFOG_NONE;
	float fogStart = 0.0f;
	float fogEnd = 1.0f;
	float fogDensity = 1.0f;
	uint32_t rangeFogEnable = FALSE;
	uint32_t stencilEnable = FALSE;
	uint32_t stencilFail = D3DSTENCILOP_KEEP;
	uint32_t stencilZFail = D3DSTENCILOP_KEEP;
	uint32_t stencilPass = D3DSTENCILOP_KEEP;
	uint32_t stencilFunction = D3DCMP_ALWAYS;
	uint32_t stencilReference = 0;
	uint32_t stencilMask = 0xFFFFFFFF;
	uint32_t stencilWriteMask = 0xFFFFFFFF;
	uint32_t textureFactor = 0xFFFFFFFF;
	uint32_t wrap0 = 0;
	uint32_t wrap1 = 0;
	uint32_t wrap2 = 0;
	uint32_t wrap3 = 0;
	uint32_t wrap4 = 0;
	uint32_t wrap5 = 0;
	uint32_t wrap6 = 0;
	uint32_t wrap7 = 0;
	uint32_t clipping = TRUE;
	uint32_t lighting = TRUE;
	uint32_t ambient = 0;
	uint32_t fogVertexMode = D3DFOG_NONE;
	uint32_t colorVertex = TRUE;
	uint32_t localViewer = TRUE;
	uint32_t normalizeNormals = FALSE;
	uint32_t diffuseMaterialSource = D3DMCS_COLOR1;
	uint32_t specularMaterialSource = D3DMCS_COLOR2;
	uint32_t ambientMaterialSource = D3DMCS_MATERIAL;
	uint32_t emissiveMaterialSource = D3DMCS_MATERIAL;
	uint32_t vertexBlend = D3DVBF_DISABLE;
	uint32_t clipPlaneEnable = 0;
	float pointSize = 64.0f;
	float pointSizeMinimum = 1.0f;
	uint32_t pointSpriteEnable = FALSE;
	uint32_t pointScaleEnable = FALSE;
	float pointScaleA = 1.0f;
	float pointScaleB = 0.0f;
	float pointScaleC = 0.0f;
	uint32_t multisampleAntiAlias = TRUE;
	uint32_t multisampleMask = 0xFFFFFFFF;
	uint32_t patchEdgeStyle = D3DPATCHEDGE_DISCRETE;
	uint32_t debugMonitorToken = D3DDMT_ENABLE;
	float pointSizeMaximum = 64.0f;
	uint32_t indexedVertexBlendEnable = FALSE;
	uint32_t colorWriteEnable = 0x0000000F;
	float tweenFactor = 0.0f;
	uint32_t blendOperation = D3DBLENDOP_ADD;
	uint32_t positionDegree = D3DDEGREE_CUBIC;
	uint32_t normalDegree = D3DDEGREE_LINEAR;
	uint32_t scissorTestEnable = FALSE;
	float slopeScaleDepthBias = 0.00f;
	uint32_t antiAliasedLineEnable = FALSE;
	float minimumTessellationLevel = 1.0f;
	float maximumTessellationLevel = 1.0f;
	float adaptivetessX = 0.0f;
	float adaptivetessY = 0.0f;
	float adaptivetessZ = 1.0f;
	float adaptivetessW = 0.0f;
	uint32_t enableAdaptiveTessellation = FALSE;
	uint32_t twoSidedStencilMode = FALSE;
	uint32_t ccwStencilFail = D3DSTENCILOP_KEEP;
	uint32_t ccwStencilZFail = D3DSTENCILOP_KEEP;
	uint32_t ccwStencilPass = D3DSTENCILOP_KEEP;
	uint32_t ccwStencilFunction = D3DCMP_ALWAYS;
	uint32_t colorWriteEnable1 = 0x0000000f;
	uint32_t colorWriteEnable2 = 0x0000000f;
	uint32_t colorWriteEnable3 = 0x0000000f;
	uint32_t blendFactor = 0xffffffff;
	uint32_t srgbWriteEnable = 0;
	float depthBias = 0.00f;
	uint32_t wrap8 = 0;
	uint32_t wrap9 = 0;
	uint32_t wrap10 = 0;
	uint32_t wrap11 = 0;
	uint32_t wrap12 = 0;
	uint32_t wrap13 = 0;
	uint32_t wrap14 = 0;
	uint32_t wrap15 = 0;
	uint32_t separateAlphaBlendEnable = FALSE;
	uint32_t sourceBlendAlpha = D3DBLEND_ONE;
	uint32_t destinationBlendAlpha = D3DBLEND_ZERO;
	uint32_t blendOperationAlpha = D3DBLENDOP_ADD;
	uint32_t screenWidth = 640;
	uint32_t screenHeight = 480;
	uint32_t filler1 = 0;
	uint32_t filler2 = 0;
};

struct ShaderState
{
	RenderState mRenderState;
	TextureStage mTextureStages[9];
	Light mLights[8];
	D3DMATERIAL9 mMaterial = 
	{
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f
	};
};

struct RealIndexBuffer;
struct RealRenderTarget;

struct DeviceState
{
	DeviceState()
	{
		for (size_t i = 1; i < 9; i++)
		{
			mShaderState.mTextureStages[i].colorOperation = D3DTOP_DISABLE;
			mShaderState.mTextureStages[i].alphaOperation = D3DTOP_DISABLE;
			mShaderState.mTextureStages[i].texureCoordinateIndex = i;
		}
	}

	ShaderState mShaderState;

	BOOL mIsRenderStateDirty = true;
	BOOL mAreTextureStagesDirty = true;
	BOOL mAreLightsDirty = true;
	BOOL mIsMaterialDirty = true;

	//IDirect3DDevice9::LightEnable
	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette
	//IDirect3DDevice9::SetFVF
	DWORD mFVF = LONG_MAX;
	BOOL mHasFVF = 0;

	//IDirect3DDevice9::SetIndices
	CIndexBuffer9* mOriginalIndexBuffer = nullptr;
	RealIndexBuffer* mIndexBuffer = nullptr;
	BOOL mHasIndexBuffer = 0;
	std::shared_ptr<RealRenderTarget> mRenderTarget;

	//IDirect3DDevice9::SetLight
	//BOOL mAreLightsDirty = true;

	//IDirect3DDevice9::SetMaterial
	//BOOL mIsMaterialDirty = true;	

	//IDirect3DDevice9::SetNPatchMode
	float mNSegments = -1;

	//IDirect3DDevice9::SetPixelShader
	CPixelShader9* mPixelShader = nullptr;
	BOOL mHasPixelShader = 0;

	//IDirect3DDevice9::SetPixelShaderConstantB
	//IDirect3DDevice9::SetPixelShaderConstantF
	//IDirect3DDevice9::SetPixelShaderConstantI
	//IDirect3DDevice9::SetRenderState
	//Render State
	bool wasBlendGroupModified = false;
	bool wasRasterizerGroupModified = false;
	bool wasDsaGroupModified = false;
	bool wasMultisampleGroupModified = false;
	bool wasFixedFunctionOtherGroupModified = false;
	bool wasFixedFunctionLightingGroupModified = false;
	bool wasFogGroupModified = false;
	bool wasPixelShaderConstantGroupModified = false;

	//bool hasZEnable = false;
	//bool hasFillMode = false;
	//bool hasShadeMode = false;
	//bool hasZWriteEnable = false;
	//bool hasAlphaTestEnable = false;
	//bool hasLastPixel = false;
	//bool hasSourceBlend = false;
	//bool hasDestinationBlend = false;
	//bool hasCullMode = false;
	//bool hasZFunction = false;
	//bool hasAlphaReference = false;
	//bool hasAlphaFunction = false;
	//bool hasDitherEnable = false;
	//bool hasAlphaBlendEnable = false;
	//bool hasFogEnable = false;
	//bool hasSpecularEnable = false;
	//bool hasFogColor = false;
	//bool hasFogTableMode = false;
	//bool hasFogStart = false;
	//bool hasFogEnd = false;
	//bool hasFogDensity = false;
	//bool hasRangeFogEnable = false;
	//bool hasStencilEnable = false;
	//bool hasStencilFail = false;
	//bool hasStencilZFail = false;
	//bool hasStencilPass = false;
	//bool hasStencilFunction = false;
	bool hasStencilReference = false;
	//bool hasStencilMask = false;
	//bool hasStencilWriteMask = false;
	bool hasTextureFactor = false;
	bool hasWrap0 = false;
	bool hasWrap1 = false;
	bool hasWrap2 = false;
	bool hasWrap3 = false;
	bool hasWrap4 = false;
	bool hasWrap5 = false;
	bool hasWrap6 = false;
	bool hasWrap7 = false;
	bool hasClipping = false;
	//bool hasLighting = false;
	//bool hasAmbient = false;
	//bool hasFogVertexMode = false;
	//bool hasColorVertex = false;
	//bool hasLocalViewer = false;
	//bool hasNormalizeNormals = false;
	//bool hasDiffuseMaterialSource = false;
	//bool hasSpecularMaterialSource = false;
	//bool hasAmbientMaterialSource = false;
	//bool hasEmissiveMaterialSource = false;
	//bool hasVertexBlend = false;
	//bool hasClipPlaneEnable = false;
	//bool hasPointSize = false;
	//bool hasPointSizeMinimum = false;
	//bool hasPointSpriteEnable = false;
	//bool hasPointScaleEnable = false;
	//bool hasPointScaleA = false;
	//bool hasPointScaleB = false;
	//bool hasPointScaleC = false;
	//bool hasMultisampleAntiAlias = false;
	bool hasMultisampleMask = false;
	bool hasPatchEdgeStyle = false;
	bool hasDebugMonitorToken = false;
	//bool hasPointSizeMaximum = false;
	//bool hasIndexedVertexBlendEnable = false;
	//bool hasColorWriteEnable = false;
	//bool hasTweenFactor = false;
	//bool hasBlendOperation = false;
	bool hasPositionDegree = false;
	bool hasNormalDegree = false;
	//bool hasScissorTestEnable = false;
	//bool hasSlopeScaleDepthBias = false;
	//bool hasAntiAliasedLineEnable = false;
	bool hasMinimumTessellationLevel = false;
	bool hasMaximumTessellationLevel = false;
	bool hasAdaptivetessX = false;
	bool hasAdaptivetessY = false;
	bool hasAdaptivetessZ = false;
	bool hasAdaptivetessW = false;
	bool hasEnableAdaptiveTessellation = false;
	//bool hasTwoSidedStencilMode = false;
	//bool hasCcwStencilFail = false;
	//bool hasCcwStencilZFail = false;
	//bool hasCcwStencilPass = false;
	//bool hasCcwStencilFunction = false;
	//bool hasColorWriteEnable1 = false;
	//bool hasColorWriteEnable2 = false;
	//bool hasColorWriteEnable3 = false;
	bool hasBlendFactor = false;
	bool hasSrgbWriteEnable = false;
	//bool hasDepthBias = false;
	bool hasWrap8 = false;
	bool hasWrap9 = false;
	bool hasWrap10 = false;
	bool hasWrap11 = false;
	bool hasWrap12 = false;
	bool hasWrap13 = false;
	bool hasWrap14 = false;
	bool hasWrap15 = false;
	//bool hasSeparateAlphaBlendEnable = false;
	//bool hasSourceBlendAlpha = false;
	//bool hasDestinationBlendAlpha = false;
	//bool hasBlendOperationAlpha = false;

	//IDirect3DDevice9::SetSamplerState
	std::unordered_map<DWORD, std::unordered_map<D3DSAMPLERSTATETYPE, DWORD> > mSamplerStates;

	//IDirect3DDevice9::SetScissorRect
	RECT m9Scissor = {};
	vk::Rect2D mScissor;

	//IDirect3DDevice9::SetStreamSource
	std::unordered_map<UINT, StreamSource> mStreamSources;

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	vk::DescriptorImageInfo mDescriptorImageInfo[16];
	//std::unordered_map<DWORD, IDirect3DBaseTexture9*> mTextures;
	IDirect3DBaseTexture9* mTextures[16] = {};

	//IDirect3DDevice9::SetTextureStageState
	//std::unordered_map<DWORD, std::unordered_map<D3DTEXTURESTAGESTATETYPE, DWORD> > mTextureStageStates;

	//IDirect3DDevice9::SetTransform
	std::unordered_map<D3DTRANSFORMSTATETYPE, D3DMATRIX> mTransforms;
	BOOL mHasTransformsChanged = true;

	//IDirect3DDevice9::SetViewport
	D3DVIEWPORT9 m9Viewport = {};
	vk::Viewport mViewport;

	//IDirect3DDevice9::SetVertexDeclaration
	CVertexDeclaration9* mVertexDeclaration = nullptr;
	BOOL mHasVertexDeclaration = 0;

	//IDirect3DDevice9::SetVertexShader
	CVertexShader9* mVertexShader = nullptr;
	BOOL mHasVertexShader = 0;


	//IDirect3DDevice9::SetVertexShaderConstantB
	//IDirect3DDevice9::SetVertexShaderConstantF
	//IDirect3DDevice9::SetVertexShaderConstantI
	float mPushConstants[64] = {};

	ShaderConstantSlots mVertexShaderConstantSlots = {};
	BOOL mAreVertexShaderSlotsDirty = true;

	ShaderConstantSlots mPixelShaderConstantSlots = {};
	BOOL mArePixelShaderSlotsDirty = true;

	bool hasPresented = true;
};

struct color_A8R8G8B8
{
	unsigned char B = 0;
	unsigned char G = 0;
	unsigned char R = 0;
	unsigned char A = 0;
};

struct Transformations
{
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	Eigen::Matrix<float, 4, 4, Eigen::DontAlign> mTotalTransformation;
	Eigen::Matrix<float, 4, 4, Eigen::DontAlign> mModel;
	Eigen::Matrix<float, 4, 4, Eigen::DontAlign> mView;
	Eigen::Matrix<float, 4, 4, Eigen::DontAlign> mProjection;
};

union PushConstants
{
	float Floats[64] = {};
	uint32_t Integers[64];
	BOOL Booleans[64];
};

#endif // CTYPES_H
