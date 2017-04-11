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

#include <boost/container/small_vector.hpp>
#include <boost/container/flat_map.hpp>

class CVertexBuffer9;
class CVertexDeclaration9;
class CIndexBuffer9;
class CPixelShader9;
class CVertexShader9;
class CTexture9;

struct Monitor
{
	HMONITOR hMonitor = NULL;
	HDC hdcMonitor = NULL;
	uint32_t Height = 0;
	uint32_t Width = 0;
	uint32_t RefreshRate = 0;
	uint32_t PixelBits = 0;
	uint32_t ColorPlanes = 0;

};

struct Vertex
{
	float x, y, z; // Position of vertex in 3D space
	DWORD color;   // Color of vertex
};

union Matrix
{
	float Value[4][4];
	float FlatValue[16];
};

struct UniformBufferObject {
	//glm::mat4 model;// = glm::mat4(1.0);
	////glm::mat4 view;// = glm::mat4(1.0);
	////glm::mat4 proj;// = glm::mat4(1.0);

	//Eigen::Matrix4f Model;
	//Eigen::Matrix4f View;
	//Eigen::Matrix4f Projection;

	Matrix Model = {
		 1.0 ,0.0 ,0.0 ,0.0
		,0.0 ,1.0 ,0.0 ,0.0
		,0.0 ,0.0 ,1.0 ,0.0
		,0.0 ,0.0 ,0.0 ,1.0 };
	Matrix View = {
		 1.0 ,0.0 ,0.0 ,0.0
		,0.0 ,1.0 ,0.0 ,0.0
		,0.0 ,0.0 ,1.0 ,0.0
		,0.0 ,0.0 ,0.0 ,1.0 };
	Matrix Projection = {
		 1.0 ,0.0 ,0.0 ,0.0
		,0.0 ,1.0 ,0.0 ,0.0
		,0.0 ,0.0 ,1.0 ,0.0
		,0.0 ,0.0 ,0.0 ,1.0 };
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

struct SpecializationConstants
{
	int lightCount = 1;
	int shadeMode = D3DSHADE_GOURAUD;
	int isLightingEnabled = true;
	int textureCount = 1;

	//Texture Stage _0
	int Constant_0 = 0;
	int Result_0 = D3DTA_CURRENT;
	int textureTransformationFlags_0 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_0 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_0 = D3DTOP_MODULATE;
	int colorArgument0_0 = D3DTA_CURRENT;
	int colorArgument1_0 = D3DTA_TEXTURE;
	int colorArgument2_0 = D3DTA_CURRENT;
	int alphaOperation_0 = D3DTOP_SELECTARG1;
	int alphaArgument0_0 = D3DTA_CURRENT;
	int alphaArgument1_0 = D3DTA_TEXTURE;
	int alphaArgument2_0 = D3DTA_CURRENT;
	float bumpMapMatrix00_0 = 0.0;
	float bumpMapMatrix01_0 = 0.0;
	float bumpMapMatrix10_0 = 0.0;
	float bumpMapMatrix11_0 = 0.0;
	float bumpMapScale_0 = 0.0;
	float bumpMapOffset_0 = 0.0;

	//Texture Stage _1
	int Constant_1 = 0;
	int Result_1 = D3DTA_CURRENT;
	int textureTransformationFlags_1 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_1 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_1 = D3DTOP_DISABLE;
	int colorArgument0_1 = D3DTA_CURRENT;
	int colorArgument1_1 = D3DTA_TEXTURE;
	int colorArgument2_1 = D3DTA_CURRENT;
	int alphaOperation_1 = D3DTOP_DISABLE;
	int alphaArgument0_1 = D3DTA_CURRENT;
	int alphaArgument1_1 = D3DTA_TEXTURE;
	int alphaArgument2_1 = D3DTA_CURRENT;
	float bumpMapMatrix00_1 = 0.0;
	float bumpMapMatrix01_1 = 0.0;
	float bumpMapMatrix10_1 = 0.0;
	float bumpMapMatrix11_1 = 0.0;
	float bumpMapScale_1 = 0.0;
	float bumpMapOffset_1 = 0.0;

