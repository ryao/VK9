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
#include "BufferManager.h"
#include "CDevice9.h"
#include "CTexture9.h"

#include "Utilities.h"

typedef std::unordered_map<UINT, StreamSource> map_type;

BufferManager::BufferManager()
{
	//Don't use. This is only here for containers.
}

BufferManager::BufferManager(CDevice9* device)
	: mDevice(device)
{
	//mVertShaderModule = LoadShaderFromResource(mDevice->mDevice,  TRI_VERT);
	//mFragshaderModule = LoadShaderFromResource(mDevice->mDevice, TRI_FRAG);

	mVertShaderModule_XYZ_DIFFUSE = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE.vert.spv");
	mFragShaderModule_XYZ_DIFFUSE = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE.frag.spv");

	mVertShaderModule_XYZ_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_TEX1.vert.spv");
	mFragShaderModule_XYZ_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_TEX1.frag.spv");

	mVertShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX1.vert.spv");
	mFragShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX1.frag.spv");

	mVertShaderModule_XYZ_NORMAL = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL.vert.spv");
	mFragShaderModule_XYZ_NORMAL = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL.frag.spv");

	mPipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	mPipelineVertexInputStateCreateInfo.pNext = NULL;
	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1; //reset later.
	mPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = mVertexInputBindingDescription;
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2; //reset later.
	mPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = mVertexInputAttributeDescription;

	mPipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	mPipelineDynamicStateCreateInfo.pDynamicStates = mDynamicStateEnables;
	mPipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	mPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	mPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	mPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	mPipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

	mPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	mPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	mPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	mPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	//mPipelineColorBlendAttachmentState[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //0xf;
	//mPipelineColorBlendAttachmentState[0].blendEnable = VK_FALSE;

	mPipelineColorBlendAttachmentState[0].colorWriteMask = 0xf;
	mPipelineColorBlendAttachmentState[0].blendEnable = VK_TRUE;
	mPipelineColorBlendAttachmentState[0].colorBlendOp = VK_BLEND_OP_ADD;
	mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; //VK_BLEND_FACTOR_SRC_COLOR
	mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; //VK_BLEND_FACTOR_DST_COLOR
	mPipelineColorBlendAttachmentState[0].alphaBlendOp = VK_BLEND_OP_ADD;
	mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //VK_BLEND_FACTOR_ONE
	mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // VK_BLEND_FACTOR_ZERO

	mPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	mPipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	mPipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
	mPipelineColorBlendStateCreateInfo.attachmentCount = 1;
	mPipelineColorBlendStateCreateInfo.pAttachments = mPipelineColorBlendAttachmentState;
	mPipelineColorBlendStateCreateInfo.blendConstants[0] = 1.0f;
	mPipelineColorBlendStateCreateInfo.blendConstants[1] = 1.0f;
	mPipelineColorBlendStateCreateInfo.blendConstants[2] = 1.0f;
	mPipelineColorBlendStateCreateInfo.blendConstants[3] = 1.0f;

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
	mDescriptorSetLayoutCreateInfo.bindingCount = 2;
	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;

	mDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	mDescriptorSetAllocateInfo.pNext = NULL;
	mDescriptorSetAllocateInfo.descriptorPool = mDevice->mDescriptorPool;
	mDescriptorSetAllocateInfo.descriptorSetCount = 1;
	//mDescriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

	mPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	mPipelineLayoutCreateInfo.pNext = NULL;
	mPipelineLayoutCreateInfo.setLayoutCount = 1;
	//mPipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;
	//mPipelineLayoutCreateInfo.pPushConstantRanges = mDevice->mPushConstants;
	//mPipelineLayoutCreateInfo.pushConstantRangeCount = 1;

	mGraphicsPipelineCreateInfo.stageCount = 2;

	mPipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_DIFFUSE;
	mPipelineShaderStageCreateInfo[0].pName = "main";

	mPipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_DIFFUSE;
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

	mResult = vkCreatePipelineCache(mDevice->mDevice, &mPipelineCacheCreateInfo, nullptr, &mPipelineCache);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkCreatePipelineCache failed with return code of " << mResult;
		return;
	}

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

	mDevice->mDeviceState.mDescriptorImageInfo[0].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[0].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[1].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[1].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[2].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[2].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[3].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[3].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[4].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[4].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[4].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[5].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[5].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[5].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[6].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[6].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[6].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[7].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[7].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[7].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[8].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[8].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[8].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[9].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[9].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[9].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[10].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[10].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[10].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[11].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[11].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[11].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[12].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[12].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[12].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[13].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[13].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[13].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[14].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[14].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[14].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	mDevice->mDeviceState.mDescriptorImageInfo[15].sampler = mSampler;
	mDevice->mDeviceState.mDescriptorImageInfo[15].imageView = mImageView;
	mDevice->mDeviceState.mDescriptorImageInfo[15].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = NULL;
	bufferCreateInfo.size = (sizeof(UniformBufferObject));
	bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferCreateInfo.flags = 0;

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformStagingBuffer, mUniformStagingBufferMemory);

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
	mWriteDescriptorSet[1].pImageInfo = mDevice->mDeviceState.mDescriptorImageInfo;
} 

