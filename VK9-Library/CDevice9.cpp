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

#include <algorithm>
#include <limits>

#include "C9.h"
#include "CDevice9.h"
#include "CCubeTexture9.h"
#include "CBaseTexture9.h"
#include "CTexture9.h"
#include "CVolumeTexture9.h"
#include "CSurface9.h"
#include "CVertexDeclaration9.h"
#include "CVertexShader9.h"
#include "CStateBlock9.h"

#include "Utilities.h"

CDevice9::CDevice9(C9* Instance, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters)
	:
	mInstance(Instance),
	mAdapter(Adapter),
	mDeviceType(DeviceType),
	mFocusWindow(hFocusWindow),
	mBehaviorFlags(BehaviorFlags)
{
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 Started.";

	//mInstance->AddRef();

	memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));

	if (mInstance->mGpuCount == 0)
	{
		mResult = VK_RESULT_MAX_ENUM;
		BOOST_LOG_TRIVIAL(fatal) << "No physical devices were found so CDevice9 could not be created.";
		return;
	}
	mPhysicalDevice = mInstance->mPhysicalDevices[mAdapter]; //pull the selected physical device from the instance.

	//Fetch the properties & features from the physical device.
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &mDeviceProperties);
	vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mDeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mDeviceMemoryProperties);

	BOOST_LOG_TRIVIAL(info) << "GPU name: " << mDeviceProperties.deviceName;

	switch (mDeviceProperties.deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		BOOST_LOG_TRIVIAL(info) << "GPU type: Integrated.";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		BOOST_LOG_TRIVIAL(info) << "GPU type: Discrete.";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		BOOST_LOG_TRIVIAL(info) << "GPU type: Virtual.";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		BOOST_LOG_TRIVIAL(info) << "GPU type: CPU.";
		break;
	default:
		break;
	}

	BOOST_LOG_TRIVIAL(info) << "GPU driver version: " << mDeviceProperties.driverVersion;
	BOOST_LOG_TRIVIAL(info) << "GPU API version: " << mDeviceProperties.apiVersion;
	BOOST_LOG_TRIVIAL(info) << "maxPushConstantSize: " << mDeviceProperties.limits.maxPushConstantsSize;

	for (size_t i = 0; i < mDeviceMemoryProperties.memoryHeapCount; i++)
	{
		if ((mDeviceMemoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) == VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
		{
			BOOST_LOG_TRIVIAL(info) << "GPU local heap (" << i << ") size:" << mDeviceMemoryProperties.memoryHeaps[i].size;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "GPU heap (" << i << ") size:" << mDeviceMemoryProperties.memoryHeaps[i].size;
		}
	}

	//Fetch the queue properties.
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &mQueueCount, NULL);
	if (mQueueCount == 0)
	{
		mResult = VK_RESULT_MAX_ENUM;
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetPhysicalDeviceQueueFamilyProperties returned no results.";
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetPhysicalDeviceQueueFamilyProperties returned " << mQueueCount << " results.";
	}
	mQueueFamilyProperties = new VkQueueFamilyProperties[mQueueCount];
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &mQueueCount, mQueueFamilyProperties);

	/*bool found = false;
	for (unsigned int i = 0; i < mQueueCount; i++)
	{
		if (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queue_info.queueFamilyIndex = i;
			found = true;
			break;
		}
	}*/

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr);
	VkExtensionProperties* extension = new VkExtensionProperties[extensionCount];
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, extension);

	for (size_t i = 0; i < extensionCount; i++)
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 extension available: " << extension[i].extensionName;
	}

	delete[] extension;

	mExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#ifdef _DEBUG
	mLayerExtensionNames.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // _DEBUG

	float queue_priorities[1] = { 0.0 };
	VkDeviceQueueCreateInfo queue_info = {};
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = nullptr;
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = queue_priorities;

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = nullptr;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledExtensionCount = mExtensionNames.size();
	device_info.ppEnabledExtensionNames = mExtensionNames.data();
	device_info.enabledLayerCount = mLayerExtensionNames.size();
	device_info.ppEnabledLayerNames = mLayerExtensionNames.data();
	device_info.pEnabledFeatures = &mDeviceFeatures; //Enable all available because we don't know ahead of time what features will be used.

	mResult = vkCreateDevice(mPhysicalDevice, &device_info, nullptr, &mDevice);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateDevice failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateDevice returned a valid device.";
	}

	/*
	Now that the rendering is setup the surface must be created.
	The surface maybe inside of a window or a whole display. (Think SDL)
	*/
	if (mPresentationParameters.Windowed)
	{
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};

		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = nullptr;
		surfaceCreateInfo.flags = 0;
		surfaceCreateInfo.hwnd = hFocusWindow;
		surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

		mResult = vkCreateWin32SurfaceKHR(mInstance->mInstance, &surfaceCreateInfo, nullptr, &mSurface);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateWin32SurfaceKHR failed with a return code of " << mResult;
			return;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateWin32SurfaceKHR succeeded.";
		}
	}
	else
	{
		//TODO: finish full screen support.
		/*vkGetDisplayPlaneSupportedDisplaysKHR(mPhysicalDevice, 0, &mDisplayCount, NULL);
		mDisplays = new VkDisplayKHR[mDisplayCount];
		vkGetDisplayPlaneSupportedDisplaysKHR(mPhysicalDevice, 0, &mDisplayCount, mDisplays);

		//vkGetDisplayModePropertiesKHR(mPhysicalDevice,mDisplays[0])

		VkDisplaySurfaceCreateInfoKHR surfaceCreateInfo = {};

		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = NULL;
		surfaceCreateInfo.flags = 0;



		vkCreateDisplayPlaneSurfaceKHR(mInstance->mInstance,&surfaceCreateInfo,NULL, &mSurface);*/
	}

	mResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &mSurfaceCapabilities);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceCapabilitiesKHR succeeded.";
	}

	/*
	Search for queues to use for graphics and presentation.
	It's easier if one queue does both so if we find one that supports both than just exit.
	Otherwise look for one for presentation and one for graphics.
	The index of the queue us stored for later use.
	*/
	for (size_t i = 0; i < mQueueCount; i++)
	{
		VkBool32 doesSupportPresentation = false;
		VkBool32 doesSupportGraphics = false;

		mResult = vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &doesSupportPresentation);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceSupportKHR failed with return code of " << mResult;
			return;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceSupportKHR succeeded.";
		}

		doesSupportGraphics = ((mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0);

		if (doesSupportPresentation && doesSupportGraphics)
		{
			mGraphicsQueueIndex = i;
			mPresentationQueueIndex = i;
			break;
		}
		else if (doesSupportPresentation && mPresentationQueueIndex == UINT32_MAX)
		{
			mPresentationQueueIndex = i;
		}
		else if (doesSupportGraphics && mGraphicsQueueIndex == UINT32_MAX)
		{
			mGraphicsQueueIndex = i;
		}

	}

	/*
	Now we need to setup our queues and buffers.
	We'll start with a command pool because that is where we get command buffers from.
	*/

	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.queueFamilyIndex = mGraphicsQueueIndex; //Found earlier.
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	mResult = vkCreateCommandPool(mDevice, &commandPoolInfo, nullptr, &mCommandPool);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateCommandPool failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateCommandPool succeeded.";
	}

	//Setup the descriptor pool for resource binding.
	VkDescriptorPoolSize descriptorPoolSizes[11] = {};
