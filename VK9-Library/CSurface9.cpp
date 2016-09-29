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
 
#include "CSurface9.h"
#include "CDevice9.h"
#include "CTexture9.h"
#include "Utilities.h"

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture,UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard,HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(0),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mLockable(0),
	mPool(D3DPOOL_DEFAULT),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1),
	mResult(VK_SUCCESS),

	mImageLayout(VK_IMAGE_LAYOUT_GENERAL),

	mRealFormat(VK_FORMAT_R8G8B8A8_UNORM)
{
	Init();
}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, HANDLE *pSharedHandle, int32_t filler)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(0),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(0),
	mLockable(Lockable),
	mPool(D3DPOOL_DEFAULT),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1),
	mResult(VK_SUCCESS),

	mImageLayout(VK_IMAGE_LAYOUT_GENERAL),

	mRealFormat(VK_FORMAT_R8G8B8A8_UNORM)
{
	Init();
}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mFormat(Format),
	mMultiSample(D3DMULTISAMPLE_NONE),
	mMultisampleQuality(0),
	mDiscard(0),
	mLockable(0),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1),
	mResult(VK_SUCCESS),

	mImageLayout(VK_IMAGE_LAYOUT_GENERAL),

	mRealFormat(VK_FORMAT_R8G8B8A8_UNORM)
{	
	Init();
}

void CSurface9::Init()
{
	mDevice->AddRef();

	mRealFormat = ConvertFormat(mFormat);
}

CSurface9::~CSurface9()
{
	mDevice->Release();
}

ULONG STDMETHODCALLTYPE CSurface9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CSurface9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DSurface9))
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

ULONG STDMETHODCALLTYPE CSurface9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
		return 0;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CSurface9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CSurface9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CSurface9::GetType()
{
	return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

void STDMETHODCALLTYPE CSurface9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::PreLoad is not implemented!";

	return; 
}

DWORD STDMETHODCALLTYPE CSurface9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::SetPriority is not implemented!";

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetContainer(REFIID riid, void** ppContainer)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetContainer is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDC(HDC* phdc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetDC is not implemented!";

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CSurface9::GetDesc(D3DSURFACE_DESC* pDesc)
{
	pDesc->Format = this->mFormat;
	pDesc->Type = D3DRTYPE_SURFACE;
	pDesc->Usage = this->mUsage;
	pDesc->Pool = this->mPool;

	pDesc->MultiSampleType = this->mMultiSample;
	pDesc->MultiSampleQuality = this->mMultisampleQuality;
	pDesc->Width = this->mWidth;
	pDesc->Height = this->mHeight;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CSurface9::LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	/*
	I'll need to Make sure direct access to image memory is allowed and that it can remain mapped.
	*/
	pLockedRect->pBits = mTexture->mData;
	pLockedRect->Pitch = mLayout.rowPitch;

	return S_OK;
}


HRESULT STDMETHODCALLTYPE CSurface9::ReleaseDC(HDC hdc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CSurface9::ReleaseDC is not implemented!";

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CSurface9::UnlockRect()
{
	/*
	I'll need to Make sure direct access to image memory is allowed and that it can remain mapped.
	*/
	return S_OK;
}