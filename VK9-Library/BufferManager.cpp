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

#define CACHE_SECONDS 1

typedef boost::container::flat_map<UINT, StreamSource> map_type;

BufferManager::BufferManager()
{
	//Don't use. This is only here for containers.
}

BufferManager::BufferManager(CDevice9* device)
	: mDevice(device)
{
	mModel <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	mView <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	mProjection <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	mVertShaderModule_XYZ_DIFFUSE = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE.vert.spv");
	mFragShaderModule_XYZ_DIFFUSE = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE.frag.spv");

	mVertShaderModule_XYZ_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_TEX1.vert.spv");
	mFragShaderModule_XYZ_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_TEX1.frag.spv");

	mVertShaderModule_XYZ_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_TEX2.vert.spv");
	mFragShaderModule_XYZ_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_TEX2.frag.spv");

	mVertShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX1.vert.spv");
	mFragShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX1.frag.spv");

	mVertShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX2.vert.spv");
	mFragShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX2.frag.spv");

	mVertShaderModule_XYZ_NORMAL = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL.vert.spv");
	mFragShaderModule_XYZ_NORMAL = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL.frag.spv");

	mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.vert.spv");
	mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.frag.spv");

	mPushConstantRanges[0].offset = 0;
	mPushConstantRanges[0].size = UBO_SIZE;
	mPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	mSpecializationInfo.pData = &mDevice->mDeviceState.mSpecializationConstants;
	mSpecializationInfo.dataSize = sizeof(SpecializationConstants);

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
	mDescriptorSetLayoutCreateInfo.bindingCount = 1;
	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;

	mDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	mDescriptorSetAllocateInfo.pNext = NULL;
	mDescriptorSetAllocateInfo.descriptorPool = mDevice->mDescriptorPool;
	mDescriptorSetAllocateInfo.descriptorSetCount = 1;
	//mDescriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

	mPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	mPipelineLayoutCreateInfo.pNext = NULL;
	mPipelineLayoutCreateInfo.setLayoutCount = 1;

	mGraphicsPipelineCreateInfo.stageCount = 2;

	mPipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_DIFFUSE;
	mPipelineShaderStageCreateInfo[0].pName = "main";
	mPipelineShaderStageCreateInfo[0].pSpecializationInfo = &mSpecializationInfo;

	mPipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_DIFFUSE;
	mPipelineShaderStageCreateInfo[1].pName = "main";
	mPipelineShaderStageCreateInfo[1].pSpecializationInfo = &mSpecializationInfo;

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

	mResult = vkCreateSampler(mDevice->mDevice, &samplerCreateInfo, NULL, &mSampler);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BufferManager vkCreateSampler failed with return code of " << mResult;
		return;
	}

	imageViewCreateInfo.image = mImage;

	mResult = vkCreateImageView(mDevice->mDevice, &imageViewCreateInfo, NULL, &mImageView);
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

	mWriteDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//mWriteDescriptorSet[1].dstSet = descriptorSet;
	mWriteDescriptorSet[0].dstBinding = 0;
	mWriteDescriptorSet[0].dstArrayElement = 0;
	mWriteDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	mWriteDescriptorSet[0].descriptorCount = 1;
	mWriteDescriptorSet[0].pImageInfo = mDevice->mDeviceState.mDescriptorImageInfo;

	mWriteDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//mWriteDescriptorSet[1].dstSet = descriptorSet;
	mWriteDescriptorSet[1].dstBinding = 1;
	mWriteDescriptorSet[1].dstArrayElement = 0;
	mWriteDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mWriteDescriptorSet[1].descriptorCount = 2;
	mWriteDescriptorSet[1].pBufferInfo = mDescriptorBufferInfo;

	//Command Buffer Setup
	mCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mCommandBufferAllocateInfo.commandPool = mDevice->mCommandPool;
	mCommandBufferAllocateInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(mDevice->mDevice, &mCommandBufferAllocateInfo, &mCommandBuffer);

	mBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	mBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	mSubmitInfo.commandBufferCount = 1;
	mSubmitInfo.pCommandBuffers = &mCommandBuffer;

	//revisit - light should be sized dynamically. Really more that 4 lights is stupid but this limit isn't correct behavior.
	CreateBuffer(sizeof(D3DLIGHT9)*4, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mLightBuffer, mLightBufferMemory);
	CreateBuffer(sizeof(D3DMATERIAL9), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mMaterialBuffer, mMaterialBufferMemory);
}

