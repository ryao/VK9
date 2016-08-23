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
 
#ifndef CQUERY9_H
#define CQUERY9_H

#include "d3d9.h" // Base class: IDirect3DQuery9
#include <vulkan/vulkan.h>
#include "CResource9.h"

class CQuery9 : public IDirect3DQuery9
{
private:
	CDevice9* mDevice;
	D3DQUERYTYPE mType;
public:
	CQuery9(CDevice9* device, D3DQUERYTYPE Type);
	~CQuery9();
	
	int mReferenceCount;
	VkResult mResult;
public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DQuery9
	virtual HRESULT STDMETHODCALLTYPE GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags);
	virtual DWORD STDMETHODCALLTYPE GetDataSize();
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) { (*ppDevice) = (IDirect3DDevice9*)mDevice; return S_OK; }
	virtual D3DQUERYTYPE STDMETHODCALLTYPE GetType();
	virtual HRESULT STDMETHODCALLTYPE Issue(DWORD dwIssueFlags);
};

#endif // CQUERY9_H
