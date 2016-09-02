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
	: mResult(VK_SUCCESS),
	mInstance(Instance),
	mAdapter(Adapter),
	mDeviceType(DeviceType),
	mFocusWindow(hFocusWindow),
	mBehaviorFlags(BehaviorFlags),
	mQueueCount(0),
	mReferenceCount(0),
	mDisplays(nullptr),
	mDisplayCount(0),
	mGraphicsQueueIndex(UINT32_MAX),
	mPresentationQueueIndex(UINT32_MAX),
	mSurfaceFormatCount(0),
	mSurfaceFormats(nullptr),
	mSwapchainPresentMode(VK_PRESENT_MODE_FIFO_KHR),
	mPresentationModeCount(0),
	mPresentationModes(nullptr),
	mSwapchainImages(nullptr),
	mSwapchainBuffers(nullptr),
	mSwapchainViews(nullptr),
	mSwapchainImageCount(0),
	mNullFence(VK_NULL_HANDLE),
	mFramebuffers(nullptr),
	mPhysicalDevice(VK_NULL_HANDLE),
	mDevice(VK_NULL_HANDLE),
	mSurface(VK_NULL_HANDLE),
	mSwapchain(VK_NULL_HANDLE),
	mCommandPool(VK_NULL_HANDLE),
	mDescriptorPool(VK_NULL_HANDLE),
	mCommandBuffer(VK_NULL_HANDLE),
	mQueue(VK_NULL_HANDLE),
	mPresentCompleteSemaphore(VK_NULL_HANDLE),
	mRenderPass(VK_NULL_HANDLE),
	mDepthFormat(VK_FORMAT_UNDEFINED),
	mDepthImage(VK_NULL_HANDLE),
	mDepthDeviceMemory(VK_NULL_HANDLE),
	mDepthView(VK_NULL_HANDLE),
	mFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE),
	mIsDirty(true),
	mIsSceneStarted(false),
	mBufferManager(nullptr)
{
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 Started.";

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
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &mQueueCount,NULL);
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
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &mQueueCount,mQueueFamilyProperties);

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
	device_info.pEnabledFeatures = nullptr;

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
	//For the multi-threaded version we'll need seperate descriptor/command pools per thread.
	VkDescriptorPoolSize descriptorPoolSizes [11] = {};
	descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptorPoolSizes[0].descriptorCount = 16; //Revisit
	descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSizes[1].descriptorCount = 16; //Revisit
	descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	descriptorPoolSizes[2].descriptorCount = 16; //Revisit
	descriptorPoolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorPoolSizes[3].descriptorCount = 16; //Revisit
	descriptorPoolSizes[4].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	descriptorPoolSizes[4].descriptorCount = 16; //Revisit
	descriptorPoolSizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	descriptorPoolSizes[5].descriptorCount = 16; //Revisit
	descriptorPoolSizes[6].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSizes[6].descriptorCount = 16; //Revisit
	descriptorPoolSizes[7].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSizes[7].descriptorCount = 16; //Revisit
	descriptorPoolSizes[8].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorPoolSizes[8].descriptorCount = 16; //Revisit
	descriptorPoolSizes[9].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	descriptorPoolSizes[9].descriptorCount = 16; //Revisit
	descriptorPoolSizes[10].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	descriptorPoolSizes[10].descriptorCount = 16; //Revisit

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = NULL;
	descriptorPoolCreateInfo.maxSets = 16; //Revisit
	descriptorPoolCreateInfo.poolSizeCount = 11;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
	/*
	This flag allows descriptors to return to the pool when they are freed. 
	If not set we'll have to reset the pool and frankly I don't want to code for that.
	The only reason I can think that you wouldn't want to do this is if it makes create/destroy cheaper but it hardly seems with it.
	*/
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	mResult = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, nullptr,&mDescriptorPool);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::CDevice9 vkCreateDescriptorPool failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 vkCreateDescriptorPool succeeded.";
	}

	//Create queue so we can submit command buffers.
	vkGetDeviceQueue(mDevice, mGraphicsQueueIndex, 0,&mQueue);

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
		//Appearently the swap chain size must match the surface size if it is defined.
		mSwapchainExtent = mSurfaceCapabilities.currentExtent;
		mPresentationParameters.BackBufferWidth = mSurfaceCapabilities.currentExtent.width;
		mPresentationParameters.BackBufferHeight = mSurfaceCapabilities.currentExtent.height;
	}

	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 the height/width of the surfaces are " << mPresentationParameters.BackBufferHeight << "/" << mPresentationParameters.BackBufferWidth;

	//initialize vulkan/d3d9 viewport and scissor structures.
	mViewport.width = (float)mPresentationParameters.BackBufferWidth;
	mViewport.height = (float)mPresentationParameters.BackBufferHeight;
	mViewport.minDepth = (float)0.0f;
	mViewport.maxDepth = (float)1.0f;

	m9Viewport.Width = mViewport.width;
	m9Viewport.Height = mViewport.height;
	m9Viewport.MinZ = mViewport.minDepth;
	m9Viewport.MaxZ = mViewport.maxDepth;

	mScissor.extent.width = mPresentationParameters.BackBufferWidth;
	mScissor.extent.height = mPresentationParameters.BackBufferHeight;
	mScissor.offset.x = 0; //Do I really need this if I initialize to zero?
	mScissor.offset.y = 0; //Do I really need this if I initialize to zero?

	m9Scissor.right = mScissor.extent.width;
	m9Scissor.bottom = mScissor.extent.height;
	m9Scissor.left = 0;
	m9Scissor.top = 0;

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
		mFormat = VK_FORMAT_B8G8R8A8_UNORM; //No prefered format so set a default.
	}
	else
	{
		mFormat = mSurfaceFormats[0].format; //Pull the prefered format.
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
	VK_PRESENT_MODE_FIFO_KHR - Wait for hte next virtical blanking interval to update the image. If the interval is missed wait for hte next one. New images will be queued for display.
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
	swapchainCreateInfo.imageColorSpace  = mSurfaceFormats[0].colorSpace;
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

	mResult = vkAllocateMemory(mDevice, &mDepthMemoryAllocateInfo, NULL,&mDepthDeviceMemory);
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

	mBufferManager = new BufferManager(this);

	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 Finished.";
} 

