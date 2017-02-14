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

#include "CSurface9.h"
#include "CDevice9.h"
#include "CTexture9.h"
#include "Utilities.h"
#include "CTypes.h"

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mSharedHandle(pSharedHandle)
{
	Init();
}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, HANDLE *pSharedHandle, int32_t filler)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mLockable(Lockable),
	mSharedHandle(pSharedHandle)
{
	Init();
}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle)
{
	Init();
}

void CSurface9::Init()
{
	//mDevice->AddRef();

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172611(v=vs.85).aspx
	Only these two are valid usage values for surface per the documentation.
	*/
	switch (mUsage)
	{
	case D3DUSAGE_DEPTHSTENCIL:
		break;
	case D3DUSAGE_RENDERTARGET:
		break;
	default:
		mUsage = D3DUSAGE_DEPTHSTENCIL;
		break;
	}

	mRealFormat = ConvertFormat(mFormat);
}

CSurface9::~CSurface9()
{
	//BOOST_LOG_TRIVIAL(info) << "CSurface9::~CSurface9";

	if (mStagingImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(mDevice->mDevice, mStagingImage, NULL);
		mStagingImage = VK_NULL_HANDLE;
	}

	if (mStagingDeviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mStagingDeviceMemory, NULL);
		mStagingDeviceMemory = VK_NULL_HANDLE;
	}

	//mDevice->Release();
}

ULONG STDMETHODCALLTYPE CSurface9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CSurface9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DSurface9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
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

ULONG STDMETHODCALLTYPE CSurface9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
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
	mFlags = Flags;

	//BOOST_LOG_TRIVIAL(info) << "CSurface9::LockRect Level:" << mMipIndex << " Handle: " << this << " Flags: " << mFlags;

	Prepare();

	if (mData == nullptr)
	{
		mResult = vkMapMemory(mDevice->mDevice, mStagingDeviceMemory, 0, mMemoryAllocateInfo.allocationSize, 0, &mData);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CSurface9::LockRect vkMapMemory failed with return code of " << mResult;
			if ((mFlags & D3DLOCK_DONOTWAIT) == D3DLOCK_DONOTWAIT)
			{
				return D3DERR_WASSTILLDRAWING;
			}
			else
			{
				return D3DERR_INVALIDCALL;
			}
		}
	}

	/*
	Dear C++ committee Why can't we just assume increment is in bytes for void pointer?
	I mean what else could I possibly mean when dealing with an untyped binary blob.
	It's not like I picked void* so "don't use void*" is a terrible argument.
	*/

	if (mLayout.offset)
	{
		char* bytes = (char*)mData;
		bytes += mLayout.offset;
		mData = (void*)bytes;
	}

	if (pRect != nullptr)
	{
		char* bytes = (char*)mData;

		bytes += (mLayout.rowPitch * pRect->top);
		bytes += (4 * pRect->left);
		mData = (void*)bytes;
	}

	pLockedRect->pBits = mData;
	pLockedRect->Pitch = mLayout.rowPitch;

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
	//counter++;

	//color_A8R8G8B8* colors;
	//colors = (color_A8R8G8B8*)mData;

	//if (mMipIndex==0)
	//{
	//	SaveImage((boost::format("image%1%_%2%.ppm") % this % counter).str().c_str(), (char*)mData, mHeight, mWidth, mLayout.rowPitch);
	//	BOOST_LOG_TRIVIAL(info) << "CSurface9::UnlockRect " << (boost::format("image%1%_%2%.ppm") % this % counter).str().c_str() << " Format:" << mFormat;
	//}

	if (mFormat == D3DFMT_X8R8G8B8)
	{
		SetAlpha((char*)mData, mHeight, mWidth, mLayout.rowPitch);
	}

	if (mData != nullptr)
	{
		vkUnmapMemory(mDevice->mDevice, mStagingDeviceMemory);
		mData = nullptr;
	}

	if ((mFlags & D3DLOCK_DISCARD) == D3DLOCK_DISCARD) // || ((mUsage & D3DUSAGE_DEPTHSTENCIL) == D3DUSAGE_DEPTHSTENCIL) && counter < 3
	{
		if (mStagingImage != VK_NULL_HANDLE)
		{
			mDevice->mGarbageManager.mImages.push_back(mStagingImage);
			mStagingImage = VK_NULL_HANDLE;
		}

		if (mStagingDeviceMemory != VK_NULL_HANDLE)
		{
			mDevice->mGarbageManager.mMemories.push_back(mStagingDeviceMemory);
			mStagingDeviceMemory = VK_NULL_HANDLE;
		}
	}

	//this->Flush();

	return S_OK;
}

void CSurface9::Prepare()
{
	if (mStagingDeviceMemory != VK_NULL_HANDLE)
	{
		if (mIsFlushed)
		{
			this->mDevice->SetImageLayout(mStagingImage, 0, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 1, 0); //VK_IMAGE_LAYOUT_PREINITIALIZED
			mIsFlushed = false;
		}		
		return;
	}

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = { mWidth, mHeight, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	mResult = vkCreateImage(mDevice->mDevice, &imageCreateInfo, NULL, &mStagingImage);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::Prepare vkCreateImage failed with return code of " << mResult;
		return;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->mDevice, mStagingImage, &memoryRequirements);

	mMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mMemoryAllocateInfo.pNext = NULL;
	mMemoryAllocateInfo.allocationSize = 0;
	mMemoryAllocateInfo.memoryTypeIndex = 0;
	mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(mDevice->mDeviceMemoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::Prepare Could not find memory type from properties.";
		return;
	}

	mResult = vkAllocateMemory(mDevice->mDevice, &mMemoryAllocateInfo, NULL, &mStagingDeviceMemory);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::Prepare vkAllocateMemory failed with return code of " << mResult;
		return;
	}

	mResult = vkBindImageMemory(mDevice->mDevice, mStagingImage, mStagingDeviceMemory, 0);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::Prepare vkBindImageMemory failed with return code of " << mResult;
		return;
	}

	mSubresource.mipLevel = 0;
	mSubresource.arrayLayer = 0; //if this is wrong you may get 4294967296.
	mSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	vkGetImageSubresourceLayout(mDevice->mDevice, mStagingImage, &mSubresource, &mLayout);
}

void CSurface9::Flush()
{
	if (mStagingDeviceMemory == VK_NULL_HANDLE)
	{
		return;
	}

	mIsFlushed = true;

	this->mDevice->SetImageLayout(mStagingImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 0); //VK_IMAGE_LAYOUT_PREINITIALIZED
	this->mDevice->SetImageLayout(mTexture->mImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, mMipIndex);

	mTexture->CopyImage(mStagingImage, mTexture->mImage, mWidth, mHeight, 0, this->mMipIndex);

	this->mDevice->SetImageLayout(mTexture->mImage, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, mMipIndex);


	//if (mStagingImage != VK_NULL_HANDLE)
	//{
	//	mDevice->mGarbageManager.mImages.push_back(mStagingImage);
	//	mStagingImage = VK_NULL_HANDLE;
	//}

	//if (mStagingDeviceMemory != VK_NULL_HANDLE)
	//{
	//	mDevice->mGarbageManager.mMemories.push_back(mStagingDeviceMemory);
	//	mStagingDeviceMemory = VK_NULL_HANDLE;
	//}
}