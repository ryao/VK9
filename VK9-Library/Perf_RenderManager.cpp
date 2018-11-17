// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

/*
"Whoever pursues righteousness and kindness will find life, righteousness, and honor." (Proverbs 21:21, ESV)
*/

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

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
#include "CTypes.h"

#include "CCubeTexture9.h"
#include "CVolumeTexture9.h"
#include "CBaseTexture9.h"
#include "CTexture9.h"
#include "CIndexBuffer9.h"
#include "CVertexBuffer9.h"
#include "CVertexDeclaration9.h"
#include "CPixelShader9.h"
#include "CVertexShader9.h"

RenderManager::RenderManager(boost::program_options::variables_map& options)
	: mOptions(options), mStateManager(mOptions)
{

}

RenderManager::~RenderManager()
{
}

void RenderManager::UpdateBuffer(std::shared_ptr<RealDevice> realDevice, std::shared_ptr<DrawContext> context)
{ //Vulkan doesn't allow vkCmdUpdateBuffer inside of a render pass.

	auto& device = realDevice->mDevice;
	auto& deviceState = realDevice->mDeviceState;

	if (!deviceState.mRenderTarget->mIsSceneStarted)
	{
		this->StartScene(realDevice, false, false, false);
	}

	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];
	//context

	vk::BufferMemoryBarrier uboBarrier;
	uboBarrier.offset = 0;


	//FF Buffers
	if (deviceState.mIsRenderStateDirty)
	{
		uboBarrier.buffer = realDevice->mRenderStateBuffer;
		uboBarrier.size = sizeof(RenderState);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		currentBuffer.updateBuffer(realDevice->mRenderStateBuffer, 0, uboBarrier.size, &deviceState.mShaderState.mRenderState);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		deviceState.mIsRenderStateDirty = false;
	}

	if (deviceState.mAreTextureStagesDirty)
	{
		uboBarrier.buffer = realDevice->mTextureStageBuffer;
		uboBarrier.size = sizeof(TextureStage) * 9;

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		currentBuffer.updateBuffer(realDevice->mTextureStageBuffer, 0, uboBarrier.size, &deviceState.mShaderState.mTextureStages);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		deviceState.mAreTextureStagesDirty = false;
	}

	if (deviceState.mAreLightsDirty)
	{
		uboBarrier.buffer = realDevice->mLightBuffer;
		uboBarrier.size = sizeof(Light) * 8;

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		currentBuffer.updateBuffer(realDevice->mLightBuffer, 0, uboBarrier.size, &deviceState.mShaderState.mLights);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		deviceState.mAreLightsDirty = false;
	}

	if (deviceState.mIsMaterialDirty)
	{
		uboBarrier.buffer = realDevice->mMaterialBuffer;
		uboBarrier.size = sizeof(D3DMATERIAL9);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		currentBuffer.updateBuffer(realDevice->mMaterialBuffer, 0, uboBarrier.size, &deviceState.mShaderState.mMaterial);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		deviceState.mIsMaterialDirty = false;
	}


	//Shader Buffers
	if (deviceState.mAreVertexShaderSlotsDirty)
	{
		uboBarrier.buffer = realDevice->mShaderVertexConstantBuffer;
		uboBarrier.size = sizeof(ShaderConstantSlots);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		currentBuffer.updateBuffer(realDevice->mShaderVertexConstantBuffer, 0, uboBarrier.size, &deviceState.mVertexShaderConstantSlots);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		deviceState.mAreVertexShaderSlotsDirty = false;
	}

	if (deviceState.mArePixelShaderSlotsDirty)
	{
		uboBarrier.buffer = realDevice->mShaderPixelConstantBuffer;
		uboBarrier.size = sizeof(ShaderConstantSlots);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		currentBuffer.updateBuffer(realDevice->mShaderPixelConstantBuffer, 0, uboBarrier.size, &deviceState.mPixelShaderConstantSlots);

		//uboBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
		//uboBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);

		deviceState.mArePixelShaderSlotsDirty = false;
	}
}

void RenderManager::StartScene(std::shared_ptr<RealDevice> realDevice, bool clearColor, bool clearDepth, bool clearStencil)
{
	auto& device = realDevice->mDevice;
	auto& deviceState = realDevice->mDeviceState;
	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];

	realDevice->mDeviceState.mRenderTarget->StartScene(currentBuffer, deviceState, clearColor, clearDepth, clearStencil, deviceState.hasPresented);
	deviceState.hasPresented = false;
}

void RenderManager::StopScene(std::shared_ptr<RealDevice> realDevice)
{
	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];

	realDevice->mDeviceState.mRenderTarget->StopScene(currentBuffer, realDevice->mQueue);
}

void RenderManager::CopyImage(std::shared_ptr<RealDevice> realDevice, vk::Image srcImage, vk::Image dstImage, int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t depth, uint32_t srcMip, uint32_t dstMip)
{
	vk::Result result;
	vk::CommandBuffer commandBuffer;
	auto& device = realDevice->mDevice;

	vk::CommandBufferAllocateInfo commandBufferInfo;
	commandBufferInfo.commandPool = realDevice->mCommandPool;
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
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = commandBuffer.begin(&commandBufferBeginInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CopyImage vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	ReallyCopyImage(commandBuffer, srcImage, dstImage, x, y, width, height, depth, srcMip, dstMip, 0, 0);

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

	result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CopyImage vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realDevice->mQueue.waitIdle();
	device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
}

void RenderManager::Clear(std::shared_ptr<RealDevice> realDevice, DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];
	auto& deviceState = realDevice->mDeviceState;

	realDevice->mDeviceState.mRenderTarget->Clear(currentBuffer, deviceState, Count, pRects, Flags, Color, Z, Stencil);
}

