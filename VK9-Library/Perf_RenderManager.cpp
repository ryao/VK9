/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

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

#include "Perf_RenderManager.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/format.hpp>

#include "Utilities.h"

#include "CCubeTexture9.h"
#include "CBaseTexture9.h"
#include "CTexture9.h"
#include "CIndexBuffer9.h"
#include "CVertexDeclaration9.h"
#include "CPixelShader9.h"
#include "CVertexShader9.h"

SamplerRequest::~SamplerRequest()
{
	if (mRealWindow != nullptr)
	{
		auto& device = mRealWindow->mRealDevice;
		device.mDevice.destroySampler(Sampler, nullptr);
	}
}

ResourceContext::~ResourceContext()
{
	if (mRealWindow != nullptr)
	{
		auto& device = mRealWindow->mRealDevice;
		device.mDevice.freeDescriptorSets(device.mDescriptorPool, 1, &DescriptorSet);
	}
}

DrawContext::~DrawContext()
{
	if (mRealWindow != nullptr)
	{
		auto& device = mRealWindow->mRealDevice;
		device.mDevice.destroyPipeline(Pipeline, nullptr);
		device.mDevice.destroyPipelineLayout(PipelineLayout, nullptr);
		device.mDevice.destroyDescriptorSetLayout(DescriptorSetLayout, nullptr);
	}
}

RenderManager::RenderManager()
{

}

RenderManager::~RenderManager()
{
}

void RenderManager::UpdateBuffer(RealWindow& realWindow)
{ //Vulkan doesn't allow vkCmdUpdateBuffer inside of a render pass.

	auto& device = realWindow.mRealDevice.mDevice;

	//The dirty flag for lights can be set by enable light or set light.
	if (realWindow.mDeviceState.mAreLightsDirty)
	{
		realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].updateBuffer(realWindow.mLightBuffer, 0, sizeof(Light)*realWindow.mDeviceState.mLights.size(), realWindow.mDeviceState.mLights.data()); //context->mSpecializationConstants.lightCount
		realWindow.mDeviceState.mAreLightsDirty = false;
	}

	if (realWindow.mDeviceState.mIsMaterialDirty)
	{
		realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].updateBuffer(realWindow.mMaterialBuffer, 0, sizeof(D3DMATERIAL9), &realWindow.mDeviceState.mMaterial);
		realWindow.mDeviceState.mIsMaterialDirty = false;
	}
}

void RenderManager::StartScene(RealWindow& realWindow, bool clear)
{
	realWindow.mIsSceneStarted = true;

	vk::Result result;
	auto& device = realWindow.mRealDevice.mDevice;

	result = device.acquireNextImageKHR(realWindow.mSwapchain, UINT64_MAX, realWindow.mPresentCompleteSemaphore, nullptr, &realWindow.mCurrentSwapchainBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::StartScene vkAcquireNextImageKHR failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	//maybe add back later
	//SetImageLayout(mSwapchainImages[mCurrentBuffer], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR); //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL

	result = realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].begin(&realWindow.mCommandBufferBeginInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::StartScene vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	//realWindow.mImageMemoryBarrier.srcAccessMask = 0;
	realWindow.mImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead; //VK_ACCESS_MEMORY_READ_BIT;
	realWindow.mImageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
	realWindow.mImageMemoryBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR; //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	realWindow.mImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	realWindow.mImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	realWindow.mImageMemoryBarrier.image = realWindow.mSwapchainImages[realWindow.mCurrentSwapchainBuffer];
	realWindow.mImageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &realWindow.mImageMemoryBarrier);

	/*
	The Vulkan spec doesn't allow updating buffers inside of a render pass so we need to put as much buffer update logic into this method to reduce violations.
	The previous UBO implementation was updating inside of a render pass and it seemed to work on Desktop but that may vary from driver to driver.
	In addition performance is not guaranteed in case of spec violation.
	*/
	UpdateBuffer(realWindow);

	realWindow.mClearValues[0].color = realWindow.mClearColorValue;
	realWindow.mClearValues[1].depthStencil = { 1.0f, 0 };

	if (clear)
	{
		realWindow.mRenderPassBeginInfo.renderPass = realWindow.mClearRenderPass; // mClearRenderPass;
	}
	else
	{
		realWindow.mRenderPassBeginInfo.renderPass = realWindow.mStoreRenderPass;
	}
	realWindow.mRenderPassBeginInfo.framebuffer = realWindow.mFramebuffers[realWindow.mCurrentSwapchainBuffer];
	realWindow.mRenderPassBeginInfo.renderArea.offset.x = 0;
	realWindow.mRenderPassBeginInfo.renderArea.offset.y = 0;
	realWindow.mRenderPassBeginInfo.renderArea.extent.width = realWindow.mSwapchainExtent.width;
	realWindow.mRenderPassBeginInfo.renderArea.extent.height = realWindow.mSwapchainExtent.height;
	realWindow.mRenderPassBeginInfo.clearValueCount = 2;
	realWindow.mRenderPassBeginInfo.pClearValues = realWindow.mClearValues;

	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].beginRenderPass(&realWindow.mRenderPassBeginInfo, vk::SubpassContents::eInline);

	//Set the pass back to store so draw calls won't be lost if they require stop/start of render pass.
	realWindow.mRenderPassBeginInfo.renderPass = realWindow.mStoreRenderPass;
	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].setViewport(0, 1, &realWindow.mDeviceState.mViewport);
	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].setScissor(0, 1, &realWindow.mDeviceState.mScissor);
}

