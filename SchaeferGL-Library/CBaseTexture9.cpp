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
 
#include "CBaseTexture9.h"
#include "CDevice9.h"

CBaseTexture9::CBaseTexture9()
{

}

CBaseTexture9::~CBaseTexture9()
{
	
}

ULONG STDMETHODCALLTYPE CBaseTexture9::AddRef(void)
{
	//TODO: Implement.

	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CBaseTexture9::Release(void)
{
	//TODO: Implement.

	return this->Release(0);
}

ULONG STDMETHODCALLTYPE CBaseTexture9::AddRef(int which, char *comment)
{
	//TODO: Implement.

	return 0;
}

ULONG STDMETHODCALLTYPE	CBaseTexture9::Release(int which, char *comment)
{
	//TODO: Implement.

	return 0;
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CBaseTexture9::GetPriority()
{
	//TODO: Implement.

	return 1;
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CBaseTexture9::PreLoad()
{
	//TODO: Implement.

	return; 
}

DWORD STDMETHODCALLTYPE CBaseTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	return 1;
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CBaseTexture9::GenerateMipSubLevels()
{
	//TODO: Implement.

	return;
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CBaseTexture9::GetAutoGenFilterType()
{
	//TODO: Implement.

	return D3DTEXF_NONE;
}

DWORD STDMETHODCALLTYPE CBaseTexture9::GetLOD()
{
	//TODO: Implement.

	return 0;
}


DWORD STDMETHODCALLTYPE CBaseTexture9::GetLevelCount()
{
	//TODO: Implement.

	return 0;
}


HRESULT STDMETHODCALLTYPE CBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CBaseTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CBaseTexture9::GetType()
{
	//TODO: Implement.

	return D3DRTYPE_SURFACE;
}