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
	VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;

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
	boost::container::flat_map<UINT, UINT> Bindings;

	//D3D9 State - Pipe
	D3DPRIMITIVETYPE PrimitiveType = D3DPT_FORCE_DWORD;
	DWORD FVF = 0;
	CVertexDeclaration9* VertexDeclaration = nullptr;
	CVertexShader9* VertexShader = nullptr;
	CPixelShader9* PixelShader = nullptr;
	int32_t StreamCount = 0;
	D3DFILLMODE FillMode = D3DFILL_FORCE_DWORD;
	D3DCULL CullMode = D3DCULL_FORCE_DWORD;

	//D3d9 State - Lights
	BOOL IsLightingEnabled = false;
	D3DSHADEMODE ShadeMode = D3DSHADE_FORCE_DWORD;

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

	VkDynamicState mDynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE] = {};
	VkPipelineColorBlendAttachmentState mPipelineColorBlendAttachmentState[1] = {};

	VkPipelineVertexInputStateCreateInfo mPipelineVertexInputStateCreateInfo = {};
	VkPipelineInputAssemblyStateCreateInfo mPipelineInputAssemblyStateCreateInfo = {};
	VkPipelineRasterizationStateCreateInfo mPipelineRasterizationStateCreateInfo = {};
	VkPipelineColorBlendStateCreateInfo mPipelineColorBlendStateCreateInfo = {};
	VkPipelineDepthStencilStateCreateInfo mPipelineDepthStencilStateCreateInfo = {};
	VkPipelineViewportStateCreateInfo mPipelineViewportStateCreateInfo = {};
	VkPipelineMultisampleStateCreateInfo mPipelineMultisampleStateCreateInfo = {};
	VkPipelineDynamicStateCreateInfo mPipelineDynamicStateCreateInfo = {};
	VkGraphicsPipelineCreateInfo mGraphicsPipelineCreateInfo = {};
	VkPipelineCacheCreateInfo mPipelineCacheCreateInfo = {};

	VkPipelineShaderStageCreateInfo mPipelineShaderStageCreateInfo[2] = {};

	VkDescriptorSetAllocateInfo mDescriptorSetAllocateInfo = {};
	VkDescriptorSetLayoutBinding mDescriptorSetLayoutBinding[16] = {};
	VkDescriptorSetLayoutCreateInfo mDescriptorSetLayoutCreateInfo = {};
	VkPipelineLayoutCreateInfo mPipelineLayoutCreateInfo = {};
	VkWriteDescriptorSet mWriteDescriptorSet[1] = {};
	VkPushConstantRange mPushConstantRanges[1] = {};

	//Created with max slots. I can pass a count to limit the number. This should prevent me from needing to realloc.
	VkVertexInputBindingDescription mVertexInputBindingDescription[16] = {};
	VkVertexInputAttributeDescription mVertexInputAttributeDescription[32] = {};

	//Command Buffer & Buffer Copy Setup
	VkCommandBufferAllocateInfo mCommandBufferAllocateInfo = {};
	VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	VkCommandBufferBeginInfo mBeginInfo = {};
	VkBufferCopy mCopyRegion = {};
	VkSubmitInfo mSubmitInfo = {};

	//VkDescriptorSetLayout mDescriptorSetLayout;
	//VkPipelineLayout mPipelineLayout;
	VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
	//VkDescriptorSet mDescriptorSet;
	//VkPipeline mPipeline;
	
	VkShaderModule mVertShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_TEX2 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_TEX2 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE_TEX2 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;

	VkSampler mSampler = VK_NULL_HANDLE;
	VkImage mImage = VK_NULL_HANDLE;
	VkImageLayout mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkDeviceMemory mDeviceMemory = VK_NULL_HANDLE;
	VkImageView mImageView = VK_NULL_HANDLE;
	int32_t mTextureWidth = 0;
	int32_t mTextureHeight = 0;
	
	int32_t mVertexCount = 0;

	VkBuffer mUniformStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mUniformStagingBufferMemory = VK_NULL_HANDLE;
	VkBuffer mUniformBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mUniformBufferMemory = VK_NULL_HANDLE;

	boost::container::small_vector< std::shared_ptr<SamplerRequest>, 16> mSamplerRequests;
	boost::container::small_vector< std::shared_ptr<DrawContext>, 16> mDrawBuffer;

	boost::container::small_vector< std::shared_ptr<ResourceContext>, 16> mUsedResourceBuffer;
	boost::container::small_vector< std::shared_ptr<ResourceContext>, 16> mUnusedResourceBuffer;

	VkDescriptorSet mLastDescriptorSet = VK_NULL_HANDLE;
	VkPipeline mLastVkPipeline = VK_NULL_HANDLE;

	Eigen::Matrix4f mModel;
	Eigen::Matrix4f mView;
	Eigen::Matrix4f mProjection;
	Eigen::Matrix4f mTotalTransformation;

	float mEpsilon = std::numeric_limits<float>::epsilon();

	void BeginDraw(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type);
	void CreatePipe(std::shared_ptr<DrawContext> context);
	void CreateDescriptorSet(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext);
	void CreateSampler(std::shared_ptr<SamplerRequest> request);

	void UpdateUniformBuffer(std::shared_ptr<DrawContext> context);
	void FlushDrawBufffer();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& deviceMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

private:

};

#endif // BUFFERMANAGER_H
