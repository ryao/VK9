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
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include "BufferManager.h"
#include "CDevice9.h"

#include "Utilities.h"

typedef std::unordered_map<UINT, StreamSource> map_type;

BufferManager::BufferManager()
	: mDevice(nullptr),
	mLastType(D3DPT_FORCE_DWORD), //used to help prevent repeating pipe creation.
	mIsDirty(true),
	mPipeline(VK_NULL_HANDLE),
	mPipelineCache(VK_NULL_HANDLE),
	mDescriptorSet(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mDescriptorSetLayout(VK_NULL_HANDLE),
	mVertShaderModule(VK_NULL_HANDLE),
	mFragShaderModule(VK_NULL_HANDLE),

	mSampler(VK_NULL_HANDLE),
	mImage(VK_NULL_HANDLE),
	mImageLayout(VK_IMAGE_LAYOUT_UNDEFINED),
	mDeviceMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE),
	mTextureWidth(0),
	mTextureHeight(0),
	mVertexCount(0),

	mUniformStagingBuffer(VK_NULL_HANDLE),
	mUniformStagingBufferMemory(VK_NULL_HANDLE),
	mUniformBuffer(VK_NULL_HANDLE),
	mUniformBufferMemory(VK_NULL_HANDLE),

	mResult(VK_SUCCESS)
{
	//Don't use. This is only here for containers.
}

