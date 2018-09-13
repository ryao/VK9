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

#include "RealInstance.h"

RealInstance::RealInstance(bool enableDebugLayers)
{
	BOOST_LOG_TRIVIAL(info) << "RealInstance::RealInstance";

	std::vector<char*> extensionNames;
	std::vector<char*> layerNames;

	extensionNames.push_back("VK_KHR_surface");
	extensionNames.push_back("VK_KHR_win32_surface");
	extensionNames.push_back("VK_KHR_get_physical_device_properties2");

	if (enableDebugLayers)
	{
		extensionNames.push_back("VK_EXT_debug_report");
		//extensionNames.push_back("VK_EXT_debug_marker");
		layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	HINSTANCE instance = LoadLibraryA("renderdoc.dll");
	mRenderDocDll = GetModuleHandleA("renderdoc.dll");
	if (mRenderDocDll != nullptr)
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mRenderDocDll, "RENDERDOC_GetAPI");
		if (RENDERDOC_GetAPI != nullptr)
		{
			int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&mRenderDocApi);
			if (ret != 1)
			{
				BOOST_LOG_TRIVIAL(warning) << "RealInstance::RealInstance unable to find RENDERDOC_API_Version_1_1_ !";
			}
			else
			{
				mRenderDocApi->SetLogFilePathTemplate("vk");
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "RealInstance::RealInstance unable to find RENDERDOC_GetAPI !";
		}

	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "RealInstance::RealInstance unable to find renderdoc.dll !";
	}

	vk::Result result;
	vk::ApplicationInfo applicationInfo("VK9", 1, "VK9", 1, VK_MAKE_VERSION(1, 1, 0));
	vk::InstanceCreateInfo createInfo({}, &applicationInfo, layerNames.size(), layerNames.data(), extensionNames.size(), extensionNames.data());

	result = vk::createInstance(&createInfo, nullptr, &mInstance);
	if (result == vk::Result::eSuccess)
	{
		//Fetch an array of available physical devices.
		result = mInstance.enumeratePhysicalDevices(&mPhysicalDeviceCount, nullptr);
		if (result == vk::Result::eSuccess)
		{
			mPhysicalDevices = new vk::PhysicalDevice[mPhysicalDeviceCount];
			mInstance.enumeratePhysicalDevices(&mPhysicalDeviceCount, mPhysicalDevices);
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance No physical devices were found.";
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "StateManager::CreateInstance failed to create vulkan instance.";
	}
}

RealInstance::~RealInstance()
{
	BOOST_LOG_TRIVIAL(info) << "RealInstance::~RealInstance";

	if (mRenderDocDll != nullptr)
	{
		FreeLibrary(mRenderDocDll);
	}

	mInstance.destroyDebugReportCallbackEXT(mCallback);

	mInstance.destroy();
}