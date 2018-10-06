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

#include "RealSurface.h"
#include "CSurface9.h"
#include "CVolume9.h"

#include "Utilities.h"

RealSurface::RealSurface(RealDevice* realDevice, CSurface9* surface9, vk::Image* parentImage)
	: mRealDevice(realDevice)
{
	BOOST_LOG_TRIVIAL(info) << "RealSurface::RealSurface";

	vk::Result result;

	mRealFormat = ConvertFormat(surface9->mFormat);

	if (mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
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

			BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface unknown format: " << four;
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface unknown format: " << surface9->mFormat;
		}

	}

	vk::ImageCreateInfo imageCreateInfo;
	vk::ImageViewCreateInfo imageViewCreateInfo;
	vk::FormatProperties formatProperties;
	realDevice->mPhysicalDevice.getFormatProperties(mRealFormat, &formatProperties);

	if (mRealFormat == vk::Format::eD16UnormS8Uint || mRealFormat == vk::Format::eD24UnormS8Uint || mRealFormat == vk::Format::eD32SfloatS8Uint)
	{
		if (!formatProperties.linearTilingFeatures && !formatProperties.optimalTilingFeatures && !formatProperties.bufferFeatures)
		{
			mRealFormat = vk::Format::eD32SfloatS8Uint; //This is probably an AMD card.
		}

		mSubresource.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
	}
	else if (mRealFormat == vk::Format::eS8Uint)
	{
		mSubresource.aspectMask = vk::ImageAspectFlagBits::eStencil;
		imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eStencil;
	}
	else if (mRealFormat == vk::Format::eD16Unorm || mRealFormat == vk::Format::eD32Sfloat)
	{
		mSubresource.aspectMask = vk::ImageAspectFlagBits::eDepth;
		imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
	}
	else
	{
		mSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = vk::Extent3D(surface9->mWidth, surface9->mHeight, 1);
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;

	if (surface9->mTexture != nullptr || surface9->mCubeTexture != nullptr)
	{
		imageCreateInfo.tiling = vk::ImageTiling::eLinear;
		imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;

		imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
	}
	else
	{
		imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
		imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized; //ePreinitialized

		if (surface9->mUsage == D3DUSAGE_DEPTHSTENCIL)
		{
			imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eDepthStencilAttachment;
		}
		else
		{
			imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment; //
		}
	}

	mExtent = imageCreateInfo.extent;

	
	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	if (surface9->mTexture != nullptr || surface9->mCubeTexture != nullptr)
	{
		imageAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	}
	else
	{
		imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	}

	result = (vk::Result)vmaCreateImage(mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&mStagingImage, &mImageAllocation, &mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mSubresource.mipLevel = 0;
	mSubresource.arrayLayer = 0; //if this is wrong you may get 4294967296.
	mLayouts[0] = {};

	if (imageCreateInfo.tiling == vk::ImageTiling::eLinear)
	{
		//BOOST_LOG_TRIVIAL(info) << "RealSurface::RealSurface (CSurface9) using format " << (VkFormat)mRealFormat;
		realDevice->mDevice.getImageSubresourceLayout(mStagingImage, &mSubresource, &mLayouts[0]);
	}

	imageViewCreateInfo.image = mStagingImage;
	//imageViewCreateInfo.viewType = vk::ImageViewType::e3D;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = mRealFormat;

	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	if (surface9->mTexture != nullptr || surface9->mCubeTexture != nullptr)
	{
		switch (surface9->mFormat)
		{
		case D3DFMT_A8:
			//TODO: Revisit A8 mapping.
			imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
			break;
		case D3DFMT_L8:
			imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
			break;
		case D3DFMT_L16:
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
	}
	else
	{
		imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
		imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
		imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
		imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	}

	if (surface9->mTexture == nullptr && surface9->mCubeTexture == nullptr)
	{
		result = realDevice->mDevice.createImageView(&imageViewCreateInfo, nullptr, &mStagingImageView);
		if (result != vk::Result::eSuccess)
		{
			BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
			return;
		}
	}
	else if (parentImage != nullptr)
	{
		imageViewCreateInfo.image = (*parentImage);

		result = realDevice->mDevice.createImageView(&imageViewCreateInfo, nullptr, &mStagingImageView);
		if (result != vk::Result::eSuccess)
		{
			BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
			return;
		}
	}
}

RealSurface::RealSurface(RealDevice* realDevice, CVolume9* volume9)
	: mRealDevice(realDevice)
{
	BOOST_LOG_TRIVIAL(info) << "RealSurface::RealSurface";

	vk::Result result;

	mRealFormat = ConvertFormat(volume9->mFormat);

	if (mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
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

			BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface unknown format: " << four;
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface unknown format: " << volume9->mFormat;
		}

	}

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e3D;
	imageCreateInfo.format = mRealFormat;
	imageCreateInfo.extent = vk::Extent3D(volume9->mWidth, volume9->mHeight, volume9->mDepth);
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eLinear;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
	imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;

	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	imageAllocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	result = (vk::Result)vmaCreateImage(mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&mStagingImage, &mImageAllocation, &mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealSurface::RealSurface vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mSubresource.mipLevel = 0;
	mSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	mSubresource.arrayLayer = 0; //if this is wrong you may get 4294967296.

	//BOOST_LOG_TRIVIAL(info) << "RealSurface::RealSurface (CVolume9) using format " << (VkFormat)mRealFormat;
	realDevice->mDevice.getImageSubresourceLayout(mStagingImage, &mSubresource, &mLayouts[0]);
}

RealSurface::~RealSurface()
{
	BOOST_LOG_TRIVIAL(info) << "RealSurface::~RealSurface";
	if (mRealDevice != nullptr)
	{
		auto& device = mRealDevice->mDevice;
		device.destroyImageView(mStagingImageView, nullptr);

		vmaDestroyImage(mRealDevice->mAllocator, (VkImage)mStagingImage, mImageAllocation);
	}
}