BufferManager::BufferManager(CDevice9* device)
	: mDevice(device),
	mLastType(D3DPT_FORCE_DWORD), //used to help prevent repeating pipe creation.
	mIsDirty(true),
	mPipeline(VK_NULL_HANDLE),
	mPipelineCache(VK_NULL_HANDLE),
	mDescriptorSet(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mDescriptorSetLayout(VK_NULL_HANDLE),
	mVertShaderModule(VK_NULL_HANDLE),
	mFragShaderModule(VK_NULL_HANDLE),

	mSampler(VK_NULL_HANDLE),
	mImage(VK_NULL_HANDLE),
	mImageLayout(VK_IMAGE_LAYOUT_UNDEFINED),
	mDeviceMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE),
	mTextureWidth(0),
	mTextureHeight(0),
	mVertexCount(0),

	mUniformStagingBuffer(VK_NULL_HANDLE),
	mUniformStagingBufferMemory(VK_NULL_HANDLE),
	mUniformBuffer(VK_NULL_HANDLE),
	mUniformBufferMemory(VK_NULL_HANDLE)
{

	//mVertShaderModule = LoadShaderFromResource(mDevice->mDevice,  TRI_VERT);
	//mFragshaderModule = LoadShaderFromResource(mDevice->mDevice, TRI_FRAG);

	mVertShaderModule = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer.vert.spv");
	mFragShaderModule = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer.frag.spv");

	mPipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	mPipelineVertexInputStateCreateInfo.pNext = NULL;
	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1; //reset later.
	mPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = mVertexInputBindingDescription;
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2; //reset later.
	mPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = mVertexInputAttributeDescription;

	mPipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	mPipelineDynamicStateCreateInfo.pDynamicStates = mDynamicStateEnables;

	mPipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	mPipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	mPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	mPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	mPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	mPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	mPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	mPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	mPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	mPipelineColorBlendAttachmentState[0].colorWriteMask = 0xf;
	mPipelineColorBlendAttachmentState[0].blendEnable = VK_FALSE;
	mPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	mPipelineColorBlendStateCreateInfo.attachmentCount = 1;
	mPipelineColorBlendStateCreateInfo.pAttachments = mPipelineColorBlendAttachmentState;

	mPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mPipelineViewportStateCreateInfo.viewportCount = 1;
	mDynamicStateEnables[mPipelineDynamicStateCreateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
	mPipelineViewportStateCreateInfo.scissorCount = 1;
	mDynamicStateEnables[mPipelineDynamicStateCreateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

	mPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	mPipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	mPipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	mPipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	mPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	mPipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
	mPipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
	mPipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
	mPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	mPipelineDepthStencilStateCreateInfo.front = mPipelineDepthStencilStateCreateInfo.back;

	mPipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	mPipelineMultisampleStateCreateInfo.pSampleMask = NULL;
	mPipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	mDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	mDescriptorSetLayoutCreateInfo.pNext = NULL;
	mDescriptorSetLayoutCreateInfo.bindingCount = 1;
	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;

	mDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	mDescriptorSetAllocateInfo.pNext = NULL;
	mDescriptorSetAllocateInfo.descriptorPool = mDevice->mDescriptorPool;
	mDescriptorSetAllocateInfo.descriptorSetCount = 1;
	mDescriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

	mPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	mPipelineLayoutCreateInfo.pNext = NULL;
	mPipelineLayoutCreateInfo.setLayoutCount = 1;
	mPipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;
	//mPipelineLayoutCreateInfo.pPushConstantRanges = mDevice->mPushConstants;
	//mPipelineLayoutCreateInfo.pushConstantRangeCount = 1;

	mGraphicsPipelineCreateInfo.stageCount = 2;

	mPipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	mPipelineShaderStageCreateInfo[0].module = mVertShaderModule;
	mPipelineShaderStageCreateInfo[0].pName = "main";

	mPipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	mPipelineShaderStageCreateInfo[1].module = mFragShaderModule;
	mPipelineShaderStageCreateInfo[1].pName = "main";

	mGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//mGraphicsPipelineCreateInfo.layout = mPipelineLayout;
	mGraphicsPipelineCreateInfo.pVertexInputState = &mPipelineVertexInputStateCreateInfo;
	mGraphicsPipelineCreateInfo.pInputAssemblyState = &mPipelineInputAssemblyStateCreateInfo;
	mGraphicsPipelineCreateInfo.pRasterizationState = &mPipelineRasterizationStateCreateInfo;
	mGraphicsPipelineCreateInfo.pColorBlendState = &mPipelineColorBlendStateCreateInfo;
	mGraphicsPipelineCreateInfo.pDepthStencilState = &mPipelineDepthStencilStateCreateInfo;
	mGraphicsPipelineCreateInfo.pViewportState = &mPipelineViewportStateCreateInfo;
	mGraphicsPipelineCreateInfo.pMultisampleState = &mPipelineMultisampleStateCreateInfo;
	mGraphicsPipelineCreateInfo.pStages = mPipelineShaderStageCreateInfo;
	mGraphicsPipelineCreateInfo.renderPass = mDevice->mRenderPass;
	mGraphicsPipelineCreateInfo.pDynamicState = &mPipelineDynamicStateCreateInfo;

	mPipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;	

	/*
	Setup the texture to be written into the descriptor set.
	*/

	const VkFormat textureFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkFormatProperties formatProperties;
	const uint32_t textureColors[2] = { 0xffff0000, 0xff00ff00 };

	const int32_t textureWidth = 1;
	const int32_t textureHeight = 1;

	vkGetPhysicalDeviceFormatProperties(mDevice->mPhysicalDevice, textureFormat, &formatProperties); //why no result.

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = textureFormat;
	imageCreateInfo.extent = { textureWidth, textureHeight, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = NULL;
	memoryAllocateInfo.allocationSize = 0;
	memoryAllocateInfo.memoryTypeIndex = 0;

	VkMemoryRequirements memoryRequirements = {};

	mResult = vkCreateImage(mDevice->mDevice, &imageCreateInfo, NULL, &mImage);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkCreateImage failed with return code of " << mResult;
		return;
	}

	vkGetImageMemoryRequirements(mDevice->mDevice, mImage, &memoryRequirements);

	memoryAllocateInfo.allocationSize = memoryRequirements.size;

	GetMemoryTypeFromProperties(mDevice->mDeviceMemoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);

	mResult = vkAllocateMemory(mDevice->mDevice, &memoryAllocateInfo, NULL, &mDeviceMemory);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkAllocateMemory failed with return code of " << mResult;
		return;
	}

	mResult = vkBindImageMemory(mDevice->mDevice, mImage, mDeviceMemory, 0);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkBindImageMemory failed with return code of " << mResult;
		return;
	}

	VkImageSubresource imageSubresource = {};
	imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSubresource.mipLevel = 0;
	imageSubresource.arrayLayer = 0;

	VkSubresourceLayout subresourceLayout = {};
	void* data = nullptr;
	int32_t x = 0;
	int32_t y = 0;

	vkGetImageSubresourceLayout(mDevice->mDevice, mImage, &imageSubresource, &subresourceLayout); //no result?

	mResult = vkMapMemory(mDevice->mDevice, mDeviceMemory, 0, memoryAllocateInfo.allocationSize, 0, &data);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkMapMemory failed with return code of " << mResult;
		return;
	}

	for (y = 0; y < textureHeight; y++) {
		uint32_t *row = (uint32_t *)((char *)data + subresourceLayout.rowPitch * y);
		for (x = 0; x < textureWidth; x++)
			row[x] = textureColors[(x & 1) ^ (y & 1)];
	}

	vkUnmapMemory(mDevice->mDevice, mDeviceMemory);

	mImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->SetImageLayout(mImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, mImageLayout); //VK_ACCESS_HOST_WRITE_BIT

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = NULL;
	samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = NULL;
	imageViewCreateInfo.image = VK_NULL_HANDLE;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = textureFormat;
	imageViewCreateInfo.components =
		{
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
		};
	imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	imageViewCreateInfo.flags = 0;

	mResult = vkCreateSampler(mDevice->mDevice, &samplerCreateInfo, NULL,&mSampler);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkCreateSampler failed with return code of " << mResult;
		return;
	}

	imageViewCreateInfo.image = mImage;

	mResult = vkCreateImageView(mDevice->mDevice, &imageViewCreateInfo, NULL,&mImageView);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkCreateImageView failed with return code of " << mResult;
		return;
	}

	mDescriptorImageInfo[0].sampler = mSampler;
	mDescriptorImageInfo[0].imageView = mImageView;
	mDescriptorImageInfo[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[1].sampler = mSampler;
	mDescriptorImageInfo[1].imageView = mImageView;
	mDescriptorImageInfo[1].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[2].sampler = mSampler;
	mDescriptorImageInfo[2].imageView = mImageView;
	mDescriptorImageInfo[2].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[3].sampler = mSampler;
	mDescriptorImageInfo[3].imageView = mImageView;
	mDescriptorImageInfo[3].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[4].sampler = mSampler;
	mDescriptorImageInfo[4].imageView = mImageView;
	mDescriptorImageInfo[4].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[5].sampler = mSampler;
	mDescriptorImageInfo[5].imageView = mImageView;
	mDescriptorImageInfo[5].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[6].sampler = mSampler;
	mDescriptorImageInfo[6].imageView = mImageView;
	mDescriptorImageInfo[6].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[7].sampler = mSampler;
	mDescriptorImageInfo[7].imageView = mImageView;
	mDescriptorImageInfo[7].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[8].sampler = mSampler;
	mDescriptorImageInfo[8].imageView = mImageView;
	mDescriptorImageInfo[8].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[9].sampler = mSampler;
	mDescriptorImageInfo[9].imageView = mImageView;
	mDescriptorImageInfo[9].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[10].sampler = mSampler;
	mDescriptorImageInfo[10].imageView = mImageView;
	mDescriptorImageInfo[10].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[11].sampler = mSampler;
	mDescriptorImageInfo[11].imageView = mImageView;
	mDescriptorImageInfo[11].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[12].sampler = mSampler;
	mDescriptorImageInfo[12].imageView = mImageView;
	mDescriptorImageInfo[12].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[13].sampler = mSampler;
	mDescriptorImageInfo[13].imageView = mImageView;
	mDescriptorImageInfo[13].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[14].sampler = mSampler;
	mDescriptorImageInfo[14].imageView = mImageView;
	mDescriptorImageInfo[14].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	mDescriptorImageInfo[15].sampler = mSampler;
	mDescriptorImageInfo[15].imageView = mImageView;
	mDescriptorImageInfo[15].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = NULL;
	bufferCreateInfo.size = (sizeof(UniformBufferObject));
	bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferCreateInfo.flags = 0;

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformStagingBuffer, mUniformStagingBufferMemory);
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mUniformBuffer, mUniformBufferMemory);

	mDescriptorBufferInfo.buffer = mUniformBuffer;
	mDescriptorBufferInfo.offset = 0;
	mDescriptorBufferInfo.range = sizeof(UniformBufferObject);

	mWriteDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//mWriteDescriptorSet.dstSet = descriptorSet;
	mWriteDescriptorSet[0].dstBinding = 0;
	mWriteDescriptorSet[0].dstArrayElement = 0;
	mWriteDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mWriteDescriptorSet[0].descriptorCount = 1;
	mWriteDescriptorSet[0].pBufferInfo = &mDescriptorBufferInfo;

	mWriteDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//mWriteDescriptorSet[1].dstSet = descriptorSet;
	mWriteDescriptorSet[1].dstBinding = 1;
	mWriteDescriptorSet[1].dstArrayElement = 0;
	mWriteDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	mWriteDescriptorSet[1].descriptorCount = 1;
	mWriteDescriptorSet[1].pImageInfo = mDescriptorImageInfo;
} 