BufferManager::~BufferManager()
{
	if (mCommandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(mDevice->mDevice, mDevice->mCommandPool, 1, &mCommandBuffer);
		mCommandBuffer = VK_NULL_HANDLE;
	}

	if (mLightBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice->mDevice, mLightBuffer, NULL);
		mLightBuffer = VK_NULL_HANDLE;
	}

	if (mLightBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mLightBufferMemory, NULL);
		mLightBufferMemory = VK_NULL_HANDLE;
	}

	if (mMaterialBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice->mDevice, mMaterialBuffer, NULL);
		mMaterialBuffer = VK_NULL_HANDLE;
	}

	if (mMaterialBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mMaterialBufferMemory, NULL);
		mMaterialBufferMemory = VK_NULL_HANDLE;
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

	if (mVertShaderModule_XYZ_TEX2 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_TEX2, NULL);
		mVertShaderModule_XYZ_TEX2 = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_TEX2 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_TEX2, NULL);
		mFragShaderModule_XYZ_TEX2 = VK_NULL_HANDLE;
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

	if (mVertShaderModule_XYZ_DIFFUSE_TEX2 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_DIFFUSE_TEX2, NULL);
		mVertShaderModule_XYZ_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_DIFFUSE_TEX2 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_DIFFUSE_TEX2, NULL);
		mFragShaderModule_XYZ_DIFFUSE_TEX2 = VK_NULL_HANDLE;
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

	if (mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2, NULL);
		mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2, NULL);
		mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	}

	if (mPipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(mDevice->mDevice, mPipelineCache, nullptr);
		mPipelineCache = VK_NULL_HANDLE;
	}

	mDrawBuffer.clear();
	mSamplerRequests.clear();

	mUsedResourceBuffer.clear();
	mUnusedResourceBuffer.clear();
}

