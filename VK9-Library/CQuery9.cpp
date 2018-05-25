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
 
#include "CQuery9.h"
#include "CDevice9.h"

#include "Utilities.h"

CQuery9::CQuery9(CDevice9* device, D3DQUERYTYPE Type)
	: mReferenceCount(1),
		mDevice(device),
		mType(Type),
	mResult(VK_SUCCESS)
{

}

CQuery9::~CQuery9()
{

}

ULONG STDMETHODCALLTYPE CQuery9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CQuery9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DQuery9))
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

ULONG STDMETHODCALLTYPE CQuery9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CQuery9::GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CQuery9::GetData is not implemented!";

	return S_OK;
}


DWORD STDMETHODCALLTYPE CQuery9::GetDataSize()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CQuery9::GetDataSize is not implemented!";

	return 0;
}

HRESULT STDMETHODCALLTYPE CQuery9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

D3DQUERYTYPE STDMETHODCALLTYPE CQuery9::GetType()
{
	return mType;
}


HRESULT STDMETHODCALLTYPE CQuery9::Issue(DWORD dwIssueFlags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CQuery9::Issue is not implemented!";

	return S_OK;	
}
