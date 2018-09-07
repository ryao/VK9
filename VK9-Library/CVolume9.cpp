// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include "CDevice9.h"
#include "CVolume9.h"
#include "CVolumeTexture9.h"

#include "Utilities.h"
#include "CTypes.h"

CVolume9::CVolume9(CDevice9* device, CVolumeTexture9* texture, UINT Width, UINT Height, UINT Depth, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mReferenceCount(1),
	mDevice(device),
	mTexture(texture),
	mWidth(Width),
	mHeight(Height),
	mDepth(Depth),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle)
{
	BOOST_LOG_TRIVIAL(info) << "CVolume9::CVolume9";
}

CVolume9::~CVolume9()
{
	BOOST_LOG_TRIVIAL(info) << "CVolume9::~CVolume9";

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
	workItem->WorkItemType = WorkItemType::Volume_Destroy;
	workItem->Id = mId;
	mCommandStreamManager->RequestWorkAndWait(workItem);
}

void CVolume9::Init()
{
	BOOST_LOG_TRIVIAL(info) << "CVolume9::Init";

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

	mTextureId = mTexture->mId;


	mCommandStreamManager = mDevice->mCommandStreamManager;
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->Id = mDevice->mId;
	workItem->WorkItemType = WorkItemType::Volume_Create;
	workItem->Argument1 = this;
	mId = mCommandStreamManager->RequestWorkAndWait(workItem);
}

void CVolume9::Flush()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::Volume_Flush;
	workItem->Id = mId;
	workItem->Argument1 = (void*)this;
	mCommandStreamManager->RequestWorkAndWait(workItem);
}

//IUnknown
HRESULT STDMETHODCALLTYPE CVolume9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVolume9))
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

ULONG STDMETHODCALLTYPE CVolume9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

ULONG STDMETHODCALLTYPE CVolume9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

/*** IDirect3DVolume9 methods ***/
HRESULT STDMETHODCALLTYPE CVolume9::GetDevice(IDirect3DDevice9** ppDevice)
{
	mDevice->AddRef();
	(*ppDevice) = (IDirect3DDevice9*)mDevice;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CVolume9::SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolume9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVolume9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolume9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVolume9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVolume9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVolume9::GetContainer(REFIID riid, void** ppContainer)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetContainer is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVolume9::GetDesc(D3DVOLUME_DESC *pDesc)
{
	pDesc->Format = this->mFormat;
	pDesc->Type = D3DRTYPE_VOLUME;
	pDesc->Usage = this->mUsage;
	pDesc->Pool = this->mPool;

	pDesc->Width = this->mWidth;
	pDesc->Height = this->mHeight;
	pDesc->Depth = this->mDepth;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CVolume9::LockBox(D3DLOCKED_BOX* pLockedVolume, CONST D3DBOX* pBox, DWORD Flags)
{
	mFlags = Flags;

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::Volume_LockRect;
	workItem->Id = mId;
	workItem->Argument1 = (void*)pLockedVolume;
	workItem->Argument2 = (void*)pBox;
	workItem->Argument3 = (void*)Flags;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CVolume9::UnlockBox()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::Volume_UnlockRect;
	workItem->Id = mId;
	workItem->Argument1 = (void*)this;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	this->Flush();

	return S_OK;
}