BufferManager::~BufferManager()
{
	if (mUniformStagingBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice->mDevice, mUniformStagingBuffer, NULL);
		mUniformStagingBuffer = VK_NULL_HANDLE;
	}

	if (mUniformStagingBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mUniformStagingBufferMemory, NULL);
		mUniformStagingBufferMemory = VK_NULL_HANDLE;
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

	if (mVertShaderModule_XYZ_DIFFUSE != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_DIFFUSE, NULL);
		mVertShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_DIFFUSE != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_DIFFUSE, NULL);
		mFragShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;
	}
	
	if (mVertShaderModule_XYZ_TEX1 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_TEX1, NULL);
		mVertShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_TEX1 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_TEX1, NULL);
		mFragShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;
	}

	if (mVertShaderModule_XYZ_DIFFUSE_TEX1 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_DIFFUSE_TEX1, NULL);
		mVertShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_DIFFUSE_TEX1 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_DIFFUSE_TEX1, NULL);
		mFragShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;
	}

	if (mVertShaderModule_XYZ_NORMAL != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_NORMAL, NULL);
		mVertShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_NORMAL != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_NORMAL, NULL);
		mFragShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;
	}

	if (mPipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(mDevice->mDevice, mPipelineCache, nullptr);
		mPipelineCache = VK_NULL_HANDLE;
	}

	FlushDrawBufffer();
}

