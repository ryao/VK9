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
 
#include "CCubeTexture9.h"
#include "CDevice9.h"
#include "CSurface9.h"

CCubeTexture9::CCubeTexture9(CDevice9* Device, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mEdgeLength(EdgeLength),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mReferenceCount(0)
{
	//TODO: Implement.
}

CCubeTexture9::~CCubeTexture9()
{
	
}

ULONG STDMETHODCALLTYPE CCubeTexture9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CCubeTexture9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetPriority()
{
	//TODO: Implement.

	return 1;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CCubeTexture9::PreLoad()
{
	//TODO: Implement.

	return; 
}

DWORD STDMETHODCALLTYPE CCubeTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	return 1;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CCubeTexture9::GenerateMipSubLevels()
{
	//TODO: Implement.

	return;
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CCubeTexture9::GetAutoGenFilterType()
{
	//TODO: Implement.

	return D3DTEXF_NONE;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetLOD()
{
	//TODO: Implement.

	return 0;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetLevelCount()
{
	//TODO: Implement.

	return 0;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CCubeTexture9::GetType()
{
	//TODO: Implement.

	return D3DRTYPE_SURFACE;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT CCubeTexture9::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT CCubeTexture9::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level)
{
	//TODO: Implement.

	return E_NOTIMPL;
}
