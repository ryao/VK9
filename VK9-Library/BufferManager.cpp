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

/*
"Whoever pursues righteousness and kindness will find life, righteousness, and honor." (Proverbs 21:21, ESV)
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
	//mModel <<
	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	0, 0, 0, 1;

	//mView <<
	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	0, 0, 0, 1;

	//mProjection <<
	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	0, 0, 0, 1;

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

	mVertShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL_TEX1.vert.spv");
	mFragShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL_TEX1.frag.spv");

	mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.vert.spv");
	mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromFile(mDevice->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.frag.spv");

	mPushConstantRanges[0].offset = 0;
	mPushConstantRanges[0].size = UBO_SIZE;
	mPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS; //VK_SHADER_STAGE_VERTEX_BIT

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
	mPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	mPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

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
	mWriteDescriptorSet[1].descriptorCount = 1;
	mWriteDescriptorSet[1].pBufferInfo = &mDescriptorBufferInfo[0];

	mWriteDescriptorSet[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//mWriteDescriptorSet[2].dstSet = descriptorSet;
	mWriteDescriptorSet[2].dstBinding = 2;
	mWriteDescriptorSet[2].dstArrayElement = 0;
	mWriteDescriptorSet[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mWriteDescriptorSet[2].descriptorCount = 1;
	mWriteDescriptorSet[2].pBufferInfo = &mDescriptorBufferInfo[1];

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
	CreateBuffer(sizeof(Light)*4, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mLightBuffer, mLightBufferMemory);
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

	if (mVertShaderModule_XYZ_NORMAL_TEX1 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_NORMAL_TEX1, NULL);
		mVertShaderModule_XYZ_NORMAL_TEX1 = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_NORMAL_TEX1 != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_NORMAL_TEX1, NULL);
		mFragShaderModule_XYZ_NORMAL_TEX1 = VK_NULL_HANDLE;
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

	//Empty cached objects. (destructors should take care of their resources.

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

	//TODO: revisit if it's valid to have declaration or FVF with either shader type.

	if (mDevice->mDeviceState.mHasVertexShader)
	{
		context->VertexShader = mDevice->mDeviceState.mVertexShader; //vert
	}

	if (mDevice->mDeviceState.mHasPixelShader)
	{
		context->PixelShader = mDevice->mDeviceState.mPixelShader; //pixel
	}

	context->StreamCount = mDevice->mDeviceState.mStreamSources.size();

	context->mSpecializationConstants = mDevice->mDeviceState.mSpecializationConstants;

	SpecializationConstants& constants = context->mSpecializationConstants;
	constants.lightCount = mDevice->mDeviceState.mLights.size();
	constants.textureCount = mDevice->mDeviceState.mTextures.size();

	mDevice->mDeviceState.mSpecializationConstants.lightCount = constants.lightCount;
	mDevice->mDeviceState.mSpecializationConstants.textureCount = constants.textureCount;

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
		auto& drawBuffer = (*mDrawBuffer[i]);

		if (drawBuffer.PrimitiveType == context->PrimitiveType
			&& drawBuffer.FVF == context->FVF
			&& drawBuffer.VertexDeclaration == context->VertexDeclaration
			&& drawBuffer.VertexShader == context->VertexShader
			&& drawBuffer.PixelShader == context->PixelShader
			&& drawBuffer.StreamCount == context->StreamCount
			&& drawBuffer.mSpecializationConstants.lightCount == constants.lightCount
			   
			&& drawBuffer.mSpecializationConstants.textureCount == constants.textureCount
			   
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_0 == constants.texureCoordinateIndex_0
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_1 == constants.texureCoordinateIndex_1
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_2 == constants.texureCoordinateIndex_2
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_3 == constants.texureCoordinateIndex_3
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_4 == constants.texureCoordinateIndex_4
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_5 == constants.texureCoordinateIndex_5
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_6 == constants.texureCoordinateIndex_6
			&& drawBuffer.mSpecializationConstants.texureCoordinateIndex_7 == constants.texureCoordinateIndex_7
			   
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_0 == constants.textureTransformationFlags_0
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_1 == constants.textureTransformationFlags_1
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_2 == constants.textureTransformationFlags_2
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_3 == constants.textureTransformationFlags_3
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_4 == constants.textureTransformationFlags_4
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_5 == constants.textureTransformationFlags_5
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_6 == constants.textureTransformationFlags_6
			&& drawBuffer.mSpecializationConstants.textureTransformationFlags_7 == constants.textureTransformationFlags_7
			   
			&& drawBuffer.mSpecializationConstants.colorOperation_0 == constants.colorOperation_0
			&& drawBuffer.mSpecializationConstants.colorOperation_1 == constants.colorOperation_1
			&& drawBuffer.mSpecializationConstants.colorOperation_2 == constants.colorOperation_2
			&& drawBuffer.mSpecializationConstants.colorOperation_3 == constants.colorOperation_3
			&& drawBuffer.mSpecializationConstants.colorOperation_4 == constants.colorOperation_4
			&& drawBuffer.mSpecializationConstants.colorOperation_5 == constants.colorOperation_5
			&& drawBuffer.mSpecializationConstants.colorOperation_6 == constants.colorOperation_6
			&& drawBuffer.mSpecializationConstants.colorOperation_7 == constants.colorOperation_7
			   
			&& drawBuffer.mSpecializationConstants.colorArgument1_0 == constants.colorArgument1_0
			&& drawBuffer.mSpecializationConstants.colorArgument1_1 == constants.colorArgument1_1
			&& drawBuffer.mSpecializationConstants.colorArgument1_2 == constants.colorArgument1_2
			&& drawBuffer.mSpecializationConstants.colorArgument1_3 == constants.colorArgument1_3
			&& drawBuffer.mSpecializationConstants.colorArgument1_4 == constants.colorArgument1_4
			&& drawBuffer.mSpecializationConstants.colorArgument1_5 == constants.colorArgument1_5
			&& drawBuffer.mSpecializationConstants.colorArgument1_6 == constants.colorArgument1_6
			&& drawBuffer.mSpecializationConstants.colorArgument1_7 == constants.colorArgument1_7
			  
			&& drawBuffer.mSpecializationConstants.colorArgument2_0 == constants.colorArgument2_0
			&& drawBuffer.mSpecializationConstants.colorArgument2_1 == constants.colorArgument2_1
			&& drawBuffer.mSpecializationConstants.colorArgument2_2 == constants.colorArgument2_2
			&& drawBuffer.mSpecializationConstants.colorArgument2_3 == constants.colorArgument2_3
			&& drawBuffer.mSpecializationConstants.colorArgument2_4 == constants.colorArgument2_4
			&& drawBuffer.mSpecializationConstants.colorArgument2_5 == constants.colorArgument2_5
			&& drawBuffer.mSpecializationConstants.colorArgument2_6 == constants.colorArgument2_6
			&& drawBuffer.mSpecializationConstants.colorArgument2_7 == constants.colorArgument2_7
			   
			&& drawBuffer.mSpecializationConstants.alphaOperation_0 == constants.alphaOperation_0
			&& drawBuffer.mSpecializationConstants.alphaOperation_1 == constants.alphaOperation_1
			&& drawBuffer.mSpecializationConstants.alphaOperation_2 == constants.alphaOperation_2
			&& drawBuffer.mSpecializationConstants.alphaOperation_3 == constants.alphaOperation_3
			&& drawBuffer.mSpecializationConstants.alphaOperation_4 == constants.alphaOperation_4
			&& drawBuffer.mSpecializationConstants.alphaOperation_5 == constants.alphaOperation_5
			&& drawBuffer.mSpecializationConstants.alphaOperation_6 == constants.alphaOperation_6
			&& drawBuffer.mSpecializationConstants.alphaOperation_7 == constants.alphaOperation_7
			   
			&& drawBuffer.mSpecializationConstants.alphaArgument1_0 == constants.alphaArgument1_0
			&& drawBuffer.mSpecializationConstants.alphaArgument1_1 == constants.alphaArgument1_1
			&& drawBuffer.mSpecializationConstants.alphaArgument1_2 == constants.alphaArgument1_2
			&& drawBuffer.mSpecializationConstants.alphaArgument1_3 == constants.alphaArgument1_3
			&& drawBuffer.mSpecializationConstants.alphaArgument1_4 == constants.alphaArgument1_4
			&& drawBuffer.mSpecializationConstants.alphaArgument1_5 == constants.alphaArgument1_5
			&& drawBuffer.mSpecializationConstants.alphaArgument1_6 == constants.alphaArgument1_6
			&& drawBuffer.mSpecializationConstants.alphaArgument1_7 == constants.alphaArgument1_7
			   
			&& drawBuffer.mSpecializationConstants.alphaArgument2_0 == constants.alphaArgument2_0
			&& drawBuffer.mSpecializationConstants.alphaArgument2_1 == constants.alphaArgument2_1
			&& drawBuffer.mSpecializationConstants.alphaArgument2_2 == constants.alphaArgument2_2
			&& drawBuffer.mSpecializationConstants.alphaArgument2_3 == constants.alphaArgument2_3
			&& drawBuffer.mSpecializationConstants.alphaArgument2_4 == constants.alphaArgument2_4
			&& drawBuffer.mSpecializationConstants.alphaArgument2_5 == constants.alphaArgument2_5
			&& drawBuffer.mSpecializationConstants.alphaArgument2_6 == constants.alphaArgument2_6
			&& drawBuffer.mSpecializationConstants.alphaArgument2_7 == constants.alphaArgument2_7
			   
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_0 == constants.bumpMapMatrix00_0
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_1 == constants.bumpMapMatrix00_1
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_2 == constants.bumpMapMatrix00_2
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_3 == constants.bumpMapMatrix00_3
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_4 == constants.bumpMapMatrix00_4
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_5 == constants.bumpMapMatrix00_5
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_6 == constants.bumpMapMatrix00_6
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix00_7 == constants.bumpMapMatrix00_7
			   
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_0 == constants.bumpMapMatrix01_0
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_1 == constants.bumpMapMatrix01_1
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_2 == constants.bumpMapMatrix01_2
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_3 == constants.bumpMapMatrix01_3
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_4 == constants.bumpMapMatrix01_4
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_5 == constants.bumpMapMatrix01_5
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_6 == constants.bumpMapMatrix01_6
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix01_7 == constants.bumpMapMatrix01_7
			   
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_0 == constants.bumpMapMatrix10_0
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_1 == constants.bumpMapMatrix10_1
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_2 == constants.bumpMapMatrix10_2
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_3 == constants.bumpMapMatrix10_3
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_4 == constants.bumpMapMatrix10_4
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_5 == constants.bumpMapMatrix10_5
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_6 == constants.bumpMapMatrix10_6
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix10_7 == constants.bumpMapMatrix10_7
			   
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_0 == constants.bumpMapMatrix11_0
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_1 == constants.bumpMapMatrix11_1
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_2 == constants.bumpMapMatrix11_2
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_3 == constants.bumpMapMatrix11_3
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_4 == constants.bumpMapMatrix11_4
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_5 == constants.bumpMapMatrix11_5
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_6 == constants.bumpMapMatrix11_6
			&& drawBuffer.mSpecializationConstants.bumpMapMatrix11_7 == constants.bumpMapMatrix11_7
			   
			&& drawBuffer.mSpecializationConstants.bumpMapScale_0 == constants.bumpMapScale_0
			&& drawBuffer.mSpecializationConstants.bumpMapScale_1 == constants.bumpMapScale_1
			&& drawBuffer.mSpecializationConstants.bumpMapScale_2 == constants.bumpMapScale_2
			&& drawBuffer.mSpecializationConstants.bumpMapScale_3 == constants.bumpMapScale_3
			&& drawBuffer.mSpecializationConstants.bumpMapScale_4 == constants.bumpMapScale_4
			&& drawBuffer.mSpecializationConstants.bumpMapScale_5 == constants.bumpMapScale_5
			&& drawBuffer.mSpecializationConstants.bumpMapScale_6 == constants.bumpMapScale_6
			&& drawBuffer.mSpecializationConstants.bumpMapScale_7 == constants.bumpMapScale_7
			   
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_0 == constants.bumpMapOffset_0
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_1 == constants.bumpMapOffset_1
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_2 == constants.bumpMapOffset_2
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_3 == constants.bumpMapOffset_3
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_4 == constants.bumpMapOffset_4
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_5 == constants.bumpMapOffset_5
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_6 == constants.bumpMapOffset_6
			&& drawBuffer.mSpecializationConstants.bumpMapOffset_7 == constants.bumpMapOffset_7
			   
			&& drawBuffer.mSpecializationConstants.colorArgument0_0 == constants.colorArgument0_0
			&& drawBuffer.mSpecializationConstants.colorArgument0_1 == constants.colorArgument0_1
			&& drawBuffer.mSpecializationConstants.colorArgument0_2 == constants.colorArgument0_2
			&& drawBuffer.mSpecializationConstants.colorArgument0_3 == constants.colorArgument0_3
			&& drawBuffer.mSpecializationConstants.colorArgument0_4 == constants.colorArgument0_4
			&& drawBuffer.mSpecializationConstants.colorArgument0_5 == constants.colorArgument0_5
			&& drawBuffer.mSpecializationConstants.colorArgument0_6 == constants.colorArgument0_6
			&& drawBuffer.mSpecializationConstants.colorArgument0_7 == constants.colorArgument0_7
			   
			&& drawBuffer.mSpecializationConstants.alphaArgument0_0 == constants.alphaArgument0_0
			&& drawBuffer.mSpecializationConstants.alphaArgument0_1 == constants.alphaArgument0_1
			&& drawBuffer.mSpecializationConstants.alphaArgument0_2 == constants.alphaArgument0_2
			&& drawBuffer.mSpecializationConstants.alphaArgument0_3 == constants.alphaArgument0_3
			&& drawBuffer.mSpecializationConstants.alphaArgument0_4 == constants.alphaArgument0_4
			&& drawBuffer.mSpecializationConstants.alphaArgument0_5 == constants.alphaArgument0_5
			&& drawBuffer.mSpecializationConstants.alphaArgument0_6 == constants.alphaArgument0_6
			&& drawBuffer.mSpecializationConstants.alphaArgument0_7 == constants.alphaArgument0_7
			   
			&& drawBuffer.mSpecializationConstants.Result_0 == constants.Result_0
			&& drawBuffer.mSpecializationConstants.Result_1 == constants.Result_1
			&& drawBuffer.mSpecializationConstants.Result_2 == constants.Result_2
			&& drawBuffer.mSpecializationConstants.Result_3 == constants.Result_3
			&& drawBuffer.mSpecializationConstants.Result_4 == constants.Result_4
			&& drawBuffer.mSpecializationConstants.Result_5 == constants.Result_5
			&& drawBuffer.mSpecializationConstants.Result_6 == constants.Result_6
			&& drawBuffer.mSpecializationConstants.Result_7 == constants.Result_7

			&& drawBuffer.mSpecializationConstants.zEnable == constants.zEnable
			&& drawBuffer.mSpecializationConstants.fillMode == constants.fillMode
			&& drawBuffer.mSpecializationConstants.shadeMode == constants.shadeMode
			&& drawBuffer.mSpecializationConstants.zWriteEnable == constants.zWriteEnable
			&& drawBuffer.mSpecializationConstants.alphaTestEnable == constants.alphaTestEnable
			&& drawBuffer.mSpecializationConstants.lastPixel == constants.lastPixel
			&& drawBuffer.mSpecializationConstants.sourceBlend == constants.sourceBlend
			&& drawBuffer.mSpecializationConstants.destinationBlend == constants.destinationBlend
			&& drawBuffer.mSpecializationConstants.cullMode == constants.cullMode
			&& drawBuffer.mSpecializationConstants.zFunction == constants.zFunction
			&& drawBuffer.mSpecializationConstants.alphaReference == constants.alphaReference
			&& drawBuffer.mSpecializationConstants.alphaFunction == constants.alphaFunction
			&& drawBuffer.mSpecializationConstants.ditherEnable == constants.ditherEnable
			&& drawBuffer.mSpecializationConstants.alphaBlendEnable == constants.alphaBlendEnable
			&& drawBuffer.mSpecializationConstants.fogEnable == constants.fogEnable
			&& drawBuffer.mSpecializationConstants.specularEnable == constants.specularEnable
			&& drawBuffer.mSpecializationConstants.fogColor == constants.fogColor
			&& drawBuffer.mSpecializationConstants.fogTableMode == constants.fogTableMode
			&& drawBuffer.mSpecializationConstants.fogStart == constants.fogStart
			&& drawBuffer.mSpecializationConstants.fogEnd == constants.fogEnd
			&& drawBuffer.mSpecializationConstants.fogDensity == constants.fogDensity
			&& drawBuffer.mSpecializationConstants.rangeFogEnable == constants.rangeFogEnable
			&& drawBuffer.mSpecializationConstants.stencilEnable == constants.stencilEnable
			&& drawBuffer.mSpecializationConstants.stencilFail == constants.stencilFail
			&& drawBuffer.mSpecializationConstants.stencilZFail == constants.stencilZFail
			&& drawBuffer.mSpecializationConstants.stencilPass == constants.stencilPass
			&& drawBuffer.mSpecializationConstants.stencilFunction == constants.stencilFunction
			&& drawBuffer.mSpecializationConstants.stencilReference == constants.stencilReference
			&& drawBuffer.mSpecializationConstants.stencilMask == constants.stencilMask
			&& drawBuffer.mSpecializationConstants.stencilWriteMask == constants.stencilWriteMask
			&& drawBuffer.mSpecializationConstants.textureFactor == constants.textureFactor
			&& drawBuffer.mSpecializationConstants.wrap0 == constants.wrap0
			&& drawBuffer.mSpecializationConstants.wrap1 == constants.wrap1
			&& drawBuffer.mSpecializationConstants.wrap2 == constants.wrap2
			&& drawBuffer.mSpecializationConstants.wrap3 == constants.wrap3
			&& drawBuffer.mSpecializationConstants.wrap4 == constants.wrap4
			&& drawBuffer.mSpecializationConstants.wrap5 == constants.wrap5
			&& drawBuffer.mSpecializationConstants.wrap6 == constants.wrap6
			&& drawBuffer.mSpecializationConstants.wrap7 == constants.wrap7
			&& drawBuffer.mSpecializationConstants.clipping == constants.clipping
			&& drawBuffer.mSpecializationConstants.lighting == constants.lighting
			&& drawBuffer.mSpecializationConstants.ambient == constants.ambient
			&& drawBuffer.mSpecializationConstants.fogVertexMode == constants.fogVertexMode
			&& drawBuffer.mSpecializationConstants.colorVertex == constants.colorVertex
			&& drawBuffer.mSpecializationConstants.localViewer == constants.localViewer
			&& drawBuffer.mSpecializationConstants.normalizeNormals == constants.normalizeNormals
			&& drawBuffer.mSpecializationConstants.diffuseMaterialSource == constants.diffuseMaterialSource
			&& drawBuffer.mSpecializationConstants.specularMaterialSource == constants.specularMaterialSource
			&& drawBuffer.mSpecializationConstants.ambientMaterialSource == constants.ambientMaterialSource
			&& drawBuffer.mSpecializationConstants.emissiveMaterialSource == constants.emissiveMaterialSource
			&& drawBuffer.mSpecializationConstants.vertexBlend == constants.vertexBlend
			&& drawBuffer.mSpecializationConstants.clipPlaneEnable == constants.clipPlaneEnable
			&& drawBuffer.mSpecializationConstants.pointSize == constants.pointSize
			&& drawBuffer.mSpecializationConstants.pointSizeMinimum == constants.pointSizeMinimum
			&& drawBuffer.mSpecializationConstants.pointSpriteEnable == constants.pointSpriteEnable
			&& drawBuffer.mSpecializationConstants.pointScaleEnable == constants.pointScaleEnable
			&& drawBuffer.mSpecializationConstants.pointScaleA == constants.pointScaleA
			&& drawBuffer.mSpecializationConstants.pointScaleB == constants.pointScaleB
			&& drawBuffer.mSpecializationConstants.pointScaleC == constants.pointScaleC
			&& drawBuffer.mSpecializationConstants.multisampleAntiAlias == constants.multisampleAntiAlias
			&& drawBuffer.mSpecializationConstants.multisampleMask == constants.multisampleMask
			&& drawBuffer.mSpecializationConstants.patchEdgeStyle == constants.patchEdgeStyle
			&& drawBuffer.mSpecializationConstants.debugMonitorToken == constants.debugMonitorToken
			&& drawBuffer.mSpecializationConstants.pointSizeMaximum == constants.pointSizeMaximum
			&& drawBuffer.mSpecializationConstants.indexedVertexBlendEnable == constants.indexedVertexBlendEnable
			&& drawBuffer.mSpecializationConstants.colorWriteEnable == constants.colorWriteEnable
			&& drawBuffer.mSpecializationConstants.tweenFactor == constants.tweenFactor
			&& drawBuffer.mSpecializationConstants.blendOperation == constants.blendOperation
			&& drawBuffer.mSpecializationConstants.positionDegree == constants.positionDegree
			&& drawBuffer.mSpecializationConstants.normalDegree == constants.normalDegree
			&& drawBuffer.mSpecializationConstants.scissorTestEnable == constants.scissorTestEnable
			&& drawBuffer.mSpecializationConstants.slopeScaleDepthBias == constants.slopeScaleDepthBias
			&& drawBuffer.mSpecializationConstants.antiAliasedLineEnable == constants.antiAliasedLineEnable
			&& drawBuffer.mSpecializationConstants.minimumTessellationLevel == constants.minimumTessellationLevel
			&& drawBuffer.mSpecializationConstants.maximumTessellationLevel == constants.maximumTessellationLevel
			&& drawBuffer.mSpecializationConstants.adaptivetessX == constants.adaptivetessX
			&& drawBuffer.mSpecializationConstants.adaptivetessY == constants.adaptivetessY
			&& drawBuffer.mSpecializationConstants.adaptivetessZ == constants.adaptivetessZ
			&& drawBuffer.mSpecializationConstants.adaptivetessW == constants.adaptivetessW
			&& drawBuffer.mSpecializationConstants.enableAdaptiveTessellation == constants.enableAdaptiveTessellation
			&& drawBuffer.mSpecializationConstants.twoSidedStencilMode == constants.twoSidedStencilMode
			&& drawBuffer.mSpecializationConstants.ccwStencilFail == constants.ccwStencilFail
			&& drawBuffer.mSpecializationConstants.ccwStencilZFail == constants.ccwStencilZFail
			&& drawBuffer.mSpecializationConstants.ccwStencilPass == constants.ccwStencilPass
			&& drawBuffer.mSpecializationConstants.ccwStencilFunction == constants.ccwStencilFunction
			&& drawBuffer.mSpecializationConstants.colorWriteEnable1 == constants.colorWriteEnable1
			&& drawBuffer.mSpecializationConstants.colorWriteEnable2 == constants.colorWriteEnable2
			&& drawBuffer.mSpecializationConstants.colorWriteEnable3 == constants.colorWriteEnable3
			&& drawBuffer.mSpecializationConstants.blendFactor == constants.blendFactor
			&& drawBuffer.mSpecializationConstants.srgbWriteEnable == constants.srgbWriteEnable
			&& drawBuffer.mSpecializationConstants.depthBias == constants.depthBias
			&& drawBuffer.mSpecializationConstants.wrap8 == constants.wrap8
			&& drawBuffer.mSpecializationConstants.wrap9 == constants.wrap9
			&& drawBuffer.mSpecializationConstants.wrap10 == constants.wrap10
			&& drawBuffer.mSpecializationConstants.wrap11 == constants.wrap11
			&& drawBuffer.mSpecializationConstants.wrap12 == constants.wrap12
			&& drawBuffer.mSpecializationConstants.wrap13 == constants.wrap13
			&& drawBuffer.mSpecializationConstants.wrap14 == constants.wrap14
			&& drawBuffer.mSpecializationConstants.wrap15 == constants.wrap15
			&& drawBuffer.mSpecializationConstants.separateAlphaBlendEnable == constants.separateAlphaBlendEnable
			&& drawBuffer.mSpecializationConstants.sourceBlendAlpha == constants.sourceBlendAlpha
			&& drawBuffer.mSpecializationConstants.destinationBlendAlpha == constants.destinationBlendAlpha
			&& drawBuffer.mSpecializationConstants.blendOperationAlpha == constants.blendOperationAlpha
			)
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
		CreatePipe(context); //If we didn't find a matching pipeline then create a new one.
	}

	/**********************************************
	* Update transformation structure.
	**********************************************/
	//UpdateBuffer(); //Illegal
	UpdatePushConstants(context);

	/**********************************************
	* Check for existing DescriptorSet. Create one if there isn't a matching one.
	**********************************************/

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

	//TODO: I need to find a way to prevent binding on every draw call.

	if (resourceContext->DescriptorSet != VK_NULL_HANDLE)
	{
		vkCmdBindDescriptorSets(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->PipelineLayout, 0, 1, &resourceContext->DescriptorSet, 0, nullptr);
		//mLastDescriptorSet = resourceContext->DescriptorSet;
	}

	//if (!mIsDirty || mLastVkPipeline != context->Pipeline)
	//{
	vkCmdBindPipeline(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->Pipeline);
	//	mLastVkPipeline = context->Pipeline;
	//}

	mVertexCount = 0;

	if (mDevice->mDeviceState.mIndexBuffer != nullptr)
	{
		vkCmdBindIndexBuffer(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], mDevice->mDeviceState.mIndexBuffer->mBuffer, 0, mDevice->mDeviceState.mIndexBuffer->mIndexType);
	}

	BOOST_FOREACH(map_type::value_type& source, mDevice->mDeviceState.mStreamSources)
	{
		vkCmdBindVertexBuffers(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], source.first, 1, &source.second.StreamData->mBuffer, &source.second.OffsetInBytes);
		mVertexCount += source.second.StreamData->mSize;
	}

	mIsDirty = false;
}