void BufferManager::BeginDraw(DrawContext& context, D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;
	std::unordered_map<D3DRENDERSTATETYPE, DWORD>::const_iterator searchResult;

	/**********************************************
	* Update UBO structure.
	**********************************************/
	if (mDevice->mDeviceState.mHasTransformsChanged || mHistoricalUniformBuffers.size()==0)
	{
		UpdateUniformBuffer();
		mDevice->mDeviceState.mHasTransformsChanged = false;
		//Print(mDevice->mDeviceState.mTransforms);
	}

	/**********************************************
	* Update the textures that are currently mapped.
	**********************************************/
	BOOST_FOREACH(const auto& pair1, mDevice->mDeviceState.mTextures)
	{
		VkDescriptorImageInfo& targetSampler = mDevice->mDeviceState.mDescriptorImageInfo[pair1.first];

		if (pair1.second != nullptr)
		{
			SamplerRequest request = {};
			
			request.MagFilter = (D3DTEXTUREFILTERTYPE)mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_MAGFILTER];
			request.MinFilter = (D3DTEXTUREFILTERTYPE)mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_MINFILTER];
			request.AddressModeU = (D3DTEXTUREADDRESS)mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_ADDRESSU];
			request.AddressModeV = (D3DTEXTUREADDRESS)mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_ADDRESSV];
			request.AddressModeW = (D3DTEXTUREADDRESS)mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_ADDRESSW];
			request.MaxAnisotropy = mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_MAXANISOTROPY];
			request.MipmapMode = (D3DTEXTUREFILTERTYPE)mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_MIPFILTER];
			request.MipLodBias = (float)mDevice->mDeviceState.mSamplerStates[request.SamplerIndex][D3DSAMP_MIPMAPLODBIAS];

			for (size_t i = 0; i < mSamplerRequests.size(); i++)
			{
				auto& storedRequest = mSamplerRequests[i];
				if (request.MagFilter == storedRequest.MagFilter
					&& request.MinFilter == storedRequest.MinFilter
					&& request.AddressModeU == storedRequest.AddressModeU
					&& request.AddressModeV == storedRequest.AddressModeV
					&& request.AddressModeW == storedRequest.AddressModeW
					&& request.MaxAnisotropy == storedRequest.MaxAnisotropy
					&& request.MipmapMode == storedRequest.MipmapMode
					&& request.MipLodBias == storedRequest.MipLodBias)
				{
					request.Sampler = storedRequest.Sampler;
				}
			}

			if (request.Sampler == VK_NULL_HANDLE)
			{
				CreateSampler(request);
			}	

			targetSampler.sampler = request.Sampler;
			targetSampler.imageView = pair1.second->mImageView;
			targetSampler.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		else
		{
			targetSampler.sampler = this->mSampler;
			targetSampler.imageView = this->mImageView;
			targetSampler.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}

	/**********************************************
	* Setup context.
	**********************************************/
	context.PrimitiveType = type;

	if (mDevice->mDeviceState.mHasVertexDeclaration)
	{
		context.VertexDeclaration = mDevice->mDeviceState.mVertexDeclaration;
	}
	else if (mDevice->mDeviceState.mHasFVF)
	{
		context.FVF = mDevice->mDeviceState.mFVF;
	}

	if (mDevice->mDeviceState.mHasVertexShader)
	{
		context.VertexShader = mDevice->mDeviceState.mVertexShader;
	}

	if (mDevice->mDeviceState.mHasPixelShader)
	{
		context.PixelShader = mDevice->mDeviceState.mPixelShader;
	}

	context.StreamCount = mDevice->mDeviceState.mStreamSources.size();

	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_FILLMODE);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
	{
		context.FillMode = (D3DFILLMODE)mDevice->mDeviceState.mRenderStates[D3DRS_FILLMODE];
	}
	else
	{
		context.FillMode = D3DFILL_SOLID;
	}

	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_CULLMODE);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
	{
		context.CullMode = (D3DCULL)searchResult->second;
	}
	else
	{
		context.CullMode = D3DCULL_CW;
	}

	int i = 0;
	BOOST_FOREACH(map_type::value_type& source, mDevice->mDeviceState.mStreamSources)
	{
		mVertexInputBindingDescription[i].binding = source.first;
		mVertexInputBindingDescription[i].stride = source.second.Stride;
		mVertexInputBindingDescription[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		context.Bindings[source.first] = source.second.Stride;

		i++;
	}

	/**********************************************
	* Check for existing pipeline. Create one if there isn't a matching one.
	**********************************************/

	for (size_t i = 0; i < mDrawBuffer.size(); i++)
	{
		if (mDrawBuffer[i].PrimitiveType == context.PrimitiveType
			&& mDrawBuffer[i].FVF == context.FVF
			&& mDrawBuffer[i].VertexDeclaration == context.VertexDeclaration
			&& mDrawBuffer[i].VertexShader == context.VertexShader
			&& mDrawBuffer[i].PixelShader == context.PixelShader
			&& mDrawBuffer[i].StreamCount == context.StreamCount
			&& mDrawBuffer[i].FillMode == context.FillMode
			&& mDrawBuffer[i].CullMode == context.CullMode)
		{
			BOOL isMatch = true;
			BOOST_FOREACH(const auto& pair, context.Bindings)
			{
				if (mDrawBuffer[i].Bindings.count(pair.first) == 0 || pair.second != mDrawBuffer[i].Bindings[pair.first])
				{
					isMatch = false;
					break;
				}
			}
			if (isMatch)
			{
				context.Pipeline = mDrawBuffer[i].Pipeline;
				context.PipelineLayout = mDrawBuffer[i].PipelineLayout;
				context.DescriptorSetLayout = mDrawBuffer[i].DescriptorSetLayout;
			}
		}
	}

	if (context.Pipeline == VK_NULL_HANDLE)
	{
		CreatePipe(context);
	}	

	/**********************************************
	* Check for existing DescriptorSet. Create one if there isn't a matching one.
	**********************************************/

	//TODO: add logic to check for existing descriptor.
	CreateDescriptorSet(context);

	/**********************************************
	* Setup bindings
	**********************************************/

	vkCmdBindDescriptorSets(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context.PipelineLayout, 0, 1, &context.DescriptorSet, 0, nullptr);

	vkCmdBindPipeline(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context.Pipeline);	

	mVertexCount = 0;

	BOOST_FOREACH(map_type::value_type& source, mDevice->mDeviceState.mStreamSources)
	{
		vkCmdBindVertexBuffers(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], source.first, 1, &source.second.StreamData->mBuffer, &source.second.OffsetInBytes);
		mVertexCount += source.second.StreamData->mSize;
	}

	if (mDevice->mDeviceState.mIndexBuffer != nullptr)
	{
		vkCmdBindIndexBuffer(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], mDevice->mDeviceState.mIndexBuffer->mBuffer, 0, mDevice->mDeviceState.mIndexBuffer->mIndexType);
	}	
}