#define MAX_DESCRIPTOR 2048

	descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptorPoolSizes[0].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetSamplers);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetSamplers = " << mDeviceProperties.limits.maxDescriptorSetSamplers;

	descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSizes[1].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxPerStageDescriptorSamplers);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxPerStageDescriptorSamplers = " << mDeviceProperties.limits.maxPerStageDescriptorSamplers;

	descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	descriptorPoolSizes[2].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetSampledImages);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetSampledImages = " << mDeviceProperties.limits.maxDescriptorSetSampledImages;

	descriptorPoolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorPoolSizes[3].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetStorageImages);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetStorageImages = " << mDeviceProperties.limits.maxDescriptorSetStorageImages;

	descriptorPoolSizes[4].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	descriptorPoolSizes[4].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxPerStageDescriptorSampledImages);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxPerStageDescriptorSampledImages = " << mDeviceProperties.limits.maxPerStageDescriptorSampledImages;

	descriptorPoolSizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	descriptorPoolSizes[5].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxPerStageDescriptorStorageImages);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxPerStageDescriptorStorageImages = " << mDeviceProperties.limits.maxPerStageDescriptorStorageImages;

	descriptorPoolSizes[6].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSizes[6].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetUniformBuffers);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetUniformBuffers = " << mDeviceProperties.limits.maxDescriptorSetUniformBuffers;

	descriptorPoolSizes[7].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSizes[7].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetStorageBuffers);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetStorageBuffers = " << mDeviceProperties.limits.maxDescriptorSetStorageBuffers;

	descriptorPoolSizes[8].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorPoolSizes[8].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetUniformBuffersDynamic = " << mDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic;

	descriptorPoolSizes[9].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	descriptorPoolSizes[9].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetStorageBuffersDynamic = " << mDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic;

	descriptorPoolSizes[10].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	descriptorPoolSizes[10].descriptorCount = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetInputAttachments);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 maxDescriptorSetInputAttachments = " << mDeviceProperties.limits.maxDescriptorSetInputAttachments;


	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = NULL;
	descriptorPoolCreateInfo.maxSets = min((uint32_t)MAX_DESCRIPTOR, mDeviceProperties.limits.maxDescriptorSetSamplers);
	descriptorPoolCreateInfo.poolSizeCount = 11;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;

	/*
	This flag allows descriptors to return to the pool when they are freed.
	If not set we'll have to reset the pool and frankly I don't want to code for that.
	The only reason I can think that you wouldn't want to do this is if it makes create/destroy cheaper but it hardly seems with it.
	*/
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	mResult = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateDescriptorPool failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateDescriptorPool succeeded.";
	}

	mGarbageManager.mDescriptorPool = mDescriptorPool;

	//Create queue so we can submit command buffers.
	vkGetDeviceQueue(mDevice, mGraphicsQueueIndex, 0, &mQueue);

	/*
	Now pull some information about the surface so we can create the swapchain correctly.
	*/

	if (mSurfaceCapabilities.currentExtent.width < (uint32_t)1 || mSurfaceCapabilities.currentExtent.height < (uint32_t)1)
	{
		//If the height/width are -1 then just set it to the requested size and hope for the best.
		mSwapchainExtent.width = mPresentationParameters.BackBufferWidth;
		mSwapchainExtent.height = mPresentationParameters.BackBufferHeight;
	}
	else
	{
		//Apparently the swap chain size must match the surface size if it is defined.
		mSwapchainExtent = mSurfaceCapabilities.currentExtent;
		mPresentationParameters.BackBufferWidth = mSurfaceCapabilities.currentExtent.width;
		mPresentationParameters.BackBufferHeight = mSurfaceCapabilities.currentExtent.height;
	}

	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 the height/width of the surfaces are " << mPresentationParameters.BackBufferHeight << "/" << mPresentationParameters.BackBufferWidth;

	//initialize vulkan/d3d9 viewport and scissor structures.
	mDeviceState.mViewport.width = (float)mPresentationParameters.BackBufferWidth;
	mDeviceState.mViewport.height = (float)mPresentationParameters.BackBufferHeight;
	mDeviceState.mViewport.minDepth = (float)0.0f;
	mDeviceState.mViewport.maxDepth = (float)1.0f;

	mDeviceState.m9Viewport.Width = mDeviceState.mViewport.width;
	mDeviceState.m9Viewport.Height = mDeviceState.mViewport.height;
	mDeviceState.m9Viewport.MinZ = mDeviceState.mViewport.minDepth;
	mDeviceState.m9Viewport.MaxZ = mDeviceState.mViewport.maxDepth;

	mDeviceState.mScissor.extent.width = mPresentationParameters.BackBufferWidth;
	mDeviceState.mScissor.extent.height = mPresentationParameters.BackBufferHeight;
	mDeviceState.mScissor.offset.x = 0; //Do I really need this if I initialize to zero?
	mDeviceState.mScissor.offset.y = 0; //Do I really need this if I initialize to zero?

	mDeviceState.m9Scissor.right = mDeviceState.mScissor.extent.width;
	mDeviceState.m9Scissor.bottom = mDeviceState.mScissor.extent.height;
	mDeviceState.m9Scissor.left = 0;
	mDeviceState.m9Scissor.top = 0;

	mResult = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &mSurfaceFormatCount, nullptr);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceFormatsKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceFormatsKHR returned " << mSurfaceFormatCount << " surface formats.";
	}
	mSurfaceFormats = new VkSurfaceFormatKHR[mSurfaceFormatCount];
	mResult = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &mSurfaceFormatCount, mSurfaceFormats);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceFormatsKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfaceFormatsKHR succeeded.";
	}

	if (mSurfaceFormatCount == 1 && mSurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		mFormat = VK_FORMAT_B8G8R8A8_UNORM; //No preferred format so set a default.
	}
	else
	{
		mFormat = mSurfaceFormats[0].format; //Pull the preferred format.
	}

	switch (mFormat)
	{
	case VK_FORMAT_UNDEFINED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_UNDEFINED surface format.";
		break;
	case VK_FORMAT_R4G4_UNORM_PACK8:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R4G4_UNORM_PACK8 surface format.";
		break;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R4G4B4A4_UNORM_PACK16 surface format.";
		break;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B4G4R4A4_UNORM_PACK16 surface format.";
		break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R5G6B5_UNORM_PACK16 surface format.";
		break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B5G6R5_UNORM_PACK16 surface format.";
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R5G5B5A1_UNORM_PACK16 surface format.";
		break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B5G5R5A1_UNORM_PACK16 surface format.";
		break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A1R5G5B5_UNORM_PACK16 surface format.";
		break;
	case VK_FORMAT_R8_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8_UNORM surface format.";
		break;
	case VK_FORMAT_R8_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8_SNORM surface format.";
		break;
	case VK_FORMAT_R8_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8_USCALED surface format.";
		break;
	case VK_FORMAT_R8_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8_SSCALED surface format.";
		break;
	case VK_FORMAT_R8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8_UINT surface format.";
		break;
	case VK_FORMAT_R8_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8_SINT surface format.";
		break;
	case VK_FORMAT_R8_SRGB:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8_SRGB surface format.";
		break;
	case VK_FORMAT_R8G8_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8_UNORM surface format.";
		break;
	case VK_FORMAT_R8G8_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8_SNORM surface format.";
		break;
	case VK_FORMAT_R8G8_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8_USCALED surface format.";
		break;
	case VK_FORMAT_R8G8_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8_SSCALED surface format.";
		break;
	case VK_FORMAT_R8G8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8_UINT surface format.";
		break;
	case VK_FORMAT_R8G8_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8_SINT surface format.";
		break;
	case VK_FORMAT_R8G8_SRGB:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8_SRGB surface format.";
		break;
	case VK_FORMAT_R8G8B8_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8_UNORM surface format.";
		break;
	case VK_FORMAT_R8G8B8_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8_SNORM surface format.";
		break;
	case VK_FORMAT_R8G8B8_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8_USCALED surface format.";
		break;
	case VK_FORMAT_R8G8B8_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8_SSCALED surface format.";
		break;
	case VK_FORMAT_R8G8B8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8_UINT surface format.";
		break;
	case VK_FORMAT_R8G8B8_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8_SINT surface format.";
		break;
	case VK_FORMAT_R8G8B8_SRGB:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8_SRGB surface format.";
		break;
	case VK_FORMAT_B8G8R8_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8_UNORM surface format.";
		break;
	case VK_FORMAT_B8G8R8_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8_SNORM surface format.";
		break;
	case VK_FORMAT_B8G8R8_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8_USCALED surface format.";
		break;
	case VK_FORMAT_B8G8R8_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8_SSCALED surface format.";
		break;
	case VK_FORMAT_B8G8R8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8_UINT surface format.";
		break;
	case VK_FORMAT_B8G8R8_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8_SINT surface format.";
		break;
	case VK_FORMAT_B8G8R8_SRGB:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8_SRGB surface format.";
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8A8_UNORM surface format.";
		break;
	case VK_FORMAT_R8G8B8A8_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8A8_SNORM surface format.";
		break;
	case VK_FORMAT_R8G8B8A8_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8A8_USCALED surface format.";
		break;
	case VK_FORMAT_R8G8B8A8_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8A8_SSCALED surface format.";
		break;
	case VK_FORMAT_R8G8B8A8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8A8_UINT surface format.";
		break;
	case VK_FORMAT_R8G8B8A8_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8A8_SINT surface format.";
		break;
	case VK_FORMAT_R8G8B8A8_SRGB:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R8G8B8A8_SRGB surface format.";
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8A8_UNORM surface format.";
		break;
	case VK_FORMAT_B8G8R8A8_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8A8_SNORM surface format.";
		break;
	case VK_FORMAT_B8G8R8A8_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8A8_USCALED surface format.";
		break;
	case VK_FORMAT_B8G8R8A8_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8A8_SSCALED surface format.";
		break;
	case VK_FORMAT_B8G8R8A8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8A8_UINT surface format.";
		break;
	case VK_FORMAT_B8G8R8A8_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8A8_SINT surface format.";
		break;
	case VK_FORMAT_B8G8R8A8_SRGB:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B8G8R8A8_SRGB surface format.";
		break;
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A8B8G8R8_UNORM_PACK32 surface format.";
		break;
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A8B8G8R8_SNORM_PACK32 surface format.";
		break;
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A8B8G8R8_USCALED_PACK32 surface format.";
		break;
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A8B8G8R8_SSCALED_PACK32 surface format.";
		break;
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A8B8G8R8_UINT_PACK32 surface format.";
		break;
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A8B8G8R8_SINT_PACK32 surface format.";
		break;
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A8B8G8R8_SRGB_PACK32 surface format.";
		break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2R10G10B10_UNORM_PACK32 surface format.";
		break;
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2R10G10B10_SNORM_PACK32 surface format.";
		break;
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2R10G10B10_USCALED_PACK32 surface format.";
		break;
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2R10G10B10_SSCALED_PACK32 surface format.";
		break;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2R10G10B10_UINT_PACK32 surface format.";
		break;
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2R10G10B10_SINT_PACK32 surface format.";
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2B10G10R10_UNORM_PACK32 surface format.";
		break;
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2B10G10R10_SNORM_PACK32 surface format.";
		break;
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2B10G10R10_USCALED_PACK32 surface format.";
		break;
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2B10G10R10_SSCALED_PACK32 surface format.";
		break;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2B10G10R10_UINT_PACK32 surface format.";
		break;
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_A2B10G10R10_SINT_PACK32 surface format.";
		break;
	case VK_FORMAT_R16_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16_UNORM surface format.";
		break;
	case VK_FORMAT_R16_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16_SNORM surface format.";
		break;
	case VK_FORMAT_R16_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16_USCALED surface format.";
		break;
	case VK_FORMAT_R16_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16_SSCALED surface format.";
		break;
	case VK_FORMAT_R16_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16_UINT surface format.";
		break;
	case VK_FORMAT_R16_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16_SINT surface format.";
		break;
	case VK_FORMAT_R16_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16_SFLOAT surface format.";
		break;
	case VK_FORMAT_R16G16_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16_UNORM surface format.";
		break;
	case VK_FORMAT_R16G16_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16_SNORM surface format.";
		break;
	case VK_FORMAT_R16G16_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16_USCALED surface format.";
		break;
	case VK_FORMAT_R16G16_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16_SSCALED surface format.";
		break;
	case VK_FORMAT_R16G16_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16_UINT surface format.";
		break;
	case VK_FORMAT_R16G16_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16_SINT surface format.";
		break;
	case VK_FORMAT_R16G16_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16_SFLOAT surface format.";
		break;
	case VK_FORMAT_R16G16B16_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16_UNORM surface format.";
		break;
	case VK_FORMAT_R16G16B16_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16_SNORM surface format.";
		break;
	case VK_FORMAT_R16G16B16_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16_USCALED surface format.";
		break;
	case VK_FORMAT_R16G16B16_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16_SSCALED surface format.";
		break;
	case VK_FORMAT_R16G16B16_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16_UINT surface format.";
		break;
	case VK_FORMAT_R16G16B16_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16_SINT surface format.";
		break;
	case VK_FORMAT_R16G16B16_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16_SFLOAT surface format.";
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16A16_UNORM surface format.";
		break;
	case VK_FORMAT_R16G16B16A16_SNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16A16_SNORM surface format.";
		break;
	case VK_FORMAT_R16G16B16A16_USCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16A16_USCALED surface format.";
		break;
	case VK_FORMAT_R16G16B16A16_SSCALED:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16A16_SSCALED surface format.";
		break;
	case VK_FORMAT_R16G16B16A16_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16A16_UINT surface format.";
		break;
	case VK_FORMAT_R16G16B16A16_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16A16_SINT surface format.";
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R16G16B16A16_SFLOAT surface format.";
		break;
	case VK_FORMAT_R32_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32_UINT surface format.";
		break;
	case VK_FORMAT_R32_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32_SINT surface format.";
		break;
	case VK_FORMAT_R32_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32_SFLOAT surface format.";
		break;
	case VK_FORMAT_R32G32_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32_UINT surface format.";
		break;
	case VK_FORMAT_R32G32_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32_SINT surface format.";
		break;
	case VK_FORMAT_R32G32_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32_SFLOAT surface format.";
		break;
	case VK_FORMAT_R32G32B32_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32B32_UINT surface format.";
		break;
	case VK_FORMAT_R32G32B32_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32B32_SINT surface format.";
		break;
	case VK_FORMAT_R32G32B32_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32B32_SFLOAT surface format.";
		break;
	case VK_FORMAT_R32G32B32A32_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32B32A32_UINT surface format.";
		break;
	case VK_FORMAT_R32G32B32A32_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32B32A32_SINT surface format.";
		break;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R32G32B32A32_SFLOAT surface format.";
		break;
	case VK_FORMAT_R64_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64_UINT surface format.";
		break;
	case VK_FORMAT_R64_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64_SINT surface format.";
		break;
	case VK_FORMAT_R64_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64_SFLOAT surface format.";
		break;
	case VK_FORMAT_R64G64_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64_UINT surface format.";
		break;
	case VK_FORMAT_R64G64_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64_SINT surface format.";
		break;
	case VK_FORMAT_R64G64_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64_SFLOAT surface format.";
		break;
	case VK_FORMAT_R64G64B64_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64B64_UINT surface format.";
		break;
	case VK_FORMAT_R64G64B64_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64B64_SINT surface format.";
		break;
	case VK_FORMAT_R64G64B64_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64B64_SFLOAT surface format.";
		break;
	case VK_FORMAT_R64G64B64A64_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64B64A64_UINT surface format.";
		break;
	case VK_FORMAT_R64G64B64A64_SINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64B64A64_SINT surface format.";
		break;
	case VK_FORMAT_R64G64B64A64_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_R64G64B64A64_SFLOAT surface format.";
		break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_B10G11R11_UFLOAT_PACK32 surface format.";
		break;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 surface format.";
		break;
	case VK_FORMAT_D16_UNORM:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_D16_UNORM surface format.";
		break;
	case VK_FORMAT_X8_D24_UNORM_PACK32:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_X8_D24_UNORM_PACK32 surface format.";
		break;
	case VK_FORMAT_D32_SFLOAT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_D32_SFLOAT surface format.";
		break;
	case VK_FORMAT_S8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_S8_UINT surface format.";
		break;
	case VK_FORMAT_D16_UNORM_S8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_D16_UNORM_S8_UINT surface format.";
		break;
	case VK_FORMAT_D24_UNORM_S8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_D24_UNORM_S8_UINT surface format.";
		break;
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_D32_SFLOAT_S8_UINT surface format.";
		break;
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC1_RGB_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC1_RGB_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC1_RGBA_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC1_RGBA_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_BC2_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC2_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC2_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC2_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_BC3_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC3_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC3_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC3_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_BC4_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC4_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC4_SNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC4_SNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC5_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC5_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC5_SNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC5_SNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC6H_UFLOAT_BLOCK surface format.";
		break;
	case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC6H_SFLOAT_BLOCK surface format.";
		break;
	case VK_FORMAT_BC7_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC7_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_BC7_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_BC7_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_EAC_R11_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_EAC_R11_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_EAC_R11_SNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_EAC_R11_SNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_EAC_R11G11_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_EAC_R11G11_SNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_4x4_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_4x4_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_5x4_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_5x4_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_5x5_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_5x5_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_6x5_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_6x5_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_6x6_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_6x6_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_8x5_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_8x5_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_8x6_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_8x6_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_8x8_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_8x8_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x5_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x5_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x6_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x6_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x8_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x8_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x10_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_10x10_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_12x10_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_12x10_SRGB_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_12x12_UNORM_BLOCK surface format.";
		break;
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_FORMAT_ASTC_12x12_SRGB_BLOCK surface format.";
		break;
	}

	if (mSurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		mTransformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		mTransformFlags = mSurfaceCapabilities.currentTransform;
	}

	mResult = vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &mPresentationModeCount, nullptr);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfacePresentModesKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfacePresentModesKHR returned " << mPresentationModeCount << " presentation modes.";
	}
	mPresentationModes = new VkPresentModeKHR[mPresentationModeCount];
	mResult = vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &mPresentationModeCount, mPresentationModes);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfacePresentModesKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetPhysicalDeviceSurfacePresentModesKHR succeeded.";
	}

	/*
	Trying modes in order of preference (Mailbox,immediate, FIFO)
	VK_PRESENT_MODE_MAILBOX_KHR - Wait for the next vertical blanking interval to update the image. New images replace the one waiting to be displayed.
	VK_PRESENT_MODE_IMMEDIATE_KHR - Do not wait for vertical blanking to update the image.
	VK_PRESENT_MODE_FIFO_KHR - Wait for the next vertical blanking interval to update the image. If the interval is missed wait for the next one. New images will be queued for display.
	*/
	for (size_t i = 0; i < mPresentationModeCount; i++)
	{
		if (mPresentationModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			mSwapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		else if (mPresentationModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			mSwapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		} //Already defaulted to FIFO so do nothing for else.
	}

	switch (mSwapchainPresentMode)
	{
	case VK_PRESENT_MODE_IMMEDIATE_KHR:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_PRESENT_MODE_IMMEDIATE_KHR presentation mode.";
		break;
	case VK_PRESENT_MODE_MAILBOX_KHR:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_PRESENT_MODE_MAILBOX_KHR presentation mode.";
		break;
	case VK_PRESENT_MODE_FIFO_KHR:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_PRESENT_MODE_FIFO_KHR presentation mode.";
		break;
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 using VK_PRESENT_MODE_FIFO_RELAXED_KHR presentation mode.";
		break;
	}

	/*
	Finally create the swap chain based on the information collected.
	This swap chain will handle the work done by the implicit swap chain in D3D9.
	*/
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = mSurface;
	swapchainCreateInfo.minImageCount = mSurfaceCapabilities.minImageCount + 1;
	swapchainCreateInfo.imageFormat = mFormat;
	swapchainCreateInfo.imageColorSpace = mSurfaceFormats[0].colorSpace;
	swapchainCreateInfo.imageExtent = mSwapchainExtent;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.preTransform = mTransformFlags;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.presentMode = mSwapchainPresentMode;
	swapchainCreateInfo.oldSwapchain = mSwapchain; //There is no old swapchain yet.
	swapchainCreateInfo.clipped = true;

	mResult = vkCreateSwapchainKHR(mDevice, &swapchainCreateInfo, nullptr, &mSwapchain);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateSwapchainKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateSwapchainKHR succeeded.";
	}

	mPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	mPresentInfo.pNext = nullptr;
	mPresentInfo.swapchainCount = 1;
	mPresentInfo.pSwapchains = &mSwapchain;

	//Create the images (buffers) that will be used by the swap chain.
	mResult = vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mSwapchainImageCount, nullptr);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetSwapchainImagesKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetSwapchainImagesKHR returned " << mSwapchainImageCount << " images.";
	}
	mSwapchainImages = new VkImage[mSwapchainImageCount];
	mSwapchainViews = new VkImageView[mSwapchainImageCount];
	mSwapchainBuffers = new VkCommandBuffer[mSwapchainImageCount];
	mResult = vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mSwapchainImageCount, mSwapchainImages);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkGetSwapchainImagesKHR failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkGetSwapchainImagesKHR succeeded.";
	}

	for (size_t i = 0; i < mSwapchainImageCount; i++)
	{
		VkImageViewCreateInfo color_image_view = {};

		color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		color_image_view.pNext = nullptr;
		color_image_view.format = mFormat;
		color_image_view.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; //VK_COMPONENT_SWIZZLE_R
		color_image_view.components.g = VK_COMPONENT_SWIZZLE_IDENTITY; //VK_COMPONENT_SWIZZLE_G
		color_image_view.components.b = VK_COMPONENT_SWIZZLE_IDENTITY; //VK_COMPONENT_SWIZZLE_B
		color_image_view.components.a = VK_COMPONENT_SWIZZLE_IDENTITY; //VK_COMPONENT_SWIZZLE_A
		color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		color_image_view.subresourceRange.baseMipLevel = 0;
		color_image_view.subresourceRange.levelCount = 1;
		color_image_view.subresourceRange.baseArrayLayer = 0;
		color_image_view.subresourceRange.layerCount = 1;
		color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		color_image_view.flags = 0;

		color_image_view.image = mSwapchainImages[i];

		mResult = vkCreateImageView(mDevice, &color_image_view, nullptr, &mSwapchainViews[i]);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateImageView failed with return code of " << mResult;
			return;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateImageView succeeded.";
		}
	}

	VkCommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = mCommandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	for (size_t i = 0; i < mSwapchainImageCount; i++)
	{
		mResult = vkAllocateCommandBuffers(mDevice, &commandBufferInfo, &mSwapchainBuffers[i]);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkAllocateCommandBuffers failed with return code of " << mResult;
			return;
		}
	}

	/*
	Setup Depth stuff.
	*/

	mDepthFormat = VK_FORMAT_D16_UNORM;

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = mDepthFormat;
	imageCreateInfo.extent = { mPresentationParameters.BackBufferWidth,mPresentationParameters.BackBufferHeight, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.flags = 0;

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.image = VK_NULL_HANDLE;
	imageViewCreateInfo.format = mDepthFormat;
	imageViewCreateInfo.subresourceRange = {};
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	VkMemoryRequirements memoryRequirements = {};

	mResult = vkCreateImage(mDevice, &imageCreateInfo, nullptr, &mDepthImage);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateImage failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateImage succeeded.";
	}

	vkGetImageMemoryRequirements(mDevice, mDepthImage, &memoryRequirements);

	mDepthMemoryAllocateInfo = {};
	mDepthMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mDepthMemoryAllocateInfo.pNext = NULL;
	mDepthMemoryAllocateInfo.memoryTypeIndex = 0;
	mDepthMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	GetMemoryTypeFromProperties(mDeviceMemoryProperties, memoryRequirements.memoryTypeBits, 0, &mDepthMemoryAllocateInfo.memoryTypeIndex);

	mResult = vkAllocateMemory(mDevice, &mDepthMemoryAllocateInfo, NULL, &mDepthDeviceMemory);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkAllocateMemory failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkAllocateMemory succeeded.";
	}

	mResult = vkBindImageMemory(mDevice, mDepthImage, mDepthDeviceMemory, 0);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkBindImageMemory failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkBindImageMemory succeeded.";
	}

	SetImageLayout(mDepthImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	imageViewCreateInfo.image = mDepthImage;
	mResult = vkCreateImageView(mDevice, &imageViewCreateInfo, NULL, &mDepthView);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateImageView failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateImageView succeeded.";
	}

	/*
	Now setup the render pass.
	*/

	VkAttachmentDescription renderAttachments[2] = {};
	renderAttachments[0].format = mFormat;
	renderAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	renderAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	renderAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	renderAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	renderAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	renderAttachments[0].initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	renderAttachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	/*
	renderAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	renderAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	*/

	renderAttachments[1].format = mDepthFormat;
	renderAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	renderAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	renderAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	renderAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	renderAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	renderAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	renderAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthReference;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.attachmentCount = 2; //revisit
	renderPassCreateInfo.pAttachments = renderAttachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 0;
	renderPassCreateInfo.pDependencies = nullptr;

	mResult = vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateRenderPass failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateRenderPass succeeded.";
	}

	/*
	Setup framebuffers to tie everything together.
	*/

	VkImageView attachments[2] = {};
	attachments[1] = mDepthView;

	VkFramebufferCreateInfo framebufferCreateInfo = {};
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.renderPass = mRenderPass;
	framebufferCreateInfo.attachmentCount = 2; //revisit
	framebufferCreateInfo.pAttachments = attachments;
	framebufferCreateInfo.width = mSwapchainExtent.width; //revisit
	framebufferCreateInfo.height = mSwapchainExtent.height; //revisit
	framebufferCreateInfo.layers = 1;

	mFramebuffers = new VkFramebuffer[mSwapchainImageCount];

	for (size_t i = 0; i < mSwapchainImageCount; i++)
	{
		attachments[0] = mSwapchainViews[i];
		mResult = vkCreateFramebuffer(mDevice, &framebufferCreateInfo, nullptr, &mFramebuffers[i]);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateFramebuffer failed with return code of " << mResult;
			return;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateFramebuffer succeeded.";
		}
	}

	mPresentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	mPresentCompleteSemaphoreCreateInfo.pNext = nullptr;
	mPresentCompleteSemaphoreCreateInfo.flags = 0;

	mCommandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	mCommandBufferInheritanceInfo.pNext = nullptr;
	mCommandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
	mCommandBufferInheritanceInfo.subpass = 0;
	mCommandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	mCommandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	mCommandBufferInheritanceInfo.queryFlags = 0;
	mCommandBufferInheritanceInfo.pipelineStatistics = 0;

	mCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	mCommandBufferBeginInfo.pNext = nullptr;
	mCommandBufferBeginInfo.flags = 0;
	mCommandBufferBeginInfo.pInheritanceInfo = &mCommandBufferInheritanceInfo;

	for (int32_t i = 0; i < 16; i++)
	{
		mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
		mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
		mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
		mDeviceState.mSamplerStates[i][D3DSAMP_BORDERCOLOR] = 0;
		mDeviceState.mSamplerStates[i][D3DSAMP_MAGFILTER] = D3DTEXF_POINT;
		mDeviceState.mSamplerStates[i][D3DSAMP_MINFILTER] = D3DTEXF_POINT;
		mDeviceState.mSamplerStates[i][D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
		mDeviceState.mSamplerStates[i][D3DSAMP_MIPMAPLODBIAS] = 0;
		mDeviceState.mSamplerStates[i][D3DSAMP_MAXMIPLEVEL] = 0;
		mDeviceState.mSamplerStates[i][D3DSAMP_MAXANISOTROPY] = 1;
		mDeviceState.mSamplerStates[i][D3DSAMP_SRGBTEXTURE] = 0;
		mDeviceState.mSamplerStates[i][D3DSAMP_ELEMENTINDEX] = 0;
		mDeviceState.mSamplerStates[i][D3DSAMP_DMAPOFFSET] = 0;
	}

	//Changed default state because -1 is used to indicate that it has not been set but actual state should be defaulted.
	mDeviceState.mFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	Light light = {};
	mDeviceState.mLights.push_back(light);
	//mDeviceState.mLights.push_back(light);
	//mDeviceState.mLights.push_back(light);
	//mDeviceState.mLights.push_back(light);

	mBufferManager = new BufferManager(this);

	mGarbageManager.mDevice = mDevice;

	//Add implicit swap chain.
	CSwapChain9* ptr = new CSwapChain9(pPresentationParameters);
	mSwapChains.push_back(ptr);

	CRenderTargetSurface9* ptr2 = new CRenderTargetSurface9(this, mSwapchainExtent.width, mSwapchainExtent.height, ConvertFormat(mFormat));
	mRenderTargets.push_back(ptr2);

#ifdef _DEBUG
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 sizeof(D3DLIGHT9): " << sizeof(D3DLIGHT9);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 sizeof(Light): " << sizeof(Light);
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 sizeof(D3DMATERIAL9): " << sizeof(D3DMATERIAL9);
#endif	

	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 Finished.";
}