vk::Result RenderManager::Present(std::shared_ptr<RealDevice> realDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (!realDevice->mDeviceState.mRenderTarget->mIsSceneStarted)
	{
		this->StartScene(realDevice, false, false, false);
	}
	this->StopScene(realDevice);

	//vk::Result result;
	auto& device = realDevice->mDevice;
	auto& deviceState = realDevice->mDeviceState;
	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];
	auto swapchain = mStateManager.GetSwapChain(realDevice, hDestWindowOverride, 0, 0, realDevice->mUseVsync);

	auto colorSurface = deviceState.mRenderTarget->mColorSurface;

	vk::Result result = swapchain->Present(currentBuffer, realDevice->mQueue, colorSurface->mStagingImage, colorSurface->mExtent.width, colorSurface->mExtent.height);
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		mStateManager.mSwapChains.erase(hDestWindowOverride);
	}
	deviceState.hasPresented = true;
	realDevice->mCurrentCommandBuffer = (realDevice->mCurrentCommandBuffer + 1) % MAXFRAMECOMMANDBUFFERS;

	//Clean up pipes.
	FlushDrawBufffer(realDevice);

	//Clean up unreferenced resources.
	//mGarbageManager.DestroyHandles();

	if (pSourceRect != nullptr || pDestRect != nullptr)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::Present RECT not supported.";
	}

	if (pDirtyRegion != nullptr)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::Present RGNDATA not supported.";
	}

	//Print(mDeviceState.mTransforms);

	return result;
}

void RenderManager::DrawIndexedPrimitive(std::shared_ptr<RealDevice> realDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	auto& device = realDevice->mDevice;
	auto& deviceState = realDevice->mDeviceState;
	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];

	if (deviceState.mIndexBuffer == nullptr)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitive called with null index buffer.";
		return;
	}

	if (!realDevice->mDeviceState.mRenderTarget->mIsSceneStarted)
	{
		this->StartScene(realDevice, false, false, false);
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(realDevice.get());
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(realDevice.get());

	BeginDraw(realDevice, context, resourceContext, Type);

	//vk::MemoryBarrier globalBarrier(vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eDepthStencilAttachmentRead);
	//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllGraphics, vk::DependencyFlags(), 1, &globalBarrier, 0, nullptr, 0, nullptr);

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb174369(v=vs.85).aspx
	https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdDrawIndexed.html
	*/
	currentBuffer.drawIndexed(ConvertPrimitiveCountToVertexCount(Type, PrimitiveCount), 1, StartIndex, BaseVertexIndex, 0);
}

void RenderManager::DrawPrimitive(std::shared_ptr<RealDevice> realDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	auto& device = realDevice->mDevice;
	auto& deviceState = realDevice->mDeviceState;
	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];

	if (!realDevice->mDeviceState.mRenderTarget->mIsSceneStarted)
	{
		this->StartScene(realDevice, false, false, false);
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(realDevice.get());
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(realDevice.get());

	BeginDraw(realDevice, context, resourceContext, PrimitiveType);

	//vk::MemoryBarrier globalBarrier(vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eDepthStencilAttachmentRead);
	//currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllGraphics, vk::DependencyFlags(), 1, &globalBarrier, 0, nullptr, 0, nullptr);

	currentBuffer.draw(ConvertPrimitiveCountToVertexCount(PrimitiveType, PrimitiveCount), 1, StartVertex, 0);
}