void BufferManager::CreatePipe(DrawContext& context)
{
	VkResult result = VK_SUCCESS;

	/**********************************************
	* Figure out flags
	**********************************************/
	uint32_t attributeCount = 0;
	uint32_t textureCount = 0;
	BOOL hasColor = 0;
	BOOL hasPosition = 0;
	BOOL hasNormal = 0;

	if (context.VertexDeclaration != nullptr)
	{
		hasColor = context.VertexDeclaration->mHasColor;
		hasPosition = context.VertexDeclaration->mHasPosition;
		hasNormal = context.VertexDeclaration->mHasNormal;
		textureCount = context.VertexDeclaration->mTextureCount;
	}
	else if (context.FVF)
	{
		if ((context.FVF & D3DFVF_XYZ) == D3DFVF_XYZ)
		{
			hasPosition = true;
		}

		if ((context.FVF & D3DFVF_NORMAL) == D3DFVF_NORMAL)
		{
			hasNormal = true;
		}

		if ((context.FVF & D3DFVF_PSIZE) == D3DFVF_PSIZE)
		{
			BOOST_LOG_TRIVIAL(warning) << "BufferManager::CreatePipe D3DFVF_PSIZE is not implemented!";
		}

		if ((context.FVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
		{
			hasColor = true;
		}

		if ((context.FVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
		{
			BOOST_LOG_TRIVIAL(warning) << "BufferManager::CreatePipe D3DFVF_SPECULAR is not implemented!";
		}

		textureCount = ConvertFormat(context.FVF);
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe unsupported layout definition.";
	}

	attributeCount += hasColor;
	attributeCount += hasPosition;
	attributeCount += hasNormal;
	attributeCount += textureCount;

	/**********************************************
	* Figure out render states & texture states
	**********************************************/
	/*
	I'll need to review this because d3d9 looks like these are per texture but it appears to be per pipeline in vulkan.
	For now I'll use the first texture and go from there.
	*/

	//auto stageSearchResult = mDevice->mDeviceState.mTextureStageStates.find(0);
	//if (stageSearchResult != mDevice->mDeviceState.mTextureStageStates.end())
	//{
	//	auto firstTextureStage = mDevice->mDeviceState.mTextureStageStates[0];
	//	auto stageSearchResult = firstTextureStage.find(D3DTSS_COLOROP);
	//	if (stageSearchResult != firstTextureStage.end())
	//	{
	//		//mPipelineColorBlendAttachmentState[0].colorBlendOp = ConvertColorOperation(mDevice->mDeviceState.mTextureStageStates[0][D3DTSS_COLOROP]);

	//	}

	//	stageSearchResult = firstTextureStage.find(D3DTSS_ALPHAOP);
	//	if (stageSearchResult != firstTextureStage.end())
	//	{
	//		//mPipelineColorBlendAttachmentState[0].alphaBlendOp = ConvertColorOperation(mDevice->mDeviceState.mTextureStageStates[0][D3DTSS_ALPHAOP]);

	//	}

	//	//mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA; //revisit
	//	//mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE; //revisit
	//	//mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //revisit
	//	//mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; //revisit


	//	//if (mPipelineColorBlendAttachmentState[0].colorBlendOp  != VK_BLEND_OP_MAX_ENUM || mPipelineColorBlendAttachmentState[0].alphaBlendOp != VK_BLEND_OP_MAX_ENUM)
	//	//{
	//	//	mPipelineColorBlendAttachmentState[0].blendEnable = VK_TRUE;
	//	//}
	//	//else
	//	//{
	//	//	mPipelineColorBlendAttachmentState[0].blendEnable = VK_FALSE;
	//	//}
	//}

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

	SetCulling(mPipelineRasterizationStateCreateInfo, context.CullMode);
	mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode(context.FillMode);
	mPipelineInputAssemblyStateCreateInfo.topology = ConvertPrimitiveType(context.PrimitiveType);

	/**********************************************
	* Figure out correct shader
	**********************************************/
	if (hasPosition && !hasColor && !hasNormal)
	{
		switch (textureCount)
		{
		case 0:
			//No textures.
			break;
		case 1:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_TEX1;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_TEX1;
			break;
		default:
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe unsupported texture count " << textureCount;
			break;
		}
	}
	else if (hasPosition && hasColor && !hasNormal)
	{
		switch (textureCount)
		{
		case 0:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_DIFFUSE;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_DIFFUSE;
			break;
		case 1:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_DIFFUSE_TEX1;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_DIFFUSE_TEX1;
			break;
		default:
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe unsupported texture count " << textureCount;
			break;
		}
	}
	else if (hasPosition && hasColor && hasNormal)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe normals are not fully yet supported.";
		switch (textureCount)
		{
		case 0:
			//No textures.
			break;
		default:
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe unsupported texture count " << textureCount;
			break;
		}
	}
	else if (hasPosition && !hasColor && hasNormal)
	{
		switch (textureCount)
		{
		case 0:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_NORMAL;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_NORMAL;
			break;
		default:
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe unsupported texture count " << textureCount;
			break;
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe unsupported layout.";
	}

	/**********************************************
	* Figure out attributes
	**********************************************/
	if (context.VertexDeclaration != nullptr)
	{
		uint32_t textureIndex = 0;

		attributeCount = context.VertexDeclaration->mVertexElements.size();

		for (size_t i = 0; i < attributeCount; i++)
		{
			D3DVERTEXELEMENT9& element = context.VertexDeclaration->mVertexElements[i];

			int t = D3DDECLTYPE_FLOAT3;

			mVertexInputAttributeDescription[i].binding = element.Stream;
			//mVertexInputAttributeDescription[i].location = location;
			mVertexInputAttributeDescription[i].format = ConvertDeclType((D3DDECLTYPE)element.Type);
			mVertexInputAttributeDescription[i].offset = element.Offset;

			switch ((D3DDECLUSAGE)element.Usage)
			{
			case D3DDECLUSAGE_POSITION:
				mVertexInputAttributeDescription[i].location = 0;
				break;
			case D3DDECLUSAGE_BLENDWEIGHT:
				break;
			case D3DDECLUSAGE_BLENDINDICES:
				break;
			case D3DDECLUSAGE_NORMAL:
				mVertexInputAttributeDescription[i].location = hasPosition;
				break;
			case D3DDECLUSAGE_PSIZE:
				break;
			case D3DDECLUSAGE_TEXCOORD:
				mVertexInputAttributeDescription[i].location = hasPosition + hasNormal + hasColor + textureIndex;
				textureIndex += 1;
				break;
			case D3DDECLUSAGE_TANGENT:
				break;
			case D3DDECLUSAGE_BINORMAL:
				break;
			case D3DDECLUSAGE_TESSFACTOR:
				break;
			case D3DDECLUSAGE_POSITIONT:
				break;
			case D3DDECLUSAGE_COLOR:
				mVertexInputAttributeDescription[i].location = hasPosition + hasNormal;
				break;
			case D3DDECLUSAGE_FOG:
				break;
			case D3DDECLUSAGE_DEPTH:
				break;
			case D3DDECLUSAGE_SAMPLE:
				break;
			default:
				break;
			}
		}
	}
	else if (context.FVF)
	{
		//revisit - make sure multiple sources is valid for FVF.
		for (size_t i = 0; i < context.StreamCount; i++)
		{
			int attributeIndex = i * attributeCount;
			uint32_t offset = 0;
			uint32_t location = 0;

			if (hasPosition)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = i;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_R32G32B32_SFLOAT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 3);
				location += 1;
				attributeIndex += 1;
			}

			if (hasNormal)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = i;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_R32G32B32_SFLOAT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 3);
				location += 1;
				attributeIndex += 1;
			}

			//D3DFVF_PSIZE

			if ((context.FVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = i;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_B8G8R8A8_UINT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += sizeof(uint32_t);
				location += 1;
				attributeIndex += 1;
			}

			if ((context.FVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = i;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_B8G8R8A8_UINT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += sizeof(uint32_t);
				location += 1;
				attributeIndex += 1;
			}

			for (size_t j = 0; j < textureCount; j++)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = i;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_R32G32_SFLOAT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 2);
				location += 1;
				attributeIndex += 1;
			}
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw unknown vertex format.";
	}

	mPipelineLayoutCreateInfo.pSetLayouts = &context.DescriptorSetLayout;
	mPipelineLayoutCreateInfo.setLayoutCount = 1;

	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = context.StreamCount;
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount;

	mDescriptorSetLayoutBinding[0].binding = 0;
	mDescriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
	mDescriptorSetLayoutBinding[0].descriptorCount = 1;
	mDescriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // VK_SHADER_STAGE_ALL_GRAPHICS
	mDescriptorSetLayoutBinding[0].pImmutableSamplers = NULL;

	mDescriptorSetLayoutBinding[1].binding = 1;
	mDescriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
	mDescriptorSetLayoutBinding[1].descriptorCount = textureCount; //Update to use mapped texture.
	mDescriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	mDescriptorSetLayoutBinding[1].pImmutableSamplers = NULL;

	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;
	if (textureCount)
	{
		mDescriptorSetLayoutCreateInfo.bindingCount = 2; //The number of elements in pBindings.
	}
	else
	{
		mDescriptorSetLayoutCreateInfo.bindingCount = 1; //Ignore second element if there are no textures.
	}

	/**********************************************
	* Create pipeline & descriptor set layout.
	**********************************************/

	result = vkCreateDescriptorSetLayout(mDevice->mDevice, &mDescriptorSetLayoutCreateInfo, nullptr, &context.DescriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreateDescriptorSet vkCreateDescriptorSetLayout failed with return code of " << result;
		return;
	}

	result = vkCreatePipelineLayout(mDevice->mDevice, &mPipelineLayoutCreateInfo, nullptr, &context.PipelineLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreatePipelineLayout failed with return code of " << result;
		return;
	}

	mGraphicsPipelineCreateInfo.layout = context.PipelineLayout;

	result = vkCreateGraphicsPipelines(mDevice->mDevice, mPipelineCache, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	//result = vkCreateGraphicsPipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreateGraphicsPipelines failed with return code of " << result;
	}

	this->mDrawBuffer.push_back(context);
}

void BufferManager::CreateDescriptorSet(DrawContext& context)
{
	VkResult result = VK_SUCCESS;

	mDescriptorSetAllocateInfo.pSetLayouts = &context.DescriptorSetLayout;
	mDescriptorSetAllocateInfo.descriptorSetCount = 1;

	/**********************************************
	* Create Descriptor set
	**********************************************/

	result = vkAllocateDescriptorSets(mDevice->mDevice, &mDescriptorSetAllocateInfo, &context.DescriptorSet);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreateDescriptorSet vkAllocateDescriptorSets failed with return code of " << result;
		return;
	}

	mDevice->mGarbageManager.mDescriptorSets.push_back(context.DescriptorSet);

	mDescriptorBufferInfo.buffer = mUniformBuffer;

	mWriteDescriptorSet[0].dstSet = context.DescriptorSet;
	mWriteDescriptorSet[0].descriptorCount = 1;
	mWriteDescriptorSet[0].pBufferInfo = &mDescriptorBufferInfo;

	mWriteDescriptorSet[1].dstSet = context.DescriptorSet;
	mWriteDescriptorSet[1].descriptorCount = mDevice->mDeviceState.mTextures.size(); //16; //Update to use mapped texture.
	mWriteDescriptorSet[1].pImageInfo = mDevice->mDeviceState.mDescriptorImageInfo;

	if (mDevice->mDeviceState.mTextures.size())
	{
		vkUpdateDescriptorSets(mDevice->mDevice, 2, mWriteDescriptorSet, 0, nullptr);
	}
	else
	{
		vkUpdateDescriptorSets(mDevice->mDevice, 1, mWriteDescriptorSet, 0, nullptr);
	}
}