CDevice9::~CDevice9()
{
	BOOST_LOG_TRIVIAL(info) << "CDevice9::~CDevice9";

	delete mBufferManager;

	if (mFramebuffers!= nullptr)
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

	if (mSwapchainBuffers!= nullptr)
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
		//for (size_t i = 0; i < mSwapchainImageCount; i++)
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

}

HRESULT STDMETHODCALLTYPE CDevice9::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	//VK_FORMAT_B8G8R8A8_UNORM 
	//Revisit the byte order could be difference based on the surface format so I need a better way to handle this.
	mClearColorValue.float32[0] = D3DCOLOR_B(Color);
	mClearColorValue.float32[1] = D3DCOLOR_G(Color);
	mClearColorValue.float32[2] = D3DCOLOR_R(Color);
	mClearColorValue.float32[3] = D3DCOLOR_A(Color);

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
	//Acording to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndScene()
{
	//Acording to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}
	this->StopScene();

	return D3D_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CDevice9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DDevice9))
	{
		(*ppv) = this;
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CDevice9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
		return 0;
	}

	return mReferenceCount;
}

	
HRESULT STDMETHODCALLTYPE CDevice9::BeginStateBlock()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::BeginStateBlock is not implemented!";

	return E_NOTIMPL;
}



HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ColorFill is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::CreateAdditionalSwapChain is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CCubeTexture9* obj = new CCubeTexture9(this, EdgeLength,Levels,Usage,Format,Pool,pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppCubeTexture) = (IDirect3DCubeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this,Width,Height,Format,MultiSample,MultisampleQuality,Discard,pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CIndexBuffer9* obj = new CIndexBuffer9(this,Length,Usage,Format,Pool,pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppIndexBuffer) = (IDirect3DIndexBuffer9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this, Width, Height, Format, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader)
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

HRESULT STDMETHODCALLTYPE CDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery)
{
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

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	//I added an extra int at the end so the signature would be different for this version. Locakable/Discard are both BOOL.
	CSurface9* obj = new CSurface9(this, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, pSharedHandle,0);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB)
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

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle)
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

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle)
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

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl)
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

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader)
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