void RenderManager::UpdateTexture(std::shared_ptr<RealDevice> realDevice, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	if (pSourceTexture == nullptr || pDestinationTexture == nullptr)
	{
		return;
	}

	auto& device = realDevice->mDevice;

	vk::CommandBuffer commandBuffer;
	vk::Result result;

	vk::CommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.commandPool = realDevice->mCommandPool;
	commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferInfo.commandBufferCount = 1;

	result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::UpdateTexture vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
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
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = commandBuffer.begin(&commandBufferBeginInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::UpdateTexture vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	std::shared_ptr<RealTexture> source;
	std::shared_ptr<RealTexture> target;
	uint32_t width = 0;
	uint32_t height = 0;

	if (pDestinationTexture->GetType() != D3DRTYPE_CUBETEXTURE)
	{
		CTexture9& target9 = (*(CTexture9*)pDestinationTexture);
		target = mStateManager.mTextures[target9.mId];

		ReallySetImageLayout(commandBuffer, target->mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);
	}
	else
	{
		CCubeTexture9& target9 = (*(CCubeTexture9*)pDestinationTexture);
		target = mStateManager.mTextures[target9.mId];

		ReallySetImageLayout(commandBuffer, target->mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);
	}

	if (pSourceTexture->GetType() == D3DRTYPE_CUBETEXTURE)
	{
		CCubeTexture9& source9 = (*(CCubeTexture9*)pSourceTexture);
		source = mStateManager.mTextures[source9.mId];

		ReallySetImageLayout(commandBuffer, source->mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);

		for (size_t i = 0; i < source->mLevels; i++)
		{
			ReallyCopyImage(commandBuffer, source->mImage, target->mImage, 0, 0, source9.mEdgeLength, source9.mEdgeLength, 1, i, i, 0, 0);
		}


	}
	else if (pSourceTexture->GetType() == D3DRTYPE_VOLUMETEXTURE)
	{
		CVolumeTexture9& source9 = (*(CVolumeTexture9*)pSourceTexture);
		source = mStateManager.mTextures[source9.mId];

		ReallySetImageLayout(commandBuffer, source->mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);

		for (size_t i = 0; i < source->mLevels; i++)
		{
			ReallyCopyImage(commandBuffer, source->mImage, target->mImage, 0, 0, source9.mWidth, source9.mHeight, source9.mDepth, i, i, 0, 0);
		}
	}
	else
	{
		CTexture9& source9 = (*(CTexture9*)pSourceTexture);
		source = mStateManager.mTextures[source9.mId];

		ReallySetImageLayout(commandBuffer, source->mImage, vk::ImageAspectFlags(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);

		for (size_t i = 0; i < source->mLevels; i++)
		{
			ReallyCopyImage(commandBuffer, source->mImage, target->mImage, 0, 0, source9.mWidth, source9.mHeight, 1, i, i, 0, 0);
		}
	}

	ReallySetImageLayout(commandBuffer, target->mImage, vk::ImageAspectFlags(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);

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

	result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::UpdateTexture vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realDevice->mQueue.waitIdle();
	device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
}

void RenderManager::BeginDraw(std::shared_ptr<RealDevice> realDevice, std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type)
{
	VkResult result = VK_SUCCESS;
	uint32_t textureCount = 0;
	//std::unordered_map<D3DRENDERSTATETYPE, DWORD>::const_iterator searchResult;
	auto& currentBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];

	auto& device = realDevice->mDevice;
	auto& deviceState = realDevice->mDeviceState;
	auto& deviceShaderState = deviceState.mShaderState;
	auto& deviceRenderState = deviceShaderState.mRenderState;
	auto& deviceSamplerStates = deviceState.mSamplerStates;

	auto& contextShaderState = context->mShaderState;
	auto& contextVertexSlots = context->mVertexShaderConstantSlots;
	auto& contextPixelSlots = context->mPixelShaderConstantSlots;

	if (deviceState.mHasVertexShader || deviceState.mHasPixelShader)
	{
		resourceContext->WasShader = true;
	}

	if (deviceState.mHasVertexDeclaration)
	{
		if (deviceState.mVertexDeclaration != nullptr)
		{
			textureCount = deviceState.mVertexDeclaration->mTextureCount;
		}
	}
	else if (deviceState.mHasFVF)
	{
		textureCount = ConvertFormat(deviceState.mFVF);
	}

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb205599(v=vs.85).aspx
	The units for the D3DRS_DEPTHBIAS and D3DRS_SLOPESCALEDEPTHBIAS render states depend on whether z-buffering or w-buffering is enabled.
	The bias is not applied to any line and point primitive.
	*/
	if (deviceRenderState.zEnable != D3DZB_FALSE && type > 3)
	{
		currentBuffer.setDepthBias(deviceRenderState.depthBias, 0.0f, deviceRenderState.slopeScaleDepthBias);
	}
	else
	{
		currentBuffer.setDepthBias(0.0f, 0.0f, 0.0f);
	}

	/**********************************************
	* Update the textures that are currently mapped.
	**********************************************/

	context->StreamCount = deviceState.mStreamSources.size();
	//context->Bindings = {};
	memset(&context->Bindings, 0, sizeof(UINT) * 64);

	int i = 0;
	for (auto& source : deviceState.mStreamSources)
	{
		realDevice->mVertexInputBindingDescription[i].binding = source.first;
		realDevice->mVertexInputBindingDescription[i].stride = source.second.Stride;
		realDevice->mVertexInputBindingDescription[i].inputRate = vk::VertexInputRate::eVertex;

		context->Bindings[source.first] = source.second.Stride;

		i++;
	}

	deviceRenderState.textureCount = 0;
	for (size_t i = 0; i < 16; i++)
	{
		auto& targetSampler = deviceState.mDescriptorImageInfo[i];

		if (deviceState.mTextures[i] != nullptr)
		{
			deviceRenderState.textureCount++;

			std::shared_ptr<SamplerRequest> request = std::make_shared<SamplerRequest>(realDevice.get());
			auto& currentSampler = deviceSamplerStates[request->SamplerIndex];

			if (deviceState.mTextures[i]->GetType() == D3DRTYPE_CUBETEXTURE)
			{
				CCubeTexture9* texture9 = (CCubeTexture9*)deviceState.mTextures[i];
				auto& texture = mStateManager.mTextures[texture9->mId];

				request->MaxLod = texture9->mLevels;
				targetSampler.imageView = texture->mImageView;
			}
			else
			{
				CTexture9* texture9 = (CTexture9*)deviceState.mTextures[i];
				auto& texture = mStateManager.mTextures[texture9->mId];

				request->MaxLod = texture9->mLevels;
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


			for (size_t i = 0; i < realDevice->mSamplerRequests.size(); i++)
			{
				auto& storedRequest = realDevice->mSamplerRequests[i];
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
					request->mRealDevice = nullptr; //Not owner.
					storedRequest->LastUsed = std::chrono::steady_clock::now();
				}
			}

			if (request->Sampler == vk::Sampler())
			{
				CreateSampler(realDevice, request);
			}

			targetSampler.sampler = request->Sampler;
			targetSampler.imageLayout = vk::ImageLayout::eGeneral;
		}
		else
		{
			targetSampler.sampler = realDevice->mSampler;
			targetSampler.imageView = realDevice->mImageView;
			targetSampler.imageLayout = vk::ImageLayout::eGeneral;
		}
	}

	deviceRenderState.textureCount = std::max(deviceRenderState.textureCount, textureCount);

	/**********************************************
	* Setup context.
	**********************************************/
	context->PrimitiveType = type;

	context->VertexDeclaration = deviceState.mVertexDeclaration;
	context->FVF = deviceState.mFVF;

	context->VertexShader = deviceState.mVertexShader;
	context->mVertexShaderConstantSlots = deviceState.mVertexShaderConstantSlots;

	context->PixelShader = deviceState.mPixelShader;
	context->mPixelShaderConstantSlots = deviceState.mPixelShaderConstantSlots;

	context->mShaderState = deviceState.mShaderState;

	/**********************************************
	* Update the stuff that need to be done outside of a render pass.
	**********************************************/
	if (deviceState.mIsRenderStateDirty || deviceState.mAreTextureStagesDirty || deviceState.mAreLightsDirty || deviceState.mIsMaterialDirty || deviceState.mAreVertexShaderSlotsDirty || deviceState.mArePixelShaderSlotsDirty)
	{
		currentBuffer.endRenderPass();
		UpdateBuffer(realDevice, context);
		currentBuffer.beginRenderPass(&deviceState.mRenderTarget->mRenderPassBeginInfo, vk::SubpassContents::eInline);
	}

	/**********************************************
	* Check for existing pipeline. Create one if there isn't a matching one.
	**********************************************/
	for (size_t i = 0; i < realDevice->mDrawBuffer.size(); i++)
	{
		auto& drawBuffer = (*realDevice->mDrawBuffer[i]);

		if (drawBuffer.PrimitiveType == context->PrimitiveType)
		{
			if (drawBuffer.FVF == context->FVF && drawBuffer.VertexDeclaration == context->VertexDeclaration)
			{
				if (drawBuffer.VertexShader == context->VertexShader && drawBuffer.PixelShader == context->PixelShader)
				{
					if (!memcmp(&drawBuffer.mShaderState.mRenderState, &context->mShaderState.mRenderState, sizeof(RenderState)))
					{
						if (drawBuffer.StreamCount == context->StreamCount && !memcmp(&drawBuffer.Bindings, &context->Bindings, 64 * sizeof(UINT)))
						{
							context->Pipeline = drawBuffer.Pipeline;
							context->PipelineLayout = drawBuffer.PipelineLayout;
							context->DescriptorSetLayout = drawBuffer.DescriptorSetLayout;
							context->mRealDevice = nullptr; //Not owner.
							drawBuffer.LastUsed = std::chrono::steady_clock::now();
							break;
						}
					}
				}
			}
		}
	}

	if (context->Pipeline == vk::Pipeline())
	{
		CreatePipe(realDevice, context, textureCount); //If we didn't find a matching pipeline then create a new one.	
	}

	/**********************************************
	* Setup bindings
	**********************************************/

	//TODO: I need to find a way to prevent binding on every draw call.

	//if (!mIsDirty || mLastVkPipeline != context->Pipeline)
	//{
	currentBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, context->Pipeline);
	//	mLastVkPipeline = context->Pipeline;
	//}

	realDevice->mVertexCount = 0;

	if (deviceState.mIndexBuffer != nullptr)
	{
		currentBuffer.bindIndexBuffer(deviceState.mIndexBuffer->mBuffer, 0, deviceState.mIndexBuffer->mIndexType);
	}

	for (auto& source : deviceState.mStreamSources)
	{
		auto& buffer = mStateManager.mVertexBuffers[source.second.StreamData->mId];
		currentBuffer.bindVertexBuffers(source.first, 1, &buffer->mBuffer, &source.second.OffsetInBytes);
		realDevice->mVertexCount += source.second.StreamData->mSize;
	}

	/**********************************************
	* Update transformation structure.
	**********************************************/
	if (context->VertexShader == nullptr)
	{
		UpdatePushConstants(realDevice, context);
	}
	else
	{
		currentBuffer.pushConstants(context->PipelineLayout, vk::ShaderStageFlagBits::eAllGraphics, 0, UBO_SIZE * 2, &deviceState.mPushConstants);
	}

	/**********************************************
	* Check for existing DescriptorSet. Create one if there isn't a matching one.
	**********************************************/
	if (context->DescriptorSetLayout != vk::DescriptorSetLayout())
	{
		std::copy(std::begin(deviceState.mDescriptorImageInfo), std::end(deviceState.mDescriptorImageInfo), std::begin(resourceContext->DescriptorImageInfo));

		//Render State
		realDevice->mDescriptorBufferInfo[0].buffer = realDevice->mRenderStateBuffer;
		realDevice->mDescriptorBufferInfo[0].offset = 0;
		realDevice->mDescriptorBufferInfo[0].range = sizeof(RenderState);

		realDevice->mWriteDescriptorSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		realDevice->mWriteDescriptorSet[0].dstSet = resourceContext->DescriptorSet;
		realDevice->mWriteDescriptorSet[0].descriptorCount = 1;
		realDevice->mWriteDescriptorSet[0].pBufferInfo = &realDevice->mDescriptorBufferInfo[0];

		//Texture Stages
		realDevice->mDescriptorBufferInfo[1].buffer = realDevice->mTextureStageBuffer;
		realDevice->mDescriptorBufferInfo[1].offset = 0;
		realDevice->mDescriptorBufferInfo[1].range = sizeof(TextureStage) * 9;

		realDevice->mWriteDescriptorSet[1].descriptorType = vk::DescriptorType::eUniformBuffer;
		realDevice->mWriteDescriptorSet[1].dstSet = resourceContext->DescriptorSet;
		realDevice->mWriteDescriptorSet[1].descriptorCount = 1;
		realDevice->mWriteDescriptorSet[1].pBufferInfo = &realDevice->mDescriptorBufferInfo[1];

		//Lights
		realDevice->mDescriptorBufferInfo[2].buffer = realDevice->mLightBuffer;
		realDevice->mDescriptorBufferInfo[2].offset = 0;
		realDevice->mDescriptorBufferInfo[2].range = sizeof(Light) * 8;

		realDevice->mWriteDescriptorSet[2].descriptorType = vk::DescriptorType::eUniformBuffer;
		realDevice->mWriteDescriptorSet[2].dstSet = resourceContext->DescriptorSet;
		realDevice->mWriteDescriptorSet[2].descriptorCount = 1;
		realDevice->mWriteDescriptorSet[2].pBufferInfo = &realDevice->mDescriptorBufferInfo[2];

		//Material
		realDevice->mDescriptorBufferInfo[3].buffer = realDevice->mMaterialBuffer;
		realDevice->mDescriptorBufferInfo[3].offset = 0;
		realDevice->mDescriptorBufferInfo[3].range = sizeof(D3DMATERIAL9);

		realDevice->mWriteDescriptorSet[3].descriptorType = vk::DescriptorType::eUniformBuffer;
		realDevice->mWriteDescriptorSet[3].dstSet = resourceContext->DescriptorSet;
		realDevice->mWriteDescriptorSet[3].descriptorCount = 1;
		realDevice->mWriteDescriptorSet[3].pBufferInfo = &realDevice->mDescriptorBufferInfo[3];



		//Vertex Shader Const
		realDevice->mDescriptorBufferInfo[4].buffer = realDevice->mShaderVertexConstantBuffer;
		realDevice->mDescriptorBufferInfo[4].offset = 0;
		realDevice->mDescriptorBufferInfo[4].range = sizeof(ShaderConstantSlots);

		realDevice->mWriteDescriptorSet[4].dstSet = resourceContext->DescriptorSet;
		realDevice->mWriteDescriptorSet[4].descriptorCount = 1;
		realDevice->mWriteDescriptorSet[4].pBufferInfo = &realDevice->mDescriptorBufferInfo[4];

		//Pixel Shader Const
		realDevice->mDescriptorBufferInfo[5].buffer = realDevice->mShaderPixelConstantBuffer;
		realDevice->mDescriptorBufferInfo[5].offset = 0;
		realDevice->mDescriptorBufferInfo[5].range = sizeof(ShaderConstantSlots);

		realDevice->mWriteDescriptorSet[5].dstSet = resourceContext->DescriptorSet;
		realDevice->mWriteDescriptorSet[5].descriptorCount = 1;
		realDevice->mWriteDescriptorSet[5].pBufferInfo = &realDevice->mDescriptorBufferInfo[5];

		//Image/Sampler
		realDevice->mWriteDescriptorSet[6].dstSet = resourceContext->DescriptorSet;
		realDevice->mWriteDescriptorSet[6].descriptorCount = 16;
		realDevice->mWriteDescriptorSet[6].pImageInfo = resourceContext->DescriptorImageInfo;

		//if (deviceRenderState.textureCount)
		//{
			currentBuffer.pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, context->PipelineLayout, 0, 7, realDevice->mWriteDescriptorSet);
		//}
		//else
		//{
		//	currentBuffer.pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, context->PipelineLayout, 0, 6, realDevice->mWriteDescriptorSet);
		//}
	}

	realDevice->mIsDirty = false;
}

