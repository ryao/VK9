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

#define MAX_DESCRIPTOR 2048

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include "Perf_StateManager.h"

#include "C9.h"
#include "CDevice9.h"
#include "CStateBlock9.h"
#include "CTexture9.h"
#include "CCubeTexture9.h"
#include "CVolumeTexture9.h"
#include "Utilities.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/format.hpp>

typedef boost::container::flat_map<UINT, StreamSource> map_type;

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData)
{
	switch (flags)
	{
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		BOOST_LOG_TRIVIAL(info) << "DebugReport(Info): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "DebugReport(Warn): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "DebugReport(Perf): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		BOOST_LOG_TRIVIAL(error) << "DebugReport(Error): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "DebugReport(Debug): " << message << " " << objectType;
		break;
	default:
		BOOST_LOG_TRIVIAL(error) << "DebugReport(?): " << message << " " << objectType;
		break;
	}

	return VK_FALSE;
}

VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHR(
	VkCommandBuffer                             commandBuffer,
	VkPipelineBindPoint                         pipelineBindPoint,
	VkPipelineLayout                            layout,
	uint32_t                                    set,
	uint32_t                                    descriptorWriteCount,
	const VkWriteDescriptorSet*                 pDescriptorWrites)
{
	pfn_vkCmdPushDescriptorSetKHR(
		commandBuffer,
		pipelineBindPoint,
		layout,
		set,
		descriptorWriteCount,
		pDescriptorWrites
	);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
	VkInstance                                  instance,
	const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkDebugReportCallbackEXT*                   pCallback)
{
	return pfn_vkCreateDebugReportCallbackEXT(
		instance,
		pCreateInfo,
		pAllocator,
		pCallback
	);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
	VkInstance                                  instance,
	VkDebugReportCallbackEXT                    callback,
	const VkAllocationCallbacks*                pAllocator)
{
	pfn_vkDestroyDebugReportCallbackEXT(
		instance,
		callback,
		pAllocator
	);
}

VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(
	VkInstance                                  instance,
	VkDebugReportFlagsEXT                       flags,
	VkDebugReportObjectTypeEXT                  objectType,
	uint64_t                                    object,
	size_t                                      location,
	int32_t                                     messageCode,
	const char*                                 pLayerPrefix,
	const char*                                 pMessage)
{
	pfn_vkDebugReportMessageEXT(
		instance,
		flags,
		objectType,
		object,
		location,
		messageCode,
		pLayerPrefix,
		pMessage
	);
}

StateManager::StateManager()
{

}

StateManager::~StateManager()
{

}

void StateManager::DestroyWindow(size_t id)
{
	mWindows[id].reset();
}