HRESULT STDMETHODCALLTYPE CDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle)
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

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitive is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitiveUP is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	/*
	We have to tell the manager what kind of buffers we're working with so it can build the pipe.
	*/
	mBufferManager->UpdatePipeline(PrimitiveType);

	vkCmdBindPipeline(mSwapchainBuffers[mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, mBufferManager->mPipeline);
	vkCmdBindDescriptorSets(mSwapchainBuffers[mCurrentBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS,mBufferManager->mPipelineLayout, 0, 1, &mBufferManager->mDescriptorSet, 0,nullptr);
	vkCmdSetViewport(mSwapchainBuffers[mCurrentBuffer], 0, 1, &mViewport);
	vkCmdSetScissor(mSwapchainBuffers[mCurrentBuffer], 0, 1, &mScissor);

	/*
	The buffer manager isn't doing much on this call but it may do more later.
	*/
	mBufferManager->BindVertexBuffers(PrimitiveType);

	vkCmdDraw(mSwapchainBuffers[mCurrentBuffer], mBufferManager->mVertexCount, 1, StartVertex, 0); //TODO: implement PrimitiveCount

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawPrimitiveUP is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawRectPatch is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo)
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
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::EndStateBlock is not implemented!";

	return E_NOTIMPL;
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

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetBackBuffer is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index,float *pPlane)
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
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetCreationParameters is not implemented!";

	return E_NOTIMPL;
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

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode)
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

	BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Height: " << pMode->Height;
	BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Width: " << pMode->Width;
	BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode RefreshRate: " << pMode->RefreshRate;
	BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Format: " << pMode->Format;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetFrontBufferData is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{
	(*pFVF) = mFVF;

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp)
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

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index,D3DLIGHT9 *pLight)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetLight is not implemented!";

	return E_NOTIMPL;
} 

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index,BOOL *pEnable)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetLightEnable is not implemented!";

	return S_OK;		
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetMaterial is not implemented!";

	return E_NOTIMPL;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetNPatchMode is not implemented!";

	return 0;
}

UINT STDMETHODCALLTYPE CDevice9::GetNumberOfSwapChains()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetNumberOfSwapChains is not implemented!";

	return 0;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPaletteEntries is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPixelShader is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPixelShaderConstantB is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPixelShaderConstantF is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPixelShaderConstantI is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRasterStatus is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue)
{
	(*pValue) = mRenderStates[State];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRenderTarget is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRenderTargetData is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetSamplerState is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT *pRect)
{
	(*pRect) = m9Scissor;

	return S_OK;
}

BOOL STDMETHODCALLTYPE CDevice9::GetSoftwareVertexProcessing()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetSoftwareVertexProcessing is not implemented!";

	return true;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride)
{
	StreamSource& value = mBufferManager->mStreamSources[StreamNumber];

	(*ppStreamData) = (IDirect3DVertexBuffer9*)value.StreamData;
	/*
	Vulkan wants 64bit uint but d3d9 uses 32bit uint. This cast just keeps compiler from complaining.
	This should be safe because only 32bit can be set and d3d9 is x86 only so endianness issues shouldn't come into play.
	*/
	(*pOffsetInBytes) = (UINT)value.OffsetInBytes;
	(*pStride) = value.Stride;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetStreamSourceFreq is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetSwapChain is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetTexture is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetTextureStageState is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	(*pMatrix) = mTransforms[State];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexDeclaration is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexShader is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexShaderConstantB is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexShaderConstantF is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetVertexShaderConstantI is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	(*pViewport) = m9Viewport;

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex,BOOL bEnable)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::LightEnable is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::MultiplyTransform is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags)
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

