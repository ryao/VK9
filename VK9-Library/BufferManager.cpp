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
#include "CCubeTexture9.h"

#include "Utilities.h"


BufferManager::BufferManager()
{
	//Don't use. This is only here for containers.
}

BufferManager::BufferManager(CDevice9* device)
	: mDevice(device)
{

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

	if (mVertShaderModule_XYZ_NORMAL_DIFFUSE != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mVertShaderModule_XYZ_NORMAL_DIFFUSE, NULL);
		mVertShaderModule_XYZ_NORMAL_DIFFUSE = VK_NULL_HANDLE;
	}

	if (mFragShaderModule_XYZ_NORMAL_DIFFUSE != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice->mDevice, mFragShaderModule_XYZ_NORMAL_DIFFUSE, NULL);
		mFragShaderModule_XYZ_NORMAL_DIFFUSE = VK_NULL_HANDLE;
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

	//Empty cached objects. (a destructor should take care of their resources.)

	mDrawBuffer.clear();
	mSamplerRequests.clear();
}

void BufferManager::BeginDraw(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;
	boost::container::flat_map<D3DRENDERSTATETYPE, DWORD>::const_iterator searchResult;
	
	/**********************************************
	* Update the stuff that need to be done outside of a render pass.
	**********************************************/
	if (mDevice->mDeviceState.mAreLightsDirty || mDevice->mDeviceState.mIsMaterialDirty)
	{
		vkCmdEndRenderPass(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer]);
		UpdateBuffer();
		vkCmdBeginRenderPass(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], &mDevice->mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	/**********************************************
	* Update the textures that are currently mapped.
	**********************************************/
	auto& deviceState = mDevice->mDeviceState;
	auto& samplerStates = deviceState.mSamplerStates;

	BOOST_FOREACH(const auto& pair1, mDevice->mDeviceState.mTextures)
	{
		VkDescriptorImageInfo& targetSampler = mDevice->mDeviceState.mDescriptorImageInfo[pair1.first];

		if (pair1.second != nullptr)
		{
			std::shared_ptr<SamplerRequest> request = std::make_shared<SamplerRequest>(mDevice);
			auto& currentSampler = samplerStates[request->SamplerIndex];

			if (pair1.second->GetType() == D3DRTYPE_CUBETEXTURE)
			{
				CCubeTexture9* texture = (CCubeTexture9*)pair1.second;

				request->MaxLod = texture->mLevels;
				targetSampler.imageView = texture->mImageView;
			}
			else
			{
				CTexture9* texture = (CTexture9*)pair1.second;

				request->MaxLod = texture->mLevels;
				targetSampler.imageView = texture->mImageView;
			}

			request->MagFilter = (D3DTEXTUREFILTERTYPE)currentSampler[D3DSAMP_MAGFILTER];
			request->MinFilter = (D3DTEXTUREFILTERTYPE)currentSampler[D3DSAMP_MINFILTER];
			request->AddressModeU = (D3DTEXTUREADDRESS)currentSampler[D3DSAMP_ADDRESSU];
			request->AddressModeV = (D3DTEXTUREADDRESS)currentSampler[D3DSAMP_ADDRESSV];
			request->AddressModeW = (D3DTEXTUREADDRESS)currentSampler[D3DSAMP_ADDRESSW];
			request->MaxAnisotropy = currentSampler[D3DSAMP_MAXANISOTROPY];
			request->MipmapMode = (D3DTEXTUREFILTERTYPE)currentSampler[D3DSAMP_MIPFILTER];
			request->MipLodBias = currentSampler[D3DSAMP_MIPMAPLODBIAS]; //bit_cast();
			

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
		context->VertexDeclaration = deviceState.mVertexDeclaration;
	}
	else if (mDevice->mDeviceState.mHasFVF)
	{
		context->FVF = deviceState.mFVF;
	}

	//TODO: revisit if it's valid to have declaration or FVF with either shader type.

	if (mDevice->mDeviceState.mHasVertexShader)
	{
		context->VertexShader = deviceState.mVertexShader; //vert	

	}

	if (mDevice->mDeviceState.mHasPixelShader)
	{
		context->PixelShader = deviceState.mPixelShader; //pixel		
	}

	if (mDevice->mDeviceState.mVertexShader != nullptr)
	{
		context->mVertexShaderConstantSlots = deviceState.mVertexShaderConstantSlots;
		resourceContext->WasShader = true;
	}

	if (mDevice->mDeviceState.mPixelShader != nullptr)
	{
		context->mPixelShaderConstantSlots = deviceState.mPixelShaderConstantSlots;
	}

	context->StreamCount = deviceState.mStreamSources.size();
	context->mSpecializationConstants = deviceState.mSpecializationConstants;
	
	SpecializationConstants& constants = context->mSpecializationConstants;
	ShaderConstantSlots& vertexSlots = context->mVertexShaderConstantSlots;
	ShaderConstantSlots& pixelSlots = context->mPixelShaderConstantSlots;

	constants.lightCount = deviceState.mLights.size();
	constants.textureCount = deviceState.mTextures.size();

	deviceState.mSpecializationConstants.lightCount = constants.lightCount;
	deviceState.mSpecializationConstants.textureCount = constants.textureCount;

	int i = 0;
	BOOST_FOREACH(map_type::value_type& source, deviceState.mStreamSources)
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
			&& drawBuffer.StreamCount == context->StreamCount

			&& drawBuffer.VertexShader == context->VertexShader
			&& drawBuffer.PixelShader == context->PixelShader	
	   
			&& drawBuffer.FVF == context->FVF
			&& drawBuffer.VertexDeclaration == context->VertexDeclaration

			&& !memcmp(&drawBuffer.mSpecializationConstants, &constants,sizeof(SpecializationConstants))
			&& !memcmp(&drawBuffer.mVertexShaderConstantSlots, &vertexSlots, sizeof(ShaderConstantSlots))
			&& !memcmp(&drawBuffer.mPixelShaderConstantSlots, &pixelSlots, sizeof(ShaderConstantSlots))
			)
		{
			if ( !memcmp(&drawBuffer.Bindings, &context->Bindings, 64 * sizeof(UINT)) )
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

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb205599(v=vs.85).aspx
	The units for the D3DRS_DEPTHBIAS and D3DRS_SLOPESCALEDEPTHBIAS render states depend on whether z-buffering or w-buffering is enabled.
	The bias is not applied to any line and point primitive.
	*/
	if (constants.zEnable != D3DZB_FALSE && type > 3)
	{
		vkCmdSetDepthBias(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], constants.depthBias, 0.0f, constants.slopeScaleDepthBias);
	}
	else
	{
		vkCmdSetDepthBias(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], 0.0f, 0.0f, 0.0f);
	}

	/**********************************************
	* Update transformation structure.
	**********************************************/
	if (context->VertexShader==nullptr)
	{
		UpdatePushConstants(context);
	}
	else
	{
		vkCmdPushConstants(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], context->PipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, UBO_SIZE * 2, &deviceState.mPushConstants);
	}

	/**********************************************
	* Check for existing DescriptorSet. Create one if there isn't a matching one.
	**********************************************/

	if (context->DescriptorSetLayout != VK_NULL_HANDLE)
	{
		std::copy(std::begin(deviceState.mDescriptorImageInfo), std::end(deviceState.mDescriptorImageInfo), std::begin(resourceContext->DescriptorImageInfo));

		if (context->VertexShader == nullptr)
		{
			mDescriptorBufferInfo[0].buffer = mLightBuffer;
			mDescriptorBufferInfo[0].offset = 0;
			mDescriptorBufferInfo[0].range = sizeof(Light) * mDevice->mDeviceState.mLights.size(); //4; 

			mDescriptorBufferInfo[1].buffer = mMaterialBuffer;
			mDescriptorBufferInfo[1].offset = 0;
			mDescriptorBufferInfo[1].range = sizeof(D3DMATERIAL9);

			mWriteDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			mWriteDescriptorSet[0].dstSet = resourceContext->DescriptorSet;
			mWriteDescriptorSet[0].descriptorCount = 1;
			mWriteDescriptorSet[0].pBufferInfo = &mDescriptorBufferInfo[0];

			mWriteDescriptorSet[1].dstSet = resourceContext->DescriptorSet;
			mWriteDescriptorSet[1].descriptorCount = 1;
			mWriteDescriptorSet[1].pBufferInfo = &mDescriptorBufferInfo[1];

			mWriteDescriptorSet[2].dstSet = resourceContext->DescriptorSet;
			mWriteDescriptorSet[2].descriptorCount = mDevice->mDeviceState.mTextures.size();
			mWriteDescriptorSet[2].pImageInfo = resourceContext->DescriptorImageInfo;

			if (mDevice->mDeviceState.mTextures.size())
			{
				vkCmdPushDescriptorSetKHR(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->PipelineLayout, 0, 3, mWriteDescriptorSet);
			}
			else
			{
				vkCmdPushDescriptorSetKHR(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->PipelineLayout, 0, 2, mWriteDescriptorSet);
			}
		}
		else
		{
			mWriteDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			mWriteDescriptorSet[0].dstSet = resourceContext->DescriptorSet;
			mWriteDescriptorSet[0].descriptorCount = mDevice->mDeviceState.mTextures.size(); //Revisit
			mWriteDescriptorSet[0].pImageInfo = resourceContext->DescriptorImageInfo;

			vkCmdPushDescriptorSetKHR(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->PipelineLayout, 0, 1, mWriteDescriptorSet);
		}
	}

	/**********************************************
	* Setup bindings
	**********************************************/

	//TODO: I need to find a way to prevent binding on every draw call.

	//if (!mIsDirty || mLastVkPipeline != context->Pipeline)
	//{
	vkCmdBindPipeline(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, context->Pipeline);
	//	mLastVkPipeline = context->Pipeline;
	//}

	mVertexCount = 0;

	if (mDevice->mDeviceState.mIndexBuffer != nullptr)
	{
		vkCmdBindIndexBuffer(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], deviceState.mIndexBuffer->mBuffer, 0, deviceState.mIndexBuffer->mIndexType);
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
	else if (context->VertexShader != nullptr)
	{
		//Nothing so far.
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

	if (constants.cullMode != D3DCULL_CCW)
	{
		mPipelineDepthStencilStateCreateInfo.back.failOp = ConvertStencilOperation(constants.ccwStencilFail);
		mPipelineDepthStencilStateCreateInfo.back.passOp = ConvertStencilOperation(constants.ccwStencilPass);
		mPipelineDepthStencilStateCreateInfo.back.compareOp = ConvertCompareOperation(constants.ccwStencilFunction);

		mPipelineDepthStencilStateCreateInfo.front.failOp = ConvertStencilOperation(constants.stencilFail);
		mPipelineDepthStencilStateCreateInfo.front.passOp = ConvertStencilOperation(constants.stencilPass);
		mPipelineDepthStencilStateCreateInfo.front.compareOp = ConvertCompareOperation(constants.stencilFunction);
	}
	else
	{
		mPipelineDepthStencilStateCreateInfo.back.failOp = ConvertStencilOperation(constants.stencilFail);
		mPipelineDepthStencilStateCreateInfo.back.passOp = ConvertStencilOperation(constants.stencilPass);
		mPipelineDepthStencilStateCreateInfo.back.compareOp = ConvertCompareOperation(constants.stencilFunction);

		mPipelineDepthStencilStateCreateInfo.front.failOp = ConvertStencilOperation(constants.ccwStencilFail);
		mPipelineDepthStencilStateCreateInfo.front.passOp = ConvertStencilOperation(constants.ccwStencilPass);
		mPipelineDepthStencilStateCreateInfo.front.compareOp = ConvertCompareOperation(constants.ccwStencilFunction);
	}


	//mPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	//mPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

	/**********************************************
	* Figure out correct shader
	**********************************************/
	if (context->VertexShader != nullptr)
	{
		mPipelineShaderStageCreateInfo[0].module = context->VertexShader->mShaderConverter.mConvertedShader.ShaderModule;
		mPipelineShaderStageCreateInfo[1].module = context->PixelShader->mShaderConverter.mConvertedShader.ShaderModule;
	}
	else
	{
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
			case 0:
				mPipelineShaderStageCreateInfo[0].module = mVertShaderModule_XYZ_NORMAL_DIFFUSE;
				mPipelineShaderStageCreateInfo[1].module = mFragShaderModule_XYZ_NORMAL_DIFFUSE;
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
	}

	/**********************************************
	* Figure out attributes
	**********************************************/
	//if (context->VertexShader != nullptr)
	//{
	//	//Revisit, make sure this copies properly.
	//	memcpy(&mVertexInputAttributeDescription, &context->VertexShader->mShaderConverter.mConvertedShader.mVertexInputAttributeDescription, sizeof(mVertexInputAttributeDescription));
	//	mPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = mVertexInputAttributeDescription;
	//	attributeCount = context->VertexShader->mShaderConverter.mConvertedShader.mVertexInputAttributeDescriptionCount;
	//}

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

	if (context->VertexShader != nullptr)
	{
		auto& convertedVertexShader = context->VertexShader->mShaderConverter.mConvertedShader;
		auto& convertedPixelShader = context->PixelShader->mShaderConverter.mConvertedShader;

		mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = context->StreamCount;
		mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount;

		memcpy(&mDescriptorSetLayoutBinding, &convertedPixelShader.mDescriptorSetLayoutBinding, sizeof(mDescriptorSetLayoutBinding));

		mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;
		mPipelineLayoutCreateInfo.pSetLayouts = &context->DescriptorSetLayout;

		mDescriptorSetLayoutCreateInfo.bindingCount = convertedPixelShader.mDescriptorSetLayoutBindingCount;
		mPipelineLayoutCreateInfo.setLayoutCount = 1;

		mVertexSpecializationInfo.pData = &context->mVertexShaderConstantSlots;
		mVertexSpecializationInfo.dataSize = sizeof(ShaderConstantSlots);
		mVertexSpecializationInfo.pMapEntries = mSlotMapEntries;
		mVertexSpecializationInfo.mapEntryCount = 1024;

		mPixelSpecializationInfo.pData = &context->mPixelShaderConstantSlots;
		mPixelSpecializationInfo.dataSize = sizeof(ShaderConstantSlots);
		mPixelSpecializationInfo.pMapEntries = mSlotMapEntries;
		mPixelSpecializationInfo.mapEntryCount = 1024;
	}
	else
	{		
		mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = context->StreamCount;
		mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount;

		mDescriptorSetLayoutBinding[0].binding = 0;
		mDescriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		mDescriptorSetLayoutBinding[0].descriptorCount = 1;
		mDescriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
		mDescriptorSetLayoutBinding[0].pImmutableSamplers = NULL;

		mDescriptorSetLayoutBinding[1].binding = 1;
		mDescriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		mDescriptorSetLayoutBinding[1].descriptorCount = 1;
		mDescriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
		mDescriptorSetLayoutBinding[1].pImmutableSamplers = NULL;

		mDescriptorSetLayoutBinding[2].binding = 2;
		mDescriptorSetLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
		mDescriptorSetLayoutBinding[2].descriptorCount = textureCount; //Update to use mapped texture.
		mDescriptorSetLayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		mDescriptorSetLayoutBinding[2].pImmutableSamplers = NULL;

		mDescriptorSetLayoutCreateInfo.pBindings = mDescriptorSetLayoutBinding;
		mPipelineLayoutCreateInfo.pSetLayouts = &context->DescriptorSetLayout;

		if (textureCount)
		{
			mDescriptorSetLayoutCreateInfo.bindingCount = 3; //The number of elements in pBindings.	
			mPipelineLayoutCreateInfo.setLayoutCount = 1;
		}
		else
		{
			mDescriptorSetLayoutCreateInfo.bindingCount = 2; //The number of elements in pBindings.	
			mPipelineLayoutCreateInfo.setLayoutCount = 1;
		}

		mVertexSpecializationInfo.pData = &mDevice->mDeviceState.mSpecializationConstants;
		mVertexSpecializationInfo.dataSize = sizeof(SpecializationConstants);
		mVertexSpecializationInfo.pMapEntries = mSlotMapEntries;
		mVertexSpecializationInfo.mapEntryCount = 251;

		mPixelSpecializationInfo.pData = &mDevice->mDeviceState.mSpecializationConstants;
		mPixelSpecializationInfo.dataSize = sizeof(SpecializationConstants);
		mPixelSpecializationInfo.pMapEntries = mSlotMapEntries;
		mPixelSpecializationInfo.mapEntryCount = 251;
	}

	result = vkCreateDescriptorSetLayout(mDevice->mDevice, &mDescriptorSetLayoutCreateInfo, nullptr, &context->DescriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::CreateDescriptorSet vkCreateDescriptorSetLayout failed with return code of " << GetResultString(result);
		return;
	}

	/**********************************************
	* Create pipeline & descriptor set layout.
	**********************************************/

	result = vkCreatePipelineLayout(mDevice->mDevice, &mPipelineLayoutCreateInfo, nullptr, &context->PipelineLayout);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreatePipelineLayout failed with return code of " << GetResultString(result);
		return;
	}

	mGraphicsPipelineCreateInfo.layout = context->PipelineLayout;

	result = vkCreateGraphicsPipelines(mDevice->mDevice, mPipelineCache, 1, &mGraphicsPipelineCreateInfo, nullptr, &context->Pipeline);
	//result = vkCreateGraphicsPipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::BeginDraw vkCreateGraphicsPipelines failed with return code of " << GetResultString(result);
	}

	this->mDrawBuffer.push_back(context);
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

	/*
	https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html
	If either magFilter or minFilter is VK_FILTER_CUBIC_IMG, anisotropyEnable must be VK_FALSE
	*/
	if (mDevice->mDeviceFeatures.samplerAnisotropy && samplerCreateInfo.minFilter != VK_FILTER_CUBIC_IMG && samplerCreateInfo.magFilter != VK_FILTER_CUBIC_IMG)
	{
		// Use max. level of anisotropy for this example
		samplerCreateInfo.maxAnisotropy = min(request->MaxAnisotropy, mDevice->mDeviceProperties.limits.maxSamplerAnisotropy);
		
		if (request->MinFilter == D3DTEXF_ANISOTROPIC ||
			request->MagFilter == D3DTEXF_ANISOTROPIC ||
			request->MipmapMode == D3DTEXF_ANISOTROPIC)
		{
			samplerCreateInfo.anisotropyEnable = VK_TRUE;
		}
		else {
			samplerCreateInfo.anisotropyEnable = VK_FALSE;
		}
	}
	else
	{
		// The device does not support anisotropic filtering or cubic is currently in use.
		samplerCreateInfo.maxAnisotropy = 1.0;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
	}

	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; // VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER; //VK_COMPARE_OP_ALWAYS
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = request->MaxLod;

	if (request->MipmapMode == D3DTEXF_NONE)
	{
		samplerCreateInfo.maxLod = 0.0f;
	}

	mResult = vkCreateSampler(mDevice->mDevice, &samplerCreateInfo, NULL, &request->Sampler);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "BufferManager::GenerateSampler vkCreateSampler failed with return code of " << GetResultString(mResult);
		return;
	}

	mSamplerRequests.push_back(request);
}

