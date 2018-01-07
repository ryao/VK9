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

#include "CTexture9.h"
#include "CDevice9.h"

#include "Utilities.h"

#include <math.h>
#include <algorithm>

CTexture9::CTexture9(CDevice9* device, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(device),
	mWidth(Width),
	mHeight(Height),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle)
{
	//mDevice->AddRef();

	mRealFormat = ConvertFormat(mFormat);

	if (!mLevels)
	{
		mLevels = std::log2( max(mWidth, mHeight) ) + 1;
	}

	//mLevels = 1; //workaround

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = { mWidth, mHeight, 1 };
	imageCreateInfo.mipLevels = mLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //VK_IMAGE_LAYOUT_PREINITIALIZED;

	mResult = vkCreateImage(mDevice->mDevice, &imageCreateInfo, NULL, &mImage);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 vkCreateImage failed with return code of " << GetResultString(mResult);
		return;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->mDevice, mImage, &memoryRequirements);

	mMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mMemoryAllocateInfo.pNext = NULL;
	//mMemoryAllocateInfo.allocationSize = 0;
	mMemoryAllocateInfo.memoryTypeIndex = 0;
	mMemoryAllocateInfo.allocationSize = memoryRequirements.size;

	if (!GetMemoryTypeFromProperties(mDevice->mDeviceMemoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 Could not find memory type from properties.";
		return;
	}

	mResult = vkAllocateMemory(mDevice->mDevice, &mMemoryAllocateInfo, NULL, &mDeviceMemory);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 vkAllocateMemory failed with return code of " << GetResultString(mResult);
		return;
	}

	mResult = vkBindImageMemory(mDevice->mDevice, mImage, mDeviceMemory, 0);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 vkBindImageMemory failed with return code of " << GetResultString(mResult);
		return;
	}

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = mImage;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	imageViewCreateInfo.subresourceRange.levelCount = mLevels;

	mResult = vkCreateImageView(mDevice->mDevice, &imageViewCreateInfo, NULL, &mImageView);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 vkCreateImageView failed with return code of " << GetResultString(mResult);
		return;
	}

	mSurfaces.reserve(mLevels);
	UINT width = mWidth, height = mHeight;
	for (size_t i = 0; i < mLevels; i++)
	{
		CSurface9* ptr = new CSurface9(mDevice, this, width, height, mLevels, mUsage, mFormat, mPool, mSharedHandle);

		ptr->mMipIndex = i;

		mSurfaces.push_back(ptr);

		width /= 2;
		height /= 2;
	}
}

CTexture9::~CTexture9()
{
	BOOST_LOG_TRIVIAL(info) << "CTexture9::~CTexture9";

	if (mImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mDevice->mDevice, mImageView, NULL);
	}

	if (mSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(mDevice->mDevice, mSampler, NULL);
		mSampler = VK_NULL_HANDLE;
	}

	if (mImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(mDevice->mDevice, mImage, NULL);
	}

	if (mDeviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice->mDevice, mDeviceMemory, NULL);
	}

	for (size_t i = 0; i < mSurfaces.size(); i++)
	{
		mSurfaces[i]->Release();
	}

	//mDevice->Release();
}

ULONG STDMETHODCALLTYPE CTexture9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CTexture9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DTexture9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DBaseTexture9))
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

ULONG STDMETHODCALLTYPE CTexture9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CTexture9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CTexture9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::PreLoad is not implemented!";

	return;
}

