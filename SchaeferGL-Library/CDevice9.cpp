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
	mCommandBuffer(VK_NULL_HANDLE),
	mQueue(VK_NULL_HANDLE),
	mPresentCompleteSemaphore(VK_NULL_HANDLE),
	mRenderPass(VK_NULL_HANDLE),
	mDepthFormat(VK_FORMAT_UNDEFINED),
	mDepthImage(VK_NULL_HANDLE),
	mDepthDeviceMemory(VK_NULL_HANDLE),
	mDepthView(VK_NULL_HANDLE)
{
	memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));

	if (mInstance->mGpuCount == 0)
	{
		mResult = VK_RESULT_MAX_ENUM;
		BOOST_LOG_TRIVIAL(fatal) << "No physical devices were found so CDevice9 could not be created.";
		return;
	}
	mPhysicalDevice = mInstance->mPhysicalDevices[mAdapter]; //pull the selected physical device from the instance.

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

	//Create queue so we can submit command buffers.
	vkGetDeviceQueue(mDevice, mGraphicsQueueIndex, 0,&mQueue);

	/*
	Now pull some information about the surface so we can create the swapchain correctly.
	*/

	if (mSurfaceCapabilities.currentExtent.width == (uint32_t)-1) 
	{
		//If the height/width are -1 then just set it to the requested size and hope for the best.
		mSwapchainExtent.width = mPresentationParameters.BackBufferWidth;
		mSwapchainExtent.height = mPresentationParameters.BackBufferHeight;
	}
	else 
	{
		//Appearently the swap chain size must match the surface size if it is defined.
		mSwapchainExtent = mSurfaceCapabilities.currentExtent;
	}

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
	swapchainCreateInfo.imageFormat = mFormat; //ConvertFormat(mPresentationParameters.BackBufferFormat);
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
		color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
		color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
		color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
		color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
		color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		color_image_view.subresourceRange.baseMipLevel = 0;
		color_image_view.subresourceRange.levelCount = 1;
		color_image_view.subresourceRange.baseArrayLayer = 0;
		color_image_view.subresourceRange.layerCount = 1;
		color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		color_image_view.flags = 0;

		SetImageLayout(mSwapchainImages[i], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = mDepthFormat;
	imageCreateInfo.extent = { mSwapchainExtent.width, mSwapchainExtent.height, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.flags = 0;

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = NULL;
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

	mResult = vkCreateImage(mDevice, &imageCreateInfo, NULL, &mDepthImage);
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

	/*
	pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits,
		0, 
		&demo->depth.mem_alloc.memoryTypeIndex);
	*/

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
	renderAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	renderAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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
}

CDevice9::~CDevice9()
{
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

	if (mSwapchainImages != nullptr)
	{
		//For some reason destroying the images causes a crash. I'm guessing it's a double free or something like that because the views have already been destroyed.
		//for (size_t i = 0; i < mSwapchainImageCount; i++)
		//{
		//	if (mSwapchainImages[i] != VK_NULL_HANDLE)
		//	{
		//		vkDestroyImage(mDevice, mSwapchainImages[i], nullptr);
		//	}	
		//}
		delete[] mSwapchainImages;
	}

	if (mDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(mDevice, nullptr);
	}	

	if (mSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(mInstance->mInstance, mSurface, nullptr);
	}

	if (mPresentationModes != nullptr)
	{
		delete[] mPresentationModes;
	}

	if (mSurfaceFormats != nullptr)
	{
		delete[] mSurfaceFormats;
	}

	if (mDisplays != nullptr)
	{
		delete[] mDisplays;
	}
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
	VkResult result = VK_SUCCESS;
	VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
	
	presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	presentCompleteSemaphoreCreateInfo.pNext = nullptr;
	presentCompleteSemaphoreCreateInfo.flags = 0;

	result = vkCreateSemaphore(mDevice, &presentCompleteSemaphoreCreateInfo, nullptr, &mPresentCompleteSemaphore);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::BeginScene vkCreateSemaphore failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	result = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mPresentCompleteSemaphore, (VkFence)0, &mCurrentBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::BeginScene vkAcquireNextImageKHR failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	SetImageLayout(mSwapchainImages[mCurrentBuffer], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	commandBufferInheritanceInfo.queryFlags = 0;
	commandBufferInheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	VkClearValue clearValues[2];
	clearValues[0].color = mClearColorValue;
	//clearValues[0].color.float32[0] = 0.2f;
	//clearValues[0].color.float32[1] = 0.2f;
	//clearValues[0].color.float32[2] = 0.2f;
	//clearValues[0].color.float32[3] = 0.2f;
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = mRenderPass;
	renderPassBeginInfo.framebuffer = mFramebuffers[mCurrentBuffer];
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = mSwapchainExtent.width;
	renderPassBeginInfo.renderArea.extent.height = mSwapchainExtent.height;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	result = vkBeginCommandBuffer(mSwapchainBuffers[mCurrentBuffer], &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::BeginScene vkBeginCommandBuffer failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	vkCmdBeginRenderPass(mSwapchainBuffers[mCurrentBuffer], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); //why doesn't this return a result.

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndScene()
{
	VkResult result = VK_SUCCESS;
	VkPipelineStageFlags pipeStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VkSubmitInfo submitInfo = {};

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &mPresentCompleteSemaphore;
	submitInfo.pWaitDstStageMask = &pipeStageFlags;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mSwapchainBuffers[mCurrentBuffer];
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	vkCmdEndRenderPass(mSwapchainBuffers[mCurrentBuffer]); // Why no result?

	result = vkEndCommandBuffer(mSwapchainBuffers[mCurrentBuffer]);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkEndCommandBuffer failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	result = vkQueueSubmit(mQueue, 1, &submitInfo, mNullFence);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkQueueSubmit failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	result = vkQueueWaitIdle(mQueue);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::EndScene vkQueueWaitIdle failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	VkResult result = VK_SUCCESS;

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &mSwapchain;
	presentInfo.pImageIndices = &mCurrentBuffer;

	SetImageLayout(mSwapchainImages[mCurrentBuffer], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	result = vkQueuePresentKHR(mQueue, &presentInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::Present vkQueuePresentKHR failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	vkDestroySemaphore(mDevice, mPresentCompleteSemaphore, nullptr);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::Present vkDestroySemaphore failed with return code of " << mResult;
		return D3DERR_INVALIDCALL;
	}

	return D3D_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CDevice9::QueryInterface(REFIID riid,void  **ppv)
{
	//TODO: Implement.

	return S_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
}

	
HRESULT STDMETHODCALLTYPE CDevice9::BeginStateBlock()
{
	//TODO: Implement.

	return E_NOTIMPL;
}



HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	IDirect3DCubeTexture9* obj = new CCubeTexture9(this, EdgeLength,Levels,Usage,Format,Pool,pSharedHandle);

	(*ppCubeTexture) = (IDirect3DCubeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	IDirect3DSurface9* obj = new CSurface9(this,Width,Height,Format,MultiSample,MultisampleQuality,Discard,pSharedHandle);

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DeletePatch(UINT Handle)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	//TODO: Implement.

	return E_NOTIMPL;
}
	
HRESULT STDMETHODCALLTYPE CDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EvictManagedResources()
{
	//TODO: Implement.

	return S_OK;	
}

UINT STDMETHODCALLTYPE CDevice9::GetAvailableTextureMem()
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index,float *pPlane)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	(*ppD3D9) = (IDirect3D9*)mInstance;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{
	//TODO: Implement.

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData) //,UINT *pBaseVertexIndex ?
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index,D3DLIGHT9 *pLight)
{
	//TODO: Implement.

	return E_NOTIMPL;
} 

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index,BOOL *pEnable)
{
	//TODO: Implement.

	return S_OK;		
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	//TODO: Implement.

	return 0;
}

UINT STDMETHODCALLTYPE CDevice9::GetNumberOfSwapChains()
{
	//TODO: Implement.

	return 0;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT *pRect)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

BOOL STDMETHODCALLTYPE CDevice9::GetSoftwareVertexProcessing()
{
	//TODO: Implement.

	return true;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	//TODO: Implement.

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex,BOOL bEnable)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{	
	//TODO: Implement.

	return S_OK;		
}

HRESULT CDevice9::SetClipPlane(DWORD Index,const float *pPlane)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CDevice9::SetCursorPosition(INT X,INT Y,DWORD Flags)
{
	//TODO: Implement.

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetFVF(DWORD FVF)
{
	//TODO: Implement.

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index,const D3DLIGHT9 *pLight)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT *pRect)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride)
{
	//TODO: Implement.
		
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{
	//TODO: Implement.

	return S_OK;	
}

BOOL STDMETHODCALLTYPE CDevice9::ShowCursor(BOOL bShow)
{
	//TODO: Implement.

	return TRUE;	
}

HRESULT STDMETHODCALLTYPE CDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::TestCooperativeLevel()
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ValidateDevice(DWORD *pNumPasses)
{
	//TODO: Implement.

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

	if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; //Added based on validation layer complaints.
	}

	if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
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