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