void RenderManager::StopScene(RealWindow& realWindow)
{
	realWindow.mIsSceneStarted = false;

	vk::Result result;
	realWindow.mPipeStageFlags = vk::PipelineStageFlagBits::eBottomOfPipe; //VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	realWindow.mSubmitInfo.waitSemaphoreCount = 1;
	realWindow.mSubmitInfo.pWaitSemaphores = &realWindow.mPresentCompleteSemaphore;
	realWindow.mSubmitInfo.pWaitDstStageMask = &realWindow.mPipeStageFlags;
	realWindow.mSubmitInfo.commandBufferCount = 1;
	realWindow.mSubmitInfo.pCommandBuffers = &realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer];
	realWindow.mSubmitInfo.signalSemaphoreCount = 0;
	realWindow.mSubmitInfo.pSignalSemaphores = nullptr;

	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].endRenderPass();

	realWindow.mPrePresentBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead; //VK_ACCESS_MEMORY_READ_BIT;
	realWindow.mPrePresentBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead; //VK_ACCESS_MEMORY_READ_BIT;
	realWindow.mPrePresentBarrier.oldLayout = vk::ImageLayout::ePresentSrcKHR; //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	realWindow.mPrePresentBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR; //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	realWindow.mPrePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	realWindow.mPrePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	realWindow.mPrePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	realWindow.mPrePresentBarrier.image = realWindow.mSwapchainImages[realWindow.mCurrentSwapchainBuffer];
	vk::ImageMemoryBarrier* memoryBarrier = &realWindow.mPrePresentBarrier;
	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, memoryBarrier);

	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].end();

	result = realWindow.mQueue.submit(1, &realWindow.mSubmitInfo, realWindow.mNullFence);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::EndScene vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
		return;
	}

}

void RenderManager::CopyImage(RealWindow& realWindow, vk::Image srcImage, vk::Image dstImage, int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t srcMip, uint32_t dstMip)
{
	vk::Result result;
	vk::CommandBuffer commandBuffer;
	auto& device = realWindow.mRealDevice.mDevice;

	vk::CommandBufferAllocateInfo commandBufferInfo;
	commandBufferInfo.commandPool = realWindow.mCommandPool;
	commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferInfo.commandBufferCount = 1;

	result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CopyImage vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
	commandBufferInheritanceInfo.renderPass = nullptr;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = nullptr;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	//commandBufferInheritanceInfo.queryFlags = 0;
	//commandBufferInheritanceInfo.pipelineStatistics = 0;

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	//commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = commandBuffer.begin(&commandBufferBeginInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CopyImage vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	ReallyCopyImage(commandBuffer, srcImage, dstImage, x, y, width, height, srcMip, dstMip, 0, 0);

	commandBuffer.end();

	vk::CommandBuffer commandBuffers[] = { commandBuffer };
	vk::Fence nullFence;
	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	result = realWindow.mQueue.submit(1, &submitInfo, nullFence);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CopyImage vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realWindow.mQueue.waitIdle();
	device.freeCommandBuffers(realWindow.mCommandPool, 1, commandBuffers);
}

void RenderManager::Clear(RealWindow& realWindow, DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	if ((Flags & D3DCLEAR_TARGET) == D3DCLEAR_TARGET)
	{
		//VK_FORMAT_B8G8R8A8_UNORM 
		//Revisit the byte order could be difference based on the surface format so I need a better way to handle this.
		realWindow.mClearColorValue.float32[3] = D3DCOLOR_A(Color); //FLT_MAX; 
		realWindow.mClearColorValue.float32[2] = D3DCOLOR_B(Color);
		realWindow.mClearColorValue.float32[1] = D3DCOLOR_G(Color);
		realWindow.mClearColorValue.float32[0] = D3DCOLOR_R(Color);
	}

	if (Count > 0 && pRects != nullptr)
	{
		BOOST_LOG_TRIVIAL(warning) << "RenderManager::Clear is not fully implemented!";
		return;
	}

	vk::ImageSubresourceRange subResourceRange;
	subResourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	subResourceRange.baseMipLevel = 0;
	subResourceRange.levelCount = 1;
	subResourceRange.baseArrayLayer = 0;
	subResourceRange.layerCount = 1;

	if (realWindow.mIsSceneStarted)
	{
		realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].endRenderPass();
		realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].clearColorImage(realWindow.mSwapchainImages[realWindow.mCurrentSwapchainBuffer], vk::ImageLayout::eTransferDstOptimal, &realWindow.mClearColorValue, 1, &subResourceRange);
		realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].beginRenderPass(&realWindow.mRenderPassBeginInfo, vk::SubpassContents::eInline);
	}
	else
	{
		this->StartScene(realWindow);
	}
}

