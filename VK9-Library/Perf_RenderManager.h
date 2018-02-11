/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

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

#include "Perf_StateManager.h"
#include <vulkan/vulkan.hpp>
#include <chrono>

#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#define UBO_SIZE 64

struct SamplerRequest
{
	//Vulkan State
	vk::Sampler Sampler;

	//D3D9 State
	DWORD SamplerIndex = 0;
	D3DTEXTUREFILTERTYPE MagFilter = D3DTEXF_NONE;
	D3DTEXTUREFILTERTYPE MinFilter = D3DTEXF_NONE;
	D3DTEXTUREADDRESS AddressModeU = D3DTADDRESS_FORCE_DWORD;
	D3DTEXTUREADDRESS AddressModeV = D3DTADDRESS_FORCE_DWORD;
	D3DTEXTUREADDRESS AddressModeW = D3DTADDRESS_FORCE_DWORD;
	DWORD MaxAnisotropy = 0;
	D3DTEXTUREFILTERTYPE MipmapMode = D3DTEXF_NONE;
	float MipLodBias = 0.0f;
	float MaxLod = 1.0f;

	//Resource Handling.
	std::chrono::steady_clock::time_point LastUsed = std::chrono::steady_clock::now();
	RealWindow* mRealWindow = nullptr; //null if not owner.
	SamplerRequest(RealWindow* realWindow) : mRealWindow(realWindow) {}
	~SamplerRequest();
};

struct ResourceContext
{
	vk::DescriptorImageInfo DescriptorImageInfo[16] = {};

	//Vulkan State
	vk::DescriptorSetLayout DescriptorSetLayout;
	vk::PipelineLayout PipelineLayout;
	vk::DescriptorSet DescriptorSet;
	BOOL WasShader = false; // descriptor set logic is different for shaders so mixing them makes Vulkan angry because the number of attachment is different and stuff.

	//Resource Handling.
	std::chrono::steady_clock::time_point LastUsed = std::chrono::steady_clock::now();
	RealWindow* mRealWindow = nullptr; //null if not owner.
	ResourceContext(RealWindow* realWindow) : mRealWindow(realWindow) {}
	~ResourceContext();
};

struct DrawContext
{
	//Vulkan State
	vk::DescriptorSetLayout DescriptorSetLayout;
	vk::Pipeline Pipeline;
	vk::PipelineLayout PipelineLayout;

	//Misc
	//boost::container::flat_map<UINT, UINT> Bindings;
	UINT Bindings[64] = {};

	//D3D9 State - Pipe
	D3DPRIMITIVETYPE PrimitiveType = D3DPT_FORCE_DWORD;
	DWORD FVF = 0;
	CVertexDeclaration9* VertexDeclaration = nullptr;
	CVertexShader9* VertexShader = nullptr;
	CPixelShader9* PixelShader = nullptr;
	int32_t StreamCount = 0;

	//D3d9 State - Lights
	ShaderConstantSlots mVertexShaderConstantSlots = {};
	ShaderConstantSlots mPixelShaderConstantSlots = {};

	//Constant Registers
	SpecializationConstants mSpecializationConstants = {};

	//Resource Handling.
	std::chrono::steady_clock::time_point LastUsed = std::chrono::steady_clock::now();
	RealWindow* mRealWindow = nullptr; //null if not owner.
	DrawContext(RealWindow* realWindow) : mRealWindow(realWindow) {}
	~DrawContext();
};

struct RenderManager
{
	StateManager mStateManager;

	float mEpsilon = std::numeric_limits<float>::epsilon();

	RenderManager();
	~RenderManager();

	void UpdateBuffer(RealWindow& realWindow);
	void StartScene(RealWindow& realWindow,bool clear = false);
	void StopScene(RealWindow& realWindow);
	void CopyImage(RealWindow& realWindow, vk::Image srcImage, vk::Image dstImage, int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t srcMip, uint32_t dstMip);
	void Clear(RealWindow& realWindow, DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
	void Present(RealWindow& realWindow, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);
	void DrawIndexedPrimitive(RealWindow& realWindow, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount);
	void DrawPrimitive(RealWindow& realWindow, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
	void UpdateTexture(RealWindow& realWindow, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture);

	void BeginDraw(RealWindow& realWindow, std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type);
	void CreatePipe(RealWindow& realWindow, std::shared_ptr<DrawContext> context);
	void CreateSampler(RealWindow& realWindow, std::shared_ptr<SamplerRequest> request);
	void UpdatePushConstants(RealWindow& realWindow, std::shared_ptr<DrawContext> context);
	void FlushDrawBufffer(RealWindow& realWindow);
};

#endif // RENDERMANAGER_H