HRESULT CDevice9::SetClipPlane(DWORD Index,const float *pPlane)
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

void STDMETHODCALLTYPE CDevice9::SetCursorPosition(INT X,INT Y,DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetCursorPosition is not implemented!";

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap)
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
	mFVF = FVF; //uses D3DFVF_XYZ | D3DFVF_DIFFUSE by default.

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetGammaRamp is not implemented!";
}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetIndices is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index,const D3DLIGHT9 *pLight)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetLight is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetMaterial is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetNPatchMode is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPaletteEntries is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPixelShader is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPixelShaderConstantB is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPixelShaderConstantF is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPixelShaderConstantI is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	mRenderStates[State] = Value;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetRenderTarget is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetSamplerState is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT *pRect)
{
	m9Scissor = (*pRect);

	mScissor.extent.width = m9Scissor.right;
	mScissor.extent.height = m9Scissor.bottom;
	mScissor.offset.x = m9Scissor.left;
	mScissor.offset.y = m9Scissor.top;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetSoftwareVertexProcessing is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride)
{	
	CVertexBuffer9* streamData = (CVertexBuffer9*)pStreamData;

	mBufferManager->mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetStreamSourceFreq is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetTexture is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetTextureStageState is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	VkResult result;
	void* data=nullptr;

	mTransforms[State] = (*pMatrix);

	switch (State)
	{
	case D3DTS_WORLD:
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				mUBO.model[i][j] = pMatrix->m[i][j];
			}
		}	
		break;
	case D3DTS_VIEW:
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				mUBO.view[i][j] = pMatrix->m[i][j];
			}
		}
		break;
	case D3DTS_PROJECTION:
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				mUBO.proj[i][j] = pMatrix->m[i][j];
			}
		}
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetTransform The following state type was ignored. " << State;
		break;
	}

	result = vkMapMemory(mDevice, mBufferManager->mUniformStagingBufferMemory, 0, sizeof(UniformBufferObject), 0, &data);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetTransform vkMapMemory failed with return code of " << result;
		return D3DERR_INVALIDCALL;
	}
	memcpy(data, &mUBO, sizeof(UniformBufferObject));
	vkUnmapMemory(mDevice, mBufferManager->mUniformStagingBufferMemory);
	mBufferManager->CopyBuffer(mBufferManager->mUniformStagingBuffer, mBufferManager->mUniformBuffer, sizeof(UniformBufferObject));

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexDeclaration is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexShader is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexShaderConstantB is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexShaderConstantF is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetVertexShaderConstantI is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{
	m9Viewport = (*pViewport);

	mViewport.width = m9Viewport.Width;
	mViewport.height = m9Viewport.Height;
	mViewport.minDepth = m9Viewport.MinZ;
	mViewport.maxDepth = m9Viewport.MaxZ;

	return S_OK;	
}

BOOL STDMETHODCALLTYPE CDevice9::ShowCursor(BOOL bShow)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ShowCursor is not implemented!";

	return TRUE;	
}

HRESULT STDMETHODCALLTYPE CDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter)
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

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::UpdateSurface is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
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

