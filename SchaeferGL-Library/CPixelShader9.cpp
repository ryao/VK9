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
 
#include "CPixelShader9.h"
#include "CDevice9.h"

#include "Utilities.h"

CPixelShader9::CPixelShader9(CDevice9* device,const DWORD* pFunction)
	: mReferenceCount(0),
	mDevice(device),
	mFunction((DWORD*)pFunction),
	mResult(VK_SUCCESS)
{

}

CPixelShader9::~CPixelShader9()
{

}

ULONG STDMETHODCALLTYPE CPixelShader9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::QueryInterface(REFIID riid,void  **ppv)
{
	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::QueryInterface is not implemented!";

	(*ppv) = this;

	return S_OK;
}

ULONG STDMETHODCALLTYPE CPixelShader9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CPixelShader9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CPixelShader9::GetType()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::GetType is not implemented!";

	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CPixelShader9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CPixelShader9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::GetFunction(void* pData, UINT* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CPixelShader9::GetFunction is not implemented!";

	return E_NOTIMPL;
}