void RenderManager::Present(RealWindow& realWindow, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (!realWindow.mIsSceneStarted)
	{
		this->StartScene(realWindow);
	}
	this->StopScene(realWindow);

	vk::Result result;

	realWindow.mPresentInfo.pImageIndices = &realWindow.mCurrentSwapchainBuffer;

	result = realWindow.mQueue.presentKHR(&realWindow.mPresentInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::Present vkQueuePresentKHR failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realWindow.mQueue.waitIdle();
	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	//Clean up pipes.
	//mBufferManager->FlushDrawBufffer();

	//Clean up unreferenced resources.
	//mGarbageManager.DestroyHandles();

	//Print(mDeviceState.mTransforms);
}

void RenderManager::DrawIndexedPrimitive(RealWindow& realWindow, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	if (realWindow.mDeviceState.mIndexBuffer == nullptr)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitive called with null index buffer.";
		return;
	}

	if (!realWindow.mIsSceneStarted)
	{
		this->StartScene(realWindow);
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(this);
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(this);

	BeginDraw(realWindow,context, resourceContext, Type);

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb174369(v=vs.85).aspx
	https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdDrawIndexed.html
	*/
	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].drawIndexed(min(realWindow.mDeviceState.mIndexBuffer->mSize, ConvertPrimitiveCountToVertexCount(Type, PrimitiveCount)), 1, StartIndex, BaseVertexIndex, 0);
}

void RenderManager::DrawPrimitive(RealWindow& realWindow, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	if (!realWindow.mIsSceneStarted)
	{
		this->StartScene(realWindow);
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(this);
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(this);

	BeginDraw(realWindow,context, resourceContext, PrimitiveType);

	realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer].draw(min(realWindow.mVertexCount, ConvertPrimitiveCountToVertexCount(PrimitiveType, PrimitiveCount)), 1, StartVertex, 0);
}