void RenderManager::CreatePipe(std::shared_ptr<RealDevice> realDevice, std::shared_ptr<DrawContext> context, uint32_t textureCount)
{
	vk::Result result;
	auto& device = realDevice->mDevice;
	auto& deviceState = realDevice->mDeviceState;
	auto& deviceShaderState = deviceState.mShaderState;
	auto& deviceRenderState = deviceShaderState.mRenderState;

	/**********************************************
	* Figure out flags
	**********************************************/
	uint32_t attributeCount = 0;
	uint32_t positionSize = 3;
	BOOL hasPosition = 0;
	BOOL hasNormal = 0;
	BOOL hasPSize = 0;
	BOOL hasColor1 = 0;
	BOOL hasColor2 = 0;
	BOOL isTransformed = 0;
	BOOL isLightingEnabled = deviceRenderState.lighting;

	if (deviceState.mVertexDeclaration != nullptr)
	{
		auto vertexDeclaration = deviceState.mVertexDeclaration;

		hasPosition = vertexDeclaration->mHasPosition;
		hasNormal = vertexDeclaration->mHasNormal;
		hasPSize = vertexDeclaration->mHasPSize;
		hasColor1 = vertexDeclaration->mHasColor1;
		hasColor2 = vertexDeclaration->mHasColor2;
	}
	else if (deviceState.mFVF)
	{
		if ((deviceState.mFVF & D3DFVF_XYZRHW) == D3DFVF_XYZRHW)
		{
			positionSize = 4;
			hasPosition = true;
			isTransformed = true;
		}
		else
		{
			if ((deviceState.mFVF & D3DFVF_XYZW) == D3DFVF_XYZW)
			{
				positionSize = 4;
				hasPosition = true;
			}
			else if ((deviceState.mFVF & D3DFVF_XYZ) == D3DFVF_XYZ)
			{
				positionSize = 3;
				hasPosition = true;
			}
			else if ((deviceState.mFVF & D3DFVF_XYZB1) == D3DFVF_XYZB1)
			{
				positionSize = 3;
				hasPosition = true;
				hasColor1 = true;
			}
			else if ((deviceState.mFVF & D3DFVF_XYZB2) == D3DFVF_XYZB2)
			{
				positionSize = 3;
				hasPosition = true;
				hasColor1 = true;
			}
			else if ((deviceState.mFVF & D3DFVF_XYZB3) == D3DFVF_XYZB3)
			{
				positionSize = 3;
				hasPosition = true;
				hasColor1 = true;
			}
			else if ((deviceState.mFVF & D3DFVF_XYZB4) == D3DFVF_XYZB4)
			{
				positionSize = 3;
				hasPosition = true;
				hasColor1 = true;
			}
			else if ((deviceState.mFVF & D3DFVF_XYZB5) == D3DFVF_XYZB5)
			{
				positionSize = 3;
				hasPosition = true;
				hasColor1 = true;
			}

			if ((deviceState.mFVF & D3DFVF_NORMAL) == D3DFVF_NORMAL)
			{
				hasNormal = true;
			}
		}

		if ((deviceState.mFVF & D3DFVF_PSIZE) == D3DFVF_PSIZE)
		{
			hasPSize = true;
		}

		if ((deviceState.mFVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
		{
			hasColor1 = true;
		}

		if ((deviceState.mFVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
		{
			hasColor2 = true;
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported layout definition.";
	}

	attributeCount += hasPosition;
	attributeCount += hasNormal;
	attributeCount += hasPSize;
	attributeCount += hasColor1;
	attributeCount += hasColor2;
	attributeCount += textureCount;

	/**********************************************
	* Figure out render states & texture states
	**********************************************/
	realDevice->mPipelineColorBlendAttachmentState[0].colorWriteMask = (vk::ColorComponentFlagBits)deviceRenderState.colorWriteEnable;
	realDevice->mPipelineColorBlendAttachmentState[0].blendEnable = deviceRenderState.alphaBlendEnable;

	realDevice->mPipelineColorBlendAttachmentState[0].colorBlendOp = ConvertColorOperation(deviceRenderState.blendOperation);
	realDevice->mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = ConvertColorFactor(deviceRenderState.sourceBlend);
	realDevice->mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = ConvertColorFactor(deviceRenderState.destinationBlend);

	realDevice->mPipelineColorBlendAttachmentState[0].alphaBlendOp = ConvertColorOperation(deviceRenderState.blendOperationAlpha);
	realDevice->mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = ConvertColorFactor(deviceRenderState.sourceBlendAlpha);
	realDevice->mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = ConvertColorFactor(deviceRenderState.destinationBlendAlpha);

	SetCulling(realDevice->mPipelineRasterizationStateCreateInfo, (D3DCULL)deviceRenderState.cullMode);
	realDevice->mPipelineRasterizationStateCreateInfo.polygonMode = ConvertFillMode((D3DFILLMODE)deviceRenderState.fillMode);

	realDevice->mPipelineInputAssemblyStateCreateInfo.topology = ConvertPrimitiveType(context->PrimitiveType);

	auto& pipelineDepthStencilStateCreateInfo = realDevice->mPipelineDepthStencilStateCreateInfo;

	pipelineDepthStencilStateCreateInfo.depthTestEnable = deviceRenderState.zEnable; //= VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = deviceRenderState.zWriteEnable; //VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = ConvertCompareOperation(deviceRenderState.zFunction);  //VK_COMPARE_OP_LESS_OR_EQUAL;
	//pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = true; //= constants.bound;
	pipelineDepthStencilStateCreateInfo.stencilTestEnable = deviceRenderState.stencilEnable; //VK_FALSE;

	//twoSidedStencilMode

	/*
	uint32_t stencilMask = 0xFFFFFFFF;
	uint32_t stencilWriteMask = 0xFFFFFFFF;
	*/

	/*
	compareMask( compareMask_ )
	, writeMask( writeMask_ )
	*/

	auto& pipelineDepthStencilStateCreateInfoBack = pipelineDepthStencilStateCreateInfo.back;
	pipelineDepthStencilStateCreateInfoBack.reference = deviceRenderState.stencilReference;
	pipelineDepthStencilStateCreateInfoBack.compareMask = deviceRenderState.stencilMask;
	pipelineDepthStencilStateCreateInfoBack.writeMask = deviceRenderState.stencilWriteMask;

	auto& pipelineDepthStencilStateCreateInfoFront = pipelineDepthStencilStateCreateInfo.front;
	pipelineDepthStencilStateCreateInfoFront.reference = deviceRenderState.stencilReference;
	pipelineDepthStencilStateCreateInfoFront.compareMask = deviceRenderState.stencilMask;
	pipelineDepthStencilStateCreateInfoFront.writeMask = deviceRenderState.stencilWriteMask;

	if (deviceRenderState.cullMode == D3DCULL_CCW)
	{
		pipelineDepthStencilStateCreateInfoBack.failOp = ConvertStencilOperation(deviceRenderState.ccwStencilFail);
		pipelineDepthStencilStateCreateInfoBack.passOp = ConvertStencilOperation(deviceRenderState.ccwStencilPass);
		pipelineDepthStencilStateCreateInfoBack.compareOp = ConvertCompareOperation(deviceRenderState.ccwStencilFunction);


		pipelineDepthStencilStateCreateInfoFront.failOp = ConvertStencilOperation(deviceRenderState.stencilFail);
		pipelineDepthStencilStateCreateInfoFront.passOp = ConvertStencilOperation(deviceRenderState.stencilPass);
		pipelineDepthStencilStateCreateInfoFront.compareOp = ConvertCompareOperation(deviceRenderState.stencilFunction);
	}
	else
	{
		pipelineDepthStencilStateCreateInfoBack.failOp = ConvertStencilOperation(deviceRenderState.stencilFail);
		pipelineDepthStencilStateCreateInfoBack.passOp = ConvertStencilOperation(deviceRenderState.stencilPass);
		pipelineDepthStencilStateCreateInfoBack.compareOp = ConvertCompareOperation(deviceRenderState.stencilFunction);

		pipelineDepthStencilStateCreateInfoFront.failOp = ConvertStencilOperation(deviceRenderState.ccwStencilFail);
		pipelineDepthStencilStateCreateInfoFront.passOp = ConvertStencilOperation(deviceRenderState.ccwStencilPass);
		pipelineDepthStencilStateCreateInfoFront.compareOp = ConvertCompareOperation(deviceRenderState.ccwStencilFunction);
	}


	//mPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	//mPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

	/**********************************************
	* Figure out correct shader
	**********************************************/
	realDevice->mGraphicsPipelineCreateInfo.stageCount = 2;

	if (context->VertexShader != nullptr)
	{
		realDevice->mPipelineShaderStageCreateInfo[0].module = mStateManager.mShaderConverters[context->VertexShader->mId]->mConvertedShader.ShaderModule;
		realDevice->mPipelineShaderStageCreateInfo[1].module = mStateManager.mShaderConverters[context->PixelShader->mId]->mConvertedShader.ShaderModule;
	}
	else
	{
		if (hasPosition && !hasNormal && !hasPSize && !hasColor1 && !hasColor2)
		{
			switch (textureCount)
			{
			case 0:
				if (isTransformed)
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZRHW;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ;
				}

				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && !hasColor && !hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ;
				}
				break;
			case 1:
				if (isTransformed)
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZRHW_TEX1;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_TEX1;
				}

				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && !hasColor && !hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_TEX1;
				}
				break;
			case 2:
				if (isTransformed)
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZRHW_TEX2;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_TEX2;
				}

				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && !hasColor && !hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_TEX2;
				}
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else if (hasPosition && !hasNormal && !hasPSize && hasColor1 && !hasColor2)
		{
			switch (textureCount)
			{
			case 0:
				if (isTransformed)
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZRHW_DIFFUSE;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_DIFFUSE;
				}

				if (deviceRenderState.pointSpriteEnable)
				{
					realDevice->mGraphicsPipelineCreateInfo.stageCount = 3;
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_DIFFUSE_TEX1;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_DIFFUSE;
				}

				break;
			case 1:
				if (isTransformed)
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZRHW_DIFFUSE_TEX1;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_DIFFUSE_TEX1;
				}

				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && hasColor && !hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_DIFFUSE_TEX1;
				}
				break;
			case 2:
				if (isTransformed)
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZRHW_DIFFUSE_TEX2;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_DIFFUSE_TEX2;
				}

				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && hasColor && !hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_DIFFUSE_TEX2;
				}
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else if (hasPosition && hasNormal && !hasPSize && hasColor1 && !hasColor2)
		{
			switch (textureCount)
			{
			case 2:
				realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2;
				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && hasColor && hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2;
				}
				break;
			case 1:
				realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX1;
				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && hasColor && hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX1;
				}
				break;
			case 0:
				realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_NORMAL_DIFFUSE;
				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && hasColor && hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_NORMAL_DIFFUSE;
				}
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else if (hasPosition  && hasNormal && !hasPSize && !hasColor1 && !hasColor2)
		{
			switch (textureCount)
			{
			case 0:
				realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_NORMAL;
				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && !hasColor && hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_NORMAL;
				}
				break;
			case 1:
				realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_NORMAL_TEX1;
				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && !hasColor && hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_NORMAL_TEX1;
				}
				break;
			case 2:
				realDevice->mPipelineShaderStageCreateInfo[0].module = realDevice->mVertShaderModule_XYZ_NORMAL_TEX2;
				if (deviceRenderState.pointSpriteEnable)
				{
					BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe point sprite not supported with hasPosition && !hasColor && hasNormal && " << textureCount;
				}
				else
				{
					realDevice->mPipelineShaderStageCreateInfo[1].module = realDevice->mFragShaderModule_XYZ_NORMAL_TEX2;
				}
				break;
			default:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported texture count " << textureCount;
				break;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe unsupported layout.";
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe hasPosition = " << hasPosition;
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe hasNormal = " << hasNormal;
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe hasPSize = " << hasPSize;
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe hasColor1 = " << hasColor1;
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe hasColor2 = " << hasColor2;
			BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreatePipe textureCount = " << textureCount;
		}
	}

	/**********************************************
	* Figure out attributes
	**********************************************/

	if (deviceState.mVertexDeclaration != nullptr)
	{
		uint32_t textureIndex = 0;

		attributeCount = deviceState.mVertexDeclaration->mVertexElements.size();

		for (size_t i = 0; i < attributeCount; i++)
		{
			D3DVERTEXELEMENT9& element = deviceState.mVertexDeclaration->mVertexElements[i];

			int t = D3DDECLTYPE_FLOAT3;

			realDevice->mVertexInputAttributeDescription[i].binding = element.Stream;
			//realDevice.mVertexInputAttributeDescription[i].location = location;
			realDevice->mVertexInputAttributeDescription[i].format = ConvertDeclType((D3DDECLTYPE)element.Type);
			realDevice->mVertexInputAttributeDescription[i].offset = element.Offset;

			switch ((D3DDECLUSAGE)element.Usage)
			{
			case D3DDECLUSAGE_POSITION:
				realDevice->mVertexInputAttributeDescription[i].location = 0;
				break;
			case D3DDECLUSAGE_POSITIONT:
				realDevice->mVertexInputAttributeDescription[i].location = 0;
				break;
			case D3DDECLUSAGE_NORMAL:
				realDevice->mVertexInputAttributeDescription[i].location = hasPosition;
				break;
			case D3DDECLUSAGE_PSIZE:
				realDevice->mVertexInputAttributeDescription[i].location = hasPosition + hasNormal;
				break;
			case D3DDECLUSAGE_COLOR:
				realDevice->mVertexInputAttributeDescription[i].location = hasPosition + hasNormal + hasPSize + element.UsageIndex;
				break;
			case D3DDECLUSAGE_TEXCOORD:
				realDevice->mVertexInputAttributeDescription[i].location = hasPosition + hasNormal + hasPSize + hasColor1 + hasColor2 + textureIndex;
				textureIndex += 1;
				break;
			case D3DDECLUSAGE_BLENDWEIGHT:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_BLENDWEIGHT is not implemented!";
				break;
			case D3DDECLUSAGE_BLENDINDICES:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_BLENDINDICES is not implemented!";
				break;
			case D3DDECLUSAGE_TANGENT:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_TANGENT is not implemented!";
				break;
			case D3DDECLUSAGE_BINORMAL:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_BINORMAL is not implemented!";
				break;
			case D3DDECLUSAGE_TESSFACTOR:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_TESSFACTOR is not implemented!";
				break;
			case D3DDECLUSAGE_FOG:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_FOG is not implemented!";
				break;
			case D3DDECLUSAGE_DEPTH:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_DEPTH is not implemented!";
				break;
			case D3DDECLUSAGE_SAMPLE:
				BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw D3DDECLUSAGE_SAMPLE is not implemented!";
				break;
			default:
				break;
			}
		}
	}
	else if (deviceState.mFVF)
	{
		uint32_t attributeIndex = 0;
		uint32_t offset = 0;
		uint32_t location = 0;

		if (hasPosition)
		{
			realDevice->mVertexInputAttributeDescription[attributeIndex].binding = 0;
			realDevice->mVertexInputAttributeDescription[attributeIndex].location = location;
			realDevice->mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eR32G32B32Sfloat;
			realDevice->mVertexInputAttributeDescription[attributeIndex].offset = offset;
			offset += (sizeof(float) * positionSize);
			location += 1;
			attributeIndex += 1;
		}

		if (hasNormal)
		{
			realDevice->mVertexInputAttributeDescription[attributeIndex].binding = 0;
			realDevice->mVertexInputAttributeDescription[attributeIndex].location = location;
			realDevice->mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eR32G32B32Sfloat;
			realDevice->mVertexInputAttributeDescription[attributeIndex].offset = offset;
			offset += (sizeof(float) * 3);
			location += 1;
			attributeIndex += 1;
		}

		if (hasPSize)
		{
			realDevice->mVertexInputAttributeDescription[attributeIndex].binding = 0;
			realDevice->mVertexInputAttributeDescription[attributeIndex].location = location;
			realDevice->mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eR32G32B32Sfloat;
			realDevice->mVertexInputAttributeDescription[attributeIndex].offset = offset;
			offset += sizeof(float);
			location += 1;
			attributeIndex += 1;
		}

		if (hasColor1)
		{
			realDevice->mVertexInputAttributeDescription[attributeIndex].binding = 0;
			realDevice->mVertexInputAttributeDescription[attributeIndex].location = location;
			realDevice->mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eB8G8R8A8Uint;
			realDevice->mVertexInputAttributeDescription[attributeIndex].offset = offset;
			offset += sizeof(uint32_t);
			location += 1;
			attributeIndex += 1;
		}

		if (hasColor2)
		{
			realDevice->mVertexInputAttributeDescription[attributeIndex].binding = 0;
			realDevice->mVertexInputAttributeDescription[attributeIndex].location = location;
			realDevice->mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eB8G8R8A8Uint;
			realDevice->mVertexInputAttributeDescription[attributeIndex].offset = offset;
			offset += sizeof(uint32_t);
			location += 1;
			attributeIndex += 1;
		}

		for (size_t j = 0; j < textureCount; j++)
		{
			realDevice->mVertexInputAttributeDescription[attributeIndex].binding = 0;
			realDevice->mVertexInputAttributeDescription[attributeIndex].location = location;
			realDevice->mVertexInputAttributeDescription[attributeIndex].format = vk::Format::eR32G32Sfloat;
			realDevice->mVertexInputAttributeDescription[attributeIndex].offset = offset;
			offset += (sizeof(float) * 2);
			location += 1;
			attributeIndex += 1;
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw unknown vertex format.";
	}

	realDevice->mDescriptorSetLayoutCreateInfo.pBindings = realDevice->mDescriptorSetLayoutBinding;
	//if (deviceRenderState.textureCount)
	//{
		realDevice->mDescriptorSetLayoutCreateInfo.bindingCount = 7; //The number of elements in pBindings.			
	//}
	//else
	//{
	//	realDevice->mDescriptorSetLayoutCreateInfo.bindingCount = 6; //The number of elements in pBindings.	
	//}

	result = device.createDescriptorSetLayout(&realDevice->mDescriptorSetLayoutCreateInfo, nullptr, &context->DescriptorSetLayout);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::CreateDescriptorSet vkCreateDescriptorSetLayout failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	/**********************************************
	* Create pipeline & descriptor set layout.
	**********************************************/
	auto& pipelineLayoutCreateInfo = realDevice->mPipelineLayoutCreateInfo;

	pipelineLayoutCreateInfo.pPushConstantRanges = realDevice->mPushConstantRanges;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;

	realDevice->mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = context->StreamCount;
	realDevice->mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeCount;

	realDevice->mPipelineLayoutCreateInfo.pSetLayouts = &context->DescriptorSetLayout;
	realDevice->mPipelineLayoutCreateInfo.setLayoutCount = 1;

	result = device.createPipelineLayout(&realDevice->mPipelineLayoutCreateInfo, nullptr, &context->PipelineLayout);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw vkCreatePipelineLayout failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	realDevice->mGraphicsPipelineCreateInfo.layout = context->PipelineLayout;
	realDevice->mGraphicsPipelineCreateInfo.renderPass = deviceState.mRenderTarget->mStoreRenderPass;

	result = device.createGraphicsPipelines(realDevice->mPipelineCache, 1, &realDevice->mGraphicsPipelineCreateInfo, nullptr, &context->Pipeline);
	//result = vkCreateGraphicsPipelines(mDevice->mDevice, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfo, nullptr, &context.Pipeline);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RenderManager::BeginDraw vkCreateGraphicsPipelines failed with return code of " << GetResultString((VkResult)result);
	}

	context->LastUsed = std::chrono::steady_clock::now();
	realDevice->mDrawBuffer.push_back(context);
}

