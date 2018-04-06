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
 
#include "CIndexBuffer9.h"
#include "CDevice9.h"

#include "Utilities.h"

CIndexBuffer9::CIndexBuffer9(CDevice9* device, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE* pSharedHandle)
	: mReferenceCount(1),
	mDevice(device),
	mLength(Length),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mCapacity(0),
	mIsDirty(true),
	mLockCount(0)
{

}

CIndexBuffer9::~CIndexBuffer9()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
	workItem->WorkItemType = WorkItemType::IndexBuffer_Destroy;
	workItem->Id = mId;
	mCommandStreamManager->RequestWorkAndWait(workItem);
}

ULONG STDMETHODCALLTYPE CIndexBuffer9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DIndexBuffer9))
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

ULONG STDMETHODCALLTYPE CIndexBuffer9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CIndexBuffer9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CIndexBuffer9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CIndexBuffer9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CIndexBuffer9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CIndexBuffer9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	return D3DRTYPE_INDEXBUFFER;
}

void STDMETHODCALLTYPE CIndexBuffer9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CIndexBuffer9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CIndexBuffer9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CIndexBuffer9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CIndexBuffer9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetDesc(D3DINDEXBUFFER_DESC* pDesc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CIndexBuffer9::GetDesc is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	if (mPool == D3DPOOL_MANAGED)
	{
		if (!(Flags & D3DLOCK_READONLY))
		{ //If the lock allows write mark the buffer as dirty.
			mIsDirty = true;
		}
	}

	InterlockedIncrement(&mLockCount);

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::IndexBuffer_Lock;
	workItem->Id = mId;
	workItem->Argument1 = (void*)OffsetToLock;
	workItem->Argument2 = (void*)SizeToLock;
	workItem->Argument3 = (void*)ppbData;
	workItem->Argument4 = (void*)Flags;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::Unlock()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::IndexBuffer_Unlock;
	workItem->Id = mId;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	InterlockedDecrement(&mLockCount);

	return S_OK;
}