void BufferManager::CreateSampler(SamplerRequest& request)
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb172602(v=vs.85).aspx
	//Mipmap filter to use during minification. See D3DTEXTUREFILTERTYPE. The default value is D3DTEXF_NONE.

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = NULL;

	samplerCreateInfo.magFilter = ConvertFilter(request.MagFilter);
	samplerCreateInfo.minFilter = ConvertFilter(request.MinFilter);
	samplerCreateInfo.addressModeU = ConvertTextureAddress(request.AddressModeU);
	samplerCreateInfo.addressModeV = ConvertTextureAddress(request.AddressModeV);
	samplerCreateInfo.addressModeW = ConvertTextureAddress(request.AddressModeW);
	samplerCreateInfo.maxAnisotropy = request.MaxAnisotropy;  //16 D3DSAMP_MAXANISOTROPY
	samplerCreateInfo.mipmapMode = ConvertMipmapMode(request.MipmapMode); //VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.mipLodBias = request.MipLodBias;

	if (samplerCreateInfo.maxAnisotropy)
	{
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
	}
	else
	{
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
	}

	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; // VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	//samplerCreateInfo.maxLod = (float)mLevels;

	mResult = vkCreateSampler(mDevice->mDevice, &samplerCreateInfo, NULL, &request.Sampler);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::GenerateSampler vkCreateSampler failed with return code of " << mResult;
		return;
	}

	mSamplerRequests.push_back(request);
}