void BufferManager::CreatePipe(std::shared_ptr<DrawContext> context)
{
	VkResult result = VK_SUCCESS;

	/**********************************************
	* Figure out flags
	**********************************************/
	SpecializationConstants& constants = context->mSpecializationConstants;
	uint32_t attributeCount = 0;
	uint32_t textureCount = 0;
	uint32_t lightCount = constants.lightCount;
	BOOL hasColor = 0;
	BOOL hasPosition = 0;
	BOOL hasNormal = 0;
	BOOL isLightingEnabled = constants.lighting;

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
	mPipelineColorBlendAttachmentState[0].colorWriteMask = constants.colorWriteEnable;
	mPipelineColorBlendAttachmentState[0].blendEnable = constants.alphaBlendEnable;

	mPipelineColorBlendAttachmentState[0].colorBlendOp = ConvertColorOperation(constants.blendOperation);
	mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = ConvertColorFactor(constants.sourceBlend);
	mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = ConvertColorFactor(constants.destinationBlend);

	mPipelineColorBlendAttachmentState[0].alphaBlendOp = ConvertColorOperation(constants.blendOperationAlpha);
	mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = ConvertColorFactor(constants.sourceBlendAlpha);
	mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = ConvertColorFactor(constants.destinationBlendAlpha);

	SetCulling(mPipelineRasterizationStateCreateInfo, (D3DCULL)constants.cullMode);
	mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode((D3DFILLMODE)constants.fillMode);
	mPipelineInputAssemblyStateCreateInfo.topology = ConvertPrimitiveType(context->PrimitiveType);

	mPipelineDepthStencilStateCreateInfo.depthTestEnable = constants.zEnable; //= VK_TRUE;
	mPipelineDepthStencilStateCreateInfo.depthWriteEnable = constants.zWriteEnable; //VK_TRUE;
	mPipelineDepthStencilStateCreateInfo.depthCompareOp = ConvertCompareOperation(constants.zFunction);  //VK_COMPARE_OP_LESS_OR_EQUAL;
	//mPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = constants.bound
	mPipelineDepthStencilStateCreateInfo.stencilTestEnable = constants.stencilEnable; //VK_FALSE;

	mPipelineDepthStencilStateCreateInfo.back.failOp = ConvertStencilOperation(constants.stencilFail);
	mPipelineDepthStencilStateCreateInfo.back.passOp = ConvertStencilOperation(constants.stencilPass);
	mPipelineDepthStencilStateCreateInfo.back.compareOp = ConvertCompareOperation(constants.stencilFunction);
	
	mPipelineDepthStencilStateCreateInfo.front.failOp = ConvertStencilOperation(constants.ccwStencilFail);
	mPipelineDepthStencilStateCreateInfo.front.passOp = ConvertStencilOperation(constants.ccwStencilPass);
	mPipelineDepthStencilStateCreateInfo.front.compareOp = ConvertCompareOperation(constants.ccwStencilFunction);

	//mPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	//mPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

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
		case 1:
			mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_NORMAL_TEX1;
			mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_NORMAL_TEX1;
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
		//TODO: revisit - make sure multiple sources is valid for FVF.
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
		//if (lightCount)
		//{
		//	mDescriptorSetLayoutCreateInfo.bindingCount = 3; //The number of elements in pBindings.	
		//}
		//else
		//{
		//	mDescriptorSetLayoutCreateInfo.bindingCount = 1; //The number of elements in pBindings.	
		//}	

		mDescriptorSetLayoutCreateInfo.bindingCount = 3; //The number of elements in pBindings.	
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
		mDescriptorBufferInfo[0].range = sizeof(Light) * mDevice->mDeviceState.mLights.size(); //4; 

		mDescriptorBufferInfo[1].buffer = mMaterialBuffer;
		mDescriptorBufferInfo[1].offset = 0;
		mDescriptorBufferInfo[1].range = sizeof(D3DMATERIAL9);

		mWriteDescriptorSet[1].dstSet = resourceContext->DescriptorSet;
		mWriteDescriptorSet[1].descriptorCount = 1;
		mWriteDescriptorSet[1].pBufferInfo = &mDescriptorBufferInfo[0];

		mWriteDescriptorSet[2].dstSet = resourceContext->DescriptorSet;
		mWriteDescriptorSet[2].descriptorCount = 1;
		mWriteDescriptorSet[2].pBufferInfo = &mDescriptorBufferInfo[1];

		vkUpdateDescriptorSets(mDevice->mDevice, 3, mWriteDescriptorSet, 0, nullptr);
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

void BufferManager::UpdateBuffer()
{ //Vulkan doesn't allow vkCmdUpdateBuffer inside of a render pass.

	//The dirty flag for lights can be set by enable light or set light.
	if (mDevice->mDeviceState.mAreLightsDirty)
	{
		vkCmdUpdateBuffer(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], mLightBuffer, 0, sizeof(Light)*mDevice->mDeviceState.mLights.size(), mDevice->mDeviceState.mLights.data()); //context->mSpecializationConstants.lightCount
		mDevice->mDeviceState.mAreLightsDirty = false;
	}

	//
	if (mDevice->mDeviceState.mIsMaterialDirty)
	{
		vkCmdUpdateBuffer(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], mMaterialBuffer, 0, sizeof(D3DMATERIAL9), &mDevice->mDeviceState.mMaterial);
		mDevice->mDeviceState.mIsMaterialDirty = false;
	}
}

void BufferManager::UpdatePushConstants(std::shared_ptr<DrawContext> context)
{
	VkResult result = VK_SUCCESS;
	void* data = nullptr;

	//if (!mDevice->mDeviceState.mHasTransformsChanged)
	//{
	//	return;
	//}

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

	//vkCmdPushConstants(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], context->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, UBO_SIZE, mTotalTransformation.data());
	vkCmdPushConstants(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], context->PipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, UBO_SIZE, mTotalTransformation.data());
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