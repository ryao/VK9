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

#include "RealWindow.h"

RealWindow::RealWindow(std::shared_ptr<RealInstance>& realInstance, std::shared_ptr<RealDevice>& realDevice)
	: mRealInstance(realInstance)
	, mRealDevice(realDevice)
{
	BOOST_LOG_TRIVIAL(info) << "RealWindow::RealWindow";
}

RealWindow::~RealWindow()
{
	BOOST_LOG_TRIVIAL(info) << "RealWindow::~RealWindow";

	//Empty cached objects. (a destructor should take care of their resources.)
	mDrawBuffer.clear();
	mSamplerRequests.clear();

	//Clean up the rest of the window state handles.
	auto& device = mRealDevice->mDevice;
	auto& instance = mRealInstance->mInstance;

	device.freeCommandBuffers(mCommandPool, 1, &mCommandBuffer);
	device.destroyBuffer(mLightBuffer, nullptr);
	device.freeMemory(mLightBufferMemory, nullptr);
	device.destroyBuffer(mMaterialBuffer, nullptr);
	device.freeMemory(mMaterialBufferMemory, nullptr);
	device.destroyImageView(mImageView, nullptr);
	device.destroyImage(mImage, nullptr);
	device.freeMemory(mDeviceMemory, nullptr);
	device.destroySampler(mSampler, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_DIFFUSE, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_DIFFUSE, nullptr);
	device.destroyShaderModule(mGeomShaderModule_XYZ_DIFFUSE, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_TEX1, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_TEX1, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_TEX2, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_TEX2, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_DIFFUSE_TEX1, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_DIFFUSE_TEX1, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_DIFFUSE_TEX2, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_DIFFUSE_TEX2, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_NORMAL, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_NORMAL, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_NORMAL_TEX1, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_NORMAL_TEX1, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_NORMAL_DIFFUSE, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_NORMAL_DIFFUSE, nullptr);
	device.destroyShaderModule(mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2, nullptr);
	device.destroyShaderModule(mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2, nullptr);
	device.destroyPipelineCache(mPipelineCache, nullptr);

	device.destroySemaphore(mPresentCompleteSemaphore, nullptr);

	if (mFramebuffers != nullptr)
	{
		for (size_t i = 0; i < mSwapchainImageCount; i++)
		{
			if (mFramebuffers[i] != nullptr)
			{
				device.destroyFramebuffer(mFramebuffers[i], nullptr);
			}
		}
		delete[] mFramebuffers;
	}

	device.destroyRenderPass(mStoreRenderPass, nullptr);
	device.destroyRenderPass(mClearRenderPass, nullptr);
	device.freeCommandBuffers(mCommandPool, mSwapchainImageCount, mSwapchainBuffers);
	delete[] mSwapchainBuffers;
	device.destroyImageView(mDepthView, nullptr);
	device.destroyImage(mDepthImage, nullptr);
	device.freeMemory(mDepthDeviceMemory, nullptr);
	device.destroyCommandPool(mCommandPool, nullptr);
	device.destroySwapchainKHR(mSwapchain, nullptr);

	if (mSwapchainViews != nullptr)
	{
		for (size_t i = 0; i < mSwapchainImageCount; i++)
		{
			if (mSwapchainViews[i] != nullptr)
			{
				device.destroyImageView(mSwapchainViews[i], nullptr);
			}
		}
		delete[] mSwapchainViews;
	}

	//if (mSwapchainImages != nullptr)
	//{
	//For some reason destroying the images causes a crash. I'm guessing it's a double free or something like that because the views have already been destroyed.
	//for (int32_t i = 0; i < mSwapchainImageCount; i++)
	//{
	//	if (mSwapchainImages[i] != VK_NULL_HANDLE)
	//	{
	//		vkDestroyImage(mDevice, mSwapchainImages[i], nullptr);
	//	}	
	//}
	delete[] mSwapchainImages;
	//}

	instance.destroySurfaceKHR(mSurface, nullptr);

	delete[] mPresentationModes;
	delete[] mSurfaceFormats;
}

void RealWindow::SetImageLayout(vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, uint32_t levelCount, uint32_t mipIndex, uint32_t layerCount)
{
	/*
	This is just a helper method to reduce repeat code.
	*/
	vk::Result result;
	vk::CommandBuffer commandBuffer;

	if (aspectMask == vk::ImageAspectFlags())
	{
		aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	vk::CommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.commandPool = mCommandPool;
	commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferInfo.commandBufferCount = 1;

	result = mRealDevice->mDevice.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealWindow::SetImageLayout vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = commandBuffer.begin(&commandBufferBeginInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealWindow::SetImageLayout vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	ReallySetImageLayout(commandBuffer, image, aspectMask, oldImageLayout, newImageLayout, levelCount, mipIndex, layerCount);

	commandBuffer.end();

	vk::CommandBuffer commandBuffers[] = { commandBuffer };
	vk::Fence nullFence;
	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.signalSemaphoreCount = 0;

	result = mQueue.submit(1, &submitInfo, nullFence);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealWindow::SetImageLayout vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mQueue.waitIdle();
	mRealDevice->mDevice.freeCommandBuffers(mCommandPool, 1, commandBuffers);
}

void RealWindow::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlagBits properties, vk::Buffer& buffer, vk::DeviceMemory& deviceMemory)
{
	vk::Result result; // = VK_SUCCESS

	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	result = mRealDevice->mDevice.createBuffer(&bufferInfo, nullptr, &buffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealWindow::CreateBuffer vkCreateBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::MemoryRequirements memoryRequirements;
	mRealDevice->mDevice.getBufferMemoryRequirements(buffer, &memoryRequirements);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize = memoryRequirements.size;
	//allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (!GetMemoryTypeFromProperties(mRealDevice->mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, properties, &allocInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "Memory type index not found!";
		return;
	}

	result = mRealDevice->mDevice.allocateMemory(&allocInfo, nullptr, &deviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealWindow::CreateBuffer vkCreateBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mRealDevice->mDevice.bindBufferMemory(buffer, deviceMemory, 0);
}

void RealWindow::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	mCommandBuffer.begin(&mBeginInfo);
	{
		mCopyRegion.size = size;
		mCommandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &mCopyRegion);
	}
	mCommandBuffer.end();
	mQueue.submit(1, &mSubmitInfo, nullptr);
	mQueue.waitIdle();
	mCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources); //So far resetting a command buffer is about 10 times faster than allocating a new one.
}