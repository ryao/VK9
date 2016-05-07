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

CSurface9::CSurface9()
{

}

CSurface9::~CSurface9()
{
	
}

ULONG STDMETHODCALLTYPE CSurface9::AddRef(void)
{
	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE CSurface9::QueryInterface(REFIID riid,void  **ppv)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CSurface9::Release(void)
{
	return this->Release(0);
}

ULONG STDMETHODCALLTYPE CSurface9::AddRef(int which, char *comment)
{
	return 0;
}

//ULONG STDMETHODCALLTYPE	CSurface9::Release(int which, char *comment)
//{
//	Assert( which >= 0 );
//	Assert( which < 2 );
//		
//	//int oldrefcs[2] = { m_refcount[0], m_refcount[1] };
//	bool deleting = false;
//		
//	m_refcount[which]--;
//	if ( (!m_refcount[0]) && (!m_refcount[1]) )
//	{
//		deleting = true;
//	}
//		
//	#if IUNKNOWN_ALLOC_SPEW
//		if (m_mark)
//		{
//			GLMPRINTF(("-A- IURelease (%08x,%d) refc -> (%d,%d) [%s] %s",this,which,m_refcount[0],m_refcount[1],comment?comment:"...",deleting?"->DELETING":""));
//			if (!comment)
//			{
//				GLMPRINTF((""))	;	// place to hang a breakpoint
//			}
//		}
//	#endif
//
//	if (deleting)
//	{
//		if (m_mark)
//		{
//			GLMPRINTF((""))	;		// place to hang a breakpoint
//		}
//		delete this;
//		return 0;
//	}
//	else
//	{
//		return m_refcount[0];
//	}
//}

HRESULT STDMETHODCALLTYPE CSurface9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CSurface9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CSurface9::GetType()
{
	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CSurface9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE CSurface9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetContainer(REFIID riid, void** ppContainer)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDC(HDC* phdc)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CSurface9::GetDesc(D3DSURFACE_DESC* pDesc)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CSurface9::LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{

	
	return S_OK;	
}


HRESULT STDMETHODCALLTYPE CSurface9::ReleaseDC(HDC hdc)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CSurface9::UnlockRect()
{

	return S_OK;	
}

ULONG STDMETHODCALLTYPE	CSurface9::Release( int which, char *comment)
{
	return 0;
}