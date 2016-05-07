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
 
#ifndef CVERTEXDECLARATION9_H
#define CVERTEXDECLARATION9_H

#include "d3d9.h" // Base class: IDirect3DVertexDeclaration9
#include <vulkan/vulkan.h>
#include "CUnknown.h"

class CDevice9;

class CVertexDeclaration9 : public IDirect3DVertexDeclaration9
{
private:
	CDevice9* mDevice;
public:
	CVertexDeclaration9();
	~CVertexDeclaration9();

public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DVertexDeclaration9
	virtual HRESULT STDMETHODCALLTYPE GetDeclaration(D3DVERTEXELEMENT9* pDecl, UINT* pNumElements);
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) { (*ppDevice) = (IDirect3DDevice9*)mDevice; return S_OK; }

	ULONG STDMETHODCALLTYPE AddRef( int which, char *comment = NULL );
	ULONG STDMETHODCALLTYPE	Release( int which, char *comment = NULL );
};

#endif // CVERTEXDECLARATION9_H