void BufferManager::UpdateUniformBuffer()
{
	VkResult result;
	void* data = nullptr;
	DeviceState&  deviceState = mDevice->mDeviceState;

	BOOST_FOREACH(const auto& pair1, deviceState.mTransforms)
	{
		switch (pair1.first)
		{
		case D3DTS_WORLD:
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					mUBO.model[i][j] = pair1.second.m[i][j];
				}
			}
			break;
		case D3DTS_VIEW:
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					mUBO.view[i][j] = pair1.second.m[i][j];
				}
			}
			break;
		case D3DTS_PROJECTION:
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					mUBO.proj[i][j] = pair1.second.m[i][j];
				}
			}
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "BufferManager::UpdateUniformBuffer The following state type was ignored. " << pair1.first;
			break;
		}
	}

	//Create a new buffer.
	mUniformBuffer = VK_NULL_HANDLE;
	mUniformBufferMemory = VK_NULL_HANDLE;
	CreateBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mUniformBuffer, mUniformBufferMemory);
	
	//Put the new buffer into the list so it can be cleaned up at the end of the render pass.
	HistoricalUniformBuffer historicalUniformBuffer;
	historicalUniformBuffer.UniformBuffer = mUniformBuffer;
	historicalUniformBuffer.UniformBufferMemory = mUniformBufferMemory;
	mHistoricalUniformBuffers.push_back(historicalUniformBuffer);

	//Copy current UBO into the staging memory buffer.
	result = vkMapMemory(mDevice->mDevice, mUniformStagingBufferMemory, 0, sizeof(UniformBufferObject), 0, &data);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetTransform vkMapMemory failed with return code of " << result;
		return;
	}
	memcpy(data, &mUBO, sizeof(UniformBufferObject));
	vkUnmapMemory(mDevice->mDevice, mUniformStagingBufferMemory);

	//Copy the staging data into the new buffer.
	CopyBuffer(mUniformStagingBuffer, mUniformBuffer, sizeof(UniformBufferObject));
}

