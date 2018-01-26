/*VkPipelineShaderStageCreateInfo
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

#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#define UBO_SIZE 64

#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include <boost/container/flat_map.hpp>
#include <boost/container/small_vector.hpp>
#include <Eigen/Dense>
#include <memory>
#include <chrono>

#include "CTypes.h"
#include "CIndexBuffer9.h"

class CDevice9;

struct SamplerRequest
{
	//Vulkan State
	VkSampler Sampler = VK_NULL_HANDLE;

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
	CDevice9* mDevice = nullptr;
	SamplerRequest(CDevice9* device) : mDevice(device) {}
	~SamplerRequest();
};

struct ResourceContext
{
	VkDescriptorImageInfo DescriptorImageInfo[16] = {};
	
	//Vulkan State
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;
	BOOL WasShader = false; // descriptor set logic is different for shaders so mixing them makes Vulkan angry because the number of attachment is different and stuff.

	//Resource Handling.
	std::chrono::steady_clock::time_point LastUsed = std::chrono::steady_clock::now();
	CDevice9* mDevice = nullptr;
	ResourceContext(CDevice9* device) : mDevice(device) {}
	~ResourceContext();
};

struct DrawContext
{
	//Vulkan State
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkPipeline Pipeline = VK_NULL_HANDLE;
	VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;

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
	CDevice9* mDevice = nullptr;
	DrawContext(CDevice9* device) : mDevice(device) {}
	~DrawContext();
};

class BufferManager
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		BufferManager();
	explicit BufferManager(CDevice9* device);
	~BufferManager();

	VkResult mResult = VK_SUCCESS;

	CDevice9* mDevice = nullptr;
	bool mIsDirty = true;

	
	int32_t mTextureWidth = 0;
	int32_t mTextureHeight = 0;

	int32_t mVertexCount = 0;

	boost::container::small_vector< std::shared_ptr<SamplerRequest>, 16> mSamplerRequests;
	boost::container::small_vector< std::shared_ptr<DrawContext>, 16> mDrawBuffer;

	VkDescriptorSet mLastDescriptorSet = VK_NULL_HANDLE;
	VkPipeline mLastVkPipeline = VK_NULL_HANDLE;

	Transformations mTransformations;

	void BeginDraw(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type);
	void CreatePipe(std::shared_ptr<DrawContext> context);
	void CreateSampler(std::shared_ptr<SamplerRequest> request);

	void UpdatePushConstants(std::shared_ptr<DrawContext> context);
	void FlushDrawBufffer();

private:

};

#endif // BUFFERMANAGER_H