	//Texture Stage _2
	int Constant_2 = 0;
	int Result_2 = D3DTA_CURRENT;
	int textureTransformationFlags_2 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_2 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_2 = D3DTOP_DISABLE;
	int colorArgument0_2 = D3DTA_CURRENT;
	int colorArgument1_2 = D3DTA_TEXTURE;
	int colorArgument2_2 = D3DTA_CURRENT;
	int alphaOperation_2 = D3DTOP_DISABLE;
	int alphaArgument0_2 = D3DTA_CURRENT;
	int alphaArgument1_2 = D3DTA_TEXTURE;
	int alphaArgument2_2 = D3DTA_CURRENT;
	float bumpMapMatrix00_2 = 0.0;
	float bumpMapMatrix01_2 = 0.0;
	float bumpMapMatrix10_2 = 0.0;
	float bumpMapMatrix11_2 = 0.0;
	float bumpMapScale_2 = 0.0;
	float bumpMapOffset_2 = 0.0;

	//Texture Stage _3
	int Constant_3 = 0;
	int Result_3 = D3DTA_CURRENT;
	int textureTransformationFlags_3 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_3 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_3 = D3DTOP_DISABLE;
	int colorArgument0_3 = D3DTA_CURRENT;
	int colorArgument1_3 = D3DTA_TEXTURE;
	int colorArgument2_3 = D3DTA_CURRENT;
	int alphaOperation_3 = D3DTOP_DISABLE;
	int alphaArgument0_3 = D3DTA_CURRENT;
	int alphaArgument1_3 = D3DTA_TEXTURE;
	int alphaArgument2_3 = D3DTA_CURRENT;
	float bumpMapMatrix00_3 = 0.0;
	float bumpMapMatrix01_3 = 0.0;
	float bumpMapMatrix10_3 = 0.0;
	float bumpMapMatrix11_3 = 0.0;
	float bumpMapScale_3 = 0.0;
	float bumpMapOffset_3 = 0.0;

	//Texture Stage _4
	int Constant_4 = 0;
	int Result_4 = D3DTA_CURRENT;
	int textureTransformationFlags_4 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_4 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_4 = D3DTOP_DISABLE;
	int colorArgument0_4 = D3DTA_CURRENT;
	int colorArgument1_4 = D3DTA_TEXTURE;
	int colorArgument2_4 = D3DTA_CURRENT;
	int alphaOperation_4 = D3DTOP_DISABLE;
	int alphaArgument0_4 = D3DTA_CURRENT;
	int alphaArgument1_4 = D3DTA_TEXTURE;
	int alphaArgument2_4 = D3DTA_CURRENT;
	float bumpMapMatrix00_4 = 0.0;
	float bumpMapMatrix01_4 = 0.0;
	float bumpMapMatrix10_4 = 0.0;
	float bumpMapMatrix11_4 = 0.0;
	float bumpMapScale_4 = 0.0;
	float bumpMapOffset_4 = 0.0;

	//Texture Stage _5
	int Constant_5 = 0;
	int Result_5 = D3DTA_CURRENT;
	int textureTransformationFlags_5 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_5 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_5 = D3DTOP_DISABLE;
	int colorArgument0_5 = D3DTA_CURRENT;
	int colorArgument1_5 = D3DTA_TEXTURE;
	int colorArgument2_5 = D3DTA_CURRENT;
	int alphaOperation_5 = D3DTOP_DISABLE;
	int alphaArgument0_5 = D3DTA_CURRENT;
	int alphaArgument1_5 = D3DTA_TEXTURE;
	int alphaArgument2_5 = D3DTA_CURRENT;
	float bumpMapMatrix00_5 = 0.0;
	float bumpMapMatrix01_5 = 0.0;
	float bumpMapMatrix10_5 = 0.0;
	float bumpMapMatrix11_5 = 0.0;
	float bumpMapScale_5 = 0.0;
	float bumpMapOffset_5 = 0.0;

	//Texture Stage _6
	int Constant_6 = 0;
	int Result_6 = D3DTA_CURRENT;
	int textureTransformationFlags_6 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_6 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_6 = D3DTOP_DISABLE;
	int colorArgument0_6 = D3DTA_CURRENT;
	int colorArgument1_6 = D3DTA_TEXTURE;
	int colorArgument2_6 = D3DTA_CURRENT;
	int alphaOperation_6 = D3DTOP_DISABLE;
	int alphaArgument0_6 = D3DTA_CURRENT;
	int alphaArgument1_6 = D3DTA_TEXTURE;
	int alphaArgument2_6 = D3DTA_CURRENT;
	float bumpMapMatrix00_6 = 0.0;
	float bumpMapMatrix01_6 = 0.0;
	float bumpMapMatrix10_6 = 0.0;
	float bumpMapMatrix11_6 = 0.0;
	float bumpMapScale_6 = 0.0;
	float bumpMapOffset_6 = 0.0;

