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
 
#include "CVertexBuffer9.h"
#include "CDevice9.h"

#include "Utilities.h"

CVertexBuffer9::CVertexBuffer9(CDevice9* device, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, HANDLE* pSharedHandle)
	: mReferenceCount(1),
	mDevice(device),
	mLength(Length),
	mUsage(Usage),
	mFVF(FVF),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mSize(0),
	mCapacity(0),
	mIsDirty(true),
	mLockCount(0),
	mId(0)
{
	this->mCommandStreamManager = this->mDevice->mCommandStreamManager;

	mSize = mLength / sizeof(float);
}

CVertexBuffer9::~CVertexBuffer9()
{	
	for (size_t id : mIds)
	{
		WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
		workItem->WorkItemType = WorkItemType::VertexBuffer_Destroy;
		workItem->Id = id;
		mCommandStreamManager->RequestWorkAndWait(workItem);
	}
}

void CVertexBuffer9::Init()
{	
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->Id = this->mDevice->mId;
	workItem->WorkItemType = WorkItemType::VertexBuffer_Create;
	workItem->Argument1 = (void*)this;
	this->mId = mCommandStreamManager->RequestWorkAndWait(workItem);
	this->mIds.push_back(this->mId); //Added so it won't be lost.
}

ULONG STDMETHODCALLTYPE CVertexBuffer9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVertexBuffer9))
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

ULONG STDMETHODCALLTYPE CVertexBuffer9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CVertexBuffer9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CVertexBuffer9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

void STDMETHODCALLTYPE CVertexBuffer9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CVertexBuffer9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexBuffer9::GetDesc is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	if (mPool == D3DPOOL_MANAGED)
	{
		if (!(Flags & D3DLOCK_READONLY))
		{ //If the lock allows write mark the buffer as dirty.
			mIsDirty = true;
		}
	}

	InterlockedIncrement(&mLockCount);

	/*
	Unless the caller indicates that they have not overwritten anything used by a previous draw call then we have to assume they have.
	Because the draw may not have happened we need to check to see if presentation has occured and if not flip to the next vertex
	*/
	if ( ((Flags & D3DLOCK_NOOVERWRITE) == D3DLOCK_NOOVERWRITE) || ((Flags & D3DLOCK_READONLY) == D3DLOCK_READONLY) )
	{
		//This is best case the caller says they didn't modify anything used in a draw call.
	}
	else
	{
		if (mFrameBit != mCommandStreamManager->mFrameBit)
		{
			mIndex = 0;
			mFrameBit = mCommandStreamManager->mFrameBit;
		}
		else
		{
			mLastIndex = mIndex;
			mIndex++;
		}
	}

	if (mIndex > mIds.size() - 1)
	{
		Init();
	}
	else
	{
		mId = mIds[mIndex];
	}

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::VertexBuffer_Lock;
	workItem->Id = mId;
	workItem->Argument1 = (void*)OffsetToLock;
	workItem->Argument2 = (void*)SizeToLock;
	workItem->Argument3 = (void*)ppbData;
	workItem->Argument4 = (void*)Flags;
	workItem->Argument5 = (void*)mIds[mLastIndex];
	mCommandStreamManager->RequestWorkAndWait(workItem);

	mLastIndex = mIndex;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::Unlock()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::VertexBuffer_Unlock;
	workItem->Id = mId;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	InterlockedDecrement(&mLockCount);

	return S_OK;	
}