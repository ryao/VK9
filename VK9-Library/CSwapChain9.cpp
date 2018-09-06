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

#include "CSwapChain9.h"
#include "CSurface9.h"
#include "CDevice9.h"
#include "Utilities.h"

CSwapChain9::CSwapChain9(CDevice9* Device, D3DPRESENT_PARAMETERS *pPresentationParameters)
	: mDevice(Device),
	mPresentationParameters(pPresentationParameters)
{
	BOOST_LOG_TRIVIAL(info) << "CSwapChain9::CSwapChain9";

	//pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8; //TODO: review this to see if there is a better way to work around 16bit textures.

	mBackBuffer = new CSurface9(Device, pPresentationParameters);
	mFrontBuffer = new CSurface9(Device, pPresentationParameters);
	mCommandStreamManager = mDevice->mCommandStreamManager;
	mId = mDevice->mId;
}

CSwapChain9::~CSwapChain9()
{
	BOOST_LOG_TRIVIAL(info) << "CSwapChain9::~CSwapChain9";

	delete mBackBuffer;
	delete mFrontBuffer;
}

void CSwapChain9::Init()
{
	mBackBuffer->Init();
	mFrontBuffer->Init();
}

ULONG STDMETHODCALLTYPE CSwapChain9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CSwapChain9::QueryInterface(REFIID riid, void  **ppv)
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

ULONG STDMETHODCALLTYPE CSwapChain9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9  **ppBackBuffer)
{
	switch (Type)
	{
	case D3DBACKBUFFER_TYPE_MONO:
		(*ppBackBuffer) = mBackBuffer;
		mBackBuffer->AddRef();
		return S_OK;
		break;
	case D3DBACKBUFFER_TYPE_LEFT:
		BOOST_LOG_TRIVIAL(warning) << "CSwapChain9::GetBackBuffer left type is not implemented!";
		return E_NOTIMPL;
	case D3DBACKBUFFER_TYPE_RIGHT:
		BOOST_LOG_TRIVIAL(warning) << "CSwapChain9::GetBackBuffer right type is not implemented!";
		return E_NOTIMPL;
	default:
		BOOST_LOG_TRIVIAL(warning) << "CSwapChain9::GetBackBuffer unknown type is not implemented!";
		return E_NOTIMPL;
	}
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetDevice(IDirect3DDevice9** ppDevice)
{
	mDevice->AddRef();
	(*ppDevice) = (IDirect3DDevice9*)mDevice;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetDisplayMode(D3DDISPLAYMODE *pMode)
{
	pMode->Format = mPresentationParameters->BackBufferFormat;
	pMode->Height = mPresentationParameters->BackBufferHeight;
	pMode->Width = mPresentationParameters->BackBufferWidth;
	pMode->RefreshRate = mPresentationParameters->FullScreen_RefreshRateInHz; //Revsit

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetFrontBufferData(IDirect3DSurface9 *pDestSurface)
{
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetFrontBufferData is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetPresentParameters(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	memcpy(pPresentationParameters, mPresentationParameters, sizeof(D3DPRESENT_PARAMETERS)); //Revsit

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetRasterStatus(D3DRASTER_STATUS *pRasterStatus)
{
	(*pRasterStatus) = {};
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetRasterStatus is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion, DWORD dwFlags)
{
	return mDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}