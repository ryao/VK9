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
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include <unordered_map>
#include <vector>

#include "CTypes.h"
#include "CIndexBuffer9.h"

class CDevice9;

class BufferManager
{
public:
	BufferManager();
	explicit BufferManager(CDevice9* device);
	~BufferManager();

	VkResult mResult = VK_SUCCESS;

	CDevice9* mDevice = nullptr;
	bool mIsDirty = true;

	std::unordered_map<UINT, StreamSource> mStreamSources;
	CIndexBuffer9* mIndexBuffer = nullptr;

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
	VkWriteDescriptorSet mWriteDescriptorSet[2] = {};

	//Created with max slots. I can pass a count to limit the number. This should prevent me from needing to realloc.
	VkVertexInputBindingDescription mVertexInputBindingDescription[16] = {};
	VkVertexInputAttributeDescription mVertexInputAttributeDescription[32] = {};

	//VkDescriptorSetLayout mDescriptorSetLayout;
	//VkPipelineLayout mPipelineLayout;
	VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
	//VkDescriptorSet mDescriptorSet;
	//VkPipeline mPipeline;
	
	VkShaderModule mVertShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;

	VkSampler mSampler = VK_NULL_HANDLE;
	VkImage mImage = VK_NULL_HANDLE;
	VkImageLayout mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkDeviceMemory mDeviceMemory = VK_NULL_HANDLE;
	VkImageView mImageView = VK_NULL_HANDLE;
	int32_t mTextureWidth = 0;
	int32_t mTextureHeight = 0;
	VkDescriptorImageInfo mDescriptorImageInfo[16] = {};
	VkDescriptorBufferInfo mDescriptorBufferInfo = {};
	uint32_t mVertexCount = 0;

	VkBuffer mUniformStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mUniformStagingBufferMemory = VK_NULL_HANDLE;
	VkBuffer mUniformBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mUniformBufferMemory = VK_NULL_HANDLE;

	D3DPRIMITIVETYPE mLastType = D3DPT_FORCE_DWORD;

	std::vector<DrawContext> mDrawBuffer;
	std::vector<HistoricalUniformBuffer> mHistoricalUniformBuffers;

	void BeginDraw(DrawContext& context, D3DPRIMITIVETYPE type);
	void UpdateUniformBuffer();
	void FlushDrawBufffer();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& deviceMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

private:

};

#endif // BUFFERMANAGER_H