void RenderManager::CreateSampler(std::shared_ptr<RealDevice> realDevice, std::shared_ptr<SamplerRequest> request)
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb172602(v=vs.85).aspx
	//Mipmap filter to use during minification. See D3DTEXTUREFILTERTYPE. The default value is D3DTEXF_NONE.

	vk::Result result;
	//auto& deviceState = realDevice.mDeviceState;
	auto& device = realDevice->mDevice;

	vk::SamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.magFilter = ConvertFilter(request->MagFilter);
	samplerCreateInfo.minFilter = ConvertFilter(request->MinFilter);
	samplerCreateInfo.addressModeU = ConvertTextureAddress(request->AddressModeU);
	samplerCreateInfo.addressModeV = ConvertTextureAddress(request->AddressModeV);
	samplerCreateInfo.addressModeW = ConvertTextureAddress(request->AddressModeW);
	samplerCreateInfo.mipmapMode = ConvertMipmapMode(request->MipmapMode); //VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.mipLodBias = request->MipLodBias;
	//samplerCreateInfo.compareEnable = true;

	/*
	https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html
	If either magFilter or minFilter is VK_FILTER_CUBIC_IMG, anisotropyEnable must be VK_FALSE
	*/
	if (realDevice->mPhysicalDeviceFeatures.samplerAnisotropy && samplerCreateInfo.minFilter != vk::Filter::eCubicIMG && samplerCreateInfo.magFilter != vk::Filter::eCubicIMG)
	{
		// Use max. level of anisotropy for this example
		samplerCreateInfo.maxAnisotropy = std::min((float)request->MaxAnisotropy, realDevice->mPhysicalDeviceProperties.limits.maxSamplerAnisotropy);

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

	realDevice->mSamplerRequests.push_back(request);
}

