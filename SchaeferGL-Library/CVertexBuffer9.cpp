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
 
#include "CVertexBuffer9.h"
#include "CDevice9.h"

#include "Utilities.h"

CVertexBuffer9::CVertexBuffer9(CDevice9* device, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, HANDLE* pSharedHandle)
	: mReferenceCount(0),
	mDevice(device),
	mLength(Length),
	mUsage(Usage),
	mFVF(FVF),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mResult(VK_SUCCESS),
	mData(nullptr),
	mSize(0),
	mCapacity(0),
	mIsDirty(true),
	mBuffer(VK_NULL_HANDLE),
	mMemory(VK_NULL_HANDLE),
	mDescriptorSetLayout(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mPipeline(VK_NULL_HANDLE),
	mPipelineCache(VK_NULL_HANDLE),
	mLastType(D3DPT_FORCE_DWORD) //used to help prevent repeating pipe creation.
{
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = NULL;
	bufferCreateInfo.size = mLength;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.flags = 0;

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = NULL;
	memoryAllocateInfo.allocationSize = 0;
	memoryAllocateInfo.memoryTypeIndex = 0;

	mMemoryRequirements = {};

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

	mResult = vkCreateBuffer(mDevice->mDevice, &bufferCreateInfo, NULL, &mBuffer);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::CVertexBuffer9 vkCreateBuffer failed with return code of " << mResult;
		return;
	}

	vkGetBufferMemoryRequirements(mDevice->mDevice, mBuffer, &mMemoryRequirements);

	memoryAllocateInfo.allocationSize = mMemoryRequirements.size;

	GetMemoryTypeFromProperties(mDevice->mDeviceMemoryProperties, mMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memoryAllocateInfo.memoryTypeIndex);

	mResult = vkAllocateMemory(mDevice->mDevice, &memoryAllocateInfo, NULL, &mMemory);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::CVertexBuffer9 vkAllocateMemory failed with return code of " << mResult;
		return;
	}

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};

	descriptorSetLayoutBinding.binding = 0;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBinding.descriptorCount = 0; //DEMO_TEXTURE_COUNT;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorSetLayoutBinding.pImmutableSamplers = NULL;

	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = NULL;
	descriptorSetLayoutCreateInfo.bindingCount = 1;
	descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

	mResult = vkCreateDescriptorSetLayout(mDevice->mDevice, &descriptorSetLayoutCreateInfo, NULL, &mDescriptorSetLayout);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::CVertexBuffer9 vkCreateDescriptorSetLayout failed with return code of " << mResult;
		return;
	}

	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = NULL;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;

	mResult = vkCreatePipelineLayout(mDevice->mDevice, &pipelineLayoutCreateInfo, NULL, &mPipelineLayout);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::CVertexBuffer9 vkCreatePipelineLayout failed with return code of " << mResult;
		return;
	}

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

CVertexBuffer9::~CVertexBuffer9()
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
	
	if (mBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice->mDevice, mBuffer, NULL);
	}

	if (mMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mMemory, NULL);
	}	
}

ULONG STDMETHODCALLTYPE CVertexBuffer9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::QueryInterface(REFIID riid,void  **ppv)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::QueryInterface is not implemented!";

	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CVertexBuffer9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CVertexBuffer9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CVertexBuffer9::GetType()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::GetType is not implemented!";

	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CVertexBuffer9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CVertexBuffer9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::GetDesc is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	VkResult result = VK_SUCCESS;

	if (mPool == D3DPOOL_MANAGED)
	{
		if(!(Flags & D3DLOCK_READONLY))
		{ //If the lock allows write mark the buffer as dirty.
			mIsDirty = true;
		}
	}

	result = vkMapMemory(mDevice->mDevice, mMemory, 0, mMemoryRequirements.size, 0, &mData);

	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::Lock vkMapMemory failed with return code of " << result;
		*ppbData = nullptr;

		return D3DERR_INVALIDCALL;
	}

	*ppbData = (char *)mData + OffsetToLock;
	mLockCount++;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::Unlock()
{
	VkResult result = VK_SUCCESS;

	vkUnmapMemory(mDevice->mDevice, mMemory);

	result = vkBindBufferMemory(mDevice->mDevice, mBuffer, mMemory, 0);
	
	mLockCount--;

	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::Unlock vkBindBufferMemory failed with return code of " << result;

		return D3DERR_INVALIDCALL;
	}

	return S_OK;	
}

VkPipeline CVertexBuffer9::GetPipeline(D3DPRIMITIVETYPE type)
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
	result = vkCreatePipelineCache(mDevice->mDevice, &mPipelineCacheCreateInfo, NULL,&mPipelineCache);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::GetPipeline vkCreatePipelineCache failed with return code of " << mResult;
		return mPipeline;
	}

	result = vkCreateGraphicsPipelines(mDevice->mDevice, mPipelineCache, 1, &mGraphicsPipelineCreateInfo, NULL, &mPipeline);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CVertexBuffer9::GetPipeline vkCreateGraphicsPipelines failed with return code of " << mResult;
		//Don't return so we can destroy cache.
	}

	vkDestroyPipelineCache(mDevice->mDevice, mPipelineCache, NULL);

	if (result == VK_SUCCESS)
	{
		mIsDirty = false;
	}

	return mPipeline;
}