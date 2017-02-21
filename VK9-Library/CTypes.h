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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "d3d9.h"
#include "d3d9types.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>

#include <vector>
#include <unordered_map>

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

struct UniformBufferObject {
	glm::mat4 model;// = glm::mat4(1.0);
	glm::mat4 view;// = glm::mat4(1.0);
	glm::mat4 proj;// = glm::mat4(1.0);
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

struct DrawContext
{
	VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkPipeline Pipeline = VK_NULL_HANDLE;
	VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
};

struct HistoricalUniformBuffer
{
	VkBuffer UniformBuffer = VK_NULL_HANDLE;
	VkDeviceMemory UniformBufferMemory = VK_NULL_HANDLE;
};

struct DeviceState
{
	//IDirect3DDevice9::LightEnable
	std::unordered_map<DWORD, BOOL> mLightSettings;

	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette
	//IDirect3DDevice9::SetFVF
	DWORD mFVF = -1;
	BOOL mFVFHasPosition = 0;
	BOOL mFVFHasColor = 0;
	BOOL mFVFHasNormal = 0;
	int32_t mFVFTextureCount = 0;
	BOOL mHasFVF = 0;

	//IDirect3DDevice9::SetIndices
	CIndexBuffer9* mIndexBuffer = nullptr;
	BOOL mHasIndexBuffer = 0;

	//IDirect3DDevice9::SetLight
	//IDirect3DDevice9::SetMaterial
	//IDirect3DDevice9::SetNPatchMode
	float mNSegments = -1;

	//IDirect3DDevice9::SetPixelShader
	CPixelShader9* mPixelShader = nullptr;
	BOOL mHasPixelShader = 0;

	//IDirect3DDevice9::SetPixelShaderConstantB
	//IDirect3DDevice9::SetPixelShaderConstantF
	//IDirect3DDevice9::SetPixelShaderConstantI
	//IDirect3DDevice9::SetRenderState
	std::unordered_map<D3DRENDERSTATETYPE, DWORD> mRenderStates;

	//IDirect3DDevice9::SetSamplerState
	std::unordered_map<DWORD, std::unordered_map<D3DSAMPLERSTATETYPE, DWORD> > mSamplerStates;

	//IDirect3DDevice9::SetScissorRect
	RECT m9Scissor = {};
	VkRect2D mScissor = {};

	//IDirect3DDevice9::SetStreamSource
	std::unordered_map<UINT, StreamSource> mStreamSources;

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	VkDescriptorImageInfo mDescriptorImageInfo[16] = {};
	std::unordered_map<DWORD, CTexture9*> mTextures;

	//IDirect3DDevice9::SetTextureStageState
	std::unordered_map<DWORD, std::unordered_map<D3DTEXTURESTAGESTATETYPE, DWORD> > mTextureStageStates;

	//IDirect3DDevice9::SetTransform
	std::unordered_map<D3DTRANSFORMSTATETYPE, D3DMATRIX> mTransforms;
	BOOL mHasTransformsChanged = 0;

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

};

struct color_A8R8G8B8
{
	char B = 0;
	char G = 0;
	char R = 0;
	char A = 0;
};

#endif // CTYPES_H
