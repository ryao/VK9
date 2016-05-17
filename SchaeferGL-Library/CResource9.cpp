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
 
#include "CResource9.h"
#include "CDevice9.h"

CResource9::CResource9()
	: mReferenceCount(0)
{

}

CResource9::~CResource9()
{
}

ULONG STDMETHODCALLTYPE CResource9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CResource9::QueryInterface(REFIID riid,void  **ppv)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CResource9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CResource9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CResource9::GetPriority()
{
	//TODO: Implement.

	return 1;
}

HRESULT STDMETHODCALLTYPE CResource9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CResource9::GetType()
{
	//TODO: Implement.

	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CResource9::PreLoad()
{
	//TODO: Implement.

	return; 
}

DWORD STDMETHODCALLTYPE CResource9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	return 1;
}

HRESULT STDMETHODCALLTYPE CResource9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

