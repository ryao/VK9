#include "CSwapChain9.h"
#include "CSurface9.h"
#include "CDevice9.h"
#include "Utilities.h"

CSwapChain9::CSwapChain9(D3DPRESENT_PARAMETERS *pPresentationParameters)
	: mPresentationParameters(pPresentationParameters)
{

}

CSwapChain9::~CSwapChain9()
{

}

ULONG STDMETHODCALLTYPE CSwapChain9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
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
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
		return 0;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9  **ppBackBuffer)
{
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetBackBuffer is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetDisplayMode(D3DDISPLAYMODE *pMode)
{
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetDisplayMode is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetFrontBufferData(IDirect3DSurface9 *pDestSurface)
{
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetFrontBufferData is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetPresentParameters(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetPresentParameters is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetRasterStatus(D3DRASTER_STATUS *pRasterStatus)
{
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::GetRasterStatus is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride,const RGNDATA *pDirtyRegion, DWORD dwFlags)
{
	BOOST_LOG_TRIVIAL(warning) << "CSurface9::Present is not implemented!";

	return E_NOTIMPL; //TODO: Implement.
}