void BufferManager::UpdatePushConstants(std::shared_ptr<DrawContext> context)
{
	VkResult result = VK_SUCCESS;
	void* data = nullptr;

	//if (!mDevice->mDeviceState.mHasTransformsChanged)
	//{
	//	return;
	//}

	mTransformations.mModel <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	mTransformations.mView <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	mTransformations.mProjection <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	BOOST_FOREACH(const auto& pair1, mDevice->mDeviceState.mTransforms)
	{
		switch (pair1.first)
		{
		case D3DTS_WORLD:

			mTransformations.mModel <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_VIEW:

			mTransformations.mView <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_PROJECTION:

			mTransformations.mProjection <<
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

	mTransformations.mTotalTransformation = mTransformations.mProjection * mTransformations.mView * mTransformations.mModel;
	//mTotalTransformation = mModel * mView * mProjection;

	vkCmdPushConstants(mDevice->mSwapchainBuffers[mDevice->mCurrentBuffer], context->PipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, UBO_SIZE * 2, &mTransformations);
}

void BufferManager::FlushDrawBufffer()
{
	/*
	Uses remove_if and chrono to remove elements that have not been used in over a second.
	*/
	mDrawBuffer.erase(std::remove_if(mDrawBuffer.begin(), mDrawBuffer.end(), [](const std::shared_ptr<DrawContext> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), mDrawBuffer.end());
	mSamplerRequests.erase(std::remove_if(mSamplerRequests.begin(), mSamplerRequests.end(), [](const std::shared_ptr<SamplerRequest> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), mSamplerRequests.end());

	mIsDirty = true;
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