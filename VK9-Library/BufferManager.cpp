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

	//mPipelineColorBlendAttachmentState[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //0xf;
	//mPipelineColorBlendAttachmentState[0].blendEnable = VK_FALSE;

	mPipelineColorBlendAttachmentState[0].colorWriteMask = 0xf;
	mPipelineColorBlendAttachmentState[0].blendEnable = VK_TRUE;
	mPipelineColorBlendAttachmentState[0].colorBlendOp = VK_BLEND_OP_ADD;
	mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR; //revisit
	mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR; //revisit
	mPipelineColorBlendAttachmentState[0].alphaBlendOp = VK_BLEND_OP_ADD;
	mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //revisit
	mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // VK_BLEND_FACTOR_DST_ALPHA; //revisit

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
	mWriteDescriptorSet[1].pImageInfo = mDevice->mDeviceState.mDescriptorImageInfo;
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
}

void BufferManager::BeginDraw(DrawContext& context, D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;
	

	/**********************************************
	* Plug texture state stuff into pipeline.
	**********************************************/

	/*
	I'll need to review this because d3d9 looks like these are per texture but it appears to be per pipeline in vulkan.
	For now I'll use the first texture and go from there.
	*/

	auto stageSearchResult = mDevice->mDeviceState.mTextureStageStates.find(0);
	if (stageSearchResult != mDevice->mDeviceState.mTextureStageStates.end())
	{
		auto stageSearchResult = mDevice->mDeviceState.mTextureStageStates[0].find(D3DTSS_COLOROP);
		if (stageSearchResult != mDevice->mDeviceState.mTextureStageStates[0].end())
		{
			//mPipelineColorBlendAttachmentState[0].colorBlendOp = ConvertColorOperation(mDevice->mDeviceState.mTextureStageStates[0][D3DTSS_COLOROP]);

		}

		stageSearchResult = mDevice->mDeviceState.mTextureStageStates[0].find(D3DTSS_ALPHAOP);
		if (stageSearchResult != mDevice->mDeviceState.mTextureStageStates[0].end())
		{
			//mPipelineColorBlendAttachmentState[0].alphaBlendOp = ConvertColorOperation(mDevice->mDeviceState.mTextureStageStates[0][D3DTSS_ALPHAOP]);

		}

		//mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA; //revisit
		//mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE; //revisit
		//mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //revisit
		//mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; //revisit


		//if (mPipelineColorBlendAttachmentState[0].colorBlendOp  != VK_BLEND_OP_MAX_ENUM || mPipelineColorBlendAttachmentState[0].alphaBlendOp != VK_BLEND_OP_MAX_ENUM)
		//{
		//	mPipelineColorBlendAttachmentState[0].blendEnable = VK_TRUE;
		//}
		//else
		//{
		//	mPipelineColorBlendAttachmentState[0].blendEnable = VK_FALSE;
		//}
	}

	std::unordered_map<D3DRENDERSTATETYPE, DWORD>::const_iterator searchResult;

	/**********************************************
	* Update Pipe Creation Information
	**********************************************/
	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_FILLMODE);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
	{
		mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode((D3DFILLMODE)mDevice->mDeviceState.mRenderStates[D3DRS_FILLMODE]);
	}

	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_CULLMODE);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
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

	mDescriptorSetLayoutBinding[0].binding = 0;
	mDescriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
	mDescriptorSetLayoutBinding[0].descriptorCount = 1;
	mDescriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // VK_SHADER_STAGE_ALL_GRAPHICS
	mDescriptorSetLayoutBinding[0].pImmutableSamplers = NULL;

	mDescriptorSetLayoutBinding[1].binding = 1;
	mDescriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
	mDescriptorSetLayoutBinding[1].descriptorCount = 16; //Update to use mapped texture.
	mDescriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	mDescriptorSetLayoutBinding[1].pImmutableSamplers = NULL;

	/**********************************************
	* Figure out flags
	**********************************************/
	uint32_t attributeCount = 0;
	uint32_t textureCount = 0;
	BOOL hasColor = 0;
	BOOL hasPosition = 0;
	BOOL hasNormal = 0;

	if (mDevice->mDeviceState.mVertexDeclaration != nullptr)
	{
		attributeCount += mDevice->mDeviceState.mVertexDeclaration->mHasColor;
		attributeCount += mDevice->mDeviceState.mVertexDeclaration->mHasPosition;
		attributeCount += mDevice->mDeviceState.mVertexDeclaration->mHasNormal;
		attributeCount += mDevice->mDeviceState.mVertexDeclaration->mTextureCount;

		hasColor = mDevice->mDeviceState.mVertexDeclaration->mHasColor;
		hasPosition = mDevice->mDeviceState.mVertexDeclaration->mHasPosition;
		hasNormal = mDevice->mDeviceState.mVertexDeclaration->mHasNormal;
		textureCount = mDevice->mDeviceState.mVertexDeclaration->mTextureCount;
	}
	else if (mDevice->mDeviceState.mFVF)
	{
		attributeCount += mDevice->mDeviceState.mFVFHasColor;
		attributeCount += mDevice->mDeviceState.mFVFHasNormal;
		attributeCount += mDevice->mDeviceState.mFVFHasPosition;
		attributeCount += mDevice->mDeviceState.mFVFTextureCount;

		hasNormal = mDevice->mDeviceState.mFVFHasNormal;
		hasColor = mDevice->mDeviceState.mFVFHasColor;
		hasPosition = mDevice->mDeviceState.mFVFHasPosition;
		textureCount = mDevice->mDeviceState.mFVFTextureCount;
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw unsupported layout definition.";
	}

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
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw unsupported texture count " << textureCount;
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
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw unsupported texture count " << textureCount;
			break;
		}
	}
	else if (hasPosition && hasColor && hasNormal)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw normals are not fully yet supported.";
		switch (textureCount)
		{
		case 0:
			//No textures.
			break;
		default:
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw unsupported texture count " << textureCount;
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
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw unsupported texture count " << textureCount;
			break;
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw unsupported layout.";
	}

	/**********************************************
	* Setup bindings
	**********************************************/
	int i = 0;
	BOOST_FOREACH(map_type::value_type& source, mDevice->mDeviceState.mStreamSources)
	{
		mVertexInputBindingDescription[i].binding = source.first;
		mVertexInputBindingDescription[i].stride = source.second.Stride;
		mVertexInputBindingDescription[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		i++;
	}

	if (mDevice->mDeviceState.mVertexDeclaration != nullptr)
	{
		uint32_t textureIndex = 0;

		attributeCount = mDevice->mDeviceState.mVertexDeclaration->mVertexElements.size();

		for (size_t i = 0; i < attributeCount; i++)
		{
			D3DVERTEXELEMENT9& element = mDevice->mDeviceState.mVertexDeclaration->mVertexElements[i];

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
	else if (mDevice->mDeviceState.mFVF)
	{
		//revisit - make sure multiple sources is valid for FVF.
		i = 0;
		BOOST_FOREACH(map_type::value_type& source, mDevice->mDeviceState.mStreamSources)
		{
			int attributeIndex = i * attributeCount;
			uint32_t offset = 0;
			uint32_t location = 0;

			if (hasPosition)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = source.first;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_R32G32B32_SFLOAT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 3);
				location += 1;
				attributeIndex += 1;
			}

			if (hasNormal)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = source.first;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_R32G32B32_SFLOAT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 3);
				location += 1;
				attributeIndex += 1;
			}

			//D3DFVF_PSIZE

			if ((mDevice->mDeviceState.mFVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = source.first;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_B8G8R8A8_UINT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += sizeof(uint32_t);
				location += 1;
				attributeIndex += 1;
			}

			if ((mDevice->mDeviceState.mFVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = source.first;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_B8G8R8A8_UINT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += sizeof(uint32_t);
				location += 1;
				attributeIndex += 1;
			}

			for (size_t j = 0; j < textureCount; j++)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = source.first;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_R32G32_SFLOAT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 2);
				location += 1;
				attributeIndex += 1;
			}

			i++;
		}
	}

	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;
	mDescriptorSetLayoutCreateInfo.bindingCount = 2; //The number of elements in pBindings.

	mDescriptorSetAllocateInfo.pSetLayouts = &context.DescriptorSetLayout;
	mDescriptorSetAllocateInfo.descriptorSetCount = 1; //mDevice->mDeviceState.mStreamSources.size(); //determines the number of descriptor sets to be allocated from the pool.

	mPipelineLayoutCreateInfo.pSetLayouts = &context.DescriptorSetLayout;
	mPipelineLayoutCreateInfo.setLayoutCount = 1; // mDevice->mDeviceState.mStreamSources.size(); // The number of descriptor sets included in the pipeline layout.

	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = mDevice->mDeviceState.mStreamSources.size();
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount; //mDevice->mDeviceState.mStreamSources.size() *



	/**********************************************
	* Create Pipe
	**********************************************/
	result = vkCreateDescriptorSetLayout(mDevice->mDevice, &mDescriptorSetLayoutCreateInfo, nullptr, &context.DescriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreateDescriptorSetLayout failed with return code of " << result;
		return;
	}

	mDescriptorSetAllocateInfo.pSetLayouts = &context.DescriptorSetLayout;

	result = vkAllocateDescriptorSets(mDevice->mDevice, &mDescriptorSetAllocateInfo, &context.DescriptorSet);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkAllocateDescriptorSets failed with return code of " << result;
		return;
	}

	mDescriptorBufferInfo.buffer = mUniformBuffer;

	mWriteDescriptorSet[0].dstSet = context.DescriptorSet;
	mWriteDescriptorSet[0].descriptorCount = 1;
	mWriteDescriptorSet[0].pBufferInfo = &mDescriptorBufferInfo;

	mWriteDescriptorSet[1].dstSet = context.DescriptorSet;
	mWriteDescriptorSet[1].descriptorCount = 16;
	mWriteDescriptorSet[1].pImageInfo = mDevice->mDeviceState.mDescriptorImageInfo;

	vkUpdateDescriptorSets(mDevice->mDevice, 2, mWriteDescriptorSet, 0, nullptr);

	result = vkCreatePipelineLayout(mDevice->mDevice, &mPipelineLayoutCreateInfo, nullptr, &context.PipelineLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreatePipelineLayout failed with return code of " << result;
		return;
	}

	mGraphicsPipelineCreateInfo.layout = context.PipelineLayout;

	/*
	Pipeline creation is expensive I need to find a way to reuse these.
	*/
	result = vkCreateGraphicsPipelines(mDevice->mDevice, mPipelineCache, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	//result = vkCreateGraphicsPipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreateGraphicsPipelines failed with return code of " << result;
		//Don't return so we can destroy cache.
	}

	vkCmdBindPipeline(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context.Pipeline);
	vkCmdBindDescriptorSets(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context.PipelineLayout, 0, 1, &context.DescriptorSet, 0, nullptr);

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

	this->mDrawBuffer.push_back(context);
}

