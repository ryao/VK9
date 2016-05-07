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
 
#include "CTexture9.h"
#include "CDevice9.h"

CTexture9::CTexture9()
{

}

CTexture9::~CTexture9()
{

}

ULONG STDMETHODCALLTYPE CTexture9::AddRef(void)
{
	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE CTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CTexture9::Release(void)
{
	return this->Release(0);
}

ULONG STDMETHODCALLTYPE CTexture9::AddRef(int which, char *comment)
{
	return 0;
}

ULONG STDMETHODCALLTYPE	CTexture9::Release(int which, char *comment)
{
	return 0;
}

HRESULT STDMETHODCALLTYPE CTexture9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CTexture9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

//D3DRESOURCETYPE STDMETHODCALLTYPE CTexture9::GetType()
//{
//	return D3DRTYPE_SURFACE;
//}

void STDMETHODCALLTYPE CTexture9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE CTexture9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CTexture9::GenerateMipSubLevels()
{
	return; //TODO: implement GenerateMipSubLevels
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CTexture9::GetAutoGenFilterType()
{
	return D3DTEXF_NONE; //TODO: implement GetAutoGenFilterType
}

DWORD STDMETHODCALLTYPE CTexture9::GetLOD()
{
	return 0; //TODO: implement GetLOD
}


DWORD STDMETHODCALLTYPE CTexture9::GetLevelCount()
{
	return 0;
}


HRESULT STDMETHODCALLTYPE CTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CTexture9::SetLOD(DWORD LODNew)
{
	return 0; //TODO: implement SetLOD
}

D3DRESOURCETYPE STDMETHODCALLTYPE CTexture9::GetType()
{
	return D3DRTYPE_SURFACE; //TODO: implement type.
}

HRESULT STDMETHODCALLTYPE CTexture9::AddDirtyRect(const RECT* pDirtyRect)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{


	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CTexture9::UnlockRect(UINT Level)
{

	return S_OK;	
}