void StateManager::CreateWindow1(size_t id, void* argument1, void* argument2)
{
	vk::Result result;
	auto instance = mInstances[id];
	CDevice9* device9 = bit_cast<CDevice9*>(argument1);
	auto& physicaldevice = instance->mPhysicalDevices[device9->mAdapter];
	auto& device = instance->mDevices[device9->mAdapter];
	std::shared_ptr<RealWindow> ptr = std::make_shared<RealWindow>(instance, device);
	vk::Bool32 doesSupportPresentation = false;
	vk::Bool32 doesSupportGraphics = false;
	uint32_t graphicsQueueIndex = 0;
	vk::Format depthFormat = vk::Format::eD16Unorm;
	vk::SurfaceTransformFlagBitsKHR transformFlags;

	mWindows.push_back(ptr);

	for (size_t i = 0; i < device->mPhysicalDeviceMemoryProperties.memoryHeapCount; i++)
	{
		ptr->mEstimatedMemory += device->mPhysicalDeviceMemoryProperties.memoryHeaps[i].size;
	}
	BOOST_LOG_TRIVIAL(info) << "StateManager::CreateWindow1 The total size of all heaps is " << ptr->mEstimatedMemory;

	for (uint32_t i = 0; i < device->mQueueFamilyPropertyCount; i++)
	{
		doesSupportGraphics = (device->mQueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlagBits(0);
		if (doesSupportGraphics)
		{
			graphicsQueueIndex = i;
			break;
		}
	}

	/*
	Now we need to setup our queues and buffers.
	We'll start with a command pool because that is where we get command buffers from.
	*/
	vk::CommandPoolCreateInfo commandPoolInfo;
	commandPoolInfo.queueFamilyIndex = graphicsQueueIndex; //Found earlier.
	commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

	result = device->mDevice.createCommandPool(&commandPoolInfo, nullptr, &ptr->mCommandPool);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateCommandPool failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	//Create queue so we can submit command buffers.
	device->mDevice.getQueue(graphicsQueueIndex, 0, &ptr->mQueue); //no result?

	/*
	Now pull some information about the surface so we can create the swapchain correctly.
	*/
	auto& presentationParameters = device9->mPresentationParameters;
	auto& deviceState = ptr->mDeviceState;


	//initialize vulkan/d3d9 viewport and scissor structures.
	//mDeviceState.mViewport.y = (float)mPresentationParameters.BackBufferHeight;
	deviceState.mViewport.width = (float)presentationParameters.BackBufferWidth;
	//mDeviceState.mViewport.height = -(float)mPresentationParameters.BackBufferHeight;
	deviceState.mViewport.height = (float)presentationParameters.BackBufferHeight;
	deviceState.mViewport.minDepth = 0.0f;
	deviceState.mViewport.maxDepth = 1.0f;

	deviceState.m9Viewport.Width = (DWORD)deviceState.mViewport.width;
	deviceState.m9Viewport.Height = (DWORD)deviceState.mViewport.height;
	deviceState.m9Viewport.MinZ = deviceState.mViewport.minDepth;
	deviceState.m9Viewport.MaxZ = deviceState.mViewport.maxDepth;

	deviceState.mScissor.offset.x = 0;
	deviceState.mScissor.offset.y = 0;
	deviceState.mScissor.extent.width = presentationParameters.BackBufferWidth;
	deviceState.mScissor.extent.height = presentationParameters.BackBufferHeight;

	deviceState.m9Scissor.right = presentationParameters.BackBufferWidth;
	deviceState.m9Scissor.bottom = presentationParameters.BackBufferHeight;
	deviceState.m9Scissor.left = 0;
	deviceState.m9Scissor.top = 0;

	

	if (ptr->mSurfaceFormatCount == 1 && ptr->mSurfaceFormats[0].format == vk::Format::eUndefined)
	{
		ptr->mFormat = vk::Format::eB8G8R8A8Unorm; //No preferred format so set a default.
	}
	else
	{
		ptr->mFormat = ptr->mSurfaceFormats[0].format; //Pull the preferred format.
	}

	



	result = device->mDevice.getSwapchainImagesKHR(ptr->mSwapchain, &ptr->mSwapchainImageCount, ptr->mSwapchainImages);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetSwapchainImagesKHR failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	for (size_t i = 0; i < ptr->mSwapchainImageCount; i++)
	{
		vk::ImageViewCreateInfo color_image_view;
		color_image_view.format = ptr->mFormat;
		color_image_view.components.r = vk::ComponentSwizzle::eIdentity;
		color_image_view.components.g = vk::ComponentSwizzle::eIdentity;
		color_image_view.components.b = vk::ComponentSwizzle::eIdentity;
		color_image_view.components.a = vk::ComponentSwizzle::eIdentity;
		color_image_view.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		color_image_view.subresourceRange.baseMipLevel = 0;
		color_image_view.subresourceRange.levelCount = 1;
		color_image_view.subresourceRange.baseArrayLayer = 0;
		color_image_view.subresourceRange.layerCount = 1;
		color_image_view.viewType = vk::ImageViewType::e2D;
		color_image_view.image = ptr->mSwapchainImages[i];

		result = device->mDevice.createImageView(&color_image_view, nullptr, &ptr->mSwapchainViews[i]);
		if (result != vk::Result::eSuccess)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
			return;
		}

		vk::CommandBufferAllocateInfo commandBufferInfo;
		commandBufferInfo.commandPool = ptr->mCommandPool;
		commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
		commandBufferInfo.commandBufferCount = 1;

		result = device->mDevice.allocateCommandBuffers(&commandBufferInfo, &ptr->mSwapchainBuffers[i]);
		if (result != vk::Result::eSuccess)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
			return;
		}
	} //for

	/*
	Setup Depth stuff.
	*/

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = depthFormat;
	imageCreateInfo.extent = vk::Extent3D(device9->mPresentationParameters.BackBufferWidth, device9->mPresentationParameters.BackBufferHeight, 1);
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;

	result = device->mDevice.createImage(&imageCreateInfo, nullptr, &ptr->mDepthImage);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::MemoryRequirements memoryRequirements;
	device->mDevice.getImageMemoryRequirements(ptr->mDepthImage, &memoryRequirements);

	vk::MemoryAllocateInfo depthMemoryAllocateInfo;
	depthMemoryAllocateInfo.memoryTypeIndex = 0;
	depthMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	GetMemoryTypeFromProperties(device->mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, 0, &depthMemoryAllocateInfo.memoryTypeIndex);

	result = device->mDevice.allocateMemory(&depthMemoryAllocateInfo, nullptr, &ptr->mDepthDeviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	//c++ version doesn't return a result code.... I don't think I like that.
	device->mDevice.bindImageMemory(ptr->mDepthImage, ptr->mDepthDeviceMemory, 0);

	ptr->SetImageLayout(ptr->mDepthImage, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.format = depthFormat;
	imageViewCreateInfo.subresourceRange = vk::ImageSubresourceRange();
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.image = ptr->mDepthImage;

	result = device->mDevice.createImageView(&imageViewCreateInfo, nullptr, &ptr->mDepthView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::AttachmentReference colorReference;
	colorReference.attachment = 0;
	colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::AttachmentReference depthReference;
	depthReference.attachment = 1;
	depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthReference;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	/*
	Now setup the render pass.
	*/
	ptr->mRenderAttachments[0].format = ptr->mFormat;
	ptr->mRenderAttachments[0].samples = vk::SampleCountFlagBits::e1;
	ptr->mRenderAttachments[0].loadOp = vk::AttachmentLoadOp::eLoad;
	ptr->mRenderAttachments[0].storeOp = vk::AttachmentStoreOp::eStore;
	ptr->mRenderAttachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	ptr->mRenderAttachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	ptr->mRenderAttachments[0].initialLayout = vk::ImageLayout::ePresentSrcKHR;
	ptr->mRenderAttachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

	ptr->mRenderAttachments[1].format = depthFormat;
	ptr->mRenderAttachments[1].samples = vk::SampleCountFlagBits::e1;
	ptr->mRenderAttachments[1].loadOp = vk::AttachmentLoadOp::eClear;
	ptr->mRenderAttachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
	ptr->mRenderAttachments[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	ptr->mRenderAttachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	ptr->mRenderAttachments[1].initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	ptr->mRenderAttachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::RenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.attachmentCount = 2; //revisit
	renderPassCreateInfo.pAttachments = ptr->mRenderAttachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 0;
	renderPassCreateInfo.pDependencies = nullptr;

	result = device->mDevice.createRenderPass(&renderPassCreateInfo, nullptr, &ptr->mStoreRenderPass);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateRenderPass failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	ptr->mRenderAttachments[0].loadOp = vk::AttachmentLoadOp::eClear;
	result = device->mDevice.createRenderPass(&renderPassCreateInfo, nullptr, &ptr->mClearRenderPass);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateRenderPass failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	/*
	Setup framebuffers to tie everything together.
	*/

	vk::ImageView attachments[2];
	attachments[1] = ptr->mDepthView;

	vk::FramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.renderPass = ptr->mStoreRenderPass;
	framebufferCreateInfo.attachmentCount = 2; //revisit
	framebufferCreateInfo.pAttachments = attachments;
	framebufferCreateInfo.width = ptr->mSwapchainExtent.width; //revisit
	framebufferCreateInfo.height = ptr->mSwapchainExtent.height; //revisit
	framebufferCreateInfo.layers = 1;

	ptr->mFramebuffers = new vk::Framebuffer[ptr->mSwapchainImageCount];

	for (size_t i = 0; i < ptr->mSwapchainImageCount; i++)
	{
		attachments[0] = ptr->mSwapchainViews[i];
		result = device->mDevice.createFramebuffer(&framebufferCreateInfo, nullptr, &ptr->mFramebuffers[i]);
		if (result != vk::Result::eSuccess)
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateFramebuffer failed with return code of " << GetResultString((VkResult)result);
			return;
		}
	}

	result = device->mDevice.createSemaphore(&ptr->mPresentCompleteSemaphoreCreateInfo, nullptr, &ptr->mPresentCompleteSemaphore);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateSemaphore failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	ptr->mPresentInfo.swapchainCount = 1;
	ptr->mPresentInfo.pSwapchains = &ptr->mSwapchain;
	ptr->mCommandBufferInheritanceInfo.subpass = 0;
	ptr->mCommandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	ptr->mCommandBufferBeginInfo.pInheritanceInfo = &ptr->mCommandBufferInheritanceInfo;

	for (int32_t i = 0; i < 16; i++)
	{
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_BORDERCOLOR] = 0;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_MAGFILTER] = D3DTEXF_POINT;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_MINFILTER] = D3DTEXF_POINT;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_MIPMAPLODBIAS] = 0;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_MAXMIPLEVEL] = 0;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_MAXANISOTROPY] = 1;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_SRGBTEXTURE] = 0;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_ELEMENTINDEX] = 0;
		ptr->mDeviceState.mSamplerStates[i][D3DSAMP_DMAPOFFSET] = 0;
	}

	//Changed default state because -1 is used to indicate that it has not been set but actual state should be defaulted.
	ptr->mDeviceState.mFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	Light light = {};
	ptr->mDeviceState.mLights.push_back(light);
	//mDeviceState.mLights.push_back(light);
	//mDeviceState.mLights.push_back(light);
	//mDeviceState.mLights.push_back(light);

	//Load fixed function shaders.
	ptr->mVertShaderModule_XYZ_DIFFUSE = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_DIFFUSE.vert.spv");
	ptr->mFragShaderModule_XYZ_DIFFUSE = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_DIFFUSE.frag.spv");
	ptr->mGeomShaderModule_XYZ_DIFFUSE = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_DIFFUSE.geom.spv");

	ptr->mVertShaderModule_XYZ_TEX1 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_TEX1.vert.spv");
	ptr->mFragShaderModule_XYZ_TEX1 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_TEX1.frag.spv");

	ptr->mVertShaderModule_XYZ_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_TEX2.vert.spv");
	ptr->mFragShaderModule_XYZ_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_TEX2.frag.spv");

	ptr->mVertShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX1.vert.spv");
	ptr->mFragShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX1.frag.spv");

	ptr->mVertShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX2.vert.spv");
	ptr->mFragShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_DIFFUSE_TEX2.frag.spv");

	ptr->mVertShaderModule_XYZ_NORMAL = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL.vert.spv");
	ptr->mFragShaderModule_XYZ_NORMAL = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL.frag.spv");

	ptr->mVertShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_TEX1.vert.spv");
	ptr->mFragShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_TEX1.frag.spv");

	ptr->mVertShaderModule_XYZ_NORMAL_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_TEX2.vert.spv");
	ptr->mFragShaderModule_XYZ_NORMAL_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_TEX2.frag.spv");

	ptr->mVertShaderModule_XYZ_NORMAL_DIFFUSE = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE.vert.spv");
	ptr->mFragShaderModule_XYZ_NORMAL_DIFFUSE = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE.frag.spv");

	ptr->mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.vert.spv");
	ptr->mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromFile(device->mDevice, "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.frag.spv");

	//pipeline stuff.
	ptr->mPushConstantRanges[0].offset = 0;
	ptr->mPushConstantRanges[0].size = UBO_SIZE * 2; //There are 2 matrices one for world transform and one for all transforms.
	ptr->mPushConstantRanges[0].stageFlags = vk::ShaderStageFlagBits::eAllGraphics; //VK_SHADER_STAGE_ALL_GRAPHICS

	ptr->mVertexSpecializationInfo.pData = &ptr->mDeviceState.mSpecializationConstants;
	ptr->mVertexSpecializationInfo.dataSize = sizeof(SpecializationConstants);

	ptr->mPixelSpecializationInfo.pData = &ptr->mDeviceState.mSpecializationConstants;
	ptr->mPixelSpecializationInfo.dataSize = sizeof(SpecializationConstants);

	ptr->mPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1; //reset later.
	ptr->mPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = ptr->mVertexInputBindingDescription;
	ptr->mPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2; //reset later.
	ptr->mPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = ptr->mVertexInputAttributeDescription;

	ptr->mDynamicStateEnables[ptr->mPipelineDynamicStateCreateInfo.dynamicStateCount++] = vk::DynamicState::eViewport;
	ptr->mDynamicStateEnables[ptr->mPipelineDynamicStateCreateInfo.dynamicStateCount++] = vk::DynamicState::eScissor;
	ptr->mDynamicStateEnables[ptr->mPipelineDynamicStateCreateInfo.dynamicStateCount++] = vk::DynamicState::eDepthBias;
	ptr->mPipelineDynamicStateCreateInfo.pDynamicStates = ptr->mDynamicStateEnables;

	ptr->mPipelineRasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
	ptr->mPipelineRasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
	ptr->mPipelineRasterizationStateCreateInfo.frontFace = vk::FrontFace::eClockwise;
	ptr->mPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	ptr->mPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	ptr->mPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_TRUE;
	ptr->mPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
	ptr->mPipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	ptr->mPipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	ptr->mPipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;

	ptr->mPipelineInputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

	//ptr->mPipelineColorBlendAttachmentState[0].colorWriteMask = 0xf;
	ptr->mPipelineColorBlendAttachmentState[0].blendEnable = VK_TRUE;
	ptr->mPipelineColorBlendAttachmentState[0].colorBlendOp = vk::BlendOp::eAdd;
	ptr->mPipelineColorBlendAttachmentState[0].srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	ptr->mPipelineColorBlendAttachmentState[0].dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
	ptr->mPipelineColorBlendAttachmentState[0].alphaBlendOp = vk::BlendOp::eAdd;
	ptr->mPipelineColorBlendAttachmentState[0].srcAlphaBlendFactor = vk::BlendFactor::eOne;
	ptr->mPipelineColorBlendAttachmentState[0].dstAlphaBlendFactor = vk::BlendFactor::eZero;

	ptr->mPipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	ptr->mPipelineColorBlendStateCreateInfo.logicOp = vk::LogicOp::eNoOp;
	ptr->mPipelineColorBlendStateCreateInfo.attachmentCount = 1;
	ptr->mPipelineColorBlendStateCreateInfo.pAttachments = ptr->mPipelineColorBlendAttachmentState;
	ptr->mPipelineColorBlendStateCreateInfo.blendConstants[0] = 1.0f;
	ptr->mPipelineColorBlendStateCreateInfo.blendConstants[1] = 1.0f;
	ptr->mPipelineColorBlendStateCreateInfo.blendConstants[2] = 1.0f;
	ptr->mPipelineColorBlendStateCreateInfo.blendConstants[3] = 1.0f;

	ptr->mPipelineViewportStateCreateInfo.viewportCount = 1;
	ptr->mPipelineViewportStateCreateInfo.scissorCount = 1;

	ptr->mPipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	ptr->mPipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	ptr->mPipelineDepthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eLessOrEqual;
	ptr->mPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	ptr->mPipelineDepthStencilStateCreateInfo.back.failOp = vk::StencilOp::eKeep;
	ptr->mPipelineDepthStencilStateCreateInfo.back.passOp = vk::StencilOp::eKeep;
	ptr->mPipelineDepthStencilStateCreateInfo.back.compareOp = vk::CompareOp::eAlways;
	ptr->mPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	ptr->mPipelineDepthStencilStateCreateInfo.front = ptr->mPipelineDepthStencilStateCreateInfo.back;
	ptr->mPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	ptr->mPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

	ptr->mPipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
	ptr->mPipelineMultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

	ptr->mDescriptorSetLayoutCreateInfo.bindingCount = 1;
	ptr->mDescriptorSetLayoutCreateInfo.pBindings = ptr->mDescriptorSetLayoutBinding;
	ptr->mDescriptorSetLayoutCreateInfo.flags = vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptorKHR;

	ptr->mDescriptorSetAllocateInfo.descriptorPool = device->mDescriptorPool;
	ptr->mDescriptorSetAllocateInfo.descriptorSetCount = 1;
	//mDescriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

	ptr->mPipelineLayoutCreateInfo.setLayoutCount = 1;

	ptr->mPipelineShaderStageCreateInfo[0].stage = vk::ShaderStageFlagBits::eVertex;
	ptr->mPipelineShaderStageCreateInfo[0].module = ptr->mVertShaderModule_XYZ_DIFFUSE;
	ptr->mPipelineShaderStageCreateInfo[0].pName = "main";
	ptr->mPipelineShaderStageCreateInfo[0].pSpecializationInfo = &ptr->mVertexSpecializationInfo;

	ptr->mPipelineShaderStageCreateInfo[1].stage = vk::ShaderStageFlagBits::eFragment;
	ptr->mPipelineShaderStageCreateInfo[1].module = ptr->mFragShaderModule_XYZ_DIFFUSE;
	ptr->mPipelineShaderStageCreateInfo[1].pName = "main";
	ptr->mPipelineShaderStageCreateInfo[1].pSpecializationInfo = &ptr->mPixelSpecializationInfo;

	ptr->mPipelineShaderStageCreateInfo[2].stage = vk::ShaderStageFlagBits::eGeometry;
	ptr->mPipelineShaderStageCreateInfo[2].module = ptr->mGeomShaderModule_XYZ_DIFFUSE;
	ptr->mPipelineShaderStageCreateInfo[2].pName = "main";
	ptr->mPipelineShaderStageCreateInfo[2].pSpecializationInfo = &ptr->mVertexSpecializationInfo;

	ptr->mGraphicsPipelineCreateInfo.pVertexInputState = &ptr->mPipelineVertexInputStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.pInputAssemblyState = &ptr->mPipelineInputAssemblyStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.pRasterizationState = &ptr->mPipelineRasterizationStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.pColorBlendState = &ptr->mPipelineColorBlendStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.pDepthStencilState = &ptr->mPipelineDepthStencilStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.pViewportState = &ptr->mPipelineViewportStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.pMultisampleState = &ptr->mPipelineMultisampleStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.pStages = ptr->mPipelineShaderStageCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.renderPass = ptr->mStoreRenderPass;
	ptr->mGraphicsPipelineCreateInfo.pDynamicState = &ptr->mPipelineDynamicStateCreateInfo;
	ptr->mGraphicsPipelineCreateInfo.stageCount = 2;

	result = device->mDevice.createPipelineCache(&ptr->mPipelineCacheCreateInfo, nullptr, &ptr->mPipelineCache);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreatePipelineCache failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	/*
	Setup the texture to be written into the descriptor set.
	*/
	const vk::Format textureFormat = vk::Format::eB8G8R8A8Unorm;
	vk::FormatProperties formatProperties;
	const uint32_t textureColors[2] = { 0xffff0000, 0xff00ff00 };
	const int32_t textureWidth = 1;
	const int32_t textureHeight = 1;

	physicaldevice.getFormatProperties(textureFormat, &formatProperties);

	vk::ImageCreateInfo imageCreateInfo2;
	imageCreateInfo2.imageType = vk::ImageType::e2D;
	imageCreateInfo2.format = textureFormat;
	imageCreateInfo2.extent = vk::Extent3D((uint32_t)textureWidth, (uint32_t)textureHeight, 1);
	imageCreateInfo2.mipLevels = 1;
	imageCreateInfo2.arrayLayers = 1;
	imageCreateInfo2.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo2.tiling = vk::ImageTiling::eLinear;
	imageCreateInfo2.usage = vk::ImageUsageFlagBits::eSampled;
	//imageCreateInfo2.flags = 0;
	imageCreateInfo2.initialLayout = vk::ImageLayout::ePreinitialized;

	vk::MemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.allocationSize = 0;
	memoryAllocateInfo.memoryTypeIndex = 0;

	vk::MemoryRequirements memoryRequirements2;

	result = device->mDevice.createImage(&imageCreateInfo2, nullptr, &ptr->mImage);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device->mDevice.getImageMemoryRequirements(ptr->mImage, &memoryRequirements2);
	memoryAllocateInfo.allocationSize = memoryRequirements2.size;
	GetMemoryTypeFromProperties(device->mPhysicalDeviceMemoryProperties, memoryRequirements2.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible, &memoryAllocateInfo.memoryTypeIndex);

	result = device->mDevice.allocateMemory(&memoryAllocateInfo, nullptr, &ptr->mDeviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device->mDevice.bindImageMemory(ptr->mImage, ptr->mDeviceMemory, 0);

	vk::ImageSubresource imageSubresource;
	imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageSubresource.mipLevel = 0;
	imageSubresource.arrayLayer = 0;

	vk::SubresourceLayout subresourceLayout;
	void* data = nullptr;
	int32_t x = 0;
	int32_t y = 0;

	device->mDevice.getImageSubresourceLayout(ptr->mImage, &imageSubresource, &subresourceLayout);

	data = device->mDevice.mapMemory(ptr->mDeviceMemory, 0, memoryAllocateInfo.allocationSize, vk::MemoryMapFlags()).value;
	if (data == nullptr)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkMapMemory failed.";
		return;
	}

	for (y = 0; y < textureHeight; y++)
	{
		uint32_t *row = (uint32_t *)((char *)data + subresourceLayout.rowPitch * y);
		for (x = 0; x < textureWidth; x++)
		{
			row[x] = textureColors[(x & 1) ^ (y & 1)];
		}
	}

	device->mDevice.unmapMemory(ptr->mDeviceMemory);
	ptr->mImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	ptr->SetImageLayout(ptr->mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::ePreinitialized, ptr->mImageLayout);

	vk::SamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.magFilter = vk::Filter::eNearest;
	samplerCreateInfo.minFilter = vk::Filter::eNearest;
	samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
	samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1;
	samplerCreateInfo.compareOp = vk::CompareOp::eNever;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	result = device->mDevice.createSampler(&samplerCreateInfo, NULL, &ptr->mSampler);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateSampler failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageViewCreateInfo imageViewCreateInfo2;
	imageViewCreateInfo2.image = ptr->mImage;
	imageViewCreateInfo2.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo2.format = textureFormat;
	imageViewCreateInfo2.components =
	{
		VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
		VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
	};
	imageViewCreateInfo2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	//imageViewCreateInfo.flags = 0;

	result = device->mDevice.createImageView(&imageViewCreateInfo2, nullptr, &ptr->mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	for (size_t i = 0; i < 16; i++)
	{
		ptr->mDeviceState.mDescriptorImageInfo[i].sampler = ptr->mSampler;
		ptr->mDeviceState.mDescriptorImageInfo[i].imageView = ptr->mImageView;
		ptr->mDeviceState.mDescriptorImageInfo[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	}

	//mWriteDescriptorSet[0].dstSet = descriptorSet;
	ptr->mWriteDescriptorSet[0].dstBinding = 0;
	ptr->mWriteDescriptorSet[0].dstArrayElement = 0;
	ptr->mWriteDescriptorSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
	ptr->mWriteDescriptorSet[0].descriptorCount = 1;
	ptr->mWriteDescriptorSet[0].pBufferInfo = &ptr->mDescriptorBufferInfo[0];

	//mWriteDescriptorSet[1].dstSet = descriptorSet;
	ptr->mWriteDescriptorSet[1].dstBinding = 1;
	ptr->mWriteDescriptorSet[1].dstArrayElement = 0;
	ptr->mWriteDescriptorSet[1].descriptorType = vk::DescriptorType::eUniformBuffer;
	ptr->mWriteDescriptorSet[1].descriptorCount = 1;
	ptr->mWriteDescriptorSet[1].pBufferInfo = &ptr->mDescriptorBufferInfo[1];

	//mWriteDescriptorSet[2].dstSet = descriptorSet;
	ptr->mWriteDescriptorSet[2].dstBinding = 2;
	ptr->mWriteDescriptorSet[2].dstArrayElement = 0;
	ptr->mWriteDescriptorSet[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
	ptr->mWriteDescriptorSet[2].descriptorCount = 1;
	ptr->mWriteDescriptorSet[2].pImageInfo = ptr->mDeviceState.mDescriptorImageInfo;

	ptr->mCommandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
	ptr->mCommandBufferAllocateInfo.commandPool = ptr->mCommandPool;
	ptr->mCommandBufferAllocateInfo.commandBufferCount = 1;

	device->mDevice.allocateCommandBuffers(&ptr->mCommandBufferAllocateInfo, &ptr->mCommandBuffer);

	ptr->mBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	ptr->mSubmitInfo.commandBufferCount = 1;
	ptr->mSubmitInfo.pCommandBuffers = &ptr->mCommandBuffer;

	//revisit - light should be sized dynamically. Really more that 4 lights is stupid but this limit isn't correct behavior.
	ptr->CreateBuffer(sizeof(Light) * 4, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, ptr->mLightBuffer, ptr->mLightBufferMemory);
	ptr->CreateBuffer(sizeof(D3DMATERIAL9), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, ptr->mMaterialBuffer, ptr->mMaterialBufferMemory);
}

void StateManager::DestroyInstance(size_t id)
{
	mInstances[id].reset();
}

void StateManager::CreateInstance()
{
	auto ptr = std::make_shared<RealInstance>();

	boost::container::small_vector<char*, 16> extensionNames;
	boost::container::small_vector<char*, 16> layerNames;

	extensionNames.push_back("VK_KHR_surface");
	extensionNames.push_back("VK_KHR_win32_surface");
	extensionNames.push_back("VK_KHR_get_physical_device_properties2");

#ifdef _DEBUG
	extensionNames.push_back("VK_EXT_debug_report");
	layerNames.push_back("VK_LAYER_LUNARG_standard_validation");

	HINSTANCE instance = LoadLibraryA("renderdoc.dll");
	HMODULE mod = GetModuleHandleA("renderdoc.dll");
	if (mod != nullptr)
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		if (RENDERDOC_GetAPI != nullptr)
		{
			int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_1, (void **)&ptr->mRenderDocApi);
			if (ret != 1)
			{
				BOOST_LOG_TRIVIAL(warning) << "StateManager::CreateInstance unable to find RENDERDOC_API_Version_1_1_ !";
			}
			else
			{
				ptr->mRenderDocApi->SetLogFilePathTemplate("vk");
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "StateManager::CreateInstance unable to find RENDERDOC_GetAPI !";
		}

	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "StateManager::CreateInstance unable to find renderdoc.dll !";
	}
#endif

	vk::Result result;
	vk::ApplicationInfo applicationInfo("VK9", 1, "VK9", 1, 0);
	vk::InstanceCreateInfo createInfo({}, &applicationInfo, layerNames.size(), layerNames.data(), extensionNames.size(), extensionNames.data());

	//Get an instance handle.
	result = vk::createInstance(&createInfo, nullptr, &ptr->mInstance);
	if (result == vk::Result::eSuccess)
	{
		pfn_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(ptr->mInstance.getProcAddr("vkCreateDebugReportCallbackEXT"));
		pfn_vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(ptr->mInstance.getProcAddr("vkDebugReportMessageEXT"));
		pfn_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(ptr->mInstance.getProcAddr("vkDestroyDebugReportCallbackEXT"));

#ifdef _DEBUG
		vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.flags = vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::ePerformanceWarning;
		callbackCreateInfo.pfnCallback = &DebugReportCallback;
		callbackCreateInfo.pUserData = this;

		result = ptr->mInstance.createDebugReportCallbackEXT(&callbackCreateInfo, nullptr, &ptr->mCallback);
		if (result == vk::Result::eSuccess)
		{
			BOOST_LOG_TRIVIAL(info) << "StateManager::CreateInstance vkCreateDebugReportCallbackEXT succeeded.";
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance vkCreateDebugReportCallbackEXT failed with return code of " << GetResultString((VkResult)result);
			return;
		}
#endif

		//Fetch an array of available physical devices.
		result = ptr->mInstance.enumeratePhysicalDevices(&ptr->mPhysicalDeviceCount, nullptr);
		if (result == vk::Result::eSuccess)
		{
			ptr->mPhysicalDevices = new vk::PhysicalDevice[ptr->mPhysicalDeviceCount];
			ptr->mInstance.enumeratePhysicalDevices(&ptr->mPhysicalDeviceCount, ptr->mPhysicalDevices);

			for (size_t i = 0; i < ptr->mPhysicalDeviceCount; i++)
			{
				auto& physicalDevice = ptr->mPhysicalDevices[i];
				auto device = std::make_shared<RealDevice>();

				//Grab the properties for GetAdapterIdentifier and other calls.
				physicalDevice.getProperties(&device->mPhysicalDeviceProperties);

				//Grab the features for GetDeviceCaps and other calls.
				physicalDevice.getFeatures(&device->mPhysicalDeviceFeatures);

				//Grab the memory properties for CDevice init and other calls.
				physicalDevice.getMemoryProperties(&device->mPhysicalDeviceMemoryProperties);

				//QueueFamilyProperties
				physicalDevice.getQueueFamilyProperties(&device->mQueueFamilyPropertyCount, nullptr);
				device->mQueueFamilyProperties = new vk::QueueFamilyProperties[device->mQueueFamilyPropertyCount];
				physicalDevice.getQueueFamilyProperties(&device->mQueueFamilyPropertyCount, device->mQueueFamilyProperties);

				//Create Actual Device
				extensionNames.clear();
				layerNames.clear();

				extensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
				//extensionNames.push_back("VK_KHR_maintenance1");
				extensionNames.push_back("VK_KHR_push_descriptor");
				//extensionNames.push_back("VK_KHR_sampler_mirror_clamp_to_edge");
#ifdef _DEBUG
				layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // _DEBUG

				float queue_priorities[1] = { 0.0 };
				vk::DeviceQueueCreateInfo queue_info = {};
				queue_info.queueCount = 1;
				queue_info.pQueuePriorities = queue_priorities;

				vk::DeviceCreateInfo device_info = {};
				device_info.queueCreateInfoCount = 1;
				device_info.pQueueCreateInfos = &queue_info;
				device_info.enabledExtensionCount = extensionNames.size();
				device_info.ppEnabledExtensionNames = extensionNames.data();
				device_info.enabledLayerCount = layerNames.size();
				device_info.ppEnabledLayerNames = layerNames.data();
				device_info.pEnabledFeatures = &device->mPhysicalDeviceFeatures; //Enable all available because we don't know ahead of time what features will be used.
				
				device->mPhysicalDevice = physicalDevice;
				result = physicalDevice.createDevice(&device_info, nullptr, &device->mDevice);
				if (result == vk::Result::eSuccess)
				{
					if (!i)
					{
						pfn_vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(device->mDevice.getProcAddr("vkCmdPushDescriptorSetKHR"));
					}

					vk::DescriptorPoolSize descriptorPoolSizes[11] = {};
					descriptorPoolSizes[0].type = vk::DescriptorType::eSampler; //VK_DESCRIPTOR_TYPE_SAMPLER;
					descriptorPoolSizes[0].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetSamplers);
					descriptorPoolSizes[1].type = vk::DescriptorType::eCombinedImageSampler; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorPoolSizes[1].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers);
					descriptorPoolSizes[2].type = vk::DescriptorType::eSampledImage; //VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					descriptorPoolSizes[2].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetSampledImages);
					descriptorPoolSizes[3].type = vk::DescriptorType::eStorageImage; //VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					descriptorPoolSizes[3].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetStorageImages);
					descriptorPoolSizes[4].type = vk::DescriptorType::eUniformTexelBuffer; //VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
					descriptorPoolSizes[4].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages);
					descriptorPoolSizes[5].type = vk::DescriptorType::eStorageTexelBuffer; //VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
					descriptorPoolSizes[5].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageImages);
					descriptorPoolSizes[6].type = vk::DescriptorType::eUniformBuffer; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorPoolSizes[6].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
					descriptorPoolSizes[7].type = vk::DescriptorType::eStorageBuffer; //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					descriptorPoolSizes[7].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffers);
					descriptorPoolSizes[8].type = vk::DescriptorType::eUniformBufferDynamic; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
					descriptorPoolSizes[8].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
					descriptorPoolSizes[9].type = vk::DescriptorType::eStorageBufferDynamic; //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
					descriptorPoolSizes[9].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic);
					descriptorPoolSizes[10].type = vk::DescriptorType::eInputAttachment; //VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
					descriptorPoolSizes[10].descriptorCount = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments);

					vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
					descriptorPoolCreateInfo.maxSets = std::min((uint32_t)MAX_DESCRIPTOR, device->mPhysicalDeviceProperties.limits.maxDescriptorSetSamplers);
					descriptorPoolCreateInfo.poolSizeCount = 11;
					descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
					descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet; //This flag allows descriptors to return to the pool when they are freed.

					result = device->mDevice.createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &device->mDescriptorPool);
					if (result == vk::Result::eSuccess)
					{
						//That's all I think.				
					}
					else
					{
						BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance vkCreateDescriptorPool failed with return code of " << GetResultString((VkResult)result);
					}
					ptr->mDevices.push_back(device);
				}
				else
				{
					BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance vkCreateDevice failed with return code of " << GetResultString((VkResult)result);
				}
			} //for
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance No physical devices were found.";
		}
		mInstances.push_back(ptr);
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance failed to create vulkan instance.";
	}
}