CDevice9::~CDevice9()
{
	BOOST_LOG_TRIVIAL(info) << "CDevice9::~CDevice9";

	mGarbageManager.DestroyHandles();

	for (size_t i = 0; i < mSwapChains.size(); i++)
	{
		delete mSwapChains[i];
	}

	delete mBufferManager;

	if (mFramebuffers != nullptr)
	{
		for (size_t i = 0; i < mSwapchainImageCount; i++)
		{
			if (mFramebuffers[i] != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(mDevice, mFramebuffers[i], nullptr);
			}
		}
		delete[] mFramebuffers;
	}

	if (mRenderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
	}

	if (mSwapchainBuffers != nullptr)
	{
		vkFreeCommandBuffers(mDevice, mCommandPool, mSwapchainImageCount, mSwapchainBuffers);
		delete[] mSwapchainBuffers;
	}

	if (mDepthView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mDevice, mDepthView, nullptr);
	}

	if (mDepthImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(mDevice, mDepthImage, nullptr);
	}

	if (mDepthDeviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice, mDepthDeviceMemory, nullptr);
	}

	if (mDescriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
	}

	if (mCommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
	}

	if (mSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
	}

	if (mSwapchainViews != nullptr)
	{
		for (size_t i = 0; i < mSwapchainImageCount; i++)
		{
			if (mSwapchainViews[i] != VK_NULL_HANDLE)
			{
				vkDestroyImageView(mDevice, mSwapchainViews[i], nullptr);
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

	if (mDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(mDevice, nullptr);
	}

	if (mSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(mInstance->mInstance, mSurface, nullptr);
	}

	delete[] mPresentationModes;

	delete[] mSurfaceFormats;

	delete[] mDisplays;

	delete[] mQueueFamilyProperties;

	//mInstance->Release();
}

HRESULT STDMETHODCALLTYPE CDevice9::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	if ((Flags & D3DCLEAR_TARGET) == D3DCLEAR_TARGET)
	{
		//VK_FORMAT_B8G8R8A8_UNORM 
		//Revisit the byte order could be difference based on the surface format so I need a better way to handle this.
		mClearColorValue.float32[3] = FLT_MAX; //D3DCOLOR_A(Color);
		mClearColorValue.float32[2] = D3DCOLOR_B(Color);
		mClearColorValue.float32[1] = D3DCOLOR_G(Color);
		mClearColorValue.float32[0] = D3DCOLOR_R(Color);		
	}

	if (Count > 0 && pRects != nullptr)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::Clear is not fully implemented!";
		return E_NOTIMPL;
	}
	else
	{
		//vkCmdClearColorImage(mSwapchainBuffers[mCurrentBuffer], mSwapchainImages[mCurrentBuffer], VK_IMAGE_LAYOUT_GENERAL, &clearColorValue, 0, nullptr);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::BeginScene() //
{
	//According to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndScene()
{
	//According to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}
	this->StopScene();

	VkResult result; // = VK_SUCCESS

	mPresentInfo.pImageIndices = &mCurrentBuffer;

	result = vkQueuePresentKHR(mQueue, &mPresentInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::Present vkQueuePresentKHR failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	result = vkQueueWaitIdle(mQueue);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::Present vkQueueWaitIdle failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	vkDestroySemaphore(mDevice, mPresentCompleteSemaphore, nullptr);

	vkResetCommandBuffer(mSwapchainBuffers[mCurrentBuffer], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

	//Clean up pipes.
	mBufferManager->FlushDrawBufffer();

	//Clean up unreferenced resources.
	mGarbageManager.DestroyHandles();

	//Print(mDeviceState.mTransforms);

	return D3D_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CDevice9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DDevice9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CDevice9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}


HRESULT STDMETHODCALLTYPE CDevice9::BeginStateBlock()
{
	this->mCurrentStateRecording = new CStateBlock9(this);

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::BeginStateBlock " << this->mCurrentStateRecording;

	return S_OK;
}



HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface, const RECT *pRect, D3DCOLOR color)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ColorFill is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::CreateAdditionalSwapChain is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9 **ppCubeTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CCubeTexture9* obj = new CCubeTexture9(this, EdgeLength, Levels, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppCubeTexture) = (IDirect3DCubeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this, nullptr, Width, Height, Format, MultiSample, MultisampleQuality, Discard, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9 **ppIndexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CIndexBuffer9* obj = new CIndexBuffer9(this, Length, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppIndexBuffer) = (IDirect3DIndexBuffer9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateTexture Length:" << Length << " Usage:" << Usage << " Format:" << Format << " Pool:" << Pool;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* ptr = new CSurface9(this, nullptr, Width, Height, 1, 0, Format, Pool, pSharedHandle);

	if (ptr->mResult != VK_SUCCESS)
	{
		delete ptr;
		ptr = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)ptr;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreatePixelShader(const DWORD *pFunction, IDirect3DPixelShader9 **ppShader)
{
	HRESULT result = S_OK;

	CPixelShader9* obj = new CPixelShader9(this, pFunction);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppShader) = (IDirect3DPixelShader9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9 **ppQuery)
{
	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb174360(v=vs.85).aspx
	*/

	//If null is passed the call is checking to see if a query type is supported.
	if (ppQuery == nullptr)
	{
		return D3DERR_NOTAVAILABLE;
	}

	HRESULT result = S_OK;

	CQuery9* obj = new CQuery9(this, Type);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppQuery) = (IDirect3DQuery9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	//I added an extra int at the end so the signature would be different for this version. Locakable/Discard are both BOOL.
	CRenderTargetSurface9* obj = new CRenderTargetSurface9(this, Width, Height, Format);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
{
	HRESULT result = S_OK;

	CStateBlock9* obj = new CStateBlock9(this, Type);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSB) = (IDirect3DStateBlock9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateStateBlock " << obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CTexture9* obj = new CTexture9(this, Width, Height, Levels, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppTexture) = (IDirect3DTexture9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateTexture Width:" << Width << " Height:" << Height << " Levels:" << Levels << " Usage:" << Usage << " Format:" << Format << " Pool:" << Pool;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CVertexBuffer9* obj = new CVertexBuffer9(this, Length, Usage, FVF, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppVertexBuffer) = (IDirect3DVertexBuffer9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateTexture Length:" << Length << " Usage:" << Usage << " Pool:" << Pool;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements, IDirect3DVertexDeclaration9 **ppDecl)
{
	HRESULT result = S_OK;

	CVertexDeclaration9* obj = new CVertexDeclaration9(this, pVertexElements);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppDecl) = (IDirect3DVertexDeclaration9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexShader(const DWORD *pFunction, IDirect3DVertexShader9 **ppShader)
{
	HRESULT result = S_OK;

	CVertexShader9* obj = new CVertexShader9(this, pFunction);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppShader) = (IDirect3DVertexShader9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9 **ppVolumeTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CVolumeTexture9* obj = new CVolumeTexture9(this, Width, Height, Depth, Levels, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppVolumeTexture) = (IDirect3DVolumeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::DeletePatch(UINT Handle)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DeletePatch is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	if (mDeviceState.mIndexBuffer == nullptr)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitive called with null index buffer.";
		return D3DERR_INVALIDCALL;
	}

	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(this);
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(this);

	mBufferManager->BeginDraw(context, resourceContext, Type);

	/*
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb174369(v=vs.85).aspx
		https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdDrawIndexed.html
	*/
	vkCmdDrawIndexed(mSwapchainBuffers[mCurrentBuffer], min(mDeviceState.mIndexBuffer->mSize, ConvertPrimitiveCountToVertexCount(Type, PrimitiveCount)), 1, StartIndex, BaseVertexIndex, 0);

	//BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitive";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitiveUP is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(this);
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(this);

	mBufferManager->BeginDraw(context, resourceContext, PrimitiveType);

	vkCmdDraw(mSwapchainBuffers[mCurrentBuffer], min(mBufferManager->mVertexCount, ConvertPrimitiveCountToVertexCount(PrimitiveType, PrimitiveCount)), 1, StartVertex, 0);

	//Print(mDeviceState.mTransforms);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawPrimitiveUP is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle, const float *pNumSegs, const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawRectPatch is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle, const float *pNumSegs, const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawTriPatch is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
	(*ppSB) = this->mCurrentStateRecording;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::EndStateBlock " << this->mCurrentStateRecording;

	this->mCurrentStateRecording = nullptr;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EvictManagedResources()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::EvictManagedResources is not implemented!";

	return S_OK;
}

UINT STDMETHODCALLTYPE CDevice9::GetAvailableTextureMem()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetAvailableTextureMem is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain, UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9 **ppBackBuffer)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetBackBuffer is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index, float *pPlane)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetClipPlane is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetClipStatus is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	pParameters->AdapterOrdinal = this->mAdapter;
	pParameters->DeviceType = this->mDeviceType;
	pParameters->hFocusWindow = this->mFocusWindow;
	pParameters->BehaviorFlags = this->mBehaviorFlags;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetCurrentTexturePalette is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetDepthStencilSurface is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
	/*
	https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkPhysicalDeviceProperties
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172513(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172635(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172591(v=vs.85).aspx
	*/

	//Translate the vulkan properties & features into D3D9 capabilities.
	pCaps->DeviceType = D3DDEVTYPE_HAL; //Assume all hardware.
	pCaps->AdapterOrdinal = 0;
	pCaps->Caps = 0;
	pCaps->Caps2 = D3DCAPS2_CANMANAGERESOURCE | D3DCAPS2_DYNAMICTEXTURES | D3DCAPS2_FULLSCREENGAMMA | D3DCAPS2_CANAUTOGENMIPMAP;
	pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD | D3DCAPS3_COPY_TO_VIDMEM | D3DCAPS3_COPY_TO_SYSTEMMEM | D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;
	pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_DEFAULT | D3DPRESENT_INTERVAL_ONE | D3DPRESENT_INTERVAL_TWO | D3DPRESENT_INTERVAL_THREE | D3DPRESENT_INTERVAL_FOUR | D3DPRESENT_INTERVAL_IMMEDIATE;
	pCaps->CursorCaps = D3DCURSORCAPS_COLOR | D3DCURSORCAPS_LOWRES;
	pCaps->DevCaps = D3DDEVCAPS_CANBLTSYSTONONLOCAL | D3DDEVCAPS_CANRENDERAFTERFLIP | D3DDEVCAPS_DRAWPRIMITIVES2 | D3DDEVCAPS_DRAWPRIMITIVES2EX | D3DDEVCAPS_DRAWPRIMTLVERTEX | D3DDEVCAPS_EXECUTESYSTEMMEMORY | D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_HWRASTERIZATION | D3DDEVCAPS_HWTRANSFORMANDLIGHT | D3DDEVCAPS_PUREDEVICE | D3DDEVCAPS_TEXTURENONLOCALVIDMEM | D3DDEVCAPS_TEXTUREVIDEOMEMORY | D3DDEVCAPS_TLVERTEXSYSTEMMEMORY | D3DDEVCAPS_TLVERTEXVIDEOMEMORY;
	pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_MASKZ | D3DPMISCCAPS_CULLNONE | D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_COLORWRITEENABLE | D3DPMISCCAPS_CLIPPLANESCALEDPOINTS | D3DPMISCCAPS_TSSARGTEMP | D3DPMISCCAPS_BLENDOP | D3DPMISCCAPS_INDEPENDENTWRITEMASKS | D3DPMISCCAPS_FOGANDSPECULARALPHA | D3DPMISCCAPS_SEPARATEALPHABLEND | D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS | D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING | D3DPMISCCAPS_FOGVERTEXCLAMPED;
	pCaps->RasterCaps = D3DPRASTERCAPS_ANISOTROPY | D3DPRASTERCAPS_COLORPERSPECTIVE | D3DPRASTERCAPS_DITHER | D3DPRASTERCAPS_DEPTHBIAS | D3DPRASTERCAPS_FOGRANGE | D3DPRASTERCAPS_FOGTABLE | D3DPRASTERCAPS_FOGVERTEX | D3DPRASTERCAPS_MIPMAPLODBIAS | D3DPRASTERCAPS_MULTISAMPLE_TOGGLE | D3DPRASTERCAPS_SCISSORTEST | D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS | D3DPRASTERCAPS_WFOG | D3DPRASTERCAPS_ZFOG | D3DPRASTERCAPS_ZTEST;
	pCaps->ZCmpCaps = D3DPCMPCAPS_NEVER | D3DPCMPCAPS_LESS | D3DPCMPCAPS_EQUAL | D3DPCMPCAPS_LESSEQUAL | D3DPCMPCAPS_GREATER | D3DPCMPCAPS_NOTEQUAL | D3DPCMPCAPS_GREATEREQUAL | D3DPCMPCAPS_ALWAYS;
	pCaps->SrcBlendCaps = D3DPBLENDCAPS_ZERO | D3DPBLENDCAPS_ONE | D3DPBLENDCAPS_SRCCOLOR | D3DPBLENDCAPS_INVSRCCOLOR | D3DPBLENDCAPS_SRCALPHA | D3DPBLENDCAPS_INVSRCALPHA | D3DPBLENDCAPS_DESTALPHA | D3DPBLENDCAPS_INVDESTALPHA | D3DPBLENDCAPS_DESTCOLOR | D3DPBLENDCAPS_INVDESTCOLOR | D3DPBLENDCAPS_SRCALPHASAT | D3DPBLENDCAPS_BOTHSRCALPHA | D3DPBLENDCAPS_BOTHINVSRCALPHA | D3DPBLENDCAPS_BLENDFACTOR | D3DPBLENDCAPS_INVSRCCOLOR2 | D3DPBLENDCAPS_SRCCOLOR2;
	pCaps->DestBlendCaps = pCaps->SrcBlendCaps;
	pCaps->AlphaCmpCaps = D3DPCMPCAPS_NEVER | D3DPCMPCAPS_LESS | D3DPCMPCAPS_EQUAL | D3DPCMPCAPS_LESSEQUAL | D3DPCMPCAPS_GREATER | D3DPCMPCAPS_NOTEQUAL | D3DPCMPCAPS_GREATEREQUAL | D3DPCMPCAPS_ALWAYS;
	pCaps->ShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB | D3DPSHADECAPS_SPECULARGOURAUDRGB | D3DPSHADECAPS_ALPHAGOURAUDBLEND | D3DPSHADECAPS_FOGGOURAUD;
	pCaps->TextureCaps = D3DPTEXTURECAPS_ALPHA | D3DPTEXTURECAPS_ALPHAPALETTE | D3DPTEXTURECAPS_PERSPECTIVE | D3DPTEXTURECAPS_PROJECTED | D3DPTEXTURECAPS_CUBEMAP | D3DPTEXTURECAPS_VOLUMEMAP | D3DPTEXTURECAPS_POW2 | D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_CUBEMAP_POW2 | D3DPTEXTURECAPS_VOLUMEMAP_POW2 | D3DPTEXTURECAPS_MIPMAP | D3DPTEXTURECAPS_MIPVOLUMEMAP | D3DPTEXTURECAPS_MIPCUBEMAP;
	pCaps->TextureFilterCaps = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MINFANISOTROPIC | D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR | D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR | D3DPTFILTERCAPS_MAGFANISOTROPIC;
	pCaps->CubeTextureFilterCaps = pCaps->TextureFilterCaps;
	pCaps->VolumeTextureFilterCaps = pCaps->TextureFilterCaps;
	pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER | D3DPTADDRESSCAPS_INDEPENDENTUV | D3DPTADDRESSCAPS_WRAP | D3DPTADDRESSCAPS_MIRROR | D3DPTADDRESSCAPS_CLAMP | D3DPTADDRESSCAPS_MIRRORONCE;
	pCaps->VolumeTextureAddressCaps = pCaps->TextureAddressCaps;
	pCaps->LineCaps = D3DLINECAPS_ALPHACMP | D3DLINECAPS_BLEND | D3DLINECAPS_TEXTURE | D3DLINECAPS_ZTEST | D3DLINECAPS_FOG;
	pCaps->MaxTextureWidth = mDeviceProperties.limits.maxImageDimension2D; //Revisit
	pCaps->MaxTextureHeight = mDeviceProperties.limits.maxImageDimension2D; //Revisit
	pCaps->MaxVolumeExtent = mDeviceProperties.limits.maxImageDimensionCube; //Revisit
	pCaps->MaxTextureRepeat = 32768; //revisit
	pCaps->MaxTextureAspectRatio = pCaps->MaxTextureWidth;
	pCaps->MaxAnisotropy = mDeviceFeatures.samplerAnisotropy;
	pCaps->MaxVertexW = 1e10f; //revisit
	pCaps->GuardBandLeft = -1e9f; //revisit
	pCaps->GuardBandTop = -1e9f; //revisit
	pCaps->GuardBandRight = 1e9f; //revisit
	pCaps->GuardBandBottom = 1e9f; //revisit
	pCaps->ExtentsAdjust = 0.0f; //revisit
	pCaps->StencilCaps = D3DSTENCILCAPS_KEEP | D3DSTENCILCAPS_ZERO | D3DSTENCILCAPS_REPLACE | D3DSTENCILCAPS_INCRSAT | D3DSTENCILCAPS_DECRSAT | D3DSTENCILCAPS_INVERT | D3DSTENCILCAPS_INCR | D3DSTENCILCAPS_DECR | D3DSTENCILCAPS_TWOSIDED;
	pCaps->FVFCaps = D3DFVFCAPS_PSIZE;
	pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_MODULATE | D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE4X | D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_ADDSIGNED | D3DTEXOPCAPS_ADDSIGNED2X | D3DTEXOPCAPS_SUBTRACT | D3DTEXOPCAPS_ADDSMOOTH | D3DTEXOPCAPS_BLENDDIFFUSEALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHA | D3DTEXOPCAPS_BLENDFACTORALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHAPM | D3DTEXOPCAPS_BLENDCURRENTALPHA | D3DTEXOPCAPS_PREMODULATE | D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA | D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA | D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE | D3DTEXOPCAPS_DOTPRODUCT3 | D3DTEXOPCAPS_MULTIPLYADD | D3DTEXOPCAPS_LERP;
	pCaps->MaxTextureBlendStages = mDeviceProperties.limits.maxDescriptorSetSamplers; //revisit
	pCaps->MaxSimultaneousTextures = mDeviceProperties.limits.maxDescriptorSetSampledImages; //revisit
	pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_MATERIALSOURCE7 | D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_LOCALVIEWER | D3DVTXPCAPS_TWEENING;
	pCaps->MaxActiveLights = 0;  //Revsit should be infinite but games may not read it that way.
	pCaps->MaxUserClipPlanes = 8; //revisit
	pCaps->MaxVertexBlendMatrices = 4; //revisit
	pCaps->MaxVertexBlendMatrixIndex = 7; //revisit
	pCaps->MaxPointSize = mDeviceProperties.limits.pointSizeRange[1]; //revisit
	pCaps->MaxPrimitiveCount = 0xFFFFFFFF; //revisit
	pCaps->MaxVertexIndex = 0xFFFFFFFF; //revisit
	pCaps->MaxStreams = mDeviceProperties.limits.maxVertexInputBindings; //revisit
	pCaps->MaxStreamStride = mDeviceProperties.limits.maxVertexInputBindingStride; //revisit
	pCaps->VertexShaderVersion = D3DVS_VERSION(3, 0);
	pCaps->MaxVertexShaderConst = 256; //revisit
	pCaps->PixelShaderVersion = D3DPS_VERSION(3, 0);
	pCaps->PixelShader1xMaxValue = 65504.f;
	pCaps->DevCaps2 = D3DDEVCAPS2_STREAMOFFSET | D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET | D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES;
	pCaps->MasterAdapterOrdinal = 0;
	pCaps->AdapterOrdinalInGroup = 0;
	pCaps->NumberOfAdaptersInGroup = 1;
	pCaps->DeclTypes = D3DDTCAPS_UBYTE4 | D3DDTCAPS_UBYTE4N | D3DDTCAPS_SHORT2N | D3DDTCAPS_SHORT4N | D3DDTCAPS_USHORT2N | D3DDTCAPS_USHORT4N | D3DDTCAPS_UDEC3 | D3DDTCAPS_DEC3N | D3DDTCAPS_FLOAT16_2 | D3DDTCAPS_FLOAT16_4;
	pCaps->NumSimultaneousRTs = 4; //revisit
	pCaps->StretchRectFilterCaps = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR;
	pCaps->VS20Caps.Caps = D3DVS20CAPS_PREDICATION;
	pCaps->VS20Caps.DynamicFlowControlDepth = 24; //revsit
	pCaps->VS20Caps.StaticFlowControlDepth = 4; //revsit
	pCaps->VS20Caps.NumTemps = 32; //revsit
	pCaps->PS20Caps.Caps = D3DPS20CAPS_ARBITRARYSWIZZLE | D3DPS20CAPS_GRADIENTINSTRUCTIONS | D3DPS20CAPS_PREDICATION;
	pCaps->PS20Caps.DynamicFlowControlDepth = 24; //revsit
	pCaps->PS20Caps.StaticFlowControlDepth = 4; //revsit
	pCaps->PS20Caps.NumTemps = 32; //revsit
	pCaps->VertexTextureFilterCaps = pCaps->TextureFilterCaps; //revisit
	pCaps->MaxVertexShader30InstructionSlots = 32768; //revisit
	pCaps->MaxPixelShader30InstructionSlots = 32768; //revisit
	pCaps->MaxVShaderInstructionsExecuted = 65535 < pCaps->MaxVertexShader30InstructionSlots * 32 ? pCaps->MaxVertexShader30InstructionSlots * 32 : 65535; //revisit
	pCaps->MaxPShaderInstructionsExecuted = 65535 < pCaps->MaxPixelShader30InstructionSlots * 32 ? pCaps->MaxPixelShader30InstructionSlots * 32 : 65535; //revisit


	pCaps->MaxNpatchTessellationLevel = 0.0f;
	pCaps->Reserved5 = 0;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	(*ppD3D9) = (IDirect3D9*)mInstance;

	mInstance->AddRef();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayMode(UINT  iSwapChain, D3DDISPLAYMODE *pMode)
{
	if (iSwapChain)
	{
		//TODO: Implement.
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetDisplayMode multiple swapchains are not implemented!";
	}
	else
	{
		pMode->Height = this->mSwapchainExtent.height;
		pMode->Width = this->mSwapchainExtent.width;
		pMode->RefreshRate = 60; //fake it till you make it.
		pMode->Format = ConvertFormat(this->mFormat);
	}

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Height: " << pMode->Height;
	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Width: " << pMode->Width;
	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode RefreshRate: " << pMode->RefreshRate;
	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Format: " << pMode->Format;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain, IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetFrontBufferData is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{
	(*pFVF) = mDeviceState.mFVF;

	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain, D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetGammaRamp is not implemented!";

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData) //,UINT *pBaseVertexIndex ?
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetIndices is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight)
{
	auto& light = mDeviceState.mLights[Index];

	pLight->Diffuse = (*(D3DCOLORVALUE*)light.Diffuse.data());
	pLight->Specular = (*(D3DCOLORVALUE*)light.Specular.data());
	pLight->Ambient = (*(D3DCOLORVALUE*)light.Ambient.data());

	pLight->Position = (*(D3DVECTOR*)light.Position.data());
	pLight->Direction = (*(D3DVECTOR*)light.Direction.data());

	pLight->Range = light.Range;
	pLight->Falloff = light.Falloff;
	pLight->Attenuation0 = light.Attenuation0;
	pLight->Attenuation1 = light.Attenuation1;
	pLight->Attenuation2 = light.Attenuation2;
	pLight->Theta = light.Theta;
	pLight->Phi = light.Phi;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index, BOOL *pEnable)
{
	(*pEnable) = mDeviceState.mLights[Index].IsEnabled;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	(*pMaterial) = mDeviceState.mMaterial;

	return S_OK;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	return mDeviceState.mNSegments;
}

UINT STDMETHODCALLTYPE CDevice9::GetNumberOfSwapChains()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetNumberOfSwapChains is not implemented!";

	return 0;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPaletteEntries is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
	(*ppShader) = (IDirect3DPixelShader9*)mDeviceState.mPixelShader;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPixelShaderConstantB is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPixelShaderConstantF is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPixelShaderConstantI is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain, D3DRASTER_STATUS *pRasterStatus)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRasterStatus is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue)
{
	SpecializationConstants* constants = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		constants = &this->mCurrentStateRecording->mDeviceState.mSpecializationConstants;
	}
	else
	{
		constants = &mDeviceState.mSpecializationConstants;
	}

	switch (State)
	{
	case D3DRS_ZENABLE:
		(*pValue) = constants->zEnable;
		break;
	case D3DRS_FILLMODE:
		(*pValue) = constants->fillMode;
		break;
	case D3DRS_SHADEMODE:
		(*pValue) = constants->shadeMode;
		break;
	case D3DRS_ZWRITEENABLE:
		(*pValue) = constants->zWriteEnable;
		break;
	case D3DRS_ALPHATESTENABLE:
		(*pValue) = constants->alphaTestEnable;
		break;
	case D3DRS_LASTPIXEL:
		(*pValue) = constants->lastPixel;
		break;
	case D3DRS_SRCBLEND:
		(*pValue) = constants->sourceBlend;
		break;
	case D3DRS_DESTBLEND:
		(*pValue) = constants->destinationBlend;
		break;
	case D3DRS_CULLMODE:
		(*pValue) = constants->cullMode;
		break;
	case D3DRS_ZFUNC:
		(*pValue) = constants->zFunction;
		break;
	case D3DRS_ALPHAREF:
		(*pValue) = constants->alphaReference;
		break;
	case D3DRS_ALPHAFUNC:
		(*pValue) = constants->alphaFunction;
		break;
	case D3DRS_DITHERENABLE:
		(*pValue) = constants->ditherEnable;
		break;
	case D3DRS_ALPHABLENDENABLE:
		(*pValue) = constants->alphaBlendEnable;
		break;
	case D3DRS_FOGENABLE:
		(*pValue) = constants->fogEnable;
		break;
	case D3DRS_SPECULARENABLE:
		(*pValue) = constants->specularEnable;
		break;
	case D3DRS_FOGCOLOR:
		(*pValue) = constants->fogColor;
		break;
	case D3DRS_FOGTABLEMODE:
		(*pValue) = constants->fogTableMode;
		break;
	case D3DRS_FOGSTART:
		(*pValue) = constants->fogStart;
		break;
	case D3DRS_FOGEND:
		(*pValue) = constants->fogEnd;
		break;
	case D3DRS_FOGDENSITY:
		(*pValue) = constants->fogDensity;
		break;
	case D3DRS_RANGEFOGENABLE:
		(*pValue) = constants->rangeFogEnable;
		break;
	case D3DRS_STENCILENABLE:
		(*pValue) = constants->stencilEnable;
		break;
	case D3DRS_STENCILFAIL:
		(*pValue) = constants->stencilFail;
		break;
	case D3DRS_STENCILZFAIL:
		(*pValue) = constants->stencilZFail;
		break;
	case D3DRS_STENCILPASS:
		(*pValue) = constants->stencilPass;
		break;
	case D3DRS_STENCILFUNC:
		(*pValue) = constants->stencilFunction;
		break;
	case D3DRS_STENCILREF:
		(*pValue) = constants->stencilReference;
		break;
	case D3DRS_STENCILMASK:
		(*pValue) = constants->stencilMask;
		break;
	case D3DRS_STENCILWRITEMASK:
		(*pValue) = constants->stencilWriteMask;
		break;
	case D3DRS_TEXTUREFACTOR:
		(*pValue) = constants->textureFactor;
		break;
	case D3DRS_WRAP0:
		(*pValue) = constants->wrap0;
		break;
	case D3DRS_WRAP1:
		(*pValue) = constants->wrap1;
		break;
	case D3DRS_WRAP2:
		(*pValue) = constants->wrap2;
		break;
	case D3DRS_WRAP3:
		(*pValue) = constants->wrap3;
		break;
	case D3DRS_WRAP4:
		(*pValue) = constants->wrap4;
		break;
	case D3DRS_WRAP5:
		(*pValue) = constants->wrap5;
		break;
	case D3DRS_WRAP6:
		(*pValue) = constants->wrap6;
		break;
	case D3DRS_WRAP7:
		(*pValue) = constants->wrap7;
		break;
	case D3DRS_CLIPPING:
		(*pValue) = constants->clipping;
		break;
	case D3DRS_LIGHTING:
		(*pValue) = constants->lighting;
		break;
	case D3DRS_AMBIENT:
		(*pValue) = constants->ambient;
		break;
	case D3DRS_FOGVERTEXMODE:
		(*pValue) = constants->fogVertexMode;
		break;
	case D3DRS_COLORVERTEX:
		(*pValue) = constants->colorVertex;
		break;
	case D3DRS_LOCALVIEWER:
		(*pValue) = constants->localViewer;
		break;
	case D3DRS_NORMALIZENORMALS:
		(*pValue) = constants->normalizeNormals;
		break;
	case D3DRS_DIFFUSEMATERIALSOURCE:
		(*pValue) = constants->diffuseMaterialSource;
		break;
	case D3DRS_SPECULARMATERIALSOURCE:
		(*pValue) = constants->specularMaterialSource;
		break;
	case D3DRS_AMBIENTMATERIALSOURCE:
		(*pValue) = constants->ambientMaterialSource;
		break;
	case D3DRS_EMISSIVEMATERIALSOURCE:
		(*pValue) = constants->emissiveMaterialSource;
		break;
	case D3DRS_VERTEXBLEND:
		(*pValue) = constants->vertexBlend;
		break;
	case D3DRS_CLIPPLANEENABLE:
		(*pValue) = constants->clipPlaneEnable;
		break;
	case D3DRS_POINTSIZE:
		(*pValue) = constants->pointSize;
		break;
	case D3DRS_POINTSIZE_MIN:
		(*pValue) = constants->pointSizeMinimum;
		break;
	case D3DRS_POINTSPRITEENABLE:
		(*pValue) = constants->pointSpriteEnable;
		break;
	case D3DRS_POINTSCALEENABLE:
		(*pValue) = constants->pointScaleEnable;
		break;
	case D3DRS_POINTSCALE_A:
		(*pValue) = constants->pointScaleA;
		break;
	case D3DRS_POINTSCALE_B:
		(*pValue) = constants->pointScaleB;
		break;
	case D3DRS_POINTSCALE_C:
		(*pValue) = constants->pointScaleC;
		break;
	case D3DRS_MULTISAMPLEANTIALIAS:
		(*pValue) = constants->multisampleAntiAlias;
		break;
	case D3DRS_MULTISAMPLEMASK:
		(*pValue) = constants->multisampleMask;
		break;
	case D3DRS_PATCHEDGESTYLE:
		(*pValue) = constants->patchEdgeStyle;
		break;
	case D3DRS_DEBUGMONITORTOKEN:
		(*pValue) = constants->debugMonitorToken;
		break;
	case D3DRS_POINTSIZE_MAX:
		(*pValue) = constants->pointSizeMaximum;
		break;
	case D3DRS_INDEXEDVERTEXBLENDENABLE:
		(*pValue) = constants->indexedVertexBlendEnable;
		break;
	case D3DRS_COLORWRITEENABLE:
		(*pValue) = constants->colorWriteEnable;
		break;
	case D3DRS_TWEENFACTOR:
		(*pValue) = constants->tweenFactor;
		break;
	case D3DRS_BLENDOP:
		(*pValue) = constants->blendOperation;
		break;
	case D3DRS_POSITIONDEGREE:
		(*pValue) = constants->positionDegree;
		break;
	case D3DRS_NORMALDEGREE:
		(*pValue) = constants->normalDegree;
		break;
	case D3DRS_SCISSORTESTENABLE:
		(*pValue) = constants->scissorTestEnable;
		break;
	case D3DRS_SLOPESCALEDEPTHBIAS:
		(*pValue) = constants->slopeScaleDepthBias;
		break;
	case D3DRS_ANTIALIASEDLINEENABLE:
		(*pValue) = constants->antiAliasedLineEnable;
		break;
	case D3DRS_MINTESSELLATIONLEVEL:
		(*pValue) = constants->minimumTessellationLevel;
		break;
	case D3DRS_MAXTESSELLATIONLEVEL:
		(*pValue) = constants->maximumTessellationLevel;
		break;
	case D3DRS_ADAPTIVETESS_X:
		(*pValue) = constants->adaptivetessX;
		break;
	case D3DRS_ADAPTIVETESS_Y:
		(*pValue) = constants->adaptivetessY;
		break;
	case D3DRS_ADAPTIVETESS_Z:
		(*pValue) = constants->adaptivetessZ;
		break;
	case D3DRS_ADAPTIVETESS_W:
		(*pValue) = constants->adaptivetessW;
		break;
	case D3DRS_ENABLEADAPTIVETESSELLATION:
		(*pValue) = constants->enableAdaptiveTessellation;
		break;
	case D3DRS_TWOSIDEDSTENCILMODE:
		(*pValue) = constants->twoSidedStencilMode;
		break;
	case D3DRS_CCW_STENCILFAIL:
		(*pValue) = constants->ccwStencilFail;
		break;
	case D3DRS_CCW_STENCILZFAIL:
		(*pValue) = constants->ccwStencilZFail;
		break;
	case D3DRS_CCW_STENCILPASS:
		(*pValue) = constants->ccwStencilPass;
		break;
	case D3DRS_CCW_STENCILFUNC:
		(*pValue) = constants->ccwStencilFunction;
		break;
	case D3DRS_COLORWRITEENABLE1:
		(*pValue) = constants->colorWriteEnable1;
		break;
	case D3DRS_COLORWRITEENABLE2:
		(*pValue) = constants->colorWriteEnable2;
		break;
	case D3DRS_COLORWRITEENABLE3:
		(*pValue) = constants->colorWriteEnable3;
		break;
	case D3DRS_BLENDFACTOR:
		(*pValue) = constants->blendFactor;
		break;
	case D3DRS_SRGBWRITEENABLE:
		(*pValue) = constants->srgbWriteEnable;
		break;
	case D3DRS_DEPTHBIAS:
		(*pValue) = constants->depthBias;
		break;
	case D3DRS_WRAP8:
		(*pValue) = constants->wrap8;
		break;
	case D3DRS_WRAP9:
		(*pValue) = constants->wrap9;
		break;
	case D3DRS_WRAP10:
		(*pValue) = constants->wrap10;
		break;
	case D3DRS_WRAP11:
		(*pValue) = constants->wrap11;
		break;
	case D3DRS_WRAP12:
		(*pValue) = constants->wrap12;
		break;
	case D3DRS_WRAP13:
		(*pValue) = constants->wrap13;
		break;
	case D3DRS_WRAP14:
		(*pValue) = constants->wrap14;
		break;
	case D3DRS_WRAP15:
		(*pValue) = constants->wrap15;
		break;
	case D3DRS_SEPARATEALPHABLENDENABLE:
		(*pValue) = constants->separateAlphaBlendEnable;
		break;
	case D3DRS_SRCBLENDALPHA:
		(*pValue) = constants->sourceBlendAlpha;
		break;
	case D3DRS_DESTBLENDALPHA:
		(*pValue) = constants->destinationBlendAlpha;
		break;
	case D3DRS_BLENDOPALPHA:
		(*pValue) = constants->blendOperationAlpha;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRenderState unknown state! " << State;
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 **ppRenderTarget)
{
	(*ppRenderTarget) = mRenderTargets[RenderTargetIndex];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRenderTargetData is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD *pValue)
{
	(*pValue) = mDeviceState.mSamplerStates[Sampler][Type];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT *pRect)
{
	(*pRect) = mDeviceState.m9Scissor;

	return S_OK;
}

BOOL STDMETHODCALLTYPE CDevice9::GetSoftwareVertexProcessing()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetSoftwareVertexProcessing is not implemented!";

	return true;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 **ppStreamData, UINT *pOffsetInBytes, UINT *pStride)
{
	StreamSource& value = mDeviceState.mStreamSources[StreamNumber];

	(*ppStreamData) = (IDirect3DVertexBuffer9*)value.StreamData;
	/*
	Vulkan wants 64bit uint but d3d9 uses 32bit uint. This cast just keeps compiler from complaining.
	This should be safe because only 32bit can be set and d3d9 is x86 only so endianness issues shouldn't come into play.
	*/
	(*pOffsetInBytes) = (UINT)value.OffsetInBytes;
	(*pStride) = value.Stride;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT *pDivider)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetStreamSourceFreq is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSwapChain(UINT  iSwapChain, IDirect3DSwapChain9 **ppSwapChain)
{
	(*ppSwapChain) = (IDirect3DSwapChain9*)mSwapChains[iSwapChain];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9 **ppTexture)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetTexture is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	switch (Type)
	{
	case D3DTSS_COLOROP:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorOperation_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorOperation_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorOperation_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorOperation_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorOperation_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorOperation_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorOperation_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorOperation_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG1:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorArgument1_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorArgument1_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorArgument1_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorArgument1_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorArgument1_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorArgument1_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorArgument1_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorArgument1_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG2:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorArgument2_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorArgument2_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorArgument2_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorArgument2_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorArgument2_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorArgument2_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorArgument2_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorArgument2_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAOP:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaOperation_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaOperation_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaOperation_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaOperation_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaOperation_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaOperation_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaOperation_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaOperation_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG1:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG2:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT00:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix00_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT01:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix01_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT10:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix10_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT11:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.bumpMapMatrix11_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXCOORDINDEX:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLSCALE:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.bumpMapScale_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLOFFSET:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.bumpMapOffset_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXTURETRANSFORMFLAGS:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG0:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorArgument0_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorArgument0_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorArgument0_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorArgument0_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorArgument0_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorArgument0_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorArgument0_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorArgument0_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG0:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_RESULTARG:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.Result_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.Result_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.Result_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.Result_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.Result_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.Result_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.Result_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.Result_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_CONSTANT:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.Constant_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.Constant_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.Constant_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.Constant_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.Constant_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.Constant_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.Constant_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.Constant_7;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	(*pMatrix) = mDeviceState.mTransforms[State];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
	(*ppDecl) = (IDirect3DVertexDeclaration9*)this->mDeviceState.mVertexDeclaration;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
	(*ppShader) = (IDirect3DVertexShader9*)mDeviceState.mVertexShader;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexShaderConstantB is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexShaderConstantF is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexShaderConstantI is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	(*pViewport) = mDeviceState.m9Viewport;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex, BOOL bEnable)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	Light light = {};

	if (state->mLights.size() == LightIndex)
	{
		light.IsEnabled = bEnable;
		state->mLights.push_back(light);
		state->mAreLightsDirty = true;
	}
	else
	{
		state->mLights[LightIndex].IsEnabled = bEnable;
		state->mAreLightsDirty = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::MultiplyTransform is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9 *pDestBuffer, IDirect3DVertexDeclaration9 *pVertexDecl, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ProcessVertices is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::Reset is not implemented!";

	return S_OK;
}

HRESULT CDevice9::SetClipPlane(DWORD Index, const float *pPlane)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetClipPlane is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetClipStatus is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetCurrentTexturePalette is not implemented!";

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CDevice9::SetCursorPosition(INT X, INT Y, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetCursorPosition is not implemented!";

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetCursorProperties is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetDepthStencilSurface is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetDialogBoxMode is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetFVF(DWORD FVF)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mFVF = FVF;
		this->mCurrentStateRecording->mDeviceState.mHasFVF = true;
		this->mCurrentStateRecording->mDeviceState.mHasVertexDeclaration = false;
	}
	else
	{
		mDeviceState.mFVF = FVF;
		mDeviceState.mHasFVF = true;
		mDeviceState.mHasVertexDeclaration = false;
	}

	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain, DWORD Flags, const D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetGammaRamp is not implemented!";
}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mIndexBuffer = (CIndexBuffer9*)pIndexData;
		this->mCurrentStateRecording->mDeviceState.mHasIndexBuffer = true;
	}
	else
	{
		mDeviceState.mIndexBuffer = (CIndexBuffer9*)pIndexData;
		mDeviceState.mHasIndexBuffer = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index, const D3DLIGHT9 *pLight)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	Light light = {};

	light.Diffuse << pLight->Diffuse.r
	,pLight->Diffuse.g
	,pLight->Diffuse.b
	,pLight->Diffuse.a;

	light.Specular << pLight->Specular.r
	,pLight->Specular.g
	,pLight->Specular.b
	,pLight->Specular.a;

	light.Ambient << pLight->Ambient.r
	,pLight->Ambient.g
	,pLight->Ambient.b
	,pLight->Ambient.a;

	light.Position << pLight->Position.x
	,pLight->Position.y
	,pLight->Position.z;

	light.Direction << pLight->Direction.x
	,pLight->Direction.y
	,pLight->Direction.z;

	light.Range = pLight->Range;
	light.Falloff = pLight->Falloff;
	light.Attenuation0 = pLight->Attenuation0;
	light.Attenuation1 = pLight->Attenuation1;
	light.Attenuation2 = pLight->Attenuation2;
	light.Theta = pLight->Theta;
	light.Phi = pLight->Phi;

	if (state->mLights.size() == Index)
	{
		state->mLights.push_back(light);
		state->mAreLightsDirty = true;
	}
	else
	{
		light.IsEnabled = state->mLights[Index].IsEnabled;

		state->mLights[Index] = light;
		state->mAreLightsDirty = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mMaterial = (*pMaterial);
		this->mCurrentStateRecording->mDeviceState.mIsMaterialDirty = true;
	}
	else
	{
		mDeviceState.mMaterial = (*pMaterial);
		mDeviceState.mIsMaterialDirty = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{
	if (nSegments > 0.0f)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetNPatchMode nPatch greater than zero not supported.";
	}

	if (this->mCurrentStateRecording != nullptr)
	{
		//BOOST_LOG_TRIVIAL(info) << "Recorded NSegments";
		this->mCurrentStateRecording->mDeviceState.mNSegments = nSegments;
	}
	else
	{
		mDeviceState.mNSegments = nSegments;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPaletteEntries is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
	if (pShader != nullptr)
	{
		pShader->AddRef();
	}

	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mPixelShader = (CPixelShader9*)pShader;
		this->mCurrentStateRecording->mDeviceState.mHasPixelShader = true;
	}
	else
	{
		if (mDeviceState.mPixelShader != nullptr)
		{
			mDeviceState.mPixelShader->Release();
		}

		mDeviceState.mPixelShader = (CPixelShader9*)pShader;
		mDeviceState.mHasPixelShader = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister, const BOOL *pConstantData, UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPixelShaderConstantB is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPixelShaderConstantF is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister, const int *pConstantData, UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPixelShaderConstantI is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	SpecializationConstants* constants = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		constants = &this->mCurrentStateRecording->mDeviceState.mSpecializationConstants;
	}
	else
	{
		constants = &mDeviceState.mSpecializationConstants;
	}

	switch (State)
	{
	case D3DRS_ZENABLE:
		constants->zEnable = Value;
		break;
	case D3DRS_FILLMODE:
		constants->fillMode = Value;
		break;
	case D3DRS_SHADEMODE:
		constants->shadeMode = Value;
		break;
	case D3DRS_ZWRITEENABLE:
		constants->zWriteEnable = Value;
		break;
	case D3DRS_ALPHATESTENABLE:
		constants->alphaTestEnable = Value;
		break;
	case D3DRS_LASTPIXEL:
		constants->lastPixel = Value;
		break;
	case D3DRS_SRCBLEND:
		constants->sourceBlend = Value;
		break;
	case D3DRS_DESTBLEND:
		constants->destinationBlend = Value;
		break;
	case D3DRS_CULLMODE:
		constants->cullMode = Value;
		break;
	case D3DRS_ZFUNC:
		constants->zFunction = Value;
		break;
	case D3DRS_ALPHAREF:
		constants->alphaReference = Value;
		break;
	case D3DRS_ALPHAFUNC:
		constants->alphaFunction = Value;
		break;
	case D3DRS_DITHERENABLE:
		constants->ditherEnable = Value;
		break;
	case D3DRS_ALPHABLENDENABLE:
		constants->alphaBlendEnable = Value;
		break;
	case D3DRS_FOGENABLE:
		constants->fogEnable = Value;
		break;
	case D3DRS_SPECULARENABLE:
		constants->specularEnable = Value;
		break;
	case D3DRS_FOGCOLOR:
		constants->fogColor = Value;
		break;
	case D3DRS_FOGTABLEMODE:
		constants->fogTableMode = Value;
		break;
	case D3DRS_FOGSTART:
		constants->fogStart = Value;
		break;
	case D3DRS_FOGEND:
		constants->fogEnd = Value;
		break;
	case D3DRS_FOGDENSITY:
		constants->fogDensity = Value;
		break;
	case D3DRS_RANGEFOGENABLE:
		constants->rangeFogEnable = Value;
		break;
	case D3DRS_STENCILENABLE:
		constants->stencilEnable = Value;
		break;
	case D3DRS_STENCILFAIL:
		constants->stencilFail = Value;
		break;
	case D3DRS_STENCILZFAIL:
		constants->stencilZFail = Value;
		break;
	case D3DRS_STENCILPASS:
		constants->stencilPass = Value;
		break;
	case D3DRS_STENCILFUNC:
		constants->stencilFunction = Value;
		break;
	case D3DRS_STENCILREF:
		constants->stencilReference = Value;
		break;
	case D3DRS_STENCILMASK:
		constants->stencilMask = Value;
		break;
	case D3DRS_STENCILWRITEMASK:
		constants->stencilWriteMask = Value;
		break;
	case D3DRS_TEXTUREFACTOR:
		constants->textureFactor = Value;
		break;
	case D3DRS_WRAP0:
		constants->wrap0 = Value;
		break;
	case D3DRS_WRAP1:
		constants->wrap1 = Value;
		break;
	case D3DRS_WRAP2:
		constants->wrap2 = Value;
		break;
	case D3DRS_WRAP3:
		constants->wrap3 = Value;
		break;
	case D3DRS_WRAP4:
		constants->wrap4 = Value;
		break;
	case D3DRS_WRAP5:
		constants->wrap5 = Value;
		break;
	case D3DRS_WRAP6:
		constants->wrap6 = Value;
		break;
	case D3DRS_WRAP7:
		constants->wrap7 = Value;
		break;
	case D3DRS_CLIPPING:
		constants->clipping = Value;
		break;
	case D3DRS_LIGHTING:
		constants->lighting = Value;
		break;
	case D3DRS_AMBIENT:
		constants->ambient = Value;
		break;
	case D3DRS_FOGVERTEXMODE:
		constants->fogVertexMode = Value;
		break;
	case D3DRS_COLORVERTEX:
		constants->colorVertex = Value;
		break;
	case D3DRS_LOCALVIEWER:
		constants->localViewer = Value;
		break;
	case D3DRS_NORMALIZENORMALS:
		constants->normalizeNormals = Value;
		break;
	case D3DRS_DIFFUSEMATERIALSOURCE:
		constants->diffuseMaterialSource = Value;
		break;
	case D3DRS_SPECULARMATERIALSOURCE:
		constants->specularMaterialSource = Value;
		break;
	case D3DRS_AMBIENTMATERIALSOURCE:
		constants->ambientMaterialSource = Value;
		break;
	case D3DRS_EMISSIVEMATERIALSOURCE:
		constants->emissiveMaterialSource = Value;
		break;
	case D3DRS_VERTEXBLEND:
		constants->vertexBlend = Value;
		break;
	case D3DRS_CLIPPLANEENABLE:
		constants->clipPlaneEnable = Value;
		break;
	case D3DRS_POINTSIZE:
		constants->pointSize = Value;
		break;
	case D3DRS_POINTSIZE_MIN:
		constants->pointSizeMinimum = Value;
		break;
	case D3DRS_POINTSPRITEENABLE:
		constants->pointSpriteEnable = Value;
		break;
	case D3DRS_POINTSCALEENABLE:
		constants->pointScaleEnable = Value;
		break;
	case D3DRS_POINTSCALE_A:
		constants->pointScaleA = Value;
		break;
	case D3DRS_POINTSCALE_B:
		constants->pointScaleB = Value;
		break;
	case D3DRS_POINTSCALE_C:
		constants->pointScaleC = Value;
		break;
	case D3DRS_MULTISAMPLEANTIALIAS:
		constants->multisampleAntiAlias = Value;
		break;
	case D3DRS_MULTISAMPLEMASK:
		constants->multisampleMask = Value;
		break;
	case D3DRS_PATCHEDGESTYLE:
		constants->patchEdgeStyle = Value;
		break;
	case D3DRS_DEBUGMONITORTOKEN:
		constants->debugMonitorToken = Value;
		break;
	case D3DRS_POINTSIZE_MAX:
		constants->pointSizeMaximum = Value;
		break;
	case D3DRS_INDEXEDVERTEXBLENDENABLE:
		constants->indexedVertexBlendEnable = Value;
		break;
	case D3DRS_COLORWRITEENABLE:
		constants->colorWriteEnable = Value;
		break;
	case D3DRS_TWEENFACTOR:
		constants->tweenFactor = Value;
		break;
	case D3DRS_BLENDOP:
		constants->blendOperation = Value;
		break;
	case D3DRS_POSITIONDEGREE:
		constants->positionDegree = Value;
		break;
	case D3DRS_NORMALDEGREE:
		constants->normalDegree = Value;
		break;
	case D3DRS_SCISSORTESTENABLE:
		constants->scissorTestEnable = Value;
		break;
	case D3DRS_SLOPESCALEDEPTHBIAS:
		constants->slopeScaleDepthBias = Value;
		break;
	case D3DRS_ANTIALIASEDLINEENABLE:
		constants->antiAliasedLineEnable = Value;
		break;
	case D3DRS_MINTESSELLATIONLEVEL:
		constants->minimumTessellationLevel = Value;
		break;
	case D3DRS_MAXTESSELLATIONLEVEL:
		constants->maximumTessellationLevel = Value;
		break;
	case D3DRS_ADAPTIVETESS_X:
		constants->adaptivetessX = Value;
		break;
	case D3DRS_ADAPTIVETESS_Y:
		constants->adaptivetessY = Value;
		break;
	case D3DRS_ADAPTIVETESS_Z:
		constants->adaptivetessZ = Value;
		break;
	case D3DRS_ADAPTIVETESS_W:
		constants->adaptivetessW = Value;
		break;
	case D3DRS_ENABLEADAPTIVETESSELLATION:
		constants->enableAdaptiveTessellation = Value;
		break;
	case D3DRS_TWOSIDEDSTENCILMODE:
		constants->twoSidedStencilMode = Value;
		break;
	case D3DRS_CCW_STENCILFAIL:
		constants->ccwStencilFail = Value;
		break;
	case D3DRS_CCW_STENCILZFAIL:
		constants->ccwStencilZFail = Value;
		break;
	case D3DRS_CCW_STENCILPASS:
		constants->ccwStencilPass = Value;
		break;
	case D3DRS_CCW_STENCILFUNC:
		constants->ccwStencilFunction = Value;
		break;
	case D3DRS_COLORWRITEENABLE1:
		constants->colorWriteEnable1 = Value;
		break;
	case D3DRS_COLORWRITEENABLE2:
		constants->colorWriteEnable2 = Value;
		break;
	case D3DRS_COLORWRITEENABLE3:
		constants->colorWriteEnable3 = Value;
		break;
	case D3DRS_BLENDFACTOR:
		constants->blendFactor = Value;
		break;
	case D3DRS_SRGBWRITEENABLE:
		constants->srgbWriteEnable = Value;
		break;
	case D3DRS_DEPTHBIAS:
		constants->depthBias = Value;
		break;
	case D3DRS_WRAP8:
		constants->wrap8 = Value;
		break;
	case D3DRS_WRAP9:
		constants->wrap9 = Value;
		break;
	case D3DRS_WRAP10:
		constants->wrap10 = Value;
		break;
	case D3DRS_WRAP11:
		constants->wrap11 = Value;
		break;
	case D3DRS_WRAP12:
		constants->wrap12 = Value;
		break;
	case D3DRS_WRAP13:
		constants->wrap13 = Value;
		break;
	case D3DRS_WRAP14:
		constants->wrap14 = Value;
		break;
	case D3DRS_WRAP15:
		constants->wrap15 = Value;
		break;
	case D3DRS_SEPARATEALPHABLENDENABLE:
		constants->separateAlphaBlendEnable = Value;
		break;
	case D3DRS_SRCBLENDALPHA:
		constants->sourceBlendAlpha = Value;
		break;
	case D3DRS_DESTBLENDALPHA:
		constants->destinationBlendAlpha = Value;
		break;
	case D3DRS_BLENDOPALPHA:
		constants->blendOperationAlpha = Value;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetRenderState unknown state! " << State;
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget)
{
	mRenderTargets[RenderTargetIndex] = (CRenderTargetSurface9*)pRenderTarget;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	DeviceState* state = NULL;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	state->mSamplerStates[Sampler][Type] = Value;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT *pRect)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.m9Scissor = (*pRect);

		this->mCurrentStateRecording->mDeviceState.mScissor.extent.width = this->mCurrentStateRecording->mDeviceState.m9Scissor.right;
		this->mCurrentStateRecording->mDeviceState.mScissor.extent.height = this->mCurrentStateRecording->mDeviceState.m9Scissor.bottom;
		this->mCurrentStateRecording->mDeviceState.mScissor.offset.x = this->mCurrentStateRecording->mDeviceState.m9Scissor.left;
		this->mCurrentStateRecording->mDeviceState.mScissor.offset.y = this->mCurrentStateRecording->mDeviceState.m9Scissor.top;
	}
	else
	{
		mDeviceState.m9Scissor = (*pRect);

		mDeviceState.mScissor.extent.width = mDeviceState.m9Scissor.right;
		mDeviceState.mScissor.extent.height = mDeviceState.m9Scissor.bottom;
		mDeviceState.mScissor.offset.x = mDeviceState.m9Scissor.left;
		mDeviceState.mScissor.offset.y = mDeviceState.m9Scissor.top;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetSoftwareVertexProcessing is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride)
{
	CVertexBuffer9* streamData = (CVertexBuffer9*)pStreamData;

	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
	}
	else
	{
		mDeviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetStreamSourceFreq is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler, IDirect3DBaseTexture9 *pTexture)
{
	auto texture = (CTexture9*)pTexture; //Check for compiler bugs.
	DeviceState* state = NULL;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	if (pTexture == nullptr)
	{
		auto it = state->mTextures.find(Sampler);
		if (it != state->mTextures.end())
		{
			state->mTextures.erase(it);
		}
	}
	else
	{
		state->mTextures[Sampler] = texture;
		//texture->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	switch (Type)
	{
	case D3DTSS_COLOROP:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorOperation_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorOperation_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorOperation_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorOperation_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorOperation_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorOperation_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorOperation_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorOperation_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG1:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorArgument1_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorArgument1_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorArgument1_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorArgument1_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorArgument1_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorArgument1_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorArgument1_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorArgument1_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG2:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorArgument2_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorArgument2_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorArgument2_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorArgument2_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorArgument2_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorArgument2_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorArgument2_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorArgument2_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAOP:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaOperation_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaOperation_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaOperation_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaOperation_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaOperation_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaOperation_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaOperation_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaOperation_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG1:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaArgument1_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaArgument1_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaArgument1_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaArgument1_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaArgument1_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaArgument1_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaArgument1_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaArgument1_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG2:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaArgument2_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaArgument2_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaArgument2_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaArgument2_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaArgument2_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaArgument2_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaArgument2_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaArgument2_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT00:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix00_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix00_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix00_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix00_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix00_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix00_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix00_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix00_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT01:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix01_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix01_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix01_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix01_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix01_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix01_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix01_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix01_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT10:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix10_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix10_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix10_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix10_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix10_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix10_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix10_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix10_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT11:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix11_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix11_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix11_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix11_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix11_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix11_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix11_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix11_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXCOORDINDEX:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.texureCoordinateIndex_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.texureCoordinateIndex_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.texureCoordinateIndex_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.texureCoordinateIndex_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.texureCoordinateIndex_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.texureCoordinateIndex_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.texureCoordinateIndex_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.texureCoordinateIndex_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLSCALE:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapScale_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.bumpMapScale_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.bumpMapScale_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.bumpMapScale_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.bumpMapScale_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.bumpMapScale_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.bumpMapScale_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.bumpMapScale_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLOFFSET:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapOffset_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.bumpMapOffset_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.bumpMapOffset_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.bumpMapOffset_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.bumpMapOffset_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.bumpMapOffset_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.bumpMapOffset_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.bumpMapOffset_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXTURETRANSFORMFLAGS:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.textureTransformationFlags_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.textureTransformationFlags_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.textureTransformationFlags_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.textureTransformationFlags_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.textureTransformationFlags_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.textureTransformationFlags_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.textureTransformationFlags_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.textureTransformationFlags_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG0:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorArgument0_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorArgument0_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorArgument0_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorArgument0_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorArgument0_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorArgument0_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorArgument0_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorArgument0_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG0:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaArgument0_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaArgument0_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaArgument0_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaArgument0_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaArgument0_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaArgument0_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaArgument0_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaArgument0_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_RESULTARG:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.Result_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.Result_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.Result_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.Result_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.Result_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.Result_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.Result_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.Result_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_CONSTANT:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.Constant_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.Constant_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.Constant_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.Constant_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.Constant_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.Constant_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.Constant_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.Constant_7 = Value;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mTransforms[State] = (*pMatrix);
		this->mCurrentStateRecording->mDeviceState.mHasTransformsChanged = true;
	}
	else
	{
		mDeviceState.mTransforms[State] = (*pMatrix);
		mDeviceState.mHasTransformsChanged = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;

		this->mCurrentStateRecording->mDeviceState.mHasVertexDeclaration = true;
		this->mCurrentStateRecording->mDeviceState.mHasFVF = false;
	}
	else
	{
		mDeviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;

		mDeviceState.mHasVertexDeclaration = true;
		mDeviceState.mHasFVF = false;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
	if (pShader != nullptr)
	{
		pShader->AddRef();
	}

	if (this->mCurrentStateRecording != nullptr)
	{
		//BOOST_LOG_TRIVIAL(info) << "Recorded VertexShader";
		this->mCurrentStateRecording->mDeviceState.mVertexShader = (CVertexShader9*)pShader;
		this->mCurrentStateRecording->mDeviceState.mHasVertexShader = true;
	}
	else
	{
		if (mDeviceState.mVertexShader != nullptr)
		{
			mDeviceState.mVertexShader->Release();
		}

		mDeviceState.mVertexShader = (CVertexShader9*)pShader;
		mDeviceState.mHasVertexShader = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister, const BOOL *pConstantData, UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexShaderConstantB is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexShaderConstantF is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister, const int *pConstantData, UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexShaderConstantI is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.m9Viewport = (*pViewport);

		this->mCurrentStateRecording->mDeviceState.mViewport.width = mDeviceState.m9Viewport.Width;
		this->mCurrentStateRecording->mDeviceState.mViewport.height = mDeviceState.m9Viewport.Height;
		this->mCurrentStateRecording->mDeviceState.mViewport.minDepth = mDeviceState.m9Viewport.MinZ;
		this->mCurrentStateRecording->mDeviceState.mViewport.maxDepth = mDeviceState.m9Viewport.MaxZ;
	}
	else
	{
		mDeviceState.m9Viewport = (*pViewport);

		mDeviceState.mViewport.width = mDeviceState.m9Viewport.Width;
		mDeviceState.mViewport.height = mDeviceState.m9Viewport.Height;
		mDeviceState.mViewport.minDepth = mDeviceState.m9Viewport.MinZ;
		mDeviceState.mViewport.maxDepth = mDeviceState.m9Viewport.MaxZ;
	}

	return S_OK;
}

BOOL STDMETHODCALLTYPE CDevice9::ShowCursor(BOOL bShow)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ShowCursor is not implemented!";

	return TRUE;
}

HRESULT STDMETHODCALLTYPE CDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect, IDirect3DSurface9 *pDestSurface, const RECT *pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::StretchRect is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::TestCooperativeLevel()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::TestCooperativeLevel is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect, IDirect3DSurface9 *pDestinationSurface, const POINT *pDestinationPoint)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::UpdateSurface is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::UpdateTexture is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ValidateDevice(DWORD *pNumPasses)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ValidateDevice is not implemented!";

	return S_OK;
}

void CDevice9::SetImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, uint32_t levelCount, uint32_t mipIndex)
{
	/*
	This is just a helper method to reduce repeat code.
	*/
	VkResult result = VK_SUCCESS;
	VkPipelineStageFlags sourceStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	if (aspectMask == 0)
	{
		aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkCommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = mCommandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(mDevice, &commandBufferInfo, &commandBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkAllocateCommandBuffers failed with return code of " << mResult;
		return;
	}

	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	commandBufferInheritanceInfo.queryFlags = 0;
	commandBufferInheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkBeginCommandBuffer failed with return code of " << mResult;
		return;
	}

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = mipIndex;
	imageMemoryBarrier.subresourceRange.levelCount = levelCount;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		break;
	case VK_IMAGE_LAYOUT_GENERAL:
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; //Added based on validation layer complaints.
		//imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		break;
	default:
		break;
	}

	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		break;
	case VK_IMAGE_LAYOUT_GENERAL:
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT; // VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		break;
	default:
		break;
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkEndCommandBuffer failed with return code of " << result;
		return;
	}

	VkCommandBuffer commandBuffers[] = { commandBuffer };
	VkFence nullFence = VK_NULL_HANDLE;
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	result = vkQueueSubmit(mQueue, 1, &submitInfo, nullFence);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkQueueSubmit failed with return code of " << result;
		return;
	}

	result = vkQueueWaitIdle(mQueue);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkQueueWaitIdle failed with return code of " << result;
		return;
	}

	vkFreeCommandBuffers(mDevice, mCommandPool, 1, commandBuffers);
}

