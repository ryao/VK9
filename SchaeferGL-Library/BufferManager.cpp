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

BufferManager::BufferManager()
	: mDevice(nullptr),
	mDescriptorSetLayout(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mPipeline(VK_NULL_HANDLE),
	mPipelineCache(VK_NULL_HANDLE),
	mLastType(D3DPT_FORCE_DWORD), //used to help prevent repeating pipe creation.
	mIsDirty(true)
{

}

BufferManager::BufferManager(CDevice9* device)
	: mDevice(device),
	mDescriptorSetLayout(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mPipeline(VK_NULL_HANDLE),
	mPipelineCache(VK_NULL_HANDLE),
	mLastType(D3DPT_FORCE_DWORD), //used to help prevent repeating pipe creation.
	mIsDirty(true)
{

	mDynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE] = {};
	mPipelineColorBlendAttachmentState[1] = {};

	mPipelineVertexInputStateCreateInfo = {};
	mPipelineInputAssemblyStateCreateInfo = {};
	mPipelineRasterizationStateCreateInfo = {};
	mPipelineColorBlendStateCreateInfo = {};
	mPipelineDepthStencilStateCreateInfo = {};
	mPipelineViewportStateCreateInfo = {};
	mPipelineMultisampleStateCreateInfo = {};
	mPipelineDynamicStateCreateInfo = {};
	mPipelineCacheCreateInfo = {};
	mGraphicsPipelineCreateInfo = {};
	mDescriptorSetLayoutBinding = {};
	mDescriptorSetLayoutCreateInfo = {};
	mPipelineLayoutCreateInfo = {};

	mPipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	mPipelineVertexInputStateCreateInfo.pNext = NULL;
	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	mPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = mVertexInputBindingDescription;
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
	mPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = mVertexInputAttributeDescription;

	mVertexInputBindingDescription[0].binding = 0; //VERTEX_BUFFER_BIND_ID  Should be reset when bound.
	mVertexInputBindingDescription[0].stride = sizeof(Vertex); //structure they should be passing in.
	mVertexInputBindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	mVertexInputAttributeDescription[0].binding = 0; //VERTEX_BUFFER_BIND_ID  Should be reset when bound.
	mVertexInputAttributeDescription[0].location = 0;
	mVertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	mVertexInputAttributeDescription[0].offset = 0;

	mVertexInputAttributeDescription[1].binding = 0; //VERTEX_BUFFER_BIND_ID  Should be reset when bound.
	mVertexInputAttributeDescription[1].location = 1;
	mVertexInputAttributeDescription[1].format = VK_FORMAT_B8G8R8A8_UNORM;
	mVertexInputAttributeDescription[1].offset = sizeof(float) * 3;

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

	mDescriptorSetLayoutBinding.binding = 0;
	mDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	mDescriptorSetLayoutBinding.descriptorCount = 0; //DEMO_TEXTURE_COUNT;
	mDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	mDescriptorSetLayoutBinding.pImmutableSamplers = NULL;

	mDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	mDescriptorSetLayoutCreateInfo.pNext = NULL;
	mDescriptorSetLayoutCreateInfo.bindingCount = 1;
	mDescriptorSetLayoutCreateInfo.pBindings = &mDescriptorSetLayoutBinding;

	mPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	mPipelineLayoutCreateInfo.pNext = NULL;
	mPipelineLayoutCreateInfo.setLayoutCount = 1;
	mPipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;

	mGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	mGraphicsPipelineCreateInfo.layout = mPipelineLayout;
	mGraphicsPipelineCreateInfo.pVertexInputState = &mPipelineVertexInputStateCreateInfo;
	mGraphicsPipelineCreateInfo.pInputAssemblyState = &mPipelineInputAssemblyStateCreateInfo;
	mGraphicsPipelineCreateInfo.pRasterizationState = &mPipelineRasterizationStateCreateInfo;
	mGraphicsPipelineCreateInfo.pColorBlendState = &mPipelineColorBlendStateCreateInfo;
	mGraphicsPipelineCreateInfo.pDepthStencilState = &mPipelineDepthStencilStateCreateInfo;
	mGraphicsPipelineCreateInfo.pViewportState = &mPipelineViewportStateCreateInfo;
	mGraphicsPipelineCreateInfo.pMultisampleState = &mPipelineMultisampleStateCreateInfo;
	mGraphicsPipelineCreateInfo.pStages = nullptr; //shaderStages
	mGraphicsPipelineCreateInfo.renderPass = mDevice->mRenderPass;
	mGraphicsPipelineCreateInfo.pDynamicState = &mPipelineDynamicStateCreateInfo;

	mPipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
}

BufferManager::~BufferManager()
{
	if (mPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice->mDevice, mPipeline, NULL);
	}

	if (mPipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(mDevice->mDevice, mPipelineLayout, NULL);
	}

	if (mDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(mDevice->mDevice, mDescriptorSetLayout, NULL);
	}
}

VkPipeline BufferManager::GetPipeline(D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;

	/**********************************************
	* Cleanup/Fetch Pipe
	**********************************************/
	//TODO: add logic to return existing pipe when possible.

	if (mLastType != type)
	{
		mIsDirty = true;
	}

	if (mPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice->mDevice, mPipeline, NULL);
		mPipeline = VK_NULL_HANDLE;
	}

	/**********************************************
	* Update Pipe Creation Information
	**********************************************/

	//TODO: if the FVF is something other than D3DFVF_XYZ | D3DFVF_DIFFUSE than update the input structures.


	mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode((D3DFILLMODE)mDevice->mRenderStates[D3DRS_FILLMODE]);

	SetCulling(mPipelineRasterizationStateCreateInfo, (D3DCULL)mDevice->mRenderStates[D3DRS_FILLMODE]);

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


	/**********************************************
	* Create Pipe
	**********************************************/

	result = vkCreateDescriptorSetLayout(mDevice->mDevice, &mDescriptorSetLayoutCreateInfo, NULL, &mDescriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::GetPipeline vkCreateDescriptorSetLayout failed with return code of " << mResult;
		return mPipeline;
	}

	result = vkCreatePipelineLayout(mDevice->mDevice, &mPipelineLayoutCreateInfo, NULL, &mPipelineLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::GetPipeline vkCreatePipelineLayout failed with return code of " << mResult;
		return mPipeline;
	}

	result = vkCreatePipelineCache(mDevice->mDevice, &mPipelineCacheCreateInfo, NULL, &mPipelineCache);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::GetPipeline vkCreatePipelineCache failed with return code of " << mResult;
		return mPipeline;
	}

	result = vkCreateGraphicsPipelines(mDevice->mDevice, mPipelineCache, 1, &mGraphicsPipelineCreateInfo, NULL, &mPipeline);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::GetPipeline vkCreateGraphicsPipelines failed with return code of " << mResult;
		//Don't return so we can destroy cache.
	}

	vkDestroyPipelineCache(mDevice->mDevice, mPipelineCache, NULL);

	if (result == VK_SUCCESS)
	{
		mIsDirty = false;
	}

	return mPipeline;
}