BufferManager::~BufferManager()
{
	if (mUniformStagingBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice->mDevice, mUniformStagingBuffer, NULL);
		mUniformStagingBuffer = VK_NULL_HANDLE;
	}

	if (mUniformBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice->mDevice, mUniformBuffer, NULL);
		mUniformBuffer = VK_NULL_HANDLE;
	}

	if (mUniformStagingBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mUniformStagingBufferMemory, NULL);
		mUniformStagingBufferMemory = VK_NULL_HANDLE;
	}

	if (mUniformBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mUniformBufferMemory, NULL);
		mUniformBufferMemory = VK_NULL_HANDLE;
	}

	if (mImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mDevice->mDevice, mImageView, nullptr);
		mImageView = VK_NULL_HANDLE;
	}

	if (mImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(mDevice->mDevice, mImage, nullptr);
		mImage = VK_NULL_HANDLE;
	}

	if (mDeviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mDeviceMemory, nullptr);
		mDeviceMemory = VK_NULL_HANDLE;
	}

	if (mSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(mDevice->mDevice, mSampler, NULL);
		mSampler = VK_NULL_HANDLE;
	}

	if (mVertShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule, NULL);
		mVertShaderModule = VK_NULL_HANDLE;
	}

	if (mFragShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule, NULL);
		mFragShaderModule = VK_NULL_HANDLE;
	}
	
	if (mPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice->mDevice, mPipeline, NULL);
		mPipeline = VK_NULL_HANDLE;
	}

	if (mDescriptorSet != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(mDevice->mDevice, mDevice->mDescriptorPool, 1, &mDescriptorSet);
		mDescriptorSet = VK_NULL_HANDLE;
	}

	if (mPipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(mDevice->mDevice, mPipelineLayout, NULL);
		mPipelineLayout = VK_NULL_HANDLE;
	}

	if (mDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(mDevice->mDevice, mDescriptorSetLayout, NULL);
		mDescriptorSetLayout = VK_NULL_HANDLE;
	}
}

