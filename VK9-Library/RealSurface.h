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

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <boost/container/small_vector.hpp>

#include "Utilities.h"
#include "CTypes.h"

#include "RealDevice.h"
#include "RealInstance.h"
#include "RealWindow.h"

#ifndef REALSURFACE_H
#define REALSURFACE_H

struct RealSurface
{
	BOOL mIsFlushed = false;
	void* mData = nullptr;
	vk::Image mStagingImage;
	vk::DeviceMemory mStagingDeviceMemory;

	vk::Format mRealFormat = vk::Format::eR8G8B8A8Unorm;
	vk::MemoryAllocateInfo mMemoryAllocateInfo;
	vk::ImageLayout mImageLayout = vk::ImageLayout::eGeneral;
	vk::SubresourceLayout mLayouts[1] = {};
	vk::ImageSubresource mSubresource;

	RealWindow* mRealWindow = nullptr; //null if not owner.
	RealSurface(RealWindow* realWindow);
	~RealSurface();
};

#endif // REALSURFACE_H