void BufferManager::BeginDraw(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;
	boost::container::flat_map<D3DRENDERSTATETYPE, DWORD>::const_iterator searchResult;

	/**********************************************
	* Update the textures that are currently mapped.
	**********************************************/
	BOOST_FOREACH(const auto& pair1, mDevice->mDeviceState.mTextures)
	{
		VkDescriptorImageInfo& targetSampler = mDevice->mDeviceState.mDescriptorImageInfo[pair1.first];

		if (pair1.second != nullptr)
		{
			std::shared_ptr<SamplerRequest> request = std::make_shared<SamplerRequest>(mDevice);

			request->MagFilter = (D3DTEXTUREFILTERTYPE)mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_MAGFILTER];
			request->MinFilter = (D3DTEXTUREFILTERTYPE)mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_MINFILTER];
			request->AddressModeU = (D3DTEXTUREADDRESS)mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_ADDRESSU];
			request->AddressModeV = (D3DTEXTUREADDRESS)mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_ADDRESSV];
			request->AddressModeW = (D3DTEXTUREADDRESS)mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_ADDRESSW];
			request->MaxAnisotropy = mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_MAXANISOTROPY];
			request->MipmapMode = (D3DTEXTUREFILTERTYPE)mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_MIPFILTER];
			request->MipLodBias = (float)mDevice->mDeviceState.mSamplerStates[request->SamplerIndex][D3DSAMP_MIPMAPLODBIAS];
			request->MaxLod = pair1.second->mLevels;

			for (size_t i = 0; i < mSamplerRequests.size(); i++)
			{
				auto& storedRequest = mSamplerRequests[i];
				if (request->MagFilter == storedRequest->MagFilter
					&& request->MinFilter == storedRequest->MinFilter
					&& request->AddressModeU == storedRequest->AddressModeU
					&& request->AddressModeV == storedRequest->AddressModeV
					&& request->AddressModeW == storedRequest->AddressModeW
					&& request->MaxAnisotropy == storedRequest->MaxAnisotropy
					&& request->MipmapMode == storedRequest->MipmapMode
					&& request->MipLodBias == storedRequest->MipLodBias
					&& request->MaxLod == storedRequest->MaxLod)
				{
					request->Sampler = storedRequest->Sampler;
					request->mDevice = nullptr; //Not owner.
					storedRequest->LastUsed = std::chrono::steady_clock::now();
				}
			}

			if (request->Sampler == VK_NULL_HANDLE)
			{
				CreateSampler(request);
			}

			targetSampler.sampler = request->Sampler;
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
	context->PrimitiveType = type;

	if (mDevice->mDeviceState.mHasVertexDeclaration)
	{
		context->VertexDeclaration = mDevice->mDeviceState.mVertexDeclaration;
	}
	else if (mDevice->mDeviceState.mHasFVF)
	{
		context->FVF = mDevice->mDeviceState.mFVF;
	}

	if (mDevice->mDeviceState.mHasVertexShader)
	{
		context->VertexShader = mDevice->mDeviceState.mVertexShader;
	}

	if (mDevice->mDeviceState.mHasPixelShader)
	{
		context->PixelShader = mDevice->mDeviceState.mPixelShader;
	}

	context->StreamCount = mDevice->mDeviceState.mStreamSources.size();

	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_FILLMODE);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
	{
		context->FillMode = (D3DFILLMODE)mDevice->mDeviceState.mRenderStates[D3DRS_FILLMODE];
	}
	else
	{
		context->FillMode = D3DFILL_SOLID;
	}

	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_CULLMODE);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
	{
		context->CullMode = (D3DCULL)searchResult->second;
	}
	else
	{
		context->CullMode = D3DCULL_CW;
	}

	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_LIGHTING);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
	{
		mDevice->mDeviceState.mSpecializationConstants.isLightingEnabled = (BOOL)searchResult->second;
	}
	else
	{
		mDevice->mDeviceState.mSpecializationConstants.isLightingEnabled = false;
	}

	context->mSpecializationConstants.lightCount = mDevice->mDeviceState.mLights.size();
	mDevice->mDeviceState.mSpecializationConstants.lightCount = context->mSpecializationConstants.lightCount;

	searchResult = mDevice->mDeviceState.mRenderStates.find(D3DRS_SHADEMODE);
	if (searchResult != mDevice->mDeviceState.mRenderStates.end())
	{
		context->ShadeMode = (D3DSHADEMODE)searchResult->second;
		mDevice->mDeviceState.mSpecializationConstants.shadeMode = searchResult->second;
	}
	else
	{
		context->ShadeMode = D3DSHADE_GOURAUD;
		mDevice->mDeviceState.mSpecializationConstants.shadeMode = D3DSHADE_GOURAUD;
	}

	context->mSpecializationConstants.textureCount = mDevice->mDeviceState.mTextures.size();
	mDevice->mDeviceState.mSpecializationConstants.textureCount = context->mSpecializationConstants.textureCount;


	int i = 0;
	BOOST_FOREACH(map_type::value_type& source, mDevice->mDeviceState.mStreamSources)
	{
		mVertexInputBindingDescription[i].binding = source.first;
		mVertexInputBindingDescription[i].stride = source.second.Stride;
		mVertexInputBindingDescription[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		context->Bindings[source.first] = source.second.Stride;

		i++;
	}

	/**********************************************
	* Check for existing pipeline. Create one if there isn't a matching one.
	**********************************************/

	for (size_t i = 0; i < mDrawBuffer.size(); i++)
	{
		if (mDrawBuffer[i]->PrimitiveType == context->PrimitiveType
			&& mDrawBuffer[i]->FVF == context->FVF
			&& mDrawBuffer[i]->VertexDeclaration == context->VertexDeclaration
			&& mDrawBuffer[i]->VertexShader == context->VertexShader
			&& mDrawBuffer[i]->PixelShader == context->PixelShader
			&& mDrawBuffer[i]->StreamCount == context->StreamCount
			&& mDrawBuffer[i]->FillMode == context->FillMode
			&& mDrawBuffer[i]->CullMode == context->CullMode
			&& mDrawBuffer[i]->mSpecializationConstants.isLightingEnabled == context->mSpecializationConstants.isLightingEnabled
			&& mDrawBuffer[i]->ShadeMode == context->ShadeMode
			&& mDrawBuffer[i]->mSpecializationConstants.lightCount == context->mSpecializationConstants.lightCount

			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_0 == context->mSpecializationConstants.texureCoordinateIndex_0
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_1 == context->mSpecializationConstants.texureCoordinateIndex_1
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_2 == context->mSpecializationConstants.texureCoordinateIndex_2
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_3 == context->mSpecializationConstants.texureCoordinateIndex_3
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_4 == context->mSpecializationConstants.texureCoordinateIndex_4
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_5 == context->mSpecializationConstants.texureCoordinateIndex_5
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_6 == context->mSpecializationConstants.texureCoordinateIndex_6
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_7 == context->mSpecializationConstants.texureCoordinateIndex_7

			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_0 == context->mSpecializationConstants.textureTransformationFlags_0
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_1 == context->mSpecializationConstants.textureTransformationFlags_1
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_2 == context->mSpecializationConstants.textureTransformationFlags_2
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_3 == context->mSpecializationConstants.textureTransformationFlags_3
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_4 == context->mSpecializationConstants.textureTransformationFlags_4
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_5 == context->mSpecializationConstants.textureTransformationFlags_5
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_6 == context->mSpecializationConstants.textureTransformationFlags_6
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_7 == context->mSpecializationConstants.textureTransformationFlags_7

			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_0 == context->mSpecializationConstants.colorOperation_0
			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_1 == context->mSpecializationConstants.colorOperation_1
			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_2 == context->mSpecializationConstants.colorOperation_2
			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_3 == context->mSpecializationConstants.colorOperation_3
			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_4 == context->mSpecializationConstants.colorOperation_4
			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_5 == context->mSpecializationConstants.colorOperation_5
			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_6 == context->mSpecializationConstants.colorOperation_6
			&& mDrawBuffer[i]->mSpecializationConstants.colorOperation_7 == context->mSpecializationConstants.colorOperation_7

			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_0 == context->mSpecializationConstants.colorArgument1_0
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_1 == context->mSpecializationConstants.colorArgument1_1
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_2 == context->mSpecializationConstants.colorArgument1_2
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_3 == context->mSpecializationConstants.colorArgument1_3
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_4 == context->mSpecializationConstants.colorArgument1_4
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_5 == context->mSpecializationConstants.colorArgument1_5
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_6 == context->mSpecializationConstants.colorArgument1_6
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument1_7 == context->mSpecializationConstants.colorArgument1_7

			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_0 == context->mSpecializationConstants.colorArgument2_0
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_1 == context->mSpecializationConstants.colorArgument2_1
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_2 == context->mSpecializationConstants.colorArgument2_2
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_3 == context->mSpecializationConstants.colorArgument2_3
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_4 == context->mSpecializationConstants.colorArgument2_4
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_5 == context->mSpecializationConstants.colorArgument2_5
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_6 == context->mSpecializationConstants.colorArgument2_6
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument2_7 == context->mSpecializationConstants.colorArgument2_7

			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_0 == context->mSpecializationConstants.alphaOperation_0
			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_1 == context->mSpecializationConstants.alphaOperation_1
			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_2 == context->mSpecializationConstants.alphaOperation_2
			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_3 == context->mSpecializationConstants.alphaOperation_3
			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_4 == context->mSpecializationConstants.alphaOperation_4
			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_5 == context->mSpecializationConstants.alphaOperation_5
			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_6 == context->mSpecializationConstants.alphaOperation_6
			&& mDrawBuffer[i]->mSpecializationConstants.alphaOperation_7 == context->mSpecializationConstants.alphaOperation_7

			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_0 == context->mSpecializationConstants.alphaArgument1_0
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_1 == context->mSpecializationConstants.alphaArgument1_1
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_2 == context->mSpecializationConstants.alphaArgument1_2
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_3 == context->mSpecializationConstants.alphaArgument1_3
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_4 == context->mSpecializationConstants.alphaArgument1_4
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_5 == context->mSpecializationConstants.alphaArgument1_5
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_6 == context->mSpecializationConstants.alphaArgument1_6
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument1_7 == context->mSpecializationConstants.alphaArgument1_7

			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_0 == context->mSpecializationConstants.alphaArgument2_0
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_1 == context->mSpecializationConstants.alphaArgument2_1
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_2 == context->mSpecializationConstants.alphaArgument2_2
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_3 == context->mSpecializationConstants.alphaArgument2_3
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_4 == context->mSpecializationConstants.alphaArgument2_4
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_5 == context->mSpecializationConstants.alphaArgument2_5
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_6 == context->mSpecializationConstants.alphaArgument2_6
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument2_7 == context->mSpecializationConstants.alphaArgument2_7

			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_0 == context->mSpecializationConstants.bumpMapMatrix00_0
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_1 == context->mSpecializationConstants.bumpMapMatrix00_1
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_2 == context->mSpecializationConstants.bumpMapMatrix00_2
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_3 == context->mSpecializationConstants.bumpMapMatrix00_3
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_4 == context->mSpecializationConstants.bumpMapMatrix00_4
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_5 == context->mSpecializationConstants.bumpMapMatrix00_5
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_6 == context->mSpecializationConstants.bumpMapMatrix00_6
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix00_7 == context->mSpecializationConstants.bumpMapMatrix00_7

			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_0 == context->mSpecializationConstants.bumpMapMatrix01_0
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_1 == context->mSpecializationConstants.bumpMapMatrix01_1
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_2 == context->mSpecializationConstants.bumpMapMatrix01_2
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_3 == context->mSpecializationConstants.bumpMapMatrix01_3
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_4 == context->mSpecializationConstants.bumpMapMatrix01_4
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_5 == context->mSpecializationConstants.bumpMapMatrix01_5
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_6 == context->mSpecializationConstants.bumpMapMatrix01_6
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix01_7 == context->mSpecializationConstants.bumpMapMatrix01_7

			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_0 == context->mSpecializationConstants.bumpMapMatrix10_0
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_1 == context->mSpecializationConstants.bumpMapMatrix10_1
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_2 == context->mSpecializationConstants.bumpMapMatrix10_2
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_3 == context->mSpecializationConstants.bumpMapMatrix10_3
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_4 == context->mSpecializationConstants.bumpMapMatrix10_4
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_5 == context->mSpecializationConstants.bumpMapMatrix10_5
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_6 == context->mSpecializationConstants.bumpMapMatrix10_6
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix10_7 == context->mSpecializationConstants.bumpMapMatrix10_7

			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_0 == context->mSpecializationConstants.bumpMapMatrix11_0
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_1 == context->mSpecializationConstants.bumpMapMatrix11_1
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_2 == context->mSpecializationConstants.bumpMapMatrix11_2
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_3 == context->mSpecializationConstants.bumpMapMatrix11_3
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_4 == context->mSpecializationConstants.bumpMapMatrix11_4
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_5 == context->mSpecializationConstants.bumpMapMatrix11_5
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_6 == context->mSpecializationConstants.bumpMapMatrix11_6
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapMatrix11_7 == context->mSpecializationConstants.bumpMapMatrix11_7

			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_0 == context->mSpecializationConstants.texureCoordinateIndex_0
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_1 == context->mSpecializationConstants.texureCoordinateIndex_1
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_2 == context->mSpecializationConstants.texureCoordinateIndex_2
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_3 == context->mSpecializationConstants.texureCoordinateIndex_3
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_4 == context->mSpecializationConstants.texureCoordinateIndex_4
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_5 == context->mSpecializationConstants.texureCoordinateIndex_5
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_6 == context->mSpecializationConstants.texureCoordinateIndex_6
			&& mDrawBuffer[i]->mSpecializationConstants.texureCoordinateIndex_7 == context->mSpecializationConstants.texureCoordinateIndex_7

			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_0 == context->mSpecializationConstants.bumpMapScale_0
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_1 == context->mSpecializationConstants.bumpMapScale_1
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_2 == context->mSpecializationConstants.bumpMapScale_2
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_3 == context->mSpecializationConstants.bumpMapScale_3
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_4 == context->mSpecializationConstants.bumpMapScale_4
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_5 == context->mSpecializationConstants.bumpMapScale_5
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_6 == context->mSpecializationConstants.bumpMapScale_6
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapScale_7 == context->mSpecializationConstants.bumpMapScale_7

			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_0 == context->mSpecializationConstants.bumpMapOffset_0
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_1 == context->mSpecializationConstants.bumpMapOffset_1
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_2 == context->mSpecializationConstants.bumpMapOffset_2
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_3 == context->mSpecializationConstants.bumpMapOffset_3
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_4 == context->mSpecializationConstants.bumpMapOffset_4
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_5 == context->mSpecializationConstants.bumpMapOffset_5
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_6 == context->mSpecializationConstants.bumpMapOffset_6
			&& mDrawBuffer[i]->mSpecializationConstants.bumpMapOffset_7 == context->mSpecializationConstants.bumpMapOffset_7

			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_0 == context->mSpecializationConstants.textureTransformationFlags_0
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_1 == context->mSpecializationConstants.textureTransformationFlags_1
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_2 == context->mSpecializationConstants.textureTransformationFlags_2
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_3 == context->mSpecializationConstants.textureTransformationFlags_3
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_4 == context->mSpecializationConstants.textureTransformationFlags_4
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_5 == context->mSpecializationConstants.textureTransformationFlags_5
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_6 == context->mSpecializationConstants.textureTransformationFlags_6
			&& mDrawBuffer[i]->mSpecializationConstants.textureTransformationFlags_7 == context->mSpecializationConstants.textureTransformationFlags_7

			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_0 == context->mSpecializationConstants.colorArgument0_0
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_1 == context->mSpecializationConstants.colorArgument0_1
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_2 == context->mSpecializationConstants.colorArgument0_2
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_3 == context->mSpecializationConstants.colorArgument0_3
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_4 == context->mSpecializationConstants.colorArgument0_4
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_5 == context->mSpecializationConstants.colorArgument0_5
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_6 == context->mSpecializationConstants.colorArgument0_6
			&& mDrawBuffer[i]->mSpecializationConstants.colorArgument0_7 == context->mSpecializationConstants.colorArgument0_7

			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_0 == context->mSpecializationConstants.alphaArgument0_0
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_1 == context->mSpecializationConstants.alphaArgument0_1
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_2 == context->mSpecializationConstants.alphaArgument0_2
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_3 == context->mSpecializationConstants.alphaArgument0_3
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_4 == context->mSpecializationConstants.alphaArgument0_4
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_5 == context->mSpecializationConstants.alphaArgument0_5
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_6 == context->mSpecializationConstants.alphaArgument0_6
			&& mDrawBuffer[i]->mSpecializationConstants.alphaArgument0_7 == context->mSpecializationConstants.alphaArgument0_7

			&& mDrawBuffer[i]->mSpecializationConstants.Result_0 == context->mSpecializationConstants.Result_0
			&& mDrawBuffer[i]->mSpecializationConstants.Result_1 == context->mSpecializationConstants.Result_1
			&& mDrawBuffer[i]->mSpecializationConstants.Result_2 == context->mSpecializationConstants.Result_2
			&& mDrawBuffer[i]->mSpecializationConstants.Result_3 == context->mSpecializationConstants.Result_3
			&& mDrawBuffer[i]->mSpecializationConstants.Result_4 == context->mSpecializationConstants.Result_4
			&& mDrawBuffer[i]->mSpecializationConstants.Result_5 == context->mSpecializationConstants.Result_5
			&& mDrawBuffer[i]->mSpecializationConstants.Result_6 == context->mSpecializationConstants.Result_6
			&& mDrawBuffer[i]->mSpecializationConstants.Result_7 == context->mSpecializationConstants.Result_7
			) //&& mDrawBuffer[i]->TextureCount == context->TextureCount  texture count should be part of vertex declaration or FVF.
		{
			BOOL isMatch = true;
			BOOST_FOREACH(const auto& pair, context->Bindings)
			{
				if (mDrawBuffer[i]->Bindings.count(pair.first) == 0 || pair.second != mDrawBuffer[i]->Bindings[pair.first])
				{
					isMatch = false;
					break;
				}
			}
			if (isMatch)
			{
				context->Pipeline = mDrawBuffer[i]->Pipeline;
				context->PipelineLayout = mDrawBuffer[i]->PipelineLayout;
				context->DescriptorSetLayout = mDrawBuffer[i]->DescriptorSetLayout;
				context->mDevice = nullptr; //Not owner.
				mDrawBuffer[i]->LastUsed = std::chrono::steady_clock::now();
			}
		}
	}

	if (context->Pipeline == VK_NULL_HANDLE)
	{
		CreatePipe(context);
	}

	/**********************************************
	* Update UBO structure.
	**********************************************/
	UpdateUniformBuffer(context);

	/**********************************************
	* Check for existing DescriptorSet. Create one if there isn't a matching one.
	**********************************************/

	//Copy information into resource context.

	if (context->DescriptorSetLayout != VK_NULL_HANDLE)
	{
		std::copy(std::begin(mDevice->mDeviceState.mDescriptorImageInfo), std::end(mDevice->mDeviceState.mDescriptorImageInfo), std::begin(resourceContext->DescriptorImageInfo));

		//Loop over cached descriptor information.
		for (size_t i = 0; i < mUsedResourceBuffer.size(); i++)
		{
			auto& resourceBuffer = mUsedResourceBuffer[i];
			BOOL imageMatches = true;

			//The image info array is currently always 16.
			for (int32_t j = 0; j < 16; j++)
			{
				auto& imageData1 = resourceBuffer->DescriptorImageInfo[j];
				auto& imageData2 = resourceContext->DescriptorImageInfo[j];

				if (imageData1.imageLayout == imageData2.imageLayout
					&& imageData1.imageView == imageData2.imageView
					&& imageData1.sampler == imageData2.sampler)
				{
					//nothing?
				}
				else
				{
					imageMatches = false;
					break;
				}
			}
			if (imageMatches)
			{
				resourceContext->DescriptorSet = resourceBuffer->DescriptorSet;
				resourceContext->mDevice = nullptr; //Not owner.
				resourceBuffer->LastUsed = std::chrono::steady_clock::now();
				break;
			}
		}

		if (resourceContext->DescriptorSet == VK_NULL_HANDLE)
		{
			CreateDescriptorSet(context, resourceContext);
		}
	}

	/**********************************************
	* Setup bindings
	**********************************************/

	//if (!mIsDirty || mLastVkPipeline != context->Pipeline)
	//{
	vkCmdBindPipeline(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->Pipeline);
	//	mLastVkPipeline = context->Pipeline;
	//}

	if (resourceContext->DescriptorSet != VK_NULL_HANDLE)
	{
		vkCmdBindDescriptorSets(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->PipelineLayout, 0, 1, &resourceContext->DescriptorSet, 0, nullptr);
		mLastDescriptorSet = resourceContext->DescriptorSet;
	}

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

	mIsDirty = false;
}

