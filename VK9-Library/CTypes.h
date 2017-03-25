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
	int lightCount = 0;
	int shadeMode = D3DSHADE_GOURAUD;
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

	//IDirect3DDevice9::SetMaterial
	D3DMATERIAL9 mMaterial = {};

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
	boost::container::flat_map<DWORD, boost::container::flat_map<D3DTEXTURESTAGESTATETYPE, DWORD> > mTextureStageStates;

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