void RenderManager::UpdatePushConstants(std::shared_ptr<RealDevice> realDevice, std::shared_ptr<DrawContext> context)
{
	//vk::Result result;
	auto& deviceState = realDevice->mDeviceState;
	//auto& device = realDevice.mRealDevice.mDevice;
	auto& currentSwapChainBuffer = realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer];
	void* data = nullptr;

	//if (!mDevice->mDeviceState.mHasTransformsChanged)
	//{
	//	return;
	//}

	auto& transformations = realDevice->mTransformations;

	transformations.mModel <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	transformations.mView <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	transformations.mProjection <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	for (const auto& pair1 : deviceState.mTransforms)
	{
		switch (pair1.first)
		{
		case D3DTS_WORLD:

			transformations.mModel <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_VIEW:

			transformations.mView <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		case D3DTS_PROJECTION:

			transformations.mProjection <<
				pair1.second.m[0][0], pair1.second.m[1][0], pair1.second.m[2][0], pair1.second.m[3][0],
				pair1.second.m[0][1], pair1.second.m[1][1], pair1.second.m[2][1], pair1.second.m[3][1],
				pair1.second.m[0][2], pair1.second.m[1][2], pair1.second.m[2][2], pair1.second.m[3][2],
				pair1.second.m[0][3], pair1.second.m[1][3], pair1.second.m[2][3], pair1.second.m[3][3];

			break;
		default:
			//These are handled with a uniform buffer.
			break;
		}
	}

	transformations.mTotalTransformation = transformations.mProjection * transformations.mView * transformations.mModel;
	//mTotalTransformation = mModel * mView * mProjection;

	currentSwapChainBuffer.pushConstants(context->PipelineLayout, vk::ShaderStageFlagBits::eAllGraphics, 0, UBO_SIZE * 2, &transformations);
}

void RenderManager::FlushDrawBufffer(std::shared_ptr<RealDevice> realDevice)
{
	/*
	Uses remove_if and chrono to remove elements that have not been used in over a second.
	*/
	auto& drawBuffer = realDevice->mDrawBuffer;
	drawBuffer.erase(std::remove_if(drawBuffer.begin(), drawBuffer.end(), [](const std::shared_ptr<DrawContext> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), drawBuffer.end());

	auto& samplerRequests = realDevice->mSamplerRequests;
	samplerRequests.erase(std::remove_if(samplerRequests.begin(), samplerRequests.end(), [](const std::shared_ptr<SamplerRequest> & o) { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - o->LastUsed).count() > CACHE_SECONDS; }), samplerRequests.end());

	realDevice->mRenderTargets.clear();

	realDevice->mIsDirty = true;
}