void StateManager::DestroyVertexBuffer(size_t id)
{
	mVertexBuffers[id].reset();
}

void StateManager::CreateVertexBuffer(size_t id, void* argument1)
{
	vk::Result result;
	auto window = mWindows[id];
	CVertexBuffer9* vertexBuffer9 = bit_cast<CVertexBuffer9*>(argument1);
	auto ptr = std::make_shared<RealVertexBuffer>(window.get());

	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.size = vertexBuffer9->mLength;
	bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
	//bufferCreateInfo.flags = 0;

	vk::MemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.allocationSize = 0;
	memoryAllocateInfo.memoryTypeIndex = 0;

	result = window->mRealDevice->mDevice.createBuffer(&bufferCreateInfo, nullptr, &ptr->mBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVertexBuffer vkCreateBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	ptr->mMemoryRequirements = window->mRealDevice->mDevice.getBufferMemoryRequirements(ptr->mBuffer);

	memoryAllocateInfo.allocationSize = ptr->mMemoryRequirements.size;

	GetMemoryTypeFromProperties(window->mRealDevice->mPhysicalDeviceMemoryProperties, ptr->mMemoryRequirements.memoryTypeBits, (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent), &memoryAllocateInfo.memoryTypeIndex);

	result = window->mRealDevice->mDevice.allocateMemory(&memoryAllocateInfo, nullptr, &ptr->mMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVertexBuffer vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	window->mRealDevice->mDevice.bindBufferMemory(ptr->mBuffer, ptr->mMemory, 0);

	uint32_t attributeStride = 0;

	if (vertexBuffer9->mFVF)
	{
		if ((vertexBuffer9->mFVF & D3DFVF_XYZ) == D3DFVF_XYZ)
		{
			attributeStride += (sizeof(float) * 3);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
		{
			attributeStride += sizeof(uint32_t);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX1) == D3DFVF_TEX1)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX2) == D3DFVF_TEX2)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX3) == D3DFVF_TEX3)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX4) == D3DFVF_TEX4)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX5) == D3DFVF_TEX5)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX6) == D3DFVF_TEX6)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX7) == D3DFVF_TEX7)
		{
			attributeStride += (sizeof(float) * 2);
		}

		if ((vertexBuffer9->mFVF & D3DFVF_TEX8) == D3DFVF_TEX8)
		{
			attributeStride += (sizeof(float) * 2);
		}

		//mSize = mLength / (sizeof(float)*3 + sizeof(DWORD));
		ptr->mSize = vertexBuffer9->mLength / attributeStride;
	}
	else
	{
		ptr->mSize = vertexBuffer9->mLength / sizeof(float); //For now assume one float. There should be at least 4 bytes.
	}

	vertexBuffer9->mSize = ptr->mSize;

	mVertexBuffers.push_back(ptr);
}