void BufferManager::CreatePipe(std::shared_ptr<DrawContext> context)
{
	VkResult result = VK_SUCCESS;

	/**********************************************
	* Figure out flags
	**********************************************/
	uint32_t attributeCount = 0;
	uint32_t textureCount = 0;
	uint32_t lightCount = context->mSpecializationConstants.lightCount;
	BOOL hasColor = 0;
	BOOL hasPosition = 0;
	BOOL hasNormal = 0;
	BOOL isLightingEnabled = context->mSpecializationConstants.isLightingEnabled;

	if (context->VertexDeclaration != nullptr)
	{
		auto vertexDeclaration = context->VertexDeclaration;

		hasColor = vertexDeclaration->mHasColor;
		hasPosition = vertexDeclaration->mHasPosition;
		hasNormal = vertexDeclaration->mHasNormal;
		textureCount = vertexDeclaration->mTextureCount;
	}
	else if (context->FVF)
	{
		if ((context->FVF & D3DFVF_XYZ) == D3DFVF_XYZ)
		{
			hasPosition = true;
		}

		if ((context->FVF & D3DFVF_NORMAL) == D3DFVF_NORMAL)
		{
			hasNormal = true;
		}

		if ((context->FVF & D3DFVF_PSIZE) == D3DFVF_PSIZE)
		{
			BOOST_LOG_TRIVIAL(warning) << "BufferManager::CreatePipe D3DFVF_PSIZE is not implemented!";
		}

		if ((context->FVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
		{
			hasColor = true;
		}

		if ((context->FVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
		{
			BOOST_LOG_TRIVIAL(warning) << "BufferManager::CreatePipe D3DFVF_SPECULAR is not implemented!";
		}

		textureCount = ConvertFormat(context->FVF);
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

	SetCulling(mPipelineRasterizationStateCreateInfo, context->CullMode);
	mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode(context->FillMode);
	mPipelineInputAssemblyStateCreateInfo.topology = ConvertPrimitiveType(context->PrimitiveType);

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
		case 2:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_TEX2;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_TEX2;
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
		case 2:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_DIFFUSE_TEX2;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_DIFFUSE_TEX2;
			break;
		default:
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreatePipe unsupported texture count " << textureCount;
			break;
		}
	}
	else if (hasPosition && hasColor && hasNormal)
	{
		switch (textureCount)
		{
		case 2:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2;
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
	if (context->VertexDeclaration != nullptr)
	{
		uint32_t textureIndex = 0;

		attributeCount = context->VertexDeclaration->mVertexElements.size();

		for (size_t i = 0; i < attributeCount; i++)
		{
			D3DVERTEXELEMENT9& element = context->VertexDeclaration->mVertexElements[i];

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
	else if (context->FVF)
	{
		//revisit - make sure multiple sources is valid for FVF.
		for (int32_t i = 0; i < context->StreamCount; i++)
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

			if ((context->FVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
			{
				mVertexInputAttributeDescription[attributeIndex].binding = i;
				mVertexInputAttributeDescription[attributeIndex].location = location;
				mVertexInputAttributeDescription[attributeIndex].format = VK_FORMAT_B8G8R8A8_UINT;
				mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += sizeof(uint32_t);
				location += 1;
				attributeIndex += 1;
			}

			if ((context->FVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
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

	mPipelineLayoutCreateInfo.pPushConstantRanges = mPushConstantRanges;
	mPipelineLayoutCreateInfo.pushConstantRangeCount = 1;

	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = context->StreamCount;
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount;

	mDescriptorSetLayoutBinding[0].binding = 0;
	mDescriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
	mDescriptorSetLayoutBinding[0].descriptorCount = textureCount; //Update to use mapped texture.
	mDescriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	mDescriptorSetLayoutBinding[0].pImmutableSamplers = NULL;

	mDescriptorSetLayoutBinding[1].binding = 1;
	mDescriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mDescriptorSetLayoutBinding[1].descriptorCount = 1;
	mDescriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	mDescriptorSetLayoutBinding[1].pImmutableSamplers = NULL;

	mDescriptorSetLayoutBinding[2].binding = 2;
	mDescriptorSetLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mDescriptorSetLayoutBinding[2].descriptorCount = 1;
	mDescriptorSetLayoutBinding[2].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	mDescriptorSetLayoutBinding[2].pImmutableSamplers = NULL;

	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;
	mPipelineLayoutCreateInfo.pSetLayouts = &context->DescriptorSetLayout;

	if (textureCount)
	{
		if (lightCount)
		{
			mDescriptorSetLayoutCreateInfo.bindingCount = 3; //The number of elements in pBindings.	
		}
		else
		{
			mDescriptorSetLayoutCreateInfo.bindingCount = 1; //The number of elements in pBindings.	
		}	

		mPipelineLayoutCreateInfo.setLayoutCount = 1;

		result = vkCreateDescriptorSetLayout(mDevice->mDevice, &mDescriptorSetLayoutCreateInfo, nullptr, &context->DescriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreateDescriptorSet vkCreateDescriptorSetLayout failed with return code of " << result;
			return;
		}
	}
	else
	{
		mDescriptorSetLayoutCreateInfo.bindingCount = 0;
		mPipelineLayoutCreateInfo.setLayoutCount = 0;
	}

	/**********************************************
	* Create pipeline & descriptor set layout.
	**********************************************/

	result = vkCreatePipelineLayout(mDevice->mDevice, &mPipelineLayoutCreateInfo, nullptr, &context->PipelineLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreatePipelineLayout failed with return code of " << result;
		return;
	}

	mGraphicsPipelineCreateInfo.layout = context->PipelineLayout;

	result = vkCreateGraphicsPipelines(mDevice->mDevice, mPipelineCache, 1, &mGraphicsPipelineCreateInfo, nullptr, &context->Pipeline);
	//result = vkCreateGraphicsPipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreateGraphicsPipelines failed with return code of " << result;
	}

	this->mDrawBuffer.push_back(context);
}

void BufferManager::CreateDescriptorSet(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext)
{
	VkResult result = VK_SUCCESS;

	if (context->DescriptorSetLayout == VK_NULL_HANDLE)
	{
		resourceContext->DescriptorSet = VK_NULL_HANDLE;
		return;
	}

	mDescriptorSetAllocateInfo.pSetLayouts = &context->DescriptorSetLayout;
	mDescriptorSetAllocateInfo.descriptorSetCount = 1;

	/**********************************************
	* Create descriptor set if there are none left over, otherwise reuse an existing descriptor set.
	**********************************************/

	if (mUnusedResourceBuffer.size())
	{
		auto& buffer = mUnusedResourceBuffer.back();
		buffer->mDevice = nullptr;

		resourceContext->DescriptorSet = buffer->DescriptorSet;

		mUnusedResourceBuffer.pop_back();
	}
	else
	{
		result = vkAllocateDescriptorSets(mDevice->mDevice, &mDescriptorSetAllocateInfo, &resourceContext->DescriptorSet);
		if (result != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreateDescriptorSet vkAllocateDescriptorSets failed with return code of " << result;
			return;
		}
	}

	mUsedResourceBuffer.push_back(resourceContext);

	mWriteDescriptorSet[0].dstSet = resourceContext->DescriptorSet;
	mWriteDescriptorSet[0].descriptorCount = mDevice->mDeviceState.mTextures.size();
	mWriteDescriptorSet[0].pImageInfo = resourceContext->DescriptorImageInfo;

	if (mDevice->mDeviceState.mLights.size())
	{
		mDescriptorBufferInfo[0].buffer = mLightBuffer;
		mDescriptorBufferInfo[0].offset = 0;
		mDescriptorBufferInfo[0].range = sizeof(D3DLIGHT9) * mDevice->mDeviceState.mLights.size();

		mDescriptorBufferInfo[1].buffer = mMaterialBuffer;
		mDescriptorBufferInfo[1].offset = 0;
		mDescriptorBufferInfo[1].range = sizeof(D3DMATERIAL9);

		mWriteDescriptorSet[1].dstSet = resourceContext->DescriptorSet;
		mWriteDescriptorSet[1].descriptorCount = 2;
		mWriteDescriptorSet[1].pBufferInfo = mDescriptorBufferInfo;

		vkUpdateDescriptorSets(mDevice->mDevice, 2, mWriteDescriptorSet, 0, nullptr);
	}
	else
	{
		vkUpdateDescriptorSets(mDevice->mDevice, 1, mWriteDescriptorSet, 0, nullptr);
	}
}

void BufferManager::CreateSampler(std::shared_ptr<SamplerRequest> request)
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb172602(v=vs.85).aspx
	//Mipmap filter to use during minification. See D3DTEXTUREFILTERTYPE. The default value is D3DTEXF_NONE.

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = NULL;

	samplerCreateInfo.magFilter = ConvertFilter(request->MagFilter);
	samplerCreateInfo.minFilter = ConvertFilter(request->MinFilter);
	samplerCreateInfo.addressModeU = ConvertTextureAddress(request->AddressModeU);
	samplerCreateInfo.addressModeV = ConvertTextureAddress(request->AddressModeV);
	samplerCreateInfo.addressModeW = ConvertTextureAddress(request->AddressModeW);
	samplerCreateInfo.maxAnisotropy = request->MaxAnisotropy;  //16 D3DSAMP_MAXANISOTROPY
	samplerCreateInfo.mipmapMode = ConvertMipmapMode(request->MipmapMode); //VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.mipLodBias = request->MipLodBias;

	if (mDevice->mDeviceFeatures.samplerAnisotropy)
	{
		// Use max. level of anisotropy for this example
		samplerCreateInfo.maxAnisotropy = min(request->MaxAnisotropy, mDevice->mDeviceProperties.limits.maxSamplerAnisotropy);
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
	}
	else
	{
		// The device does not support anisotropic filtering
		samplerCreateInfo.maxAnisotropy = 1.0;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
	}

	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; // VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER; //VK_COMPARE_OP_ALWAYS
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = request->MaxLod;

	mResult = vkCreateSampler(mDevice->mDevice, &samplerCreateInfo, NULL, &request->Sampler);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::GenerateSampler vkCreateSampler failed with return code of " << mResult;
		return;
	}

	mSamplerRequests.push_back(request);
}

void BufferManager::UpdateUniformBuffer(std::shared_ptr<DrawContext> context)
{
	VkResult result = VK_SUCCESS;
	void* data = nullptr;

	if (mDevice->mDeviceState.mAreLightsDirty)
	{
		vkCmdUpdateBuffer(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], mLightBuffer, 0, sizeof(D3DLIGHT9)*context->mSpecializationConstants.lightCount, mDevice->mDeviceState.mLights.data());
		mDevice->mDeviceState.mAreLightsDirty = false;
	}

	if (mDevice->mDeviceState.mIsMaterialDirty)
	{
		vkCmdUpdateBuffer(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], mMaterialBuffer, 0, sizeof(D3DMATERIAL9), &mDevice->mDeviceState.mMaterial);
		mDevice->mDeviceState.mIsMaterialDirty = false;
	}

	if (!mDevice->mDeviceState.mHasTransformsChanged)
	{
		return;
	}

	BOOST_FOREACH(const auto& pair1, mDevice->mDeviceState.mTransforms)
	{
		switch (pair1.first)
		{
		case D3DTS_WORLD:

			mModel <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_VIEW:

			mView <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_PROJECTION:

			mProjection <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "BufferManager::UpdateUniformBuffer The following state type was ignored. " << pair1.first;
			break;
		}
	}

	mTotalTransformation = mProjection * mView * mModel;
	//mTotalTransformation = mModel * mView * mProjection;

	vkCmdPushConstants(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], context->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, UBO_SIZE, mTotalTransformation.data());
}

void BufferManager::FlushDrawBufffer()
{
	/*
	Uses remove_if and chrono to remove elements that have not been used in over a second.
	*/
	mDrawBuffer.erase(std::remove_if(mDrawBuffer.begin(), mDrawBuffer.end(), [](const std::shared_ptr<DrawContext> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), mDrawBuffer.end());
	mSamplerRequests.erase(std::remove_if(mSamplerRequests.begin(), mSamplerRequests.end(), [](const std::shared_ptr<SamplerRequest> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), mSamplerRequests.end());

	/*
	Add expired buffers to unused list so they can be reused and then remove them from the used queue.
	*/

	for (size_t i = 0; i < mUsedResourceBuffer.size(); i++)
	{
		if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - mUsedResourceBuffer[i]->LastUsed).count() > CACHE_SECONDS)
		{
			mUnusedResourceBuffer.push_back(mUsedResourceBuffer[i]);
		}
	}
	mUsedResourceBuffer.erase(std::remove_if(mUsedResourceBuffer.begin(), mUsedResourceBuffer.end(), [](const std::shared_ptr<ResourceContext> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), mUsedResourceBuffer.end());

	mIsDirty = true;
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
	vkBeginCommandBuffer(mCommandBuffer, &mBeginInfo);
	{
		mCopyRegion.size = size;
		vkCmdCopyBuffer(mCommandBuffer, srcBuffer, dstBuffer, 1, &mCopyRegion);
	}
	vkEndCommandBuffer(mCommandBuffer);

	vkQueueSubmit(mDevice->mQueue, 1, &mSubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mDevice->mQueue);
	vkResetCommandBuffer(mCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT); //So far resetting a command buffer is about 10 times faster than allocating a new one.
}

SamplerRequest::~SamplerRequest()
{
	if (mDevice != nullptr && Sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(mDevice->mDevice, Sampler, NULL);
	}
}

ResourceContext::~ResourceContext()
{
	if (mDevice != nullptr && DescriptorSet != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(mDevice->mDevice, mDevice->mDescriptorPool, 1, &DescriptorSet);
	}
}

DrawContext::~DrawContext()
{
	if (mDevice != nullptr)
	{
		if (Pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(mDevice->mDevice, Pipeline, NULL);
		}
		if (PipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(mDevice->mDevice, PipelineLayout, NULL);
		}
		if (DescriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(mDevice->mDevice, DescriptorSetLayout, NULL);
		}
	}

}