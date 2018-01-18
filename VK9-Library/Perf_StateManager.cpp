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

#include "Perf_StateManager.h"

#include "C9.h"
#include "CDevice9.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/format.hpp>

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

RealWindow::RealWindow()
{

}

RealWindow::~RealWindow()
{

}

RealDevice::RealDevice()
{

}

RealDevice::~RealDevice()
{
	delete[] mQueueFamilyProperties;
	mDevice.destroyDescriptorPool(mDescriptorPool);
	mDevice.destroy();
}

RealInstance::RealInstance()
{

}

RealInstance::~RealInstance()
{
#ifdef _DEBUG
	mInstance.destroyDebugReportCallbackEXT(mCallback);
#endif
	mInstance.destroy();
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
	auto ptr = std::make_shared<RealWindow>();
	vk::Result result;
	auto instance = mInstances[id];
	CDevice9* device9 = (CDevice9*)argument1;
	auto& physicaldevice = instance->mPhysicalDevices[device9->mAdapter];
	auto& device = instance->mDevices[device9->mAdapter];
	vk::Bool32 doesSupportPresentation = false;
	vk::Bool32 doesSupportGraphics = false;
	uint32_t graphicsQueueIndex = 0;
	uint32_t presentationQueueIndex = 0;
	vk::Extent2D swapchainExtent = {};
	vk::Format format = vk::Format::eUndefined;
	vk::Format depthFormat = vk::Format::eD16Unorm;
	vk::SurfaceTransformFlagBitsKHR transformFlags;

	vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.hinstance = (HINSTANCE)argument2;
	surfaceCreateInfo.hwnd = device9->mFocusWindow; //hFocusWindow;

	result = instance->mInstance.createWin32SurfaceKHR(&surfaceCreateInfo, nullptr, &ptr->mSurface);
	if (result == vk::Result::eSuccess)
	{
		result = physicaldevice.getSurfaceCapabilitiesKHR(ptr->mSurface, &ptr->mSurfaceCapabilities);
		if (result == vk::Result::eSuccess)
		{
			/*
			Search for queues to use for graphics and presentation.
			It's easier if one queue does both so if we find one that supports both than just exit.
			Otherwise look for one for presentation and one for graphics.
			The index of the queue us stored for later use.
			*/
			for (uint32_t i = 0; i < device.mQueueFamilyPropertyCount; i++)
			{
				result = physicaldevice.getSurfaceSupportKHR(i, ptr->mSurface, &doesSupportPresentation);
				doesSupportGraphics = (device.mQueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlagBits(0);

				if (doesSupportPresentation && doesSupportGraphics)
				{
					graphicsQueueIndex = i;
					presentationQueueIndex = i;
					break;
				}
				else if (doesSupportPresentation && presentationQueueIndex == UINT32_MAX)
				{
					presentationQueueIndex = i;
				}
				else if (doesSupportGraphics && graphicsQueueIndex == UINT32_MAX)
				{
					graphicsQueueIndex = i;
				}
			}

			/*
			Now we need to setup our queues and buffers.
			We'll start with a command pool because that is where we get command buffers from.
			*/
			vk::CommandPoolCreateInfo commandPoolInfo;
			commandPoolInfo.queueFamilyIndex = graphicsQueueIndex; //Found earlier.
			commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

			result = device.mDevice.createCommandPool(&commandPoolInfo, nullptr, &ptr->mCommandPool);
			if (result == vk::Result::eSuccess)
			{
				//Create queue so we can submit command buffers.
				device.mDevice.getQueue(graphicsQueueIndex, 0, &ptr->mQueue); //no result?

				/*
				Now pull some information about the surface so we can create the swapchain correctly.
				*/
				if (ptr->mSurfaceCapabilities.currentExtent.width < (uint32_t)1 || ptr->mSurfaceCapabilities.currentExtent.height < (uint32_t)1)
				{
					//If the height/width are -1 then just set it to the requested size and hope for the best.
					swapchainExtent.width = device9->mPresentationParameters.BackBufferWidth;
					swapchainExtent.height = device9->mPresentationParameters.BackBufferHeight;
				}
				else
				{
					//Apparently the swap chain size must match the surface size if it is defined.
					swapchainExtent = ptr->mSurfaceCapabilities.currentExtent;
					device9->mPresentationParameters.BackBufferWidth = ptr->mSurfaceCapabilities.currentExtent.width;
					device9->mPresentationParameters.BackBufferHeight = ptr->mSurfaceCapabilities.currentExtent.height;
				}

				//initialize vulkan/d3d9 viewport and scissor structures.
				//mDeviceState.mViewport.y = (float)mPresentationParameters.BackBufferHeight;
				mDeviceState.mViewport.width = (float)device9->mPresentationParameters.BackBufferWidth;
				//mDeviceState.mViewport.height = -(float)mPresentationParameters.BackBufferHeight;
				mDeviceState.mViewport.height = (float)device9->mPresentationParameters.BackBufferHeight;
				mDeviceState.mViewport.minDepth = 0.0f;
				mDeviceState.mViewport.maxDepth = 1.0f;

				mDeviceState.m9Viewport.Width = (DWORD)mDeviceState.mViewport.width;
				mDeviceState.m9Viewport.Height = (DWORD)mDeviceState.mViewport.height;
				mDeviceState.m9Viewport.MinZ = mDeviceState.mViewport.minDepth;
				mDeviceState.m9Viewport.MaxZ = mDeviceState.mViewport.maxDepth;

				mDeviceState.mScissor.offset.x = 0;
				mDeviceState.mScissor.offset.y = 0;
				mDeviceState.mScissor.extent.width = device9->mPresentationParameters.BackBufferWidth;
				mDeviceState.mScissor.extent.height = device9->mPresentationParameters.BackBufferHeight;

				mDeviceState.m9Scissor.right = device9->mPresentationParameters.BackBufferWidth;
				mDeviceState.m9Scissor.bottom = device9->mPresentationParameters.BackBufferHeight;
				mDeviceState.m9Scissor.left = 0;
				mDeviceState.m9Scissor.top = 0;

				result = physicaldevice.getSurfaceFormatsKHR(ptr->mSurface, &ptr->mSurfaceFormatCount, nullptr);
				if (result == vk::Result::eSuccess)
				{
					ptr->mSurfaceFormats = new vk::SurfaceFormatKHR[ptr->mSurfaceFormatCount];
					result = physicaldevice.getSurfaceFormatsKHR(ptr->mSurface, &ptr->mSurfaceFormatCount, ptr->mSurfaceFormats);
					if (result == vk::Result::eSuccess)
					{
						if (ptr->mSurfaceFormatCount == 1 && ptr->mSurfaceFormats[0].format == vk::Format::eUndefined)
						{
							format = vk::Format::eB8G8R8A8Unorm; //No preferred format so set a default.
						}
						else
						{
							format = ptr->mSurfaceFormats[0].format; //Pull the preferred format.
						}

						if (ptr->mSurfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
						{
							transformFlags = vk::SurfaceTransformFlagBitsKHR::eIdentity;
						}
						else
						{
							transformFlags = ptr->mSurfaceCapabilities.currentTransform;
						}

						result = physicaldevice.getSurfacePresentModesKHR(ptr->mSurface, &ptr->mPresentationModeCount, nullptr);
						if (result == vk::Result::eSuccess)
						{
							ptr->mPresentationModes = new vk::PresentModeKHR[ptr->mPresentationModeCount];
							result = physicaldevice.getSurfacePresentModesKHR(ptr->mSurface, &ptr->mPresentationModeCount, ptr->mPresentationModes);
							if (result == vk::Result::eSuccess)
							{
								/*
								Trying modes in order of preference (Mailbox,immediate,FIFO)
								VK_PRESENT_MODE_MAILBOX_KHR - Wait for the next vertical blanking interval to update the image. New images replace the one waiting to be displayed.
								VK_PRESENT_MODE_IMMEDIATE_KHR - Do not wait for vertical blanking to update the image.
								VK_PRESENT_MODE_FIFO_KHR - Wait for the next vertical blanking interval to update the image. If the interval is missed wait for the next one. New images will be queued for display.
								*/
								ptr->mSwapchainPresentMode = vk::PresentModeKHR::eFifo;
								for (size_t i = 0; i < ptr->mPresentationModeCount; i++)
								{
									if (ptr->mPresentationModes[i] == vk::PresentModeKHR::eMailbox)
									{
										ptr->mSwapchainPresentMode = vk::PresentModeKHR::eMailbox;
										break;
									}
									else if (ptr->mPresentationModes[i] == vk::PresentModeKHR::eImmediate)
									{
										ptr->mSwapchainPresentMode = vk::PresentModeKHR::eImmediate;
									} //Already defaulted to FIFO so do nothing for else.
								}

								/*
								Finally create the swap chain based on the information collected.
								This swap chain will handle the work done by the implicit swap chain in D3D9.
								*/
								vk::SwapchainCreateInfoKHR swapchainCreateInfo;
								swapchainCreateInfo.surface = ptr->mSurface;
								swapchainCreateInfo.minImageCount = ptr->mSurfaceCapabilities.minImageCount + 1;
								swapchainCreateInfo.imageFormat = format;
								swapchainCreateInfo.imageColorSpace = ptr->mSurfaceFormats[0].colorSpace;
								swapchainCreateInfo.imageExtent = swapchainExtent;
								swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
								swapchainCreateInfo.preTransform = transformFlags;
								swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
								swapchainCreateInfo.imageArrayLayers = 1;
								swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
								swapchainCreateInfo.queueFamilyIndexCount = 0;
								swapchainCreateInfo.pQueueFamilyIndices = nullptr;
								swapchainCreateInfo.presentMode = ptr->mSwapchainPresentMode;
								swapchainCreateInfo.oldSwapchain = ptr->mSwapchain; //There is no old swapchain yet.
								swapchainCreateInfo.clipped = true;

								result = device.mDevice.createSwapchainKHR(&swapchainCreateInfo, nullptr, &ptr->mSwapchain);
								if (result == vk::Result::eSuccess)
								{
									//Create the images (buffers) that will be used by the swap chain.
									result = device.mDevice.getSwapchainImagesKHR(ptr->mSwapchain, &ptr->mSwapchainImageCount, ptr->mSwapchainImages);
									if (result == vk::Result::eSuccess)
									{
										ptr->mSwapchainImages = new vk::Image[ptr->mSwapchainImageCount];
										ptr->mSwapchainViews = new vk::ImageView[ptr->mSwapchainImageCount];
										ptr->mSwapchainBuffers = new vk::CommandBuffer[ptr->mSwapchainImageCount];

										result = device.mDevice.getSwapchainImagesKHR(ptr->mSwapchain, &ptr->mSwapchainImageCount, ptr->mSwapchainImages);
										if (result == vk::Result::eSuccess)
										{
											for (size_t i = 0; i < ptr->mSwapchainImageCount; i++)
											{
												vk::ImageViewCreateInfo color_image_view;
												color_image_view.format = format;
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

												result = device.mDevice.createImageView(&color_image_view, nullptr, &ptr->mSwapchainViews[i]);
												if (result != vk::Result::eSuccess)
												{
													BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
												}

												vk::CommandBufferAllocateInfo commandBufferInfo;
												commandBufferInfo.commandPool = ptr->mCommandPool;
												commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
												commandBufferInfo.commandBufferCount = 1;

												result = device.mDevice.allocateCommandBuffers(&commandBufferInfo, &ptr->mSwapchainBuffers[i]);
												if (result != vk::Result::eSuccess)
												{
													BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
												}
											} //for

											/*
											Setup Depth stuff.
											*/

											vk::ImageCreateInfo imageCreateInfo;
											imageCreateInfo.imageType = vk::ImageType::e2D;
											imageCreateInfo.format = depthFormat;
											imageCreateInfo.extent = { device9->mPresentationParameters.BackBufferWidth,device9->mPresentationParameters.BackBufferHeight, 1 };
											imageCreateInfo.mipLevels = 1;
											imageCreateInfo.arrayLayers = 1;
											imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
											imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
											imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;

											result = device.mDevice.createImage(&imageCreateInfo, nullptr, &ptr->mDepthImage);
											if (result == vk::Result::eSuccess)
											{
												vk::MemoryRequirements memoryRequirements;
												device.mDevice.getImageMemoryRequirements(ptr->mDepthImage, &memoryRequirements);

												vk::MemoryAllocateInfo depthMemoryAllocateInfo;
												depthMemoryAllocateInfo.memoryTypeIndex = 0;
												depthMemoryAllocateInfo.allocationSize = memoryRequirements.size;

												GetMemoryTypeFromProperties(device.mPhysicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, 0, &depthMemoryAllocateInfo.memoryTypeIndex);

												result = device.mDevice.allocateMemory(&depthMemoryAllocateInfo, nullptr, &ptr->mDepthDeviceMemory);
												if (result == vk::Result::eSuccess)
												{
													//c++ version doesn't result a result code.... I don't think I like that.
													device.mDevice.bindImageMemory(ptr->mDepthImage, ptr->mDepthDeviceMemory, 0);

													SetImageLayout(ptr->mDepthImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

													vk::ImageViewCreateInfo imageViewCreateInfo;
													imageViewCreateInfo.format = depthFormat;
													imageViewCreateInfo.subresourceRange = {};
													imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
													imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
													imageViewCreateInfo.subresourceRange.levelCount = 1;
													imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
													imageViewCreateInfo.subresourceRange.layerCount = 1;
													imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
													imageViewCreateInfo.image = ptr->mDepthImage;

													result = device.mDevice.createImageView(&imageViewCreateInfo, nullptr, &ptr->mDepthView);
													if (result == vk::Result::eSuccess)
													{
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



													}
													else
													{
														BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
													}
												}
												else
												{
													BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkAllocateMemory failed with return code of " << GetResultString((VkResult)result);
												}

											}
											else
											{
												BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateImage failed with return code of " << GetResultString((VkResult)result);
											}
										}
										else
										{
											BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetSwapchainImagesKHR failed with return code of " << GetResultString((VkResult)result);
										}
									}
									else
									{
										BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetSwapchainImagesKHR failed with return code of " << GetResultString((VkResult)result);
									}
								}
								else
								{
									BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateSwapchainKHR failed with return code of " << GetResultString((VkResult)result);
								}
							}
							else
							{
								BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetPhysicalDeviceSurfacePresentModesKHR failed with return code of " << GetResultString((VkResult)result);
							}
						}
						else
						{
							BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetPhysicalDeviceSurfacePresentModesKHR failed with return code of " << GetResultString((VkResult)result);
						}
					}
					else
					{
						BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetPhysicalDeviceSurfaceFormatsKHR failed with return code of " << GetResultString((VkResult)result);
					}
				}
				else
				{
					BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetPhysicalDeviceSurfaceFormatsKHR failed with return code of " << GetResultString((VkResult)result);
				}

				mWindows.push_back(ptr);
			}
			else
			{
				BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateCommandPool failed with return code of " << GetResultString((VkResult)result);
			}		
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed with return code of " << GetResultString((VkResult)result);
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateWindow1 vkCreateWin32SurfaceKHR failed with a return code of " << GetResultString((VkResult)result);
	}
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
				RealDevice device;

				//Grab the properties for GetAdapterIdentifier and other calls.
				physicalDevice.getProperties(&device.mPhysicalDeviceProperties);

				//Grab the features for GetDeviceCaps and other calls.
				physicalDevice.getFeatures(&device.mPhysicalDeviceFeatures);

				//Grab the memory properties for CDevice init and other calls.
				physicalDevice.getMemoryProperties(&device.mPhysicalDeviceMemoryProperties);

				//QueueFamilyProperties
				physicalDevice.getQueueFamilyProperties(&device.mQueueFamilyPropertyCount, nullptr);
				device.mQueueFamilyProperties = new vk::QueueFamilyProperties[device.mQueueFamilyPropertyCount];
				physicalDevice.getQueueFamilyProperties(&device.mQueueFamilyPropertyCount, device.mQueueFamilyProperties);

				//Create Actual Device
				extensionNames.clear();
				layerNames.clear();

				extensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
				extensionNames.push_back("VK_KHR_maintenance1");
				extensionNames.push_back("VK_KHR_push_descriptor");
				extensionNames.push_back("VK_KHR_sampler_mirror_clamp_to_edge");
#ifdef _DEBUG
				extensionNames.push_back("VK_LAYER_LUNARG_standard_validation");
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
				device_info.pEnabledFeatures = &device.mPhysicalDeviceFeatures; //Enable all available because we don't know ahead of time what features will be used.

				result = physicalDevice.createDevice(&device_info, nullptr, &device.mDevice);
				if (result == vk::Result::eSuccess)
				{
					vk::DescriptorPoolSize descriptorPoolSizes[11] = {};
					descriptorPoolSizes[0].type = vk::DescriptorType::eSampler; //VK_DESCRIPTOR_TYPE_SAMPLER;
					descriptorPoolSizes[0].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetSamplers);
					descriptorPoolSizes[1].type = vk::DescriptorType::eCombinedImageSampler; //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorPoolSizes[1].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers);
					descriptorPoolSizes[2].type = vk::DescriptorType::eSampledImage; //VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					descriptorPoolSizes[2].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetSampledImages);
					descriptorPoolSizes[3].type = vk::DescriptorType::eStorageImage; //VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					descriptorPoolSizes[3].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetStorageImages);
					descriptorPoolSizes[4].type = vk::DescriptorType::eUniformTexelBuffer; //VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
					descriptorPoolSizes[4].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages);
					descriptorPoolSizes[5].type = vk::DescriptorType::eStorageTexelBuffer; //VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
					descriptorPoolSizes[5].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageImages);
					descriptorPoolSizes[6].type = vk::DescriptorType::eUniformBuffer; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorPoolSizes[6].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
					descriptorPoolSizes[7].type = vk::DescriptorType::eStorageBuffer; //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					descriptorPoolSizes[7].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffers);
					descriptorPoolSizes[8].type = vk::DescriptorType::eUniformBufferDynamic; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
					descriptorPoolSizes[8].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
					descriptorPoolSizes[9].type = vk::DescriptorType::eStorageBufferDynamic; //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
					descriptorPoolSizes[9].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic);
					descriptorPoolSizes[10].type = vk::DescriptorType::eInputAttachment; //VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
					descriptorPoolSizes[10].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments);

					vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
					descriptorPoolCreateInfo.maxSets = min((uint32_t)MAX_DESCRIPTOR, device.mPhysicalDeviceProperties.limits.maxDescriptorSetSamplers);
					descriptorPoolCreateInfo.poolSizeCount = 11;
					descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
					descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet; //This flag allows descriptors to return to the pool when they are freed.

					result = device.mDevice.createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &device.mDescriptorPool);
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