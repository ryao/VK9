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
 
#include "CVertexDeclaration9.h"
#include "CDevice9.h"

#include "Utilities.h"

CVertexDeclaration9::CVertexDeclaration9(CDevice9* device, const D3DVERTEXELEMENT9* pVertexElements)
	: mReferenceCount(0),
	mDevice(device),
	mVertexElements((D3DVERTEXELEMENT9*)pVertexElements),
	mResult(VK_SUCCESS)
{
}

CVertexDeclaration9::~CVertexDeclaration9()
{

}

ULONG STDMETHODCALLTYPE CVertexDeclaration9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CVertexDeclaration9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVertexDeclaration9))
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

ULONG STDMETHODCALLTYPE CVertexDeclaration9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
		return 0;
	}

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CVertexDeclaration9::GetDeclaration(D3DVERTEXELEMENT9* pDecl, UINT* pNumElements)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CVertexDeclaration9::GetDeclaration is not implemented!";

	return E_NOTIMPL;
}