void BufferManager::UpdateUniformBuffer(BOOL recalculateMatrices)
{
	VkResult result;
	void* data = nullptr;

	if (recalculateMatrices)
	{
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
	}

	//Copy current UBO into staging memory buffer.
	result = vkMapMemory(mDevice->mDevice, mUniformStagingBufferMemory, 0, sizeof(UniformBufferObject), 0, &data);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetTransform vkMapMemory failed with return code of " << result;
		return;
	}
	memcpy(data, &mUBO, sizeof(UniformBufferObject));
	vkUnmapMemory(mDevice->mDevice, mUniformStagingBufferMemory);

	//Move the existing buffer into the list so it can be cleaned up at the end of the render pass.
	HistoricalUniformBuffer historicalUniformBuffer;
	historicalUniformBuffer.UniformBuffer = mUniformBuffer;
	historicalUniformBuffer.UniformBufferMemory = mUniformBufferMemory;
	mHistoricalUniformBuffers.push_back(historicalUniformBuffer);

	//Create a new buffer and copy the staging data into it.
	CreateBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mUniformBuffer, mUniformBufferMemory);
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
		if (mDrawBuffer[i].DescriptorSet != VK_NULL_HANDLE)
		{
			vkFreeDescriptorSets(mDevice->mDevice, mDevice->mDescriptorPool, 1, &mDrawBuffer[i].DescriptorSet);
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