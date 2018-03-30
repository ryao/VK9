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
 

#include "CVolumeTexture9.h"
#include "CDevice9.h"
#include "CSurface9.h"

#include "Utilities.h"

CVolumeTexture9::CVolumeTexture9(CDevice9* device, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mReferenceCount(1),
	mDevice(device),
	mWidth(Width),
	mHeight(Height),
	mDepth(Depth),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mResult(VK_SUCCESS)
{
	BOOST_LOG_TRIVIAL(info) << "CVolumeTexture9::CVolumeTexture9";

	if (Usage & D3DUSAGE_AUTOGENMIPMAP)
	{
		mLevels = 1;
	}
	else if (!mLevels)
	{
		mLevels = min(std::log2(max(mWidth, mHeight)) + 1, 10);
	}

	//for (size_t i = 0; i < Depth; i++)
	//{
	//	UINT width = mWidth;
	//	UINT height = mHeight;
	//	std::vector<CSurface9*> surfaces;
	//	for (size_t j = 0; j < mLevels; j++)
	//	{
	//		CSurface9* ptr = new CSurface9(mDevice, this, width, height, mLevels, mUsage, mFormat, mPool, mSharedHandle);

	//		ptr->mMipIndex = j;
	//		ptr->mTargetLayer = i;

	//		surfaces.push_back(ptr);

	//		width /= 2;
	//		height /= 2;
	//	}
	//	mSurfaces.push_back(surfaces);
	//}
}

CVolumeTexture9::~CVolumeTexture9()
{
	BOOST_LOG_TRIVIAL(info) << "CVolumeTexture9::~CVolumeTexture9";

	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < mSurfaces[i].size(); j++)
		{
			mSurfaces[i][j]->Release();
		}
	}

	//WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
	//workItem->WorkItemType = WorkItemType::VolumeTexture_Destroy;
	//workItem->Id = mId;
	//mCommandStreamManager->RequestWork(workItem);
}

ULONG STDMETHODCALLTYPE CVolumeTexture9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVolumeTexture9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DTexture9))
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

ULONG STDMETHODCALLTYPE CVolumeTexture9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CVolumeTexture9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CVolumeTexture9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CVolumeTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CVolumeTexture9::GenerateMipSubLevels()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::GenerateMipSubLevels is not implemented!";

	return;
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CVolumeTexture9::GetAutoGenFilterType()
{
	return mMipFilter;
}

DWORD STDMETHODCALLTYPE CVolumeTexture9::GetLOD()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::GetLOD is not implemented!";

	return 0;
}

DWORD STDMETHODCALLTYPE CVolumeTexture9::GetLevelCount()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::GetLevelCount is not implemented!";

	return 0;
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	mMipFilter = FilterType; //revisit

	return S_OK;
}

DWORD STDMETHODCALLTYPE CVolumeTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::SetLOD is not implemented!";

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CVolumeTexture9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::AddDirtyBox(const D3DBOX* pDirtyBox)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::AddDirtyBox is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::GetLevelDesc is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::GetVolumeLevel is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags)
{
	//TODO: Implement.
	
	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::LockBox is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CVolumeTexture9::UnlockBox(UINT Level)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolumeTexture9::UnlockBox is not implemented!";

	return S_OK;	
}

void CVolumeTexture9::Flush()
{
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < mSurfaces[i].size(); j++)
		{
			mSurfaces[i][j]->Flush();
		}
	}
}