	//Texture Stage _7
	int Constant_7 = 0;
	int Result_7 = D3DTA_CURRENT;
	int textureTransformationFlags_7 = D3DTTFF_DISABLE;
	int texureCoordinateIndex_7 = D3DTSS_TCI_PASSTHRU;
	int colorOperation_7 = D3DTOP_DISABLE;
	int colorArgument0_7 = D3DTA_CURRENT;
	int colorArgument1_7 = D3DTA_TEXTURE;
	int colorArgument2_7 = D3DTA_CURRENT;
	int alphaOperation_7 = D3DTOP_DISABLE;
	int alphaArgument0_7 = D3DTA_CURRENT;
	int alphaArgument1_7 = D3DTA_TEXTURE;
	int alphaArgument2_7 = D3DTA_CURRENT;
	float bumpMapMatrix00_7 = 0.0;
	float bumpMapMatrix01_7 = 0.0;
	float bumpMapMatrix10_7 = 0.0;
	float bumpMapMatrix11_7 = 0.0;
	float bumpMapScale_7 = 0.0;
	float bumpMapOffset_7 = 0.0;
};

struct DeviceState
{
	//IDirect3DDevice9::LightEnable
	boost::container::flat_map<DWORD, BOOL> mLightSettings;

	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette
	//IDirect3DDevice9::SetFVF
	DWORD mFVF = LONG_MAX;
	BOOL mHasFVF = 0;

	//IDirect3DDevice9::SetIndices
	CIndexBuffer9* mIndexBuffer = nullptr;
	BOOL mHasIndexBuffer = 0;

	//IDirect3DDevice9::SetLight
	boost::container::small_vector<D3DLIGHT9, 4> mLights;
	BOOL mAreLightsDirty = true;

	//IDirect3DDevice9::SetMaterial
	D3DMATERIAL9 mMaterial = {};
	BOOL mIsMaterialDirty = true;

	//IDirect3DDevice9::SetNPatchMode
	float mNSegments = -1;

	//IDirect3DDevice9::SetPixelShader
	CPixelShader9* mPixelShader = nullptr;
	BOOL mHasPixelShader = 0;

	//IDirect3DDevice9::SetPixelShaderConstantB
	//IDirect3DDevice9::SetPixelShaderConstantF
	//IDirect3DDevice9::SetPixelShaderConstantI
	//IDirect3DDevice9::SetRenderState
	boost::container::flat_map<D3DRENDERSTATETYPE, DWORD> mRenderStates;

	//IDirect3DDevice9::SetSamplerState
	boost::container::flat_map<DWORD, boost::container::flat_map<D3DSAMPLERSTATETYPE, DWORD> > mSamplerStates;

	//IDirect3DDevice9::SetScissorRect
	RECT m9Scissor = {};
	VkRect2D mScissor = {};

	//IDirect3DDevice9::SetStreamSource
	boost::container::flat_map<UINT, StreamSource> mStreamSources;

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	VkDescriptorImageInfo mDescriptorImageInfo[16] = {};
	boost::container::flat_map<DWORD, CTexture9*> mTextures;

	//IDirect3DDevice9::SetTextureStageState
	//boost::container::flat_map<DWORD, boost::container::flat_map<D3DTEXTURESTAGESTATETYPE, DWORD> > mTextureStageStates;

	//IDirect3DDevice9::SetTransform
	boost::container::flat_map<D3DTRANSFORMSTATETYPE, D3DMATRIX> mTransforms;
	BOOL mHasTransformsChanged = true;

	//IDirect3DDevice9::SetViewport
	D3DVIEWPORT9 m9Viewport = {};
	VkViewport mViewport = {};

	//IDirect3DDevice9::SetVertexDeclaration
	CVertexDeclaration9* mVertexDeclaration = nullptr;
	BOOL mHasVertexDeclaration = 0;

	//IDirect3DDevice9::SetVertexShader
	CVertexShader9* mVertexShader = nullptr;
	BOOL mHasVertexShader = 0;

	//IDirect3DDevice9::SetVertexShaderConstantB
	//IDirect3DDevice9::SetVertexShaderConstantF
	//IDirect3DDevice9::SetVertexShaderConstantI

	//Used for shader specialization.
	SpecializationConstants mSpecializationConstants = {};
};

struct color_A8R8G8B8
{
	unsigned char B = 0;
	unsigned char G = 0;
	unsigned char R = 0;
	unsigned char A = 0;
};

#endif // CTYPES_H
