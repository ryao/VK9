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
 
#ifndef CSURFACE9_H
#define CSURFACE9_H

#include "d3d9.h" // Base class: IDirect3DSurface9
#include <vulkan/vulkan.h>
#include "CResource9.h"

class CSurface9 : public IDirect3DSurface9,CResource9
{
private:
	
public:
	CSurface9(CDevice9* Device, IDirect3DTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, HANDLE *pSharedHandle);
	CSurface9(CDevice9* Device, IDirect3DTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, HANDLE *pSharedHandle,int32_t filler); //CreateRenderTarget
	CSurface9(CDevice9* Device, IDirect3DTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, HANDLE *pSharedHandle);
	~CSurface9();

	CDevice9* mDevice;
	IDirect3DTexture9* mTexture;
	UINT mWidth;
	UINT mHeight;
	D3DFORMAT mFormat;
	D3DMULTISAMPLE_TYPE mMultiSample;
	DWORD mMultisampleQuality;
	BOOL mDiscard;
	BOOL mLockable;
	HANDLE* mSharedHandle;

	int mReferenceCount;
	VkResult mResult;

	VkFormat mRealFormat;
	void* mData;

	VkSampler mSampler;
	VkImage mImage;
	VkImageLayout mImageLayout;
	VkMemoryAllocateInfo mMemoryAllocateInfo = {};
	VkDeviceMemory mDeviceMemory;
	VkImageView mImageView;

	void Init();
public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DResource9
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice){(*ppDevice)=(IDirect3DDevice9*)mDevice; return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);
	virtual DWORD STDMETHODCALLTYPE GetPriority();
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual D3DRESOURCETYPE STDMETHODCALLTYPE GetType();
	virtual void STDMETHODCALLTYPE PreLoad();
	virtual DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);

	//IDirect3DSurface9
	virtual HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer);
	virtual HRESULT STDMETHODCALLTYPE GetDC(HDC* phdc);
	virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DSURFACE_DESC* pDesc);
	virtual HRESULT STDMETHODCALLTYPE LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hdc);
	virtual HRESULT STDMETHODCALLTYPE UnlockRect();
};

#endif // CSURFACE9_H
