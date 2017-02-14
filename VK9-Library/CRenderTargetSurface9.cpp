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

#include "CRenderTargetSurface9.h"
#include "CDevice9.h"
#include "Utilities.h"

CRenderTargetSurface9::CRenderTargetSurface9(CDevice9* Device, UINT Width, UINT Height, D3DFORMAT Format)
	: mDevice(Device),
	mWidth(Width),
	mHeight(Height),
	mFormat(Format)
{

}

CRenderTargetSurface9::~CRenderTargetSurface9()
{

}

//IUnknown
HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DSurface9))
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

ULONG STDMETHODCALLTYPE CRenderTargetSurface9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

ULONG STDMETHODCALLTYPE CRenderTargetSurface9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

//IDirect3DResource9
HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::FreePrivateData is not implemented!";

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CRenderTargetSurface9::GetPriority()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::GetPriority is not implemented!";

	return 0;
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::GetPrivateData is not implemented!";

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CRenderTargetSurface9::GetType()
{
	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CRenderTargetSurface9::PreLoad()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::PreLoad is not implemented!";

	return;
}

DWORD STDMETHODCALLTYPE CRenderTargetSurface9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::SetPriority is not implemented!";

	return 0;
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::SetPrivateData is not implemented!";

	return E_NOTIMPL;
}

//IDirect3DSurface9
HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::GetContainer(REFIID riid, void** ppContainer)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::GetContainer is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::GetDC(HDC* phdc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::GetDC is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::GetDesc(D3DSURFACE_DESC* pDesc)
{
	pDesc->Format = this->mFormat;
	pDesc->Type = D3DRTYPE_SURFACE;
	pDesc->Usage = D3DUSAGE_RENDERTARGET;
	pDesc->Pool = D3DPOOL_DEFAULT;

	pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
	pDesc->MultiSampleQuality = 0;
	pDesc->Width = this->mWidth;
	pDesc->Height = this->mHeight;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::LockRect is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::ReleaseDC(HDC hdc)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::ReleaseDC is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CRenderTargetSurface9::UnlockRect()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CRenderTargetSurface9::UnlockRect is not implemented!";

	return E_NOTIMPL;
}