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

#include "Utilities.h"

CTexture9::CTexture9(CDevice9* device, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mReferenceCount(1),
	mDevice(device),
	mWidth(Width),
	mHeight(Height),
	mDepth(0),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mResult(VK_SUCCESS)
{
	mDevice->AddRef();

	if (mLevels>0) //one or more means make that many.
	{
		mSurfaces.reserve(mLevels);
		UINT width=mWidth, height=mHeight;
		for (size_t i = mLevels; i > 0; i--)
		{
			CSurface9* ptr = new CSurface9(mDevice, (IDirect3DTexture9*)this, mWidth, mHeight, mLevels, mUsage, mFormat, mPool, mSharedHandle);

			mSurfaces.push_back(ptr);

			width /= 2;
			height /= 2;
		}
	}
	else //zero means make'em all.
	{
		UINT width = mWidth, height = mHeight;
		while (width>2 && height > 2)
		{
			CSurface9* ptr = new CSurface9(mDevice, (IDirect3DTexture9*)this, mWidth, mHeight, mLevels, mUsage, mFormat, mPool, mSharedHandle);

			mSurfaces.push_back(ptr);

			width /= 2;
			height /= 2;
		}
		mLevels = mSurfaces.size();
	}
}

CTexture9::~CTexture9()
{
	for (size_t i = 0; i < mSurfaces.size(); i++)
	{
		delete mSurfaces[i];
	}

	mDevice->Release();
}

ULONG STDMETHODCALLTYPE CTexture9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DTexture9))
	{
		(*ppv) = this;
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
	{
		(*ppv) = this;
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CTexture9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
		return 0;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CTexture9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CTexture9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CTexture9::GenerateMipSubLevels()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GenerateMipSubLevels is not implemented!";

	return;
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CTexture9::GetAutoGenFilterType()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetAutoGenFilterType is not implemented!";

	return D3DTEXF_NONE;
}

DWORD STDMETHODCALLTYPE CTexture9::GetLOD()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::GetLOD is not implemented!";

	return 0;
}


DWORD STDMETHODCALLTYPE CTexture9::GetLevelCount()
{
	return mLevels;
}


HRESULT STDMETHODCALLTYPE CTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetAutoGenFilterType is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::SetLOD is not implemented!";

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CTexture9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE CTexture9::AddDirtyRect(const RECT* pDirtyRect)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CTexture9::AddDirtyRect is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	return mSurfaces[Level]->GetDesc(pDesc);
}

HRESULT STDMETHODCALLTYPE CTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
	(*ppSurfaceLevel) = ((IDirect3DSurface9*)this->mSurfaces[Level]);

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	return mSurfaces[Level]->LockRect(pLockedRect, pRect, Flags);
}

HRESULT STDMETHODCALLTYPE CTexture9::UnlockRect(UINT Level)
{
	return mSurfaces[Level]->UnlockRect();
}
