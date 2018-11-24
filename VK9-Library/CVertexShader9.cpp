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
 
#include "CVertexShader9.h"
#include "CDevice9.h"

#include "Utilities.h"

CVertexShader9::CVertexShader9(CDevice9* device, const DWORD* pFunction)
	: mDevice(device),
	mFunction((DWORD*)pFunction),
	mId(0)
{
	BOOST_LOG_TRIVIAL(info) << "CVertexShader9::CVertexShader9";
}

CVertexShader9::~CVertexShader9()
{
	BOOST_LOG_TRIVIAL(info) << "CVertexShader9::~CVertexShader9";

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
	workItem->WorkItemType = WorkItemType::Shader_Destroy;
	workItem->Id = mId;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	free(mFunction);
}

ULONG STDMETHODCALLTYPE CVertexShader9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CVertexShader9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVertexShader9))
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

ULONG STDMETHODCALLTYPE CVertexShader9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CVertexShader9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CVertexShader9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexShader9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CVertexShader9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexShader9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexShader9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexShader9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CVertexShader9::GetType()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexShader9::GetType is not implemented!";

	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CVertexShader9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexShader9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CVertexShader9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexShader9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexShader9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexShader9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

//HRESULT STDMETHODCALLTYPE CVertexShader9::GetDevice(IDirect3DDevice9** ppDevice)
//{
//	return E_NOTIMPL;
//}

HRESULT STDMETHODCALLTYPE CVertexShader9::GetFunction(void* pData, UINT* pSizeOfData)
{
	(*pSizeOfData) = mSize;

	if (pData==nullptr)
	{
		return S_OK;
	}

	memcpy(pData, mFunction, mSize);

	return S_OK;
}