DWORD STDMETHODCALLTYPE CTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CTexture9::GenerateMipSubLevels()
{
	VkResult result = VK_SUCCESS;
	VkPipelineStageFlags sourceStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkCommandBuffer commandBuffer;
	VkFilter realFilter = ConvertFilter(mMipFilter);

	VkCommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = mDevice->mCommandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(mDevice->mDevice, &commandBufferInfo, &commandBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::GenerateMipSubLevels vkAllocateCommandBuffers failed with return code of " << GetResultString(mResult);
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
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::GenerateMipSubLevels vkBeginCommandBuffer failed with return code of " << GetResultString(mResult);
		return;
	}

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = 0;
	imageMemoryBarrier.dstAccessMask = 0;

	VkImageSubresourceRange mipSubRange = {};
	mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	mipSubRange.levelCount = 1;
	mipSubRange.layerCount = 1;

	/*
	I'm debating whether or not to have the population of the image here. If I don't I'll end up creating another command for that. On the other hand this method should purely populate the other levels as per the spec.
	*/

	// Transition zero mip level to transfer source
	//mipSubRange.baseMipLevel = 0;

	//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	//imageMemoryBarrier.image = mImage;
	//imageMemoryBarrier.subresourceRange = mipSubRange;
	//vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	for (UINT i = 1; i < mLevels; i++) //Changed to match mLevels datatype
	{
		VkImageBlit imageBlit{};

		// Source
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.layerCount = 1;
		imageBlit.srcSubresource.mipLevel = 0;
		imageBlit.srcOffsets[1].x = int32_t(mWidth);
		imageBlit.srcOffsets[1].y = int32_t(mHeight);
		imageBlit.srcOffsets[1].z = 1;

		// Destination
		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.layerCount = 1;
		imageBlit.dstSubresource.mipLevel = i;
		imageBlit.dstOffsets[1].x = int32_t(mWidth >> i);
		imageBlit.dstOffsets[1].y = int32_t(mHeight >> i);
		imageBlit.dstOffsets[1].z = 1;

		mipSubRange.baseMipLevel = i;

		// Transition current mip level to transfer dest
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.image = mImage;
		imageMemoryBarrier.subresourceRange = mipSubRange;
		vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		// Blit from zero level
		vkCmdBlitImage(commandBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);
	}

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::GenerateMipSubLevels vkEndCommandBuffer failed with return code of " << GetResultString(result);
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

	result = vkQueueSubmit(mDevice->mQueue, 1, &submitInfo, nullFence);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::GenerateMipSubLevels vkQueueSubmit failed with return code of " << GetResultString(result);
		return;
	}

	result = vkQueueWaitIdle(mDevice->mQueue);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::GenerateMipSubLevels vkQueueWaitIdle failed with return code of " << GetResultString(result);
		return;
	}

	vkFreeCommandBuffers(mDevice->mDevice, mDevice->mCommandPool, 1, commandBuffers);
	commandBuffer = VK_NULL_HANDLE;

	return;
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CTexture9::GetAutoGenFilterType()
{
	return mMipFilter;
}

DWORD STDMETHODCALLTYPE CTexture9::GetLOD()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetLOD is not implemented!";

	return 0;
}


DWORD STDMETHODCALLTYPE CTexture9::GetLevelCount()
{
	return mLevels;
}


HRESULT STDMETHODCALLTYPE CTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	mMipFilter = FilterType; //revisit

	return S_OK;
}

DWORD STDMETHODCALLTYPE CTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetLOD is not implemented!";

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CTexture9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE CTexture9::AddDirtyRect(const RECT* pDirtyRect)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::AddDirtyRect is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	return mSurfaces[Level]->GetDesc(pDesc);
}

HRESULT STDMETHODCALLTYPE CTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
	IDirect3DSurface9* surface = (IDirect3DSurface9*)this->mSurfaces[Level];

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb205912(v=vs.85).aspx
	"Calling this method will increase the internal reference count on the IDirect3DSurface9 interface."
	*/
	surface->AddRef();

	(*ppSurfaceLevel) = surface;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	//if (Level == 0)
	//{
	//	BOOST_LOG_TRIVIAL(info) << "CTexture9::LockRect Level:" << Level << " Handle: " << this;
	//}
	
	return mSurfaces[Level]->LockRect(pLockedRect, pRect, Flags);
}

HRESULT STDMETHODCALLTYPE CTexture9::UnlockRect(UINT Level)
{
	//if (Level == 0)
	//{
	//	BOOST_LOG_TRIVIAL(info) << "CTexture9::UnlockRect Level:" << Level << " Handle: " << this;
	//}
	
	return mSurfaces[Level]->UnlockRect();
}

void CTexture9::Flush()
{
	for (size_t i = 0; i < mSurfaces.size(); i++)
	{
		mSurfaces[i]->Flush();
	}
}