void CDevice9::StartScene()
{
	mIsSceneStarted = true;

	VkResult result; // = VK_SUCCESS

	//BeginPaint(mFocusWindow, mPaintInformation);

	result = vkCreateSemaphore(mDevice, &mPresentCompleteSemaphoreCreateInfo, nullptr, &mPresentCompleteSemaphore);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::StartScene vkCreateSemaphore failed with return code of " << mResult;
		return;
	}

	result = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mPresentCompleteSemaphore, (VkFence)0, &mCurrentBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::StartScene vkAcquireNextImageKHR failed with return code of " << mResult;
		return;
	}

	//maybe add back later
	//SetImageLayout(mSwapchainImages[mCurrentBuffer], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR); //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL

	mClearValues[0].color = mClearColorValue;
	mClearValues[1].depthStencil = { 1.0f, 0 };

	mRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	mRenderPassBeginInfo.pNext = nullptr;
	mRenderPassBeginInfo.renderPass = mRenderPass;
	mRenderPassBeginInfo.framebuffer = mFramebuffers[mCurrentBuffer];
	mRenderPassBeginInfo.renderArea.offset.x = 0;
	mRenderPassBeginInfo.renderArea.offset.y = 0;
	mRenderPassBeginInfo.renderArea.extent.width = mSwapchainExtent.width;
	mRenderPassBeginInfo.renderArea.extent.height = mSwapchainExtent.height;
	mRenderPassBeginInfo.clearValueCount = 2;
	mRenderPassBeginInfo.pClearValues = mClearValues;

	result = vkBeginCommandBuffer(mSwapchainBuffers[mCurrentBuffer], &mCommandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::StartScene vkBeginCommandBuffer failed with return code of " << mResult;
		return;
	}

	mImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	mImageMemoryBarrier.pNext = nullptr;
	mImageMemoryBarrier.srcAccessMask = 0;
	mImageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT; //VK_ACCESS_MEMORY_READ_BIT   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	mImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	mImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mImageMemoryBarrier.image = mSwapchainImages[mCurrentBuffer];
	mImageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	vkCmdPipelineBarrier(mSwapchainBuffers[mCurrentBuffer], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &mImageMemoryBarrier);

	/*
	The Vulkan spec doesn't allow updating buffers inside of a render pass so we need to put as much buffer update logic into this method to reduce violations.
	The previous UBO implementation was updating inside of a render pass and it seemed to work on Desktop but that may vary from driver to driver.
	In addition performance is not guaranteed in case of spec violation.
	*/
	mBufferManager->UpdateBuffer();

	vkCmdBeginRenderPass(mSwapchainBuffers[mCurrentBuffer], &mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); //why doesn't this return a result.

	vkCmdSetViewport(mSwapchainBuffers[mCurrentBuffer], 0, 1, &mDeviceState.mViewport);
	vkCmdSetScissor(mSwapchainBuffers[mCurrentBuffer], 0, 1, &mDeviceState.mScissor);
}

