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
 
#include "CSurface9.h"
#include "CDevice9.h"

#include "Utilities.h"

CSurface9::CSurface9(CDevice9* Device, IDirect3DTexture9* Texture,UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard,HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(0),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mLockable(0),
	mPool(D3DPOOL_DEFAULT),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1),
	mResult(VK_SUCCESS),

	mSampler(VK_NULL_HANDLE),
	mImage(VK_NULL_HANDLE),
	mImageLayout(VK_IMAGE_LAYOUT_GENERAL),
	mDeviceMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE),

	mRealFormat(VK_FORMAT_R8G8B8A8_UNORM),
	mData(nullptr)
{
	mDevice->AddRef();
	Init();
}

CSurface9::CSurface9(CDevice9* Device, IDirect3DTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, HANDLE *pSharedHandle, int32_t filler)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(0),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(0),
	mLockable(Lockable),
	mPool(D3DPOOL_DEFAULT),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1),
	mResult(VK_SUCCESS),

	mSampler(VK_NULL_HANDLE),
	mImage(VK_NULL_HANDLE),
	mImageLayout(VK_IMAGE_LAYOUT_GENERAL),
	mDeviceMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE),

	mRealFormat(VK_FORMAT_R8G8B8A8_UNORM),
	mData(nullptr)
{
	mDevice->AddRef();
	Init();
}

CSurface9::CSurface9(CDevice9* Device, IDirect3DTexture9* Texture, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mFormat(Format),
	mMultiSample(D3DMULTISAMPLE_NONE),
	mMultisampleQuality(0),
	mDiscard(0),
	mLockable(0),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1),
	mResult(VK_SUCCESS),

	mSampler(VK_NULL_HANDLE),
	mImage(VK_NULL_HANDLE),
	mImageLayout(VK_IMAGE_LAYOUT_GENERAL),
	mDeviceMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE),

	mRealFormat(VK_FORMAT_R8G8B8A8_UNORM),
	mData(nullptr)
{
	mDevice->AddRef();
	Init();
}

void CSurface9::Init()
{
	mRealFormat = ConvertFormat(mFormat);

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
										  //imageCreateInfo.extent = { mWidth, mHeight, 1 };
	imageCreateInfo.extent = { 1, 1 ,1 }; //testing
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	mMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mMemoryAllocateInfo.pNext = NULL;
	mMemoryAllocateInfo.allocationSize = 0;
	mMemoryAllocateInfo.memoryTypeIndex = 0;

	VkMemoryRequirements memoryRequirements = {};

	mResult = vkCreateImage(mDevice->mDevice, &imageCreateInfo, NULL, &mImage);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::CSurface9 vkCreateImage failed with return code of " << mResult;
		return;
	}

	vkGetImageMemoryRequirements(mDevice->mDevice, mImage, &memoryRequirements);

	mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(mDevice->mDeviceMemoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::CSurface9 Could not find memory type from properties.";
		return;
	}

	mResult = vkAllocateMemory(mDevice->mDevice, &mMemoryAllocateInfo, NULL, &mDeviceMemory);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::CSurface9 vkAllocateMemory failed with return code of " << mResult;
		return;
	}

	mResult = vkBindImageMemory(mDevice->mDevice, mImage, mDeviceMemory, 0);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::CSurface9 vkBindImageMemory failed with return code of " << mResult;
		return;
	}

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = NULL;
	samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	imageViewCreateInfo.pNext = NULL;
	imageViewCreateInfo.image = VK_NULL_HANDLE;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = mRealFormat;
	imageViewCreateInfo.components =
	{
		VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
		VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
	};
	imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	imageViewCreateInfo.flags = 0;

	mResult = vkCreateSampler(mDevice->mDevice, &samplerCreateInfo, NULL, &mSampler);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::CSurface9 vkCreateSampler failed with return code of " << mResult;
		return;
	}

	imageViewCreateInfo.image = mImage;
	mResult = vkCreateImageView(mDevice->mDevice, &imageViewCreateInfo, NULL, &mImageView);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::CSurface9 vkCreateImageView failed with return code of " << mResult;
		return;
	}
}

CSurface9::~CSurface9()
{
	if (mImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mDevice->mDevice, mImageView, NULL);
	}
	if (mImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(mDevice->mDevice, mImage, NULL);
	}
	if (mDeviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mDeviceMemory, NULL);
	}
	if (mSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(mDevice->mDevice, mSampler, NULL);
	}

	mDevice->Release();
}

ULONG STDMETHODCALLTYPE CSurface9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CSurface9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DSurface9))
	{
		(*ppv) = this;
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
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

ULONG STDMETHODCALLTYPE CSurface9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
		return 0;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CSurface9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CSurface9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CSurface9::GetType()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetType is not implemented!";

	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CSurface9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CSurface9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetContainer(REFIID riid, void** ppContainer)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetContainer is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDC(HDC* phdc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetDC is not implemented!";

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CSurface9::GetDesc(D3DSURFACE_DESC* pDesc)
{
	pDesc->Format = this->mFormat;
	pDesc->Type = D3DRTYPE_SURFACE;
	pDesc->Usage = this->mUsage;
	pDesc->Pool = this->mPool;

	pDesc->MultiSampleType = this->mMultiSample;
	pDesc->MultiSampleQuality = this->mMultisampleQuality;
	pDesc->Width = this->mWidth;
	pDesc->Height = this->mHeight;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CSurface9::LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	VkResult result = VK_SUCCESS;

	result = vkMapMemory(mDevice->mDevice, mDeviceMemory, 0, mMemoryAllocateInfo.allocationSize, 0, &mData);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::LockRect vkMapMemory failed with return code of " << result;
		pLockedRect->pBits = nullptr;
		return D3DERR_INVALIDCALL;
	}

	pLockedRect->pBits = mData;

	return S_OK;
}


HRESULT STDMETHODCALLTYPE CSurface9::ReleaseDC(HDC hdc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::ReleaseDC is not implemented!";

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CSurface9::UnlockRect()
{
	vkUnmapMemory(mDevice->mDevice, mDeviceMemory); //No return value so I can't verify success.
	mData = nullptr;

	return S_OK;
}