void RenderManager::UpdateTexture(RealWindow& realWindow, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	if (pSourceTexture == nullptr || pDestinationTexture == nullptr)
	{
		return;
	}

	vk::CommandBuffer commandBuffer;
	vk::Result result;

	vk::CommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.commandPool = realWindow.mCommandPool;
	commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferInfo.commandBufferCount = 1;

	result = realWindow.mRealDevice.mDevice.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::UpdateTexture vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
	commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	//commandBufferInheritanceInfo.queryFlags = 0;
	//commandBufferInheritanceInfo.pipelineStatistics = 0;

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	//commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = commandBuffer.begin(&commandBufferBeginInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::UpdateTexture vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	//TODO: Handle dirty regions and multiple mip levels.

	if (pSourceTexture->GetType() != D3DRTYPE_CUBETEXTURE)
	{
		CTexture9& source = (*(CTexture9*)pSourceTexture);
		CTexture9& target = (*(CTexture9*)pDestinationTexture);

		ReallySetImageLayout(commandBuffer, source.mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1);
		ReallySetImageLayout(commandBuffer, target.mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, 0, 1);
		ReallyCopyImage(commandBuffer, source.mImage, target.mImage, 0, 0, source.mWidth, source.mHeight, 0, 0, 0, 0);
		ReallySetImageLayout(commandBuffer, target.mImage, vk::ImageAspectFlags(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 0, 1);
	}
	else
	{
		CCubeTexture9& source = (*(CCubeTexture9*)pSourceTexture);
		CCubeTexture9& target = (*(CCubeTexture9*)pDestinationTexture);

		ReallySetImageLayout(commandBuffer, source.mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 6);
		ReallySetImageLayout(commandBuffer, target.mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, 0, 6);
		ReallyCopyImage(commandBuffer, source.mImage, target.mImage, 0, 0, source.mEdgeLength, source.mEdgeLength, 0, 0, 0, 0);
		ReallySetImageLayout(commandBuffer, target.mImage, vk::ImageAspectFlags(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 0, 6);
	}

	commandBuffer.end();

	vk::CommandBuffer commandBuffers[] = { commandBuffer };
	vk::Fence nullFence;

	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	result = realWindow.mQueue.submit(1, &submitInfo, nullFence);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::UpdateTexture vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realWindow.mQueue.waitIdle();
	realWindow.mRealDevice.mDevice.freeCommandBuffers(realWindow.mCommandPool, 1, commandBuffers);
}

void RenderManager::BeginDraw(RealWindow& realWindow, std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;
	boost::container::flat_map<D3DRENDERSTATETYPE, DWORD>::const_iterator searchResult;

	auto& currentSwapChainBuffer = realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer];

	/**********************************************
	* Update the stuff that need to be done outside of a render pass.
	**********************************************/
	if (realWindow.mDeviceState.mAreLightsDirty || realWindow.mDeviceState.mIsMaterialDirty)
	{
		currentSwapChainBuffer.endRenderPass();
		UpdateBuffer(realWindow);
		currentSwapChainBuffer.beginRenderPass(&realWindow.mRenderPassBeginInfo, vk::SubpassContents::eInline);
	}

	/**********************************************
	* Update the textures that are currently mapped.
	**********************************************/
	auto& deviceState = realWindow.mDeviceState;
	auto& samplerStates = deviceState.mSamplerStates;

	BOOST_FOREACH(const auto& pair1, deviceState.mTextures)
	{
		vk::DescriptorImageInfo& targetSampler = deviceState.mDescriptorImageInfo[pair1.first];

		if (pair1.second != nullptr)
		{
			std::shared_ptr<SamplerRequest> request = std::make_shared<SamplerRequest>(&realWindow);
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


			for (size_t i = 0; i < realWindow.mSamplerRequests.size(); i++)
			{
				auto& storedRequest = realWindow.mSamplerRequests[i];
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
					request->mRealWindow = nullptr; //Not owner.
					storedRequest->LastUsed = std::chrono::steady_clock::now();
				}
			}

			if (request->Sampler == nullptr)
			{
				CreateSampler(realWindow,request);
			}

			targetSampler.sampler = request->Sampler;
			targetSampler.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		}
		else
		{
			targetSampler.sampler = realWindow.mSampler;
			targetSampler.imageView = realWindow.mImageView;
			targetSampler.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		}
	}

	/**********************************************
	* Setup context.
	**********************************************/
	context->PrimitiveType = type;

	if (deviceState.mHasVertexDeclaration)
	{
		context->VertexDeclaration = deviceState.mVertexDeclaration;
	}
	else if (deviceState.mHasFVF)
	{
		context->FVF = deviceState.mFVF;
	}

	//TODO: revisit if it's valid to have declaration or FVF with either shader type.

	if (deviceState.mHasVertexShader)
	{
		context->VertexShader = deviceState.mVertexShader; //vert	
	}

	if (deviceState.mHasPixelShader)
	{
		context->PixelShader = deviceState.mPixelShader; //pixel		
	}

	if (deviceState.mVertexShader != nullptr)
	{
		context->mVertexShaderConstantSlots = deviceState.mVertexShaderConstantSlots;
		resourceContext->WasShader = true;
	}

	if (deviceState.mPixelShader != nullptr)
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
	BOOST_FOREACH(auto& source, deviceState.mStreamSources)
	{
		realWindow.mVertexInputBindingDescription[i].binding = source.first;
		realWindow.mVertexInputBindingDescription[i].stride = source.second.Stride;
		realWindow.mVertexInputBindingDescription[i].inputRate = vk::VertexInputRate::eVertex;

		context->Bindings[source.first] = source.second.Stride;

		i++;
	}

	/**********************************************
	* Check for existing pipeline. Create one if there isn't a matching one.
	**********************************************/

	for (size_t i = 0; i < realWindow.mDrawBuffer.size(); i++)
	{
		auto& drawBuffer = (*realWindow.mDrawBuffer[i]);

		if (drawBuffer.PrimitiveType == context->PrimitiveType
			&& drawBuffer.StreamCount == context->StreamCount

			&& drawBuffer.VertexShader == context->VertexShader
			&& drawBuffer.PixelShader == context->PixelShader

			&& drawBuffer.FVF == context->FVF
			&& drawBuffer.VertexDeclaration == context->VertexDeclaration

			&& !memcmp(&drawBuffer.mSpecializationConstants, &constants, sizeof(SpecializationConstants))
			&& !memcmp(&drawBuffer.mVertexShaderConstantSlots, &vertexSlots, sizeof(ShaderConstantSlots))
			&& !memcmp(&drawBuffer.mPixelShaderConstantSlots, &pixelSlots, sizeof(ShaderConstantSlots))
			)
		{
			if (!memcmp(&drawBuffer.Bindings, &context->Bindings, 64 * sizeof(UINT)))
			{
				context->Pipeline = drawBuffer.Pipeline;
				context->PipelineLayout = drawBuffer.PipelineLayout;
				context->DescriptorSetLayout = drawBuffer.DescriptorSetLayout;
				context->mRealWindow = nullptr; //Not owner.
				drawBuffer.LastUsed = std::chrono::steady_clock::now();
			}
		}
	}

	if (context->Pipeline == nullptr)
	{
		CreatePipe(realWindow,context); //If we didn't find a matching pipeline then create a new one.	
	}

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb205599(v=vs.85).aspx
	The units for the D3DRS_DEPTHBIAS and D3DRS_SLOPESCALEDEPTHBIAS render states depend on whether z-buffering or w-buffering is enabled.
	The bias is not applied to any line and point primitive.
	*/
	if (constants.zEnable != D3DZB_FALSE && type > 3)
	{
		currentSwapChainBuffer.setDepthBias(constants.depthBias, 0.0f, constants.slopeScaleDepthBias);
	}
	else
	{
		currentSwapChainBuffer.setDepthBias(0.0f, 0.0f, 0.0f);
	}

	/**********************************************
	* Update transformation structure.
	**********************************************/
	if (context->VertexShader == nullptr)
	{
		UpdatePushConstants(realWindow,context);
	}
	else
	{
		currentSwapChainBuffer.pushConstants(context->PipelineLayout, vk::ShaderStageFlagBits::eAllGraphics, 0, UBO_SIZE * 2, &deviceState.mPushConstants);
	}

	/**********************************************
	* Check for existing DescriptorSet. Create one if there isn't a matching one.
	**********************************************/

	if (context->DescriptorSetLayout != VK_NULL_HANDLE)
	{
		std::copy(std::begin(deviceState.mDescriptorImageInfo), std::end(deviceState.mDescriptorImageInfo), std::begin(resourceContext->DescriptorImageInfo));

		if (context->VertexShader == nullptr)
		{
			realWindow.mDescriptorBufferInfo[0].buffer = realWindow.mLightBuffer;
			realWindow.mDescriptorBufferInfo[0].offset = 0;
			realWindow.mDescriptorBufferInfo[0].range = sizeof(Light) * deviceState.mLights.size(); //4; 

			realWindow.mDescriptorBufferInfo[1].buffer = realWindow.mMaterialBuffer;
			realWindow.mDescriptorBufferInfo[1].offset = 0;
			realWindow.mDescriptorBufferInfo[1].range = sizeof(D3DMATERIAL9);

			realWindow.mWriteDescriptorSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
			realWindow.mWriteDescriptorSet[0].dstSet = resourceContext->DescriptorSet;
			realWindow.mWriteDescriptorSet[0].descriptorCount = 1;
			realWindow.mWriteDescriptorSet[0].pBufferInfo = &realWindow.mDescriptorBufferInfo[0];

			realWindow.mWriteDescriptorSet[1].dstSet = resourceContext->DescriptorSet;
			realWindow.mWriteDescriptorSet[1].descriptorCount = 1;
			realWindow.mWriteDescriptorSet[1].pBufferInfo = &realWindow.mDescriptorBufferInfo[1];

			realWindow.mWriteDescriptorSet[2].dstSet = resourceContext->DescriptorSet;
			realWindow.mWriteDescriptorSet[2].descriptorCount = deviceState.mTextures.size();
			realWindow.mWriteDescriptorSet[2].pImageInfo = resourceContext->DescriptorImageInfo;

			if (deviceState.mTextures.size())
			{
				currentSwapChainBuffer.pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, context->PipelineLayout, 0, 3, realWindow.mWriteDescriptorSet);
			}
			else
			{
				currentSwapChainBuffer.pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, context->PipelineLayout, 0, 2, realWindow.mWriteDescriptorSet);
			}
		}
		else
		{
			realWindow.mWriteDescriptorSet[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			realWindow.mWriteDescriptorSet[0].dstSet = resourceContext->DescriptorSet;
			realWindow.mWriteDescriptorSet[0].descriptorCount = deviceState.mTextures.size(); //Revisit
			realWindow.mWriteDescriptorSet[0].pImageInfo = resourceContext->DescriptorImageInfo;

			currentSwapChainBuffer.pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, context->PipelineLayout, 0, 1, realWindow.mWriteDescriptorSet);
		}
	}

	/**********************************************
	* Setup bindings
	**********************************************/

	//TODO: I need to find a way to prevent binding on every draw call.

	//if (!mIsDirty || mLastVkPipeline != context->Pipeline)
	//{
	currentSwapChainBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, context->Pipeline);
	//	mLastVkPipeline = context->Pipeline;
	//}

	realWindow.mVertexCount = 0;

	if (deviceState.mIndexBuffer != nullptr)
	{
		currentSwapChainBuffer.bindIndexBuffer(deviceState.mIndexBuffer->mBuffer, 0, deviceState.mIndexBuffer->mIndexType);
	}

	BOOST_FOREACH(auto& source, deviceState.mStreamSources)
	{
		currentSwapChainBuffer.bindVertexBuffers(source.first, 1, &source.second.StreamData->mBuffer, &source.second.OffsetInBytes);
		realWindow.mVertexCount += source.second.StreamData->mSize;
	}

	realWindow.mIsDirty = false;
}