void CDevice9::StopScene()
{
	mIsSceneStarted = false;

	VkResult result; // = VK_SUCCESS
	mPipeStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	mSubmitInfo.pNext = nullptr;
	mSubmitInfo.waitSemaphoreCount = 1;
	mSubmitInfo.pWaitSemaphores = &mPresentCompleteSemaphore;
	mSubmitInfo.pWaitDstStageMask = &mPipeStageFlags;
	mSubmitInfo.commandBufferCount = 1;
	mSubmitInfo.pCommandBuffers = &mSwapchainBuffers[mCurrentBuffer];
	mSubmitInfo.signalSemaphoreCount = 0;
	mSubmitInfo.pSignalSemaphores = nullptr;

	vkCmdEndRenderPass(mSwapchainBuffers[mCurrentBuffer]); // Why no result?

	mPrePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	mPrePresentBarrier.pNext = nullptr;
	mPrePresentBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT; //VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	mPrePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT; //VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT 
	mPrePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	mPrePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	mPrePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mPrePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mPrePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	mPrePresentBarrier.image = mSwapchainImages[mCurrentBuffer];
	VkImageMemoryBarrier* memoryBarrier = &mPrePresentBarrier;
	vkCmdPipelineBarrier(mSwapchainBuffers[mCurrentBuffer], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, memoryBarrier);

	result = vkEndCommandBuffer(mSwapchainBuffers[mCurrentBuffer]);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkEndCommandBuffer failed with return code of " << mResult;
		return;
	}

	result = vkQueueSubmit(mQueue, 1, &mSubmitInfo, mNullFence);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkQueueSubmit failed with return code of " << mResult;
		return;
	}

	//result = vkQueueWaitIdle(mQueue);
	//if (result != VK_SUCCESS)
	//{
	//	BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkQueueWaitIdle failed with return code of " << mResult;
	//	return;
	//}
}