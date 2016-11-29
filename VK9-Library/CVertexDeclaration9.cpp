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
	: mReferenceCount(1),
	mDevice(device),
	mResult(VK_SUCCESS),
	mHasPosition(0),
	mHasColor(0),
	mTextureCount(0)
{
	size_t i = 0;

	while (pVertexElements[i].Stream != 0xFF)
	{
		mVertexElements.push_back(pVertexElements[i]);

		switch(pVertexElements[i].Usage)
		{
		case D3DDECLUSAGE_POSITION:
			mHasPosition = true;
			break;
		case D3DDECLUSAGE_COLOR:
			mHasColor = true;
			break;
		case D3DDECLUSAGE_TEXCOORD:
			mTextureCount++;
			break;
		case D3DDECLUSAGE_NORMAL:
			mHasNormal = true;
			break;
		default:
			BOOST_LOG_TRIVIAL(fatal) << "CVertexDeclaration9::CVertexDeclaration9 unknown vertex usage " << (uint32_t)pVertexElements[i].Usage;
			break;
		}

		i++;
	}
}

CVertexDeclaration9::~CVertexDeclaration9()
{

}

ULONG STDMETHODCALLTYPE CVertexDeclaration9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
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

ULONG STDMETHODCALLTYPE CVertexDeclaration9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CVertexDeclaration9::GetDeclaration(D3DVERTEXELEMENT9* pDecl, UINT* pNumElements)
{
	(*pNumElements) = mVertexElements.size() + 1; //+1 for the terminator.

	if (pDecl!=nullptr) //If null only return the count so the caller can use it to construct an array of the correct size.
	{
		for (size_t i = 0; i < mVertexElements.size(); i++)
		{
			pDecl[i] = mVertexElements[i];
		}
		pDecl[mVertexElements.size()] = D3DDECL_END(); //not a mistake raw D3DVERTEXELEMENT9 arrays contain a terminator but I don't store it because all I reall need is a count.
	}

	return E_NOTIMPL;
}
