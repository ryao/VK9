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
 
#include "CIndexBuffer9.h"
#include "CDevice9.h"

CIndexBuffer9::CIndexBuffer9()
{

}

CIndexBuffer9::~CIndexBuffer9()
{

}

ULONG STDMETHODCALLTYPE CIndexBuffer9::AddRef(void)
{
	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::QueryInterface(REFIID riid,void  **ppv)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CIndexBuffer9::Release(void)
{
	return this->Release(0);
}

ULONG STDMETHODCALLTYPE CIndexBuffer9::AddRef(int which, char *comment)
{
	return 0;
}

ULONG STDMETHODCALLTYPE	CIndexBuffer9::Release(int which, char *comment)
{
	return 0;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CIndexBuffer9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CIndexBuffer9::GetType()
{
	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CIndexBuffer9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE CIndexBuffer9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}


void STDMETHODCALLTYPE CIndexBuffer9::UnlockActualSize( unsigned int nActualSize, const void *pActualData)
{

}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetDesc(D3DINDEXBUFFER_DESC* pDesc)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{


	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::Unlock()
{


	return S_OK;	
}
