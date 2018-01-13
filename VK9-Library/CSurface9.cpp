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
#include "CCubeTexture9.h"
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

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, HANDLE *pSharedHandle)
	: mDevice(Device),
	mCubeTexture(Texture),
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

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, HANDLE *pSharedHandle, int32_t filler)
	: mDevice(Device),
	mCubeTexture(Texture),
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

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mCubeTexture(Texture),
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

	if (mCubeTexture != nullptr)
	{
		imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	mResult = vkCreateImage(mDevice->mDevice, &imageCreateInfo, NULL, &mStagingImage);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::Prepare vkCreateImage failed with return code of " << GetResultString(mResult);
		return;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->mDevice, mStagingImage, &memoryRequirements);

	mMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mMemoryAllocateInfo.pNext = NULL;
	//mMemoryAllocateInfo.allocationSize = 0;
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
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::Prepare vkAllocateMemory failed with return code of " << GetResultString(mResult);
		return;
	}

	mResult = vkBindImageMemory(mDevice->mDevice, mStagingImage, mStagingDeviceMemory, 0);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CSurface9::Prepare vkBindImageMemory failed with return code of " << GetResultString(mResult);
		return;
	}

	mSubresource.mipLevel = 0;
	//mSubresource.arrayLayer = 0; //if this is wrong you may get 4294967296.
	mSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	mSubresource.arrayLayer = 0;

	vkGetImageSubresourceLayout(mDevice->mDevice, mStagingImage, &mSubresource, &mLayouts[0]);
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

HRESULT STDMETHODCALLTYPE CSurface9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
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
	
	char* bytes = nullptr;

	//BOOST_LOG_TRIVIAL(info) << "CSurface9::LockRect Level:" << mMipIndex << " Handle: " << this << " Flags: " << mFlags;

	if (mData == nullptr)
	{
		if (mIsFlushed)
		{
			this->mDevice->SetImageLayout(mStagingImage, 0, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL); //VK_IMAGE_LAYOUT_PREINITIALIZED			
		}

		mResult = vkMapMemory(mDevice->mDevice, mStagingDeviceMemory, 0, mMemoryAllocateInfo.allocationSize, 0, &mData);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "CSurface9::LockRect vkMapMemory failed with return code of " << GetResultString(mResult);
			if ((mFlags & D3DLOCK_DONOTWAIT) == D3DLOCK_DONOTWAIT)
			{
				return D3DERR_WASSTILLDRAWING;
			}
			else
			{
				return D3DERR_INVALIDCALL;
			}
		}

		bytes = (char*)mData;

		if (mLayouts[0].offset)
		{
			bytes += mLayouts[0].offset;
		}

		if (pRect != nullptr)
		{
			bytes += (mLayouts[0].rowPitch * pRect->top);
			bytes += (4 * pRect->left);
		}
	}

	pLockedRect->pBits = (void*)bytes;
	pLockedRect->Pitch = mLayouts[0].rowPitch;

	mIsFlushed = false;

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
	if (mData != nullptr)
	{
		if (mFormat == D3DFMT_X8R8G8B8)
		{
			SetAlpha((char*)mData, mHeight, mWidth, mLayouts[0].rowPitch);
		}

		vkUnmapMemory(mDevice->mDevice, mStagingDeviceMemory);
		mData = nullptr;
	}

	this->Flush();

	return S_OK;
}

void CSurface9::Flush()
{
	if (mIsFlushed)
	{
		return;
	}
	mIsFlushed = true;

	VkCommandBuffer commandBuffer;

	VkCommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = mDevice->mCommandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	mResult = vkAllocateCommandBuffers(mDevice->mDevice, &commandBufferInfo, &commandBuffer);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkAllocateCommandBuffers failed with return code of " << GetResultString(mResult);
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

	mResult = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkBeginCommandBuffer failed with return code of " << GetResultString(mResult);
		return;
	}

	ReallySetImageLayout(commandBuffer, mStagingImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 0,1);
	ReallySetImageLayout(commandBuffer, mTexture->mImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, mMipIndex, mTargetLayer+1);
	ReallyCopyImage(commandBuffer, mStagingImage, mTexture->mImage,0,0, mWidth, mHeight, 0, this->mMipIndex, 0, mTargetLayer);
	ReallySetImageLayout(commandBuffer, mTexture->mImage, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, mMipIndex, mTargetLayer+1);

	mResult = vkEndCommandBuffer(commandBuffer);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkEndCommandBuffer failed with return code of " << GetResultString(mResult);
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

	mResult = vkQueueSubmit(mDevice->mQueue, 1, &submitInfo, nullFence);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkQueueSubmit failed with return code of " << GetResultString(mResult);
		return;
	}

	mResult = vkQueueWaitIdle(mDevice->mQueue);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkQueueWaitIdle failed with return code of " << GetResultString(mResult);
		return;
	}

	vkFreeCommandBuffers(mDevice->mDevice, mDevice->mCommandPool, 1, commandBuffers);
}