void BufferManager::FlushDrawBufffer()
{
	for (size_t i = 0; i < mDrawBuffer.size(); i++)
	{
		if (mDrawBuffer[i].Pipeline!= VK_NULL_HANDLE)
		{
			vkDestroyPipeline(mDevice->mDevice, mDrawBuffer[i].Pipeline, NULL);
		}
		if (mDrawBuffer[i].PipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(mDevice->mDevice, mDrawBuffer[i].PipelineLayout, NULL);
		}
		if (mDrawBuffer[i].DescriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(mDevice->mDevice, mDrawBuffer[i].DescriptorSetLayout, NULL);
		}		
	}
	mDrawBuffer.clear();

	for (size_t i = 0; i < mHistoricalUniformBuffers.size(); i++)
	{		
		if (mHistoricalUniformBuffers[i].UniformBuffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(mDevice->mDevice, mHistoricalUniformBuffers[i].UniformBuffer, NULL);
		}

		if (mHistoricalUniformBuffers[i].UniformBufferMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(mDevice->mDevice, mHistoricalUniformBuffers[i].UniformBufferMemory, NULL);
		}
	}
	mHistoricalUniformBuffers.clear();

	for (size_t i = 0; i < mSamplerRequests.size(); i++)
	{
		if (mSamplerRequests[i].Sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(mDevice->mDevice, mSamplerRequests[i].Sampler, NULL);
		}
	}
	mSamplerRequests.clear();
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