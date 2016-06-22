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
	mDescriptorSet(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mDescriptorSetLayout(VK_NULL_HANDLE)

{
	//Don't use. This is only here for containers.
}

BufferManager::BufferManager(CDevice9* device)
	: mDevice(device),
	mLastType(D3DPT_FORCE_DWORD), //used to help prevent repeating pipe creation.
	mIsDirty(true),
	mPipeline(VK_NULL_HANDLE),
	mDescriptorSet(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mDescriptorSetLayout(VK_NULL_HANDLE)
{

	mVertShaderModule = LoadShaderFromResource(mDevice->mDevice,  TRI_VERT);
	mFragshaderModule = LoadShaderFromResource(mDevice->mDevice, TRI_FRAG);

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

	mGraphicsPipelineCreateInfo.stageCount = 2;

	mPipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	mPipelineShaderStageCreateInfo[0].module = mVertShaderModule;
	mPipelineShaderStageCreateInfo[0].pName = "main";

	mPipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mPipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	mPipelineShaderStageCreateInfo[1].module = mFragshaderModule;
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
}

BufferManager::~BufferManager()
{
	if (mVertShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule, NULL);
	}

	if (mFragshaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragshaderModule, NULL);
	}
	
	if (mPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice->mDevice, mPipeline, NULL);
	}

	if (mDescriptorSet != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(mDevice->mDevice, mDevice->mDescriptorPool, 1, &mDescriptorSet);
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

void BufferManager::UpdatePipeline(D3DPRIMITIVETYPE type)
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

	mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = mStreamSources.size();
	mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = mStreamSources.size() * 2;
	mDescriptorSetAllocateInfo.descriptorSetCount = mStreamSources.size();
	mPipelineLayoutCreateInfo.setLayoutCount = mStreamSources.size();

	int i = 0;
	BOOST_FOREACH(map_type::value_type& source, mStreamSources)
	{
		int ai1 = i * 2; //attribute index 1
		int ai2 = ai1 + 1; //attribute index 1

		mDescriptorSetLayoutBinding[i].binding = source.first;
		mDescriptorSetLayoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		mDescriptorSetLayoutBinding[i].descriptorCount = 1;
		mDescriptorSetLayoutBinding[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		mDescriptorSetLayoutBinding[i].pImmutableSamplers = NULL;	

		vkCmdBindVertexBuffers(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], source.first, 1, &source.second.StreamData->mBuffer, &source.second.OffsetInBytes);

		mVertexInputBindingDescription[i].binding = source.first;
		mVertexInputBindingDescription[i].stride = source.second.Stride;
		mVertexInputBindingDescription[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		//TODO: if the FVF is something other than D3DFVF_XYZ | D3DFVF_DIFFUSE than update the input structures.
		mVertexInputAttributeDescription[ai1].binding = source.first;
		mVertexInputAttributeDescription[ai1].location = 0;
		mVertexInputAttributeDescription[ai1].format = VK_FORMAT_R32G32B32_SFLOAT;
		mVertexInputAttributeDescription[ai1].offset = 0;

		mVertexInputAttributeDescription[ai2].binding = source.first;
		mVertexInputAttributeDescription[ai2].location = 1;
		mVertexInputAttributeDescription[ai2].format = VK_FORMAT_B8G8R8A8_UNORM;
		mVertexInputAttributeDescription[ai2].offset = sizeof(float) * 3;
		
		i++;
	}

	mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;
	mDescriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;
	mPipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;


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

	if (result == VK_SUCCESS)
	{
		mIsDirty = false;
	}

	return;
}