void RenderManager::CreatePipe(RealWindow& realWindow, std::shared_ptr<DrawContext> context)
{
	vk::Result result;
	auto& deviceState = realWindow.mDeviceState;
	auto& device = realWindow.mRealDevice.mDevice;

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
			BOOST_LOG_TRIVIAL(warning) << "RenderManager::CreatePipe D3DFVF_PSIZE is not implemented!";
		}

		if ((context->FVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
		{
			hasColor = true;
		}

		if ((context->FVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
		{
			BOOST_LOG_TRIVIAL(warning) << "RenderManager::CreatePipe D3DFVF_SPECULAR is not implemented!";
		}

		textureCount = ConvertFormat(context->FVF);
	}
	else if (context->VertexShader != nullptr)
	{
		//Nothing so far.
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported layout definition.";
	}

	attributeCount += hasColor;
	attributeCount += hasPosition;
	attributeCount += hasNormal;
	attributeCount += textureCount;

	/**********************************************
	* Figure out render states & texture states
	**********************************************/
	realWindow.mPipelineColorBlendAttachmentState[0].colorWriteMask = (vk::ColorComponentFlagBits)constants.colorWriteEnable;
	realWindow.mPipelineColorBlendAttachmentState[0].blendEnable = constants.alphaBlendEnable;

	realWindow.mPipelineColorBlendAttachmentState[0].colorBlendOp = ConvertColorOperation(constants.blendOperation);
	realWindow.mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = ConvertColorFactor(constants.sourceBlend);
	realWindow.mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = ConvertColorFactor(constants.destinationBlend);

	realWindow.mPipelineColorBlendAttachmentState[0].alphaBlendOp = ConvertColorOperation(constants.blendOperationAlpha);
	realWindow.mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = ConvertColorFactor(constants.sourceBlendAlpha);
	realWindow.mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = ConvertColorFactor(constants.destinationBlendAlpha);

	SetCulling(realWindow.mPipelineRasterizationStateCreateInfo, (D3DCULL)constants.cullMode);
	realWindow.mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode((D3DFILLMODE)constants.fillMode);
	realWindow.mPipelineInputAssemblyStateCreateInfo.topology = ConvertPrimitiveType(context->PrimitiveType);

	realWindow.mPipelineDepthStencilStateCreateInfo.depthTestEnable = constants.zEnable; //= VK_TRUE;
	realWindow.mPipelineDepthStencilStateCreateInfo.depthWriteEnable = constants.zWriteEnable; //VK_TRUE;
	realWindow.mPipelineDepthStencilStateCreateInfo.depthCompareOp = ConvertCompareOperation(constants.zFunction);  //VK_COMPARE_OP_LESS_OR_EQUAL;
	//realWindow.mPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = constants.bound
	realWindow.mPipelineDepthStencilStateCreateInfo.stencilTestEnable = constants.stencilEnable; //VK_FALSE;

	if (constants.cullMode != D3DCULL_CCW)
	{
		realWindow.mPipelineDepthStencilStateCreateInfo.back.failOp = ConvertStencilOperation(constants.ccwStencilFail);
		realWindow.mPipelineDepthStencilStateCreateInfo.back.passOp = ConvertStencilOperation(constants.ccwStencilPass);
		realWindow.mPipelineDepthStencilStateCreateInfo.back.compareOp = ConvertCompareOperation(constants.ccwStencilFunction);

		realWindow.mPipelineDepthStencilStateCreateInfo.front.failOp = ConvertStencilOperation(constants.stencilFail);
		realWindow.mPipelineDepthStencilStateCreateInfo.front.passOp = ConvertStencilOperation(constants.stencilPass);
		realWindow.mPipelineDepthStencilStateCreateInfo.front.compareOp = ConvertCompareOperation(constants.stencilFunction);
	}
	else
	{
		realWindow.mPipelineDepthStencilStateCreateInfo.back.failOp = ConvertStencilOperation(constants.stencilFail);
		realWindow.mPipelineDepthStencilStateCreateInfo.back.passOp = ConvertStencilOperation(constants.stencilPass);
		realWindow.mPipelineDepthStencilStateCreateInfo.back.compareOp = ConvertCompareOperation(constants.stencilFunction);

		realWindow.mPipelineDepthStencilStateCreateInfo.front.failOp = ConvertStencilOperation(constants.ccwStencilFail);
		realWindow.mPipelineDepthStencilStateCreateInfo.front.passOp = ConvertStencilOperation(constants.ccwStencilPass);
		realWindow.mPipelineDepthStencilStateCreateInfo.front.compareOp = ConvertCompareOperation(constants.ccwStencilFunction);
	}


	//mPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	//mPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

	/**********************************************
	* Figure out correct shader
	**********************************************/
	if (context->VertexShader != nullptr)
	{
		realWindow.mPipelineShaderStageCreateInfo[0].module = context->VertexShader->mShaderConverter.mConvertedShader.ShaderModule;
		realWindow.mPipelineShaderStageCreateInfo[1].module = context->PixelShader->mShaderConverter.mConvertedShader.ShaderModule;
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
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_TEX1;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_TEX1;
				break;
			case 2:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_TEX2;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_TEX2;
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else if (hasPosition && hasColor && !hasNormal)
		{
			switch (textureCount)
			{
			case 0:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_DIFFUSE;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_DIFFUSE;
				break;
			case 1:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_DIFFUSE_TEX1;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_DIFFUSE_TEX1;
				break;
			case 2:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_DIFFUSE_TEX2;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_DIFFUSE_TEX2;
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else if (hasPosition && hasColor && hasNormal)
		{
			switch (textureCount)
			{
			case 2:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2;
				break;
			case 0:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_NORMAL_DIFFUSE;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_NORMAL_DIFFUSE;
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else if (hasPosition && !hasColor && hasNormal)
		{
			switch (textureCount)
			{
			case 0:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_NORMAL;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_NORMAL;
				break;
			case 1:
				realWindow.mPipelineShaderStageCreateInfo[0].module = realWindow.mVertShaderModule_XYZ_NORMAL_TEX1;
				realWindow.mPipelineShaderStageCreateInfo[1].module = realWindow.mFragShaderModule_XYZ_NORMAL_TEX1;
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported layout.";
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

			realWindow.mVertexInputAttributeDescription[i].binding = element.Stream;
			//realWindow.mVertexInputAttributeDescription[i].location = location;
			realWindow.mVertexInputAttributeDescription[i].format = ConvertDeclType((D3DDECLTYPE)element.Type);
			realWindow.mVertexInputAttributeDescription[i].offset = element.Offset;

			switch ((D3DDECLUSAGE)element.Usage)
			{
			case D3DDECLUSAGE_POSITION:
				realWindow.mVertexInputAttributeDescription[i].location = 0;
				break;
			case D3DDECLUSAGE_BLENDWEIGHT:
				break;
			case D3DDECLUSAGE_BLENDINDICES:
				break;
			case D3DDECLUSAGE_NORMAL:
				realWindow.mVertexInputAttributeDescription[i].location = hasPosition;
				break;
			case D3DDECLUSAGE_PSIZE:
				break;
			case D3DDECLUSAGE_TEXCOORD:
				realWindow.mVertexInputAttributeDescription[i].location = hasPosition + hasNormal + hasColor + textureIndex;
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
				realWindow.mVertexInputAttributeDescription[i].location = hasPosition + hasNormal;
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
				realWindow.mVertexInputAttributeDescription[attributeIndex].binding = i;
				realWindow.mVertexInputAttributeDescription[attributeIndex].location = location;
				realWindow.mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eR32G32B32Sfloat;
				realWindow.mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 3);
				location += 1;
				attributeIndex += 1;
			}

			if (hasNormal)
			{
				realWindow.mVertexInputAttributeDescription[attributeIndex].binding = i;
				realWindow.mVertexInputAttributeDescription[attributeIndex].location = location;
				realWindow.mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eR32G32B32Sfloat;
				realWindow.mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 3);
				location += 1;
				attributeIndex += 1;
			}

			//D3DFVF_PSIZE
			if ((context->FVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
			{
				realWindow.mVertexInputAttributeDescription[attributeIndex].binding = i;
				realWindow.mVertexInputAttributeDescription[attributeIndex].location = location;
				realWindow.mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eB8G8R8A8Uint;
				realWindow.mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += sizeof(uint32_t);
				location += 1;
				attributeIndex += 1;
			}

			if ((context->FVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
			{
				realWindow.mVertexInputAttributeDescription[attributeIndex].binding = i;
				realWindow.mVertexInputAttributeDescription[attributeIndex].location = location;
				realWindow.mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eB8G8R8A8Uint;
				realWindow.mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += sizeof(uint32_t);
				location += 1;
				attributeIndex += 1;
			}

			for (size_t j = 0; j < textureCount; j++)
			{
				realWindow.mVertexInputAttributeDescription[attributeIndex].binding = i;
				realWindow.mVertexInputAttributeDescription[attributeIndex].location = location;
				realWindow.mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eR32G32Sfloat;
				realWindow.mVertexInputAttributeDescription[attributeIndex].offset = offset;
				offset += (sizeof(float) * 2);
				location += 1;
				attributeIndex += 1;
			}
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw unknown vertex format.";
	}

	realWindow.mPipelineLayoutCreateInfo.pPushConstantRanges = realWindow.mPushConstantRanges;
	realWindow.mPipelineLayoutCreateInfo.pushConstantRangeCount = 1;

	if (context->VertexShader != nullptr)
	{
		auto& convertedVertexShader = context->VertexShader->mShaderConverter.mConvertedShader;
		auto& convertedPixelShader = context->PixelShader->mShaderConverter.mConvertedShader;

		realWindow.mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = context->StreamCount;
		realWindow.mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount;

		memcpy(&realWindow.mDescriptorSetLayoutBinding, &convertedPixelShader.mDescriptorSetLayoutBinding, sizeof(realWindow.mDescriptorSetLayoutBinding));

		realWindow.mDescriptorSetLayoutCreateInfo.pBindings = realWindow.mDescriptorSetLayoutBinding;
		realWindow.mPipelineLayoutCreateInfo.pSetLayouts = &context->DescriptorSetLayout;

		realWindow.mDescriptorSetLayoutCreateInfo.bindingCount = convertedPixelShader.mDescriptorSetLayoutBindingCount;
		realWindow.mPipelineLayoutCreateInfo.setLayoutCount = 1;

		realWindow.mVertexSpecializationInfo.pData = &context->mVertexShaderConstantSlots;
		realWindow.mVertexSpecializationInfo.dataSize = sizeof(ShaderConstantSlots);
		realWindow.mVertexSpecializationInfo.pMapEntries = realWindow.mSlotMapEntries;
		realWindow.mVertexSpecializationInfo.mapEntryCount = 1024;

		realWindow.mPixelSpecializationInfo.pData = &context->mPixelShaderConstantSlots;
		realWindow.mPixelSpecializationInfo.dataSize = sizeof(ShaderConstantSlots);
		realWindow.mPixelSpecializationInfo.pMapEntries = realWindow.mSlotMapEntries;
		realWindow.mPixelSpecializationInfo.mapEntryCount = 1024;
	}
	else
	{
		realWindow.mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = context->StreamCount;
		realWindow.mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount;

		realWindow.mDescriptorSetLayoutBinding[0].binding = 0;
		realWindow.mDescriptorSetLayoutBinding[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		realWindow.mDescriptorSetLayoutBinding[0].descriptorCount = 1;
		realWindow.mDescriptorSetLayoutBinding[0].stageFlags = vk::ShaderStageFlagBits::eAllGraphics;
		realWindow.mDescriptorSetLayoutBinding[0].pImmutableSamplers = nullptr;

		realWindow.mDescriptorSetLayoutBinding[1].binding = 1;
		realWindow.mDescriptorSetLayoutBinding[1].descriptorType = vk::DescriptorType::eUniformBuffer;
		realWindow.mDescriptorSetLayoutBinding[1].descriptorCount = 1;
		realWindow.mDescriptorSetLayoutBinding[1].stageFlags = vk::ShaderStageFlagBits::eAllGraphics;
		realWindow.mDescriptorSetLayoutBinding[1].pImmutableSamplers = nullptr;

		realWindow.mDescriptorSetLayoutBinding[2].binding = 2;
		realWindow.mDescriptorSetLayoutBinding[2].descriptorType = vk::DescriptorType::eCombinedImageSampler; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER'
		realWindow.mDescriptorSetLayoutBinding[2].descriptorCount = textureCount; //Update to use mapped texture.
		realWindow.mDescriptorSetLayoutBinding[2].stageFlags = vk::ShaderStageFlagBits::eFragment;
		realWindow.mDescriptorSetLayoutBinding[2].pImmutableSamplers = nullptr;

		realWindow.mDescriptorSetLayoutCreateInfo.pBindings = realWindow.mDescriptorSetLayoutBinding;
		realWindow.mPipelineLayoutCreateInfo.pSetLayouts = &context->DescriptorSetLayout;

		if (textureCount)
		{
			realWindow.mDescriptorSetLayoutCreateInfo.bindingCount = 3; //The number of elements in pBindings.	
			realWindow.mPipelineLayoutCreateInfo.setLayoutCount = 1;
		}
		else
		{
			realWindow.mDescriptorSetLayoutCreateInfo.bindingCount = 2; //The number of elements in pBindings.	
			realWindow.mPipelineLayoutCreateInfo.setLayoutCount = 1;
		}

		realWindow.mVertexSpecializationInfo.pData = &deviceState.mSpecializationConstants;
		realWindow.mVertexSpecializationInfo.dataSize = sizeof(SpecializationConstants);
		realWindow.mVertexSpecializationInfo.pMapEntries = realWindow.mSlotMapEntries;
		realWindow.mVertexSpecializationInfo.mapEntryCount = 251;

		realWindow.mPixelSpecializationInfo.pData = &deviceState.mSpecializationConstants;
		realWindow.mPixelSpecializationInfo.dataSize = sizeof(SpecializationConstants);
		realWindow.mPixelSpecializationInfo.pMapEntries = realWindow.mSlotMapEntries;
		realWindow.mPixelSpecializationInfo.mapEntryCount = 251;
	}

	result = device.createDescriptorSetLayout(&realWindow.mDescriptorSetLayoutCreateInfo, nullptr, &context->DescriptorSetLayout);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreateDescriptorSet vkCreateDescriptorSetLayout failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	/**********************************************
	* Create pipeline & descriptor set layout.
	**********************************************/

	result = device.createPipelineLayout(&realWindow.mPipelineLayoutCreateInfo, nullptr, &context->PipelineLayout);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw vkCreatePipelineLayout failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realWindow.mGraphicsPipelineCreateInfo.layout = context->PipelineLayout;

	result = device.createGraphicsPipelines(realWindow.mPipelineCache, 1, &realWindow.mGraphicsPipelineCreateInfo, nullptr, &context->Pipeline);
	//result = vkCreateGraphicsPipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw vkCreateGraphicsPipelines failed with return code of " << GetResultString((VkResult)result);
	}

	realWindow.mDrawBuffer.push_back(context);
}

void RenderManager::CreateSampler(RealWindow& realWindow, std::shared_ptr<SamplerRequest> request)
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb172602(v=vs.85).aspx
	//Mipmap filter to use during minification. See D3DTEXTUREFILTERTYPE. The default value is D3DTEXF_NONE.

	vk::Result result;
	//auto& deviceState = realWindow.mDeviceState;
	auto& device = realWindow.mRealDevice.mDevice;

	vk::SamplerCreateInfo samplerCreateInfo;
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
	if (realWindow.mRealDevice.mPhysicalDeviceFeatures.samplerAnisotropy && samplerCreateInfo.minFilter != vk::Filter::eCubicIMG && samplerCreateInfo.magFilter != vk::Filter::eCubicIMG)
	{
		// Use max. level of anisotropy for this example
		samplerCreateInfo.maxAnisotropy = min(request->MaxAnisotropy, realWindow.mRealDevice.mPhysicalDeviceProperties.limits.maxSamplerAnisotropy);

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

	samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite; // VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareOp = vk::CompareOp::eNever; //VK_COMPARE_OP_ALWAYS
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = request->MaxLod;

	if (request->MipmapMode == D3DTEXF_NONE)
	{
		samplerCreateInfo.maxLod = 0.0f;
	}

	result = device.createSampler(&samplerCreateInfo, nullptr, &request->Sampler);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::GenerateSampler vkCreateSampler failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realWindow.mSamplerRequests.push_back(request);
}

void RenderManager::UpdatePushConstants(RealWindow& realWindow, std::shared_ptr<DrawContext> context)
{
	vk::Result result;
	auto& deviceState = realWindow.mDeviceState;
	//auto& device = realWindow.mRealDevice.mDevice;
	auto& currentSwapChainBuffer = realWindow.mSwapchainBuffers[realWindow.mCurrentSwapchainBuffer];
	void* data = nullptr;

	//if (!mDevice->mDeviceState.mHasTransformsChanged)
	//{
	//	return;
	//}

	realWindow.mTransformations.mModel <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	realWindow.mTransformations.mView <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	realWindow.mTransformations.mProjection <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	BOOST_FOREACH(const auto& pair1, deviceState.mTransforms)
	{
		switch (pair1.first)
		{
		case D3DTS_WORLD:

			realWindow.mTransformations.mModel <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_VIEW:

			realWindow.mTransformations.mView <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_PROJECTION:

			realWindow.mTransformations.mProjection <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "RenderManager::UpdateUniformBuffer The following state type was ignored. " << pair1.first;
			break;
		}
	}

	realWindow.mTransformations.mTotalTransformation = realWindow.mTransformations.mProjection * realWindow.mTransformations.mView * realWindow.mTransformations.mModel;
	//mTotalTransformation = mModel * mView * mProjection;

	currentSwapChainBuffer.pushConstants(context->PipelineLayout, vk::ShaderStageFlagBits::eAllGraphics, 0, UBO_SIZE * 2, &realWindow.mTransformations);
}

void RenderManager::FlushDrawBufffer(RealWindow& realWindow)
{
	/*
	Uses remove_if and chrono to remove elements that have not been used in over a second.
	*/
	realWindow.mDrawBuffer.erase(std::remove_if(realWindow.mDrawBuffer.begin(), realWindow.mDrawBuffer.end(), [](const std::shared_ptr<DrawContext> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), realWindow.mDrawBuffer.end());
	realWindow.mSamplerRequests.erase(std::remove_if(realWindow.mSamplerRequests.begin(), realWindow.mSamplerRequests.end(), [](const std::shared_ptr<SamplerRequest> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), realWindow.mSamplerRequests.end());

	realWindow.mIsDirty = true;
}