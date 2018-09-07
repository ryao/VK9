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

#include "CStateBlock9.h"
#include "CDevice9.h"
#include "CTexture9.h"

#include "Utilities.h"

CStateBlock9::CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type)
	: mDevice(device),
	mType(Type),
	mId(0)
{
	//We want at least one light so the UBO won't be messed up for fixed function.
	Light light = {};
	mDeviceState.mLights.push_back(light);
}

CStateBlock9::CStateBlock9(CDevice9* device)
	: mDevice(device)
{
	mCommandStreamManager = device->mCommandStreamManager;
	this->mId = device->mId;
}

CStateBlock9::~CStateBlock9()
{

}

ULONG STDMETHODCALLTYPE CStateBlock9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CStateBlock9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DStateBlock9))
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

ULONG STDMETHODCALLTYPE CStateBlock9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Capture()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->Id = this->mId;
	workItem->WorkItemType = WorkItemType::StateBlock_Capture;
	workItem->Argument1 = this;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Apply()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->Id = this->mId;
	workItem->WorkItemType = WorkItemType::StateBlock_Apply;
	workItem->Argument1 = this;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}