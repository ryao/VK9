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
 
#include "CCubeTexture9.h"
#include "CDevice9.h"
#include "CSurface9.h"

#include "Utilities.h"

CCubeTexture9::CCubeTexture9(CDevice9* device, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(device),
	mEdgeLength(EdgeLength),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1),
	mResult(VK_SUCCESS)
{
	BOOST_LOG_TRIVIAL(info) << "CCubeTexture9::CCubeTexture9";
	//mDevice->AddRef();

	if (Usage & D3DUSAGE_AUTOGENMIPMAP)
	{
		mLevels = 1;
	}
	else if (!mLevels)
	{
		mLevels = min(std::log2(mEdgeLength) + 1 , 10);
	}

	//mLevels = 1; //workaround

	mSurfaces.reserve(6);
	for (size_t i = 0; i < 6; i++)
	{
		UINT width = mEdgeLength;
		UINT height = mEdgeLength;
		boost::container::small_vector<CSurface9*, 5> surfaces;
		for (size_t j = 0; j < mLevels; j++)
		{
			CSurface9* ptr = new CSurface9(mDevice, this, width, height, mLevels, mUsage, mFormat, mPool, mSharedHandle);

			ptr->mMipIndex = j;
			ptr->mTargetLayer = i;

			surfaces.push_back(ptr);

			width /= 2;
			height /= 2;
		}
		mSurfaces.push_back(surfaces);
	}
}

CCubeTexture9::~CCubeTexture9()
{
	BOOST_LOG_TRIVIAL(info) << "CCubeTexture9::~CCubeTexture9";

	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < mSurfaces[i].size(); j++)
		{
			mSurfaces[i][j]->Release();
		}		
	}

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
	workItem->WorkItemType = WorkItemType::CubeTexture_Destroy;
	workItem->Id = mId;
	mCommandStreamManager->RequestWork(workItem);
}

ULONG STDMETHODCALLTYPE CCubeTexture9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DCubeTexture9))
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

ULONG STDMETHODCALLTYPE CCubeTexture9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CCubeTexture9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CCubeTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CCubeTexture9::GenerateMipSubLevels()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::CubeTexture_GenerateMipSubLevels;
	workItem->Id = mId;
	workItem->Argument1 = this;
	mCommandStreamManager->RequestWork(workItem);
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CCubeTexture9::GetAutoGenFilterType()
{
	return mMipFilter;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetLOD()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::GetLOD is not implemented!";

	return 0;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetLevelCount()
{
	return mLevels;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	mMipFilter = FilterType; //revisit

	return S_OK;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::SetLOD is not implemented!";

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CCubeTexture9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::AddDirtyRect is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CCubeTexture9::GetCubeMapSurface is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	return mSurfaces[0][Level]->GetDesc(pDesc);
}

HRESULT CCubeTexture9::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	//if (Level == 0)
	//{
	//	BOOST_LOG_TRIVIAL(info) << "CTexture9::LockRect Level:" << Level << " Handle: " << this;
	//}
	HRESULT result = mSurfaces[FaceType][Level]->LockRect(pLockedRect, pRect, Flags);	
	return result;
}

HRESULT CCubeTexture9::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level)
{
	//if (Level == 0)
	//{
	//	BOOST_LOG_TRIVIAL(info) << "CTexture9::UnlockRect Level:" << Level << " Handle: " << this;
	//}
	HRESULT result = mSurfaces[FaceType][Level]->UnlockRect();
	return result;
}

void CCubeTexture9::Flush()
{
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < mSurfaces[i].size(); j++)
		{
			mSurfaces[i][j]->Flush();
		}
	}
}