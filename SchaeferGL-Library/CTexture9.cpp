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
 
#include "CTexture9.h"
#include "CDevice9.h"

#include "Utilities.h"

CTexture9::CTexture9(CDevice9* device, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mReferenceCount(0),
	mDevice(device),
	mWidth(Width),
	mHeight(Height),
	mDepth(0),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mResult(VK_SUCCESS),

	mRealFormat(VK_FORMAT_R8G8B8A8_UNORM),
	mSampler(VK_NULL_HANDLE),
	mImage(VK_NULL_HANDLE),
	mImageLayout(VK_IMAGE_LAYOUT_GENERAL),
	mDeviceMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE)
{
	mRealFormat = ConvertFormat(mFormat);

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = mRealFormat;
	imageCreateInfo.extent = { mWidth, mHeight, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;	

	mResult = vkCreateImage(mDevice->mDevice, &imageCreateInfo, NULL, &mImage);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 vkCreateImage failed with return code of " << mResult;
		return;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->mDevice, mImage, &memoryRequirements);

	mMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mMemoryAllocateInfo.pNext = NULL;
	mMemoryAllocateInfo.allocationSize = memoryRequirements.size;
	mMemoryAllocateInfo.memoryTypeIndex = 0;

	if (!GetMemoryTypeFromProperties(mDevice->mDeviceMemoryProperties, memoryRequirements.memoryTypeBits, 0, &mMemoryAllocateInfo.memoryTypeIndex))
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 Could not find memory type from properties.";
		return;
	}

	mResult = vkAllocateMemory(mDevice->mDevice, &mMemoryAllocateInfo, NULL,&mDeviceMemory);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 vkAllocateMemory failed with return code of " << mResult;
		return;
	}

	mResult = vkBindImageMemory(mDevice->mDevice, mImage, mDeviceMemory, 0);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CTexture9 vkBindImageMemory failed with return code of " << mResult;
		return;
	}

}

CTexture9::~CTexture9()
{

}

ULONG STDMETHODCALLTYPE CTexture9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::QueryInterface is not implemented!";

	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CTexture9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
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
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GenerateMipSubLevels is not implemented!";

	return;
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CTexture9::GetAutoGenFilterType()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetAutoGenFilterType is not implemented!";

	return D3DTEXF_NONE;
}

DWORD STDMETHODCALLTYPE CTexture9::GetLOD()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetLOD is not implemented!";

	return 0;
}


DWORD STDMETHODCALLTYPE CTexture9::GetLevelCount()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetLevelCount is not implemented!";

	return 0;
}


HRESULT STDMETHODCALLTYPE CTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetAutoGenFilterType is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetLOD is not implemented!";

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CTexture9::GetType()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetType is not implemented!";

	return D3DRTYPE_SURFACE;
}

HRESULT STDMETHODCALLTYPE CTexture9::AddDirtyRect(const RECT* pDirtyRect)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::AddDirtyRect is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetLevelDesc is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetSurfaceLevel is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::LockRect is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CTexture9::UnlockRect(UINT Level)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::UnlockRect is not implemented!";

	return S_OK;	
}