void CDevice9::SetImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
	/*
	This is just a helper method to reduce repeat code.
	*/
	VkResult result = VK_SUCCESS;
	VkPipelineStageFlags sourceStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	// If the command buffer hasn't been created yet create it so it can be used.
	if (mCommandBuffer == VK_NULL_HANDLE)
	{
		VkCommandBufferAllocateInfo commandBufferInfo = {};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.pNext = nullptr;
		commandBufferInfo.commandPool = mCommandPool;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;

		result = vkAllocateCommandBuffers(mDevice, &commandBufferInfo, &mCommandBuffer);
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

		result = vkBeginCommandBuffer(mCommandBuffer, &commandBufferBeginInfo);
		if (result != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkBeginCommandBuffer failed with return code of " << mResult;
			return;
		}
	}

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = 0;
	imageMemoryBarrier.dstAccessMask = 0;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = { aspectMask, 0, 1, 0, 1 };

	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		break;
	case VK_IMAGE_LAYOUT_GENERAL:
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; //Added based on validation layer complaints.
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
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
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		break;
	default:
		break;
	}

	vkCmdPipelineBarrier(mCommandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	//flush init command

	if (mCommandBuffer != VK_NULL_HANDLE)
	{
		result = vkEndCommandBuffer(mCommandBuffer);
		if (result != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkEndCommandBuffer failed with return code of " << mResult;
			return;
		}

		VkCommandBuffer commandBuffers[] = { mCommandBuffer };
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
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkQueueSubmit failed with return code of " << mResult;
			return;
		}

		result = vkQueueWaitIdle(mQueue);
		if (result != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CDevice9::SetImageLayout vkQueueWaitIdle failed with return code of " << mResult;
			return;
		}

		vkFreeCommandBuffers(mDevice,mCommandPool, 1, commandBuffers);
		mCommandBuffer = VK_NULL_HANDLE;
	}
}

void CDevice9::StartScene()
{
	mIsSceneStarted = true;

	VkResult result; // = VK_SUCCESS

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
	mImageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	mImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	mImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mImageMemoryBarrier.image = mSwapchainImages[mCurrentBuffer];
	mImageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	vkCmdPipelineBarrier(mSwapchainBuffers[mCurrentBuffer], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &mImageMemoryBarrier);

	vkCmdBeginRenderPass(mSwapchainBuffers[mCurrentBuffer], &mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); //why doesn't this return a result.
}

void CDevice9::StopScene()
{
	mIsSceneStarted = false;

	VkResult result; // = VK_SUCCESS
	VkPipelineStageFlags pipeStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	mSubmitInfo.pNext = nullptr;
	mSubmitInfo.waitSemaphoreCount = 1;
	mSubmitInfo.pWaitSemaphores = &mPresentCompleteSemaphore;
	mSubmitInfo.pWaitDstStageMask = &pipeStageFlags;
	mSubmitInfo.commandBufferCount = 1;
	mSubmitInfo.pCommandBuffers = &mSwapchainBuffers[mCurrentBuffer];
	mSubmitInfo.signalSemaphoreCount = 0;
	mSubmitInfo.pSignalSemaphores = nullptr;

	vkCmdEndRenderPass(mSwapchainBuffers[mCurrentBuffer]); // Why no result?

	mPrePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	mPrePresentBarrier.pNext = nullptr;
	mPrePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	mPrePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	mPrePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	mPrePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	mPrePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mPrePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mPrePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	mPrePresentBarrier.image = mSwapchainImages[mCurrentBuffer];
	VkImageMemoryBarrier* memoryBarrier = &mPrePresentBarrier;
	vkCmdPipelineBarrier(mSwapchainBuffers[mCurrentBuffer], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0,nullptr, 1, memoryBarrier);

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

	mPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	mPresentInfo.pNext = nullptr;
	mPresentInfo.swapchainCount = 1;
	mPresentInfo.pSwapchains = &mSwapchain;
	mPresentInfo.pImageIndices = &mCurrentBuffer;

	result = vkQueuePresentKHR(mQueue, &mPresentInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkQueuePresentKHR failed with return code of " << mResult;
		return;
	}

	result = vkQueueWaitIdle(mQueue);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkQueueWaitIdle failed with return code of " << mResult;
		return;
	}

	vkDestroySemaphore(mDevice, mPresentCompleteSemaphore, nullptr);
}