void StateManager::DestroyIndexBuffer(size_t id)
{
	mIndexBuffers[id].reset();
}

void StateManager::CreateIndexBuffer(size_t id, void* argument1)
{
	vk::Result result;
	auto window = mWindows[id];
	CIndexBuffer9* indexBuffer9 = bit_cast<CIndexBuffer9*>(argument1);
	auto ptr = std::make_shared<RealIndexBuffer>(window.get());

	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.size = indexBuffer9->mLength;
	bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
	//bufferCreateInfo.flags = 0;

	vk::MemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.allocationSize = 0;
	memoryAllocateInfo.memoryTypeIndex = 0;

	result = window->mRealDevice->mDevice.createBuffer(&bufferCreateInfo, nullptr, &ptr->mBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateIndexBuffer vkCreateBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	ptr->mMemoryRequirements = window->mRealDevice->mDevice.getBufferMemoryRequirements(ptr->mBuffer);

	memoryAllocateInfo.allocationSize = ptr->mMemoryRequirements.size;

	GetMemoryTypeFromProperties(window->mRealDevice->mPhysicalDeviceMemoryProperties, ptr->mMemoryRequirements.memoryTypeBits, (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent), &memoryAllocateInfo.memoryTypeIndex);

	result = window->mRealDevice->mDevice.allocateMemory(&memoryAllocateInfo, nullptr, &ptr->mMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateIndexBuffer vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	window->mRealDevice->mDevice.bindBufferMemory(ptr->mBuffer, ptr->mMemory, 0);

	switch (indexBuffer9->mFormat)
	{
	case D3DFMT_INDEX16:
		ptr->mIndexType = vk::IndexType::eUint16;
		indexBuffer9->mSize = indexBuffer9->mLength / sizeof(uint16_t); //WORD
		break;
	case D3DFMT_INDEX32:
		ptr->mIndexType = vk::IndexType::eUint32;
		indexBuffer9->mSize = indexBuffer9->mLength / sizeof(uint32_t);
		break;
	default:
		BOOST_LOG_TRIVIAL(fatal) << "CIndexBuffer9::CIndexBuffer9 invalid D3DFORMAT of " << indexBuffer9->mFormat;
		break;
	}

	ptr->mSize = indexBuffer9->mSize;

	mIndexBuffers.push_back(ptr);
}

void StateManager::DestroyTexture(size_t id)
{
	mTextures[id]->mRealWindow->mEstimatedMemoryUsed -= mTextures[id]->mMemoryAllocateInfo.allocationSize;
	mTextures[id].reset();
}

void StateManager::CreateTexture(size_t id, void* argument1)
{
	vk::Result result;
	auto window = mWindows[id];
	CTexture9* texture9 = bit_cast<CTexture9*>(argument1);
	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(window.get());
	auto& device = window->mRealDevice->mDevice;

	ptr->mRealFormat = ConvertFormat(texture9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateTexture unknown format: " << texture9->mFormat;
	}

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = ptr->mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = vk::Extent3D(texture9->mWidth, texture9->mHeight, 1);
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
	//imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;

	result = device.createImage(&imageCreateInfo, nullptr, &ptr->mImage);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateTexture vkCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::MemoryRequirements memoryRequirements;
	device.getImageMemoryRequirements(ptr->mImage, &memoryRequirements);

	//mMemoryAllocateInfo.allocationSize = 0;
	ptr->mMemoryAllocateInfo.memoryTypeIndex = 0;
	ptr->mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	window->mEstimatedMemoryUsed += memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(window->mRealDevice->mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &ptr->mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateTexture Could not find memory type from properties.";
		return;
	}

	result = device.allocateMemory(&ptr->mMemoryAllocateInfo, nullptr, &ptr->mDeviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateTexture vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device.bindImageMemory(ptr->mImage, ptr->mDeviceMemory, 0);

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = ptr->mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = ptr->mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = device.createImageView(&imageViewCreateInfo, nullptr, &ptr->mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateTexture vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mTextures.push_back(ptr);
}

void StateManager::DestroyCubeTexture(size_t id)
{
	mTextures[id]->mRealWindow->mEstimatedMemoryUsed -= mTextures[id]->mMemoryAllocateInfo.allocationSize;
	mTextures[id].reset();
}

void StateManager::CreateCubeTexture(size_t id, void* argument1)
{
	vk::Result result;
	std::shared_ptr<RealWindow> window = mWindows[id];
	CCubeTexture9* texture9 = bit_cast<CCubeTexture9*>(argument1);
	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(window.get());
	auto& device = window->mRealDevice->mDevice;

	ptr->mRealFormat = ConvertFormat(texture9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture unknown format: " << texture9->mFormat;
	}

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = ptr->mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = vk::Extent3D(texture9->mEdgeLength, texture9->mEdgeLength, 1);
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 6;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
	imageCreateInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	result = device.createImage(&imageCreateInfo, nullptr, &ptr->mImage);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImage failed with return code of " << GetResultString((VkResult)result);
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImage format:" << (VkFormat)imageCreateInfo.format;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImage imageType:" << (VkImageType)imageCreateInfo.imageType;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImage tiling:" << (VkImageTiling)imageCreateInfo.tiling;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImage usage:" << (VkImageUsageFlags)imageCreateInfo.usage;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImage flags:" << (VkImageCreateFlags)imageCreateInfo.flags;
		return;
	}

	vk::MemoryRequirements memoryRequirements;
	window->mRealDevice->mDevice.getImageMemoryRequirements(ptr->mImage, &memoryRequirements);

	//mMemoryAllocateInfo.allocationSize = 0;
	ptr->mMemoryAllocateInfo.memoryTypeIndex = 0;
	ptr->mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	window->mEstimatedMemoryUsed += memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(window->mRealDevice->mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &ptr->mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture Could not find memory type from properties.";
		return;
	}

	result = device.allocateMemory(&ptr->mMemoryAllocateInfo, nullptr, &ptr->mDeviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device.bindImageMemory(ptr->mImage, ptr->mDeviceMemory, 0);

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = ptr->mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::eCube; //e2D
	imageViewCreateInfo.format = ptr->mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	//imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 6;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = device.createImageView(&imageViewCreateInfo, nullptr, &ptr->mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateCubeTexture vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mTextures.push_back(ptr);
}

void StateManager::DestroyVolumeTexture(size_t id)
{
	mTextures[id]->mRealWindow->mEstimatedMemoryUsed -= mTextures[id]->mMemoryAllocateInfo.allocationSize;
	mTextures[id].reset();
}

void StateManager::CreateVolumeTexture(size_t id, void* argument1)
{
	vk::Result result;
	std::shared_ptr<RealWindow> window = mWindows[id];
	CVolumeTexture9* texture9 = bit_cast<CVolumeTexture9*>(argument1);
	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(window.get());
	auto& device = window->mRealDevice->mDevice;

	ptr->mRealFormat = ConvertFormat(texture9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture unknown format: " << texture9->mFormat;
	}

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e3D;
	imageCreateInfo.format = ptr->mRealFormat;
	imageCreateInfo.extent = vk::Extent3D(texture9->mWidth, texture9->mHeight, texture9->mDepth);
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
	//imageCreateInfo.flags = vk::ImageCreateFlagBits::eVolumeCompatible;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	result = device.createImage(&imageCreateInfo, nullptr, &ptr->mImage);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImage failed with return code of " << GetResultString((VkResult)result);
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImage format:" << (VkFormat)imageCreateInfo.format;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImage imageType:" << (VkImageType)imageCreateInfo.imageType;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImage tiling:" << (VkImageTiling)imageCreateInfo.tiling;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImage usage:" << (VkImageUsageFlags)imageCreateInfo.usage;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImage flags:" << (VkImageCreateFlags)imageCreateInfo.flags;
		return;
	}

	vk::MemoryRequirements memoryRequirements;
	window->mRealDevice->mDevice.getImageMemoryRequirements(ptr->mImage, &memoryRequirements);

	//mMemoryAllocateInfo.allocationSize = 0;
	ptr->mMemoryAllocateInfo.memoryTypeIndex = 0;
	ptr->mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	window->mEstimatedMemoryUsed += memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(window->mRealDevice->mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &ptr->mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture Could not find memory type from properties.";
		return;
	}

	result = device.allocateMemory(&ptr->mMemoryAllocateInfo, nullptr, &ptr->mDeviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	device.bindImageMemory(ptr->mImage, ptr->mDeviceMemory, 0);

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = ptr->mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::e3D;
	imageViewCreateInfo.format = ptr->mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	//imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = device.createImageView(&imageViewCreateInfo, nullptr, &ptr->mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolumeTexture vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mTextures.push_back(ptr);
}

void StateManager::DestroySurface(size_t id)
{
	mSurfaces[id].reset();
}

void StateManager::CreateSurface(size_t id, void* argument1)
{
	vk::Result result;
	auto window = mWindows[id];
	CSurface9* surface9 = bit_cast<CSurface9*>(argument1);
	std::shared_ptr<RealSurface> ptr = std::make_shared<RealSurface>(window.get());

	ptr->mRealFormat = ConvertFormat(surface9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		if (surface9->mFormat > 199)
		{
			char four[5] =
			{
				(char)(surface9->mFormat & 0xFF),
				(char)((surface9->mFormat >> 8) & 0xFF),
				(char)((surface9->mFormat >> 16) & 0xFF),
				(char)((surface9->mFormat >> 24) & 0xFF),
				'\0'
			};

			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface unknown format: " << four;
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface unknown format: " << surface9->mFormat;
		}

	}

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = ptr->mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = vk::Extent3D(surface9->mWidth, surface9->mHeight, 1);
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eLinear;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc;
	imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;
	ptr->mExtent = imageCreateInfo.extent;

	//if (surface9->mCubeTexture != nullptr)
	//{
	//	imageCreateInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
	//}

	result = window->mRealDevice->mDevice.createImage(&imageCreateInfo, nullptr, &ptr->mStagingImage);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkCreateImage failed with return code of " << GetResultString((VkResult)result);
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkCreateImage format:" << (VkFormat)imageCreateInfo.format;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkCreateImage imageType:" << (VkImageType)imageCreateInfo.imageType;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkCreateImage tiling:" << (VkImageTiling)imageCreateInfo.tiling;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkCreateImage usage:" << (VkImageUsageFlags)imageCreateInfo.usage;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkCreateImage flags:" << (VkImageCreateFlags)imageCreateInfo.flags;
		return;
	}

	vk::MemoryRequirements memoryRequirements;
	window->mRealDevice->mDevice.getImageMemoryRequirements(ptr->mStagingImage, &memoryRequirements);

	//mMemoryAllocateInfo.allocationSize = 0;
	ptr->mMemoryAllocateInfo.memoryTypeIndex = 0;
	ptr->mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(window->mRealDevice->mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &ptr->mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface Could not find memory type from properties.";
		return;
	}

	result = window->mRealDevice->mDevice.allocateMemory(&ptr->mMemoryAllocateInfo, nullptr, &ptr->mStagingDeviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	window->mRealDevice->mDevice.bindImageMemory(ptr->mStagingImage, ptr->mStagingDeviceMemory, 0);

	ptr->mSubresource.mipLevel = 0;
	ptr->mSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	ptr->mSubresource.arrayLayer = 0; //if this is wrong you may get 4294967296.

	window->mRealDevice->mDevice.getImageSubresourceLayout(ptr->mStagingImage, &ptr->mSubresource, &ptr->mLayouts[0]);

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = ptr->mStagingImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::e3D;
	imageViewCreateInfo.format = ptr->mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (surface9->mFormat)
	{
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = window->mRealDevice->mDevice.createImageView(&imageViewCreateInfo, nullptr, &ptr->mStagingImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateSurface vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mSurfaces.push_back(ptr);
}

void StateManager::DestroyVolume(size_t id)
{
	mSurfaces[id].reset();
}

void StateManager::CreateVolume(size_t id, void* argument1)
{
	vk::Result result;
	auto window = mWindows[id];
	CVolume9* volume9 = bit_cast<CVolume9*>(argument1);
	std::shared_ptr<RealSurface> ptr = std::make_shared<RealSurface>(window.get());

	ptr->mRealFormat = ConvertFormat(volume9->mFormat);

	if (ptr->mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		if (volume9->mFormat > 199)
		{
			char four[5] =
			{
				(char)(volume9->mFormat & 0xFF),
				(char)((volume9->mFormat >> 8) & 0xFF),
				(char)((volume9->mFormat >> 16) & 0xFF),
				(char)((volume9->mFormat >> 24) & 0xFF),
				'\0'
			};

			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume unknown format: " << four;
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume unknown format: " << volume9->mFormat;
		}

	}

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e3D;
	imageCreateInfo.format = ptr->mRealFormat;
	imageCreateInfo.extent = vk::Extent3D(volume9->mWidth, volume9->mHeight, volume9->mDepth);
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eLinear;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc;
	imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;

	//if (Volume9->mCubeTexture != nullptr)
	//{
	//	imageCreateInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
	//}

	result = window->mRealDevice->mDevice.createImage(&imageCreateInfo, nullptr, &ptr->mStagingImage);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume vkCreateImage failed with return code of " << GetResultString((VkResult)result);
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume vkCreateImage format:" << (VkFormat)imageCreateInfo.format;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume vkCreateImage imageType:" << (VkImageType)imageCreateInfo.imageType;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume vkCreateImage tiling:" << (VkImageTiling)imageCreateInfo.tiling;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume vkCreateImage usage:" << (VkImageUsageFlags)imageCreateInfo.usage;
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume vkCreateImage flags:" << (VkImageCreateFlags)imageCreateInfo.flags;
		return;
	}

	vk::MemoryRequirements memoryRequirements;
	window->mRealDevice->mDevice.getImageMemoryRequirements(ptr->mStagingImage, &memoryRequirements);

	//mMemoryAllocateInfo.allocationSize = 0;
	ptr->mMemoryAllocateInfo.memoryTypeIndex = 0;
	ptr->mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(window->mRealDevice->mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &ptr->mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume Could not find memory type from properties.";
		return;
	}

	result = window->mRealDevice->mDevice.allocateMemory(&ptr->mMemoryAllocateInfo, nullptr, &ptr->mStagingDeviceMemory);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateVolume vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	window->mRealDevice->mDevice.bindImageMemory(ptr->mStagingImage, ptr->mStagingDeviceMemory, 0);

	ptr->mSubresource.mipLevel = 0;
	ptr->mSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	ptr->mSubresource.arrayLayer = 0; //if this is wrong you may get 4294967296.

	window->mRealDevice->mDevice.getImageSubresourceLayout(ptr->mStagingImage, &ptr->mSubresource, &ptr->mLayouts[0]);

	mSurfaces.push_back(ptr);
}

void StateManager::DestroyShader(size_t id)
{
	mShaderConverters[id].reset();
}

void StateManager::CreateShader(size_t id, void* argument1, void* argument2, void* argument3)
{
	//vk::Result result;
	auto window = mWindows[id];
	DWORD* pFunction = (DWORD*)(argument1);
	bool isVertex = (bool)(argument2);
	size_t* size = (size_t*)(argument3);

	if (isVertex)
	{
		std::shared_ptr<ShaderConverter> ptr = std::make_shared<ShaderConverter>(window->mRealDevice->mDevice, window->mDeviceState.mVertexShaderConstantSlots);
		ptr->Convert((uint32_t*)pFunction);
		(*size) = ptr->mConvertedShader.Size;
		mShaderConverters.push_back(ptr);
	}
	else
	{
		std::shared_ptr<ShaderConverter> ptr = std::make_shared<ShaderConverter>(window->mRealDevice->mDevice, window->mDeviceState.mPixelShaderConstantSlots);
		ptr->Convert((uint32_t*)pFunction);
		(*size) = ptr->mConvertedShader.Size;
		mShaderConverters.push_back(ptr);
	}

}

void StateManager::DestroyQuery(size_t id)
{
	mQueries[id].reset();
}

void StateManager::CreateQuery(size_t id, void* argument1)
{
	auto window = mWindows[id];
	CQuery9* query9 = bit_cast<CQuery9*>(argument1);
	std::shared_ptr<RealQuery> ptr = std::make_shared<RealQuery>(window.get());

	vk::QueryPoolCreateInfo info;
	info.queryCount = 1;

	/*
	case D3DQUERYTYPE_VCACHE:
	break;
	case D3DQUERYTYPE_RESOURCEMANAGER:
	break;
	case D3DQUERYTYPE_VERTEXSTATS:
	break;
	case D3DQUERYTYPE_PIPELINETIMINGS:
	break;
	case D3DQUERYTYPE_INTERFACETIMINGS:
	break;
	case D3DQUERYTYPE_VERTEXTIMINGS:
	break;
	case D3DQUERYTYPE_PIXELTIMINGS:
	break;
	case D3DQUERYTYPE_BANDWIDTHTIMINGS:
	break;
	case D3DQUERYTYPE_CACHEUTILIZATION:
	break;
	case D3DQUERYTYPE_EVENT:
	break;
	*/

	/*
	eInputAssemblyVertices = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT,
	eInputAssemblyPrimitives = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT,
	eVertexShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT,
	eGeometryShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT,
	eGeometryShaderPrimitives = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT,
	eClippingInvocations = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT,
	eClippingPrimitives = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT,
	eFragmentShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT,
	eTessellationControlShaderPatches = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT,
	eTessellationEvaluationShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT,
	eComputeShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT
	*/

	switch (query9->mType)
	{
	case D3DQUERYTYPE_OCCLUSION:
		info.queryType = vk::QueryType::eOcclusion;
		break;
	case D3DQUERYTYPE_TIMESTAMP:
		info.queryType = vk::QueryType::eTimestamp;
		break;
	case D3DQUERYTYPE_TIMESTAMPDISJOINT:
		info.queryType = vk::QueryType::eTimestamp;
		break;
	case D3DQUERYTYPE_TIMESTAMPFREQ:
		info.queryType = vk::QueryType::eTimestamp;
		break;
	default:
		info.queryType = vk::QueryType::ePipelineStatistics;
		info.pipelineStatistics = vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations;
		BOOST_LOG_TRIVIAL(info) << "StateManager::CreateQuery Unsupported query type " << query9->mType;
		break;
	}

	vk::ResultValue<vk::QueryPool> poolResult = window->mRealDevice->mDevice.createQueryPool(info, nullptr); //&(ptr->mQueryPool)
	if (poolResult.result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateQuery vkCreateQueryPool failed with return code of " << GetResultString((VkResult)poolResult.result);
		return;
	}
	ptr->mQueryPool = poolResult.value;

	mQueries.push_back(ptr);
}

std::shared_ptr<RealSwapChain> StateManager::GetSwapChain(std::shared_ptr<RealWindow> realWindow, HWND handle)
{
	auto it = mSwapChains.find(handle);
	if (it != mSwapChains.end())
	{
		return (*it).second;
	}
	else
	{
		vk::Instance instance = realWindow->mRealInstance->mInstance;
		vk::PhysicalDevice physicalDevice = realWindow->mRealDevice->mPhysicalDevice;
		vk::Device device = realWindow->mRealDevice->mDevice;
		HWND windowHandle = handle;
		uint32_t width = realWindow->mSwapchainExtent.width;
		uint32_t height = realWindow->mSwapchainExtent.height;
		auto output = std::make_shared<RealSwapChain>(instance, physicalDevice, device, windowHandle, width, height);
		mSwapChains[handle] = output;

		return output;
	}
}