void BufferManager::BindVertexBuffers(D3DPRIMITIVETYPE type)
{
	mVertexCount = 0;

	BOOST_FOREACH(map_type::value_type& source, mStreamSources)
	{
		vkCmdBindVertexBuffers(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], source.first, 1, &source.second.StreamData->mBuffer, &source.second.OffsetInBytes);
		mVertexCount += source.second.StreamData->mSize;
	}
}

void BufferManager::UpdatePipeline(D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;
	std::unordered_map<D3DRENDERSTATETYPE, DWORD>::const_iterator searchResult;

	/**********************************************
	* Cleanup/Fetch Pipe
	**********************************************/
	//TODO: add logic to return existing pipe when possible.

	if (mLastType != type)
	{
		mIsDirty = true;
		mLastType = type;
	}
	else
	{
		mIsDirty = false;
		return;
	}

	if (mPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice->mDevice, mPipeline, NULL);
		mPipeline = VK_NULL_HANDLE;
	}

	if (mDescriptorSet != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(mDevice->mDevice, mDevice->mDescriptorPool, 1, &mDescriptorSet);
		mDescriptorSet = VK_NULL_HANDLE;
	}

	if (mPipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(mDevice->mDevice, mPipelineLayout, NULL);
		mPipelineLayout = VK_NULL_HANDLE;
	}

	if (mDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(mDevice->mDevice, mDescriptorSetLayout, NULL);
		mDescriptorSetLayout = VK_NULL_HANDLE;
	}

	/**********************************************
	* Update Pipe Creation Information
	**********************************************/

	searchResult = mDevice->mRenderStates.find(D3DRS_FILLMODE);
	if (searchResult != mDevice->mRenderStates.end())
	{
		mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode((D3DFILLMODE)mDevice->mRenderStates[D3DRS_FILLMODE]);
	}

	searchResult = mDevice->mRenderStates.find(D3DRS_CULLMODE);
	if (searchResult != mDevice->mRenderStates.end())
	{
		SetCulling(mPipelineRasterizationStateCreateInfo, (D3DCULL)searchResult->second);
	}

	/*
	// D3DRS_ZBIAS is not defined.
	if (mDevice->mRenderStates[D3DRS_ZBIAS] > 0)
	{
	mPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	}
	else
	{
	mPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_TRUE;
	}
	*/

	mPipelineInputAssemblyStateCreateInfo.topology = ConvertPrimitiveType(type);

	int i = 0;
	BOOST_FOREACH(map_type::value_type& source, mStreamSources)
	{
		int ai1 = i * 2; //attribute index 1
		int ai2 = ai1 + 1; //attribute index 1
		uint32_t offset = 0;
		uint32_t location = 0;

		mDescriptorSetLayoutBinding[i].binding = source.first;
		mDescriptorSetLayoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
		mDescriptorSetLayoutBinding[i].descriptorCount = 1;
		mDescriptorSetLayoutBinding[i].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS; //VK_SHADER_STAGE_FRAGMENT_BIT
		mDescriptorSetLayoutBinding[i].pImmutableSamplers = NULL;	

		mVertexInputBindingDescription[i].binding = source.first;
		mVertexInputBindingDescription[i].stride = source.second.Stride;
		mVertexInputBindingDescription[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		if (mDevice->mFVF & D3DFVF_XYZ)
		{
			mVertexInputAttributeDescription[ai1].binding = source.first;
			mVertexInputAttributeDescription[ai1].location = location;
			mVertexInputAttributeDescription[ai1].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputAttributeDescription[ai1].offset = offset;
			offset += (sizeof(float) * 3);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_DIFFUSE)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_B8G8R8A8_UINT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += sizeof(uint32_t);
			location += 1;
		}

		/*
		Curiously examples use 0 for TEX1 so I'm not sure if this is valid even though it's defined.
		*/
		if (mDevice->mFVF & D3DFVF_TEX0)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX1)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX2)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX3)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX4)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX5)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX6)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX7)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		if (mDevice->mFVF & D3DFVF_TEX8)
		{
			mVertexInputAttributeDescription[ai2].binding = source.first;
			mVertexInputAttributeDescription[ai2].location = location;
			mVertexInputAttributeDescription[ai2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputAttributeDescription[ai2].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
		}

		i++;
	} 

	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;
	mDescriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;
	mPipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;

	mDescriptorSetLayoutCreateInfo.bindingCount = mStreamSources.size();
	mDescriptorSetAllocateInfo.descriptorSetCount = mStreamSources.size();
	mPipelineLayoutCreateInfo.setLayoutCount = mStreamSources.size();

	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = mStreamSources.size();
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = mStreamSources.size() * 2;
	


	/**********************************************
	* Create Pipe
	**********************************************/

	result = vkCreateDescriptorSetLayout(mDevice->mDevice, &mDescriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::UpdatePipeline vkCreateDescriptorSetLayout failed with return code of " << mResult;
		return;
	}

	mDescriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

	result = vkAllocateDescriptorSets(mDevice->mDevice, &mDescriptorSetAllocateInfo, &mDescriptorSet);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::UpdatePipeline vkAllocateDescriptorSets failed with return code of " << mResult;
		return;
	}

	mWriteDescriptorSet[0].dstSet = mDescriptorSet;
	mWriteDescriptorSet[0].descriptorCount = 1;
	mWriteDescriptorSet[0].pBufferInfo = &mDescriptorBufferInfo;

	mWriteDescriptorSet[1].dstSet = mDescriptorSet;
	mWriteDescriptorSet[1].descriptorCount = 1;
	mWriteDescriptorSet[1].pImageInfo = mDescriptorImageInfo;

	vkUpdateDescriptorSets(mDevice->mDevice, 2, mWriteDescriptorSet, 0, NULL);

	result = vkCreatePipelineLayout(mDevice->mDevice, &mPipelineLayoutCreateInfo, NULL, &mPipelineLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::UpdatePipeline vkCreatePipelineLayout failed with return code of " << mResult;
		return;
	}

	mGraphicsPipelineCreateInfo.layout = mPipelineLayout;

	result = vkCreatePipelineCache(mDevice->mDevice, &mPipelineCacheCreateInfo, NULL, &mPipelineCache);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::UpdatePipeline vkCreatePipelineCache failed with return code of " << mResult;
		return;
	}

	result = vkCreateGraphicsPipelines(mDevice->mDevice, mPipelineCache, 1, &mGraphicsPipelineCreateInfo, NULL, &mPipeline);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::UpdatePipeline vkCreateGraphicsPipelines failed with return code of " << mResult;
		//Don't return so we can destroy cache.
	}

	vkDestroyPipelineCache(mDevice->mDevice, mPipelineCache, NULL);
	mPipelineCache = VK_NULL_HANDLE;

	return;
}

void BufferManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& deviceMemory)
{
	VkResult result; // = VK_SUCCESS

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	result = vkCreateBuffer(mDevice->mDevice, &bufferInfo, nullptr, &buffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreateBuffer vkCreateBuffer failed with return code of " << mResult;
		return;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(mDevice->mDevice, buffer, &memoryRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (!GetMemoryTypeFromProperties(mDevice->mDeviceMemoryProperties, memoryRequirements.memoryTypeBits, properties, &allocInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "Memory type index not found!";
		return;
	}

	result = vkAllocateMemory(mDevice->mDevice, &allocInfo, nullptr, &deviceMemory);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreateBuffer vkCreateBuffer failed with return code of " << mResult;
		return;
	}

	vkBindBufferMemory(mDevice->mDevice, buffer, deviceMemory, 0);
}

void BufferManager::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = mDevice->mCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(mDevice->mDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(mDevice->mQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mDevice->mQueue);

	vkFreeCommandBuffers(mDevice->mDevice, mDevice->mCommandPool, 1, &commandBuffer);
}