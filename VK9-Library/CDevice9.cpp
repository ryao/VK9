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

#include <algorithm>
#include <limits>

#include "C9.h"
#include "CDevice9.h"
#include "CCubeTexture9.h"
#include "CBaseTexture9.h"
#include "CTexture9.h"
#include "CVolumeTexture9.h"
#include "CSurface9.h"
#include "CVertexDeclaration9.h"
#include "CVertexShader9.h"
#include "CStateBlock9.h"

#include "Utilities.h"

CDevice9::CDevice9(C9* Instance, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters)
	:
	mInstance(Instance),
	mAdapter(Adapter),
	mDeviceType(DeviceType),
	mFocusWindow(hFocusWindow),
	mBehaviorFlags(BehaviorFlags)
{
	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 Started.";

	memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	mCommandStreamManager = mInstance->mCommandStreamManager;
	mId = mInstance->mId;

	if (!mPresentationParameters.Windowed)
	{
		DEVMODE newSettings = {};
		EnumDisplaySettings(0, 0, &newSettings);
		newSettings.dmPelsWidth = mPresentationParameters.BackBufferWidth;
		newSettings.dmPelsHeight = mPresentationParameters.BackBufferHeight;
		newSettings.dmBitsPerPel = 32;
		newSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&newSettings, CDS_FULLSCREEN);
		SetWindowPos(hFocusWindow, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		InvalidateRect(hFocusWindow, 0, true);
	}

	WorkItem workItem;
	workItem.Id = mId;
	workItem.WorkItemType = WorkItemType::Device_Create;
	workItem.Argument1 = (void*)this;
	workItem.Argument2 = (void*)GetModuleHandle(nullptr);
	mId = mCommandStreamManager->RequestWork(workItem);

	//Add implicit swap chain.
	CSwapChain9* ptr = new CSwapChain9(pPresentationParameters);
	mSwapChains.push_back(ptr);

	//Add implicit render target
	CRenderTargetSurface9* ptr2 = new CRenderTargetSurface9(this, mPresentationParameters.BackBufferWidth, mPresentationParameters.BackBufferHeight, VK_FORMAT_UNDEFINED);
	mRenderTargets.push_back(ptr2);

	BOOST_LOG_TRIVIAL(info) << "CDevice9::CDevice9 Finished.";
}

CDevice9::~CDevice9()
{
	WorkItem workItem;
	workItem.WorkItemType = WorkItemType::Device_Destroy;
	workItem.Id = mId;
	mCommandStreamManager->RequestWork(workItem);

	BOOST_LOG_TRIVIAL(info) << "CDevice9::~CDevice9";

	for (size_t i = 0; i < mSwapChains.size(); i++)
	{
		delete mSwapChains[i];
	}

	delete[] mDisplays;
}

HRESULT STDMETHODCALLTYPE CDevice9::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	if ((Flags & D3DCLEAR_TARGET) == D3DCLEAR_TARGET)
	{
		//VK_FORMAT_B8G8R8A8_UNORM 
		//Revisit the byte order could be difference based on the surface format so I need a better way to handle this.
		mClearColorValue.float32[3] = D3DCOLOR_A(Color); //FLT_MAX; 
		mClearColorValue.float32[2] = D3DCOLOR_B(Color);
		mClearColorValue.float32[1] = D3DCOLOR_G(Color);
		mClearColorValue.float32[0] = D3DCOLOR_R(Color);
	}

	if (Count > 0 && pRects != nullptr)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::Clear is not fully implemented!";
		return E_NOTIMPL;
	}

	VkImageSubresourceRange subResourceRange = {};
	subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subResourceRange.baseMipLevel = 0;
	subResourceRange.levelCount = 1;
	subResourceRange.baseArrayLayer = 0;
	subResourceRange.layerCount = 1;

	if (mIsSceneStarted)
	{
		vkCmdEndRenderPass(mSwapchainBuffers[mCurrentBuffer]);
		vkCmdClearColorImage(mSwapchainBuffers[mCurrentBuffer], mSwapchainImages[mCurrentBuffer], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &mClearColorValue, 1, &subResourceRange);
		vkCmdBeginRenderPass(mSwapchainBuffers[mCurrentBuffer], &mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	else
	{
		this->StartScene(true);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::BeginScene() //
{
	//According to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndScene()
{
	//According to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}
	this->StopScene();

	VkResult result; // = VK_SUCCESS

	mPresentInfo.pImageIndices = &mCurrentBuffer;

	result = vkQueuePresentKHR(mQueue, &mPresentInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::Present vkQueuePresentKHR failed with return code of " << GetResultString(result);
		return D3DERR_INVALIDCALL;
	}

	result = vkQueueWaitIdle(mQueue);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::Present vkQueueWaitIdle failed with return code of " << GetResultString(result);
		return D3DERR_INVALIDCALL;
	}

	vkResetCommandBuffer(mSwapchainBuffers[mCurrentBuffer], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

	//Clean up pipes.
	mBufferManager->FlushDrawBufffer();

	//Clean up unreferenced resources.
	mGarbageManager.DestroyHandles();

	//Print(mDeviceState.mTransforms);

	return D3D_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CDevice9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DDevice9))
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

ULONG STDMETHODCALLTYPE CDevice9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}


HRESULT STDMETHODCALLTYPE CDevice9::BeginStateBlock()
{
	this->mCurrentStateRecording = new CStateBlock9(this);

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::BeginStateBlock " << this->mCurrentStateRecording;

	return S_OK;
}



HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface, const RECT *pRect, D3DCOLOR color)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ColorFill is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::CreateAdditionalSwapChain is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9 **ppCubeTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CCubeTexture9* obj = new CCubeTexture9(this, EdgeLength, Levels, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppCubeTexture) = (IDirect3DCubeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this, (CTexture9*)nullptr, Width, Height, Format, MultiSample, MultisampleQuality, Discard, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9 **ppIndexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CIndexBuffer9* obj = new CIndexBuffer9(this, Length, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppIndexBuffer) = (IDirect3DIndexBuffer9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateTexture Length:" << Length << " Usage:" << Usage << " Format:" << Format << " Pool:" << Pool;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* ptr = new CSurface9(this, (CTexture9*)nullptr, Width, Height, 1, 0, Format, Pool, pSharedHandle);

	if (ptr->mResult != VK_SUCCESS)
	{
		delete ptr;
		ptr = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)ptr;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreatePixelShader(const DWORD *pFunction, IDirect3DPixelShader9 **ppShader)
{
	HRESULT result = S_OK;

	CPixelShader9* obj = new CPixelShader9(this, pFunction);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppShader) = (IDirect3DPixelShader9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9 **ppQuery)
{
	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb174360(v=vs.85).aspx
	*/

	//If null is passed the call is checking to see if a query type is supported.
	if (ppQuery == nullptr)
	{
		return D3DERR_NOTAVAILABLE;
	}

	HRESULT result = S_OK;

	CQuery9* obj = new CQuery9(this, Type);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppQuery) = (IDirect3DQuery9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	//I added an extra int at the end so the signature would be different for this version. Locakable/Discard are both BOOL.
	CRenderTargetSurface9* obj = new CRenderTargetSurface9(this, Width, Height, Format);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
{
	HRESULT result = S_OK;

	CStateBlock9* obj = new CStateBlock9(this, Type);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppSB) = (IDirect3DStateBlock9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateStateBlock " << obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CTexture9* obj = new CTexture9(this, Width, Height, Levels, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppTexture) = (IDirect3DTexture9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateTexture Width:" << Width << " Height:" << Height << " Levels:" << Levels << " Usage:" << Usage << " Format:" << Format << " Pool:" << Pool;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CVertexBuffer9* obj = new CVertexBuffer9(this, Length, Usage, FVF, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppVertexBuffer) = (IDirect3DVertexBuffer9*)obj;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::CreateTexture Length:" << Length << " Usage:" << Usage << " Pool:" << Pool;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements, IDirect3DVertexDeclaration9 **ppDecl)
{
	HRESULT result = S_OK;

	CVertexDeclaration9* obj = new CVertexDeclaration9(this, pVertexElements);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppDecl) = (IDirect3DVertexDeclaration9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexShader(const DWORD *pFunction, IDirect3DVertexShader9 **ppShader)
{
	HRESULT result = S_OK;

	CVertexShader9* obj = new CVertexShader9(this, pFunction);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppShader) = (IDirect3DVertexShader9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9 **ppVolumeTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CVolumeTexture9* obj = new CVolumeTexture9(this, Width, Height, Depth, Levels, Usage, Format, Pool, pSharedHandle);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppVolumeTexture) = (IDirect3DVolumeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::DeletePatch(UINT Handle)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DeletePatch is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	if (mDeviceState.mIndexBuffer == nullptr)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitive called with null index buffer.";
		return D3DERR_INVALIDCALL;
	}

	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(this);
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(this);

	mBufferManager->BeginDraw(context, resourceContext, Type);

	/*
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb174369(v=vs.85).aspx
		https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdDrawIndexed.html
	*/
	vkCmdDrawIndexed(mSwapchainBuffers[mCurrentBuffer], min(mDeviceState.mIndexBuffer->mSize, ConvertPrimitiveCountToVertexCount(Type, PrimitiveCount)), 1, StartIndex, BaseVertexIndex, 0);

	//BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitive";
	//Print(mDeviceState.mTransforms);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawIndexedPrimitiveUP is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	std::shared_ptr<DrawContext> context = std::make_shared<DrawContext>(this);
	std::shared_ptr<ResourceContext> resourceContext = std::make_shared<ResourceContext>(this);

	mBufferManager->BeginDraw(context, resourceContext, PrimitiveType);

	vkCmdDraw(mSwapchainBuffers[mCurrentBuffer], min(mBufferManager->mVertexCount, ConvertPrimitiveCountToVertexCount(PrimitiveType, PrimitiveCount)), 1, StartVertex, 0);

	//Print(mDeviceState.mTransforms);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawPrimitiveUP is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle, const float *pNumSegs, const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawRectPatch is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle, const float *pNumSegs, const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	if (!mIsSceneStarted)
	{
		this->StartScene();
	}

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::DrawTriPatch is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
	(*ppSB) = this->mCurrentStateRecording;

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::EndStateBlock " << this->mCurrentStateRecording;

	this->mCurrentStateRecording = nullptr;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EvictManagedResources()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::EvictManagedResources is not implemented!";

	return S_OK;
}

UINT STDMETHODCALLTYPE CDevice9::GetAvailableTextureMem()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetAvailableTextureMem is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain, UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9 **ppBackBuffer)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetBackBuffer is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index, float *pPlane)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetClipPlane is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetClipStatus is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	pParameters->AdapterOrdinal = this->mAdapter;
	pParameters->DeviceType = this->mDeviceType;
	pParameters->hFocusWindow = this->mFocusWindow;
	pParameters->BehaviorFlags = this->mBehaviorFlags;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetCurrentTexturePalette is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetDepthStencilSurface is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
	/*
	https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkPhysicalDeviceProperties
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172513(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172635(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172591(v=vs.85).aspx
	*/

	//Translate the vulkan properties & features into D3D9 capabilities.
	pCaps->DeviceType = D3DDEVTYPE_HAL; //Assume all hardware.
	pCaps->AdapterOrdinal = 0;
	pCaps->Caps = 0;
	pCaps->Caps2 = D3DCAPS2_CANMANAGERESOURCE | D3DCAPS2_DYNAMICTEXTURES | D3DCAPS2_FULLSCREENGAMMA | D3DCAPS2_CANAUTOGENMIPMAP;
	pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD | D3DCAPS3_COPY_TO_VIDMEM | D3DCAPS3_COPY_TO_SYSTEMMEM | D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;
	pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_DEFAULT | D3DPRESENT_INTERVAL_ONE | D3DPRESENT_INTERVAL_TWO | D3DPRESENT_INTERVAL_THREE | D3DPRESENT_INTERVAL_FOUR | D3DPRESENT_INTERVAL_IMMEDIATE;
	pCaps->CursorCaps = D3DCURSORCAPS_COLOR | D3DCURSORCAPS_LOWRES;
	pCaps->DevCaps = D3DDEVCAPS_CANBLTSYSTONONLOCAL | D3DDEVCAPS_CANRENDERAFTERFLIP | D3DDEVCAPS_DRAWPRIMITIVES2 | D3DDEVCAPS_DRAWPRIMITIVES2EX | D3DDEVCAPS_DRAWPRIMTLVERTEX | D3DDEVCAPS_EXECUTESYSTEMMEMORY | D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_HWRASTERIZATION | D3DDEVCAPS_HWTRANSFORMANDLIGHT | D3DDEVCAPS_PUREDEVICE | D3DDEVCAPS_TEXTURENONLOCALVIDMEM | D3DDEVCAPS_TEXTUREVIDEOMEMORY | D3DDEVCAPS_TLVERTEXSYSTEMMEMORY | D3DDEVCAPS_TLVERTEXVIDEOMEMORY;
	pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_MASKZ | D3DPMISCCAPS_CULLNONE | D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_COLORWRITEENABLE | D3DPMISCCAPS_CLIPPLANESCALEDPOINTS | D3DPMISCCAPS_TSSARGTEMP | D3DPMISCCAPS_BLENDOP | D3DPMISCCAPS_INDEPENDENTWRITEMASKS | D3DPMISCCAPS_FOGANDSPECULARALPHA | D3DPMISCCAPS_SEPARATEALPHABLEND | D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS | D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING | D3DPMISCCAPS_FOGVERTEXCLAMPED;
	pCaps->RasterCaps = D3DPRASTERCAPS_ANISOTROPY | D3DPRASTERCAPS_COLORPERSPECTIVE | D3DPRASTERCAPS_DITHER | D3DPRASTERCAPS_DEPTHBIAS | D3DPRASTERCAPS_FOGRANGE | D3DPRASTERCAPS_FOGTABLE | D3DPRASTERCAPS_FOGVERTEX | D3DPRASTERCAPS_MIPMAPLODBIAS | D3DPRASTERCAPS_MULTISAMPLE_TOGGLE | D3DPRASTERCAPS_SCISSORTEST | D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS | D3DPRASTERCAPS_WFOG | D3DPRASTERCAPS_ZFOG | D3DPRASTERCAPS_ZTEST;
	pCaps->ZCmpCaps = D3DPCMPCAPS_NEVER | D3DPCMPCAPS_LESS | D3DPCMPCAPS_EQUAL | D3DPCMPCAPS_LESSEQUAL | D3DPCMPCAPS_GREATER | D3DPCMPCAPS_NOTEQUAL | D3DPCMPCAPS_GREATEREQUAL | D3DPCMPCAPS_ALWAYS;
	pCaps->SrcBlendCaps = D3DPBLENDCAPS_ZERO | D3DPBLENDCAPS_ONE | D3DPBLENDCAPS_SRCCOLOR | D3DPBLENDCAPS_INVSRCCOLOR | D3DPBLENDCAPS_SRCALPHA | D3DPBLENDCAPS_INVSRCALPHA | D3DPBLENDCAPS_DESTALPHA | D3DPBLENDCAPS_INVDESTALPHA | D3DPBLENDCAPS_DESTCOLOR | D3DPBLENDCAPS_INVDESTCOLOR | D3DPBLENDCAPS_SRCALPHASAT | D3DPBLENDCAPS_BOTHSRCALPHA | D3DPBLENDCAPS_BOTHINVSRCALPHA | D3DPBLENDCAPS_BLENDFACTOR | D3DPBLENDCAPS_INVSRCCOLOR2 | D3DPBLENDCAPS_SRCCOLOR2;
	pCaps->DestBlendCaps = pCaps->SrcBlendCaps;
	pCaps->AlphaCmpCaps = D3DPCMPCAPS_NEVER | D3DPCMPCAPS_LESS | D3DPCMPCAPS_EQUAL | D3DPCMPCAPS_LESSEQUAL | D3DPCMPCAPS_GREATER | D3DPCMPCAPS_NOTEQUAL | D3DPCMPCAPS_GREATEREQUAL | D3DPCMPCAPS_ALWAYS;
	pCaps->ShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB | D3DPSHADECAPS_SPECULARGOURAUDRGB | D3DPSHADECAPS_ALPHAGOURAUDBLEND | D3DPSHADECAPS_FOGGOURAUD;
	pCaps->TextureCaps = D3DPTEXTURECAPS_ALPHA | D3DPTEXTURECAPS_ALPHAPALETTE | D3DPTEXTURECAPS_PERSPECTIVE | D3DPTEXTURECAPS_PROJECTED | D3DPTEXTURECAPS_CUBEMAP | D3DPTEXTURECAPS_VOLUMEMAP | D3DPTEXTURECAPS_POW2 | D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_CUBEMAP_POW2 | D3DPTEXTURECAPS_VOLUMEMAP_POW2 | D3DPTEXTURECAPS_MIPMAP | D3DPTEXTURECAPS_MIPVOLUMEMAP | D3DPTEXTURECAPS_MIPCUBEMAP;
	pCaps->TextureFilterCaps = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MINFANISOTROPIC | D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR | D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR | D3DPTFILTERCAPS_MAGFANISOTROPIC;
	pCaps->CubeTextureFilterCaps = pCaps->TextureFilterCaps;
	pCaps->VolumeTextureFilterCaps = pCaps->TextureFilterCaps;
	pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER | D3DPTADDRESSCAPS_INDEPENDENTUV | D3DPTADDRESSCAPS_WRAP | D3DPTADDRESSCAPS_MIRROR | D3DPTADDRESSCAPS_CLAMP | D3DPTADDRESSCAPS_MIRRORONCE;
	pCaps->VolumeTextureAddressCaps = pCaps->TextureAddressCaps;
	pCaps->LineCaps = D3DLINECAPS_ALPHACMP | D3DLINECAPS_BLEND | D3DLINECAPS_TEXTURE | D3DLINECAPS_ZTEST | D3DLINECAPS_FOG;
	pCaps->MaxTextureWidth = mDeviceProperties.limits.maxImageDimension2D; //Revisit
	pCaps->MaxTextureHeight = mDeviceProperties.limits.maxImageDimension2D; //Revisit
	pCaps->MaxVolumeExtent = mDeviceProperties.limits.maxImageDimensionCube; //Revisit
	pCaps->MaxTextureRepeat = 32768; //revisit
	pCaps->MaxTextureAspectRatio = pCaps->MaxTextureWidth;
	pCaps->MaxAnisotropy = mDeviceFeatures.samplerAnisotropy;
	pCaps->MaxVertexW = 1e10f; //revisit
	pCaps->GuardBandLeft = -1e9f; //revisit
	pCaps->GuardBandTop = -1e9f; //revisit
	pCaps->GuardBandRight = 1e9f; //revisit
	pCaps->GuardBandBottom = 1e9f; //revisit
	pCaps->ExtentsAdjust = 0.0f; //revisit
	pCaps->StencilCaps = D3DSTENCILCAPS_KEEP | D3DSTENCILCAPS_ZERO | D3DSTENCILCAPS_REPLACE | D3DSTENCILCAPS_INCRSAT | D3DSTENCILCAPS_DECRSAT | D3DSTENCILCAPS_INVERT | D3DSTENCILCAPS_INCR | D3DSTENCILCAPS_DECR | D3DSTENCILCAPS_TWOSIDED;
	pCaps->FVFCaps = D3DFVFCAPS_PSIZE;
	pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_MODULATE | D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE4X | D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_ADDSIGNED | D3DTEXOPCAPS_ADDSIGNED2X | D3DTEXOPCAPS_SUBTRACT | D3DTEXOPCAPS_ADDSMOOTH | D3DTEXOPCAPS_BLENDDIFFUSEALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHA | D3DTEXOPCAPS_BLENDFACTORALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHAPM | D3DTEXOPCAPS_BLENDCURRENTALPHA | D3DTEXOPCAPS_PREMODULATE | D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA | D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA | D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE | D3DTEXOPCAPS_DOTPRODUCT3 | D3DTEXOPCAPS_MULTIPLYADD | D3DTEXOPCAPS_LERP;
	pCaps->MaxTextureBlendStages = mDeviceProperties.limits.maxDescriptorSetSamplers; //revisit
	pCaps->MaxSimultaneousTextures = mDeviceProperties.limits.maxDescriptorSetSampledImages; //revisit
	pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_MATERIALSOURCE7 | D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_LOCALVIEWER | D3DVTXPCAPS_TWEENING;
	pCaps->MaxActiveLights = 0;  //Revsit should be infinite but games may not read it that way.
	pCaps->MaxUserClipPlanes = 8; //revisit
	pCaps->MaxVertexBlendMatrices = 4; //revisit
	pCaps->MaxVertexBlendMatrixIndex = 7; //revisit
	pCaps->MaxPointSize = mDeviceProperties.limits.pointSizeRange[1]; //revisit
	pCaps->MaxPrimitiveCount = 0xFFFFFFFF; //revisit
	pCaps->MaxVertexIndex = 0xFFFFFFFF; //revisit
	pCaps->MaxStreams = mDeviceProperties.limits.maxVertexInputBindings; //revisit
	pCaps->MaxStreamStride = mDeviceProperties.limits.maxVertexInputBindingStride; //revisit
	pCaps->VertexShaderVersion = D3DVS_VERSION(3, 0);
	pCaps->MaxVertexShaderConst = 256; //revisit
	pCaps->PixelShaderVersion = D3DPS_VERSION(3, 0);
	pCaps->PixelShader1xMaxValue = 65504.f;
	pCaps->DevCaps2 = D3DDEVCAPS2_STREAMOFFSET | D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET | D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES;
	pCaps->MasterAdapterOrdinal = 0;
	pCaps->AdapterOrdinalInGroup = 0;
	pCaps->NumberOfAdaptersInGroup = 1;
	pCaps->DeclTypes = D3DDTCAPS_UBYTE4 | D3DDTCAPS_UBYTE4N | D3DDTCAPS_SHORT2N | D3DDTCAPS_SHORT4N | D3DDTCAPS_USHORT2N | D3DDTCAPS_USHORT4N | D3DDTCAPS_UDEC3 | D3DDTCAPS_DEC3N | D3DDTCAPS_FLOAT16_2 | D3DDTCAPS_FLOAT16_4;
	pCaps->NumSimultaneousRTs = 4; //revisit
	pCaps->StretchRectFilterCaps = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR;
	pCaps->VS20Caps.Caps = D3DVS20CAPS_PREDICATION;
	pCaps->VS20Caps.DynamicFlowControlDepth = 24; //revsit
	pCaps->VS20Caps.StaticFlowControlDepth = 4; //revsit
	pCaps->VS20Caps.NumTemps = 32; //revsit
	pCaps->PS20Caps.Caps = D3DPS20CAPS_ARBITRARYSWIZZLE | D3DPS20CAPS_GRADIENTINSTRUCTIONS | D3DPS20CAPS_PREDICATION;
	pCaps->PS20Caps.DynamicFlowControlDepth = 24; //revsit
	pCaps->PS20Caps.StaticFlowControlDepth = 4; //revsit
	pCaps->PS20Caps.NumTemps = 32; //revsit
	pCaps->VertexTextureFilterCaps = pCaps->TextureFilterCaps; //revisit
	pCaps->MaxVertexShader30InstructionSlots = 32768; //revisit
	pCaps->MaxPixelShader30InstructionSlots = 32768; //revisit
	pCaps->MaxVShaderInstructionsExecuted = 65535; //65535 < pCaps->MaxVertexShader30InstructionSlots * 32 ? pCaps->MaxVertexShader30InstructionSlots * 32 : 65535; //revisit
	pCaps->MaxPShaderInstructionsExecuted = 65535; //65535 < pCaps->MaxPixelShader30InstructionSlots * 32 ? pCaps->MaxPixelShader30InstructionSlots * 32 : 65535; //revisit


	pCaps->MaxNpatchTessellationLevel = 0.0f;
	pCaps->Reserved5 = 0;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	(*ppD3D9) = (IDirect3D9*)mInstance;

	mInstance->AddRef();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayMode(UINT  iSwapChain, D3DDISPLAYMODE *pMode)
{
	if (iSwapChain)
	{
		//TODO: Implement.
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetDisplayMode multiple swapchains are not implemented!";
	}
	else
	{
		pMode->Height = this->mSwapchainExtent.height;
		pMode->Width = this->mSwapchainExtent.width;
		pMode->RefreshRate = 60; //fake it till you make it.
		pMode->Format = ConvertFormat(this->mFormat);
	}

	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Height: " << pMode->Height;
	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Width: " << pMode->Width;
	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode RefreshRate: " << pMode->RefreshRate;
	//BOOST_LOG_TRIVIAL(info) << "CDevice9::GetDisplayMode Format: " << pMode->Format;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain, IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetFrontBufferData is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{
	(*pFVF) = mDeviceState.mFVF;

	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain, D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetGammaRamp is not implemented!";

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData) //,UINT *pBaseVertexIndex ?
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetIndices is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight)
{
	auto& light = mDeviceState.mLights[Index];

	pLight->Type = (*(D3DLIGHTTYPE*)light.Type);
	pLight->Diffuse = (*(D3DCOLORVALUE*)light.Diffuse);
	pLight->Specular = (*(D3DCOLORVALUE*)light.Specular);
	pLight->Ambient = (*(D3DCOLORVALUE*)light.Ambient);

	pLight->Position = (*(D3DVECTOR*)light.Position);
	pLight->Direction = (*(D3DVECTOR*)light.Direction);

	pLight->Range = light.Range;
	pLight->Falloff = light.Falloff;
	pLight->Attenuation0 = light.Attenuation0;
	pLight->Attenuation1 = light.Attenuation1;
	pLight->Attenuation2 = light.Attenuation2;
	pLight->Theta = light.Theta;
	pLight->Phi = light.Phi;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index, BOOL *pEnable)
{
	(*pEnable) = mDeviceState.mLights[Index].IsEnabled;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	(*pMaterial) = mDeviceState.mMaterial;

	return S_OK;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	return mDeviceState.mNSegments;
}

UINT STDMETHODCALLTYPE CDevice9::GetNumberOfSwapChains()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetNumberOfSwapChains is not implemented!";

	return 0;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetPaletteEntries is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
	(*ppShader) = (IDirect3DPixelShader9*)mDeviceState.mPixelShader;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{
	auto& slots = mDeviceState.mPixelShaderConstantSlots;
	for (size_t i = 0; i < BoolCount; i++)
	{
		pConstantData[i] = slots.BooleanConstants[StartRegister + i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{
	auto& slots = mDeviceState.mPixelShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4fCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		pConstantData[i] = slots.FloatConstants[startIndex + i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{
	auto& slots = mDeviceState.mPixelShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4iCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		pConstantData[i] = slots.IntegerConstants[startIndex + i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain, D3DRASTER_STATUS *pRasterStatus)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRasterStatus is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue)
{
	SpecializationConstants* constants = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		constants = &this->mCurrentStateRecording->mDeviceState.mSpecializationConstants;
	}
	else
	{
		constants = &mDeviceState.mSpecializationConstants;
	}

	switch (State)
	{
	case D3DRS_ZENABLE:
		(*pValue) = constants->zEnable;
		break;
	case D3DRS_FILLMODE:
		(*pValue) = constants->fillMode;
		break;
	case D3DRS_SHADEMODE:
		(*pValue) = constants->shadeMode;
		break;
	case D3DRS_ZWRITEENABLE:
		(*pValue) = constants->zWriteEnable;
		break;
	case D3DRS_ALPHATESTENABLE:
		(*pValue) = constants->alphaTestEnable;
		break;
	case D3DRS_LASTPIXEL:
		(*pValue) = constants->lastPixel;
		break;
	case D3DRS_SRCBLEND:
		(*pValue) = constants->sourceBlend;
		break;
	case D3DRS_DESTBLEND:
		(*pValue) = constants->destinationBlend;
		break;
	case D3DRS_CULLMODE:
		(*pValue) = constants->cullMode;
		break;
	case D3DRS_ZFUNC:
		(*pValue) = constants->zFunction;
		break;
	case D3DRS_ALPHAREF:
		(*pValue) = constants->alphaReference;
		break;
	case D3DRS_ALPHAFUNC:
		(*pValue) = constants->alphaFunction;
		break;
	case D3DRS_DITHERENABLE:
		(*pValue) = constants->ditherEnable;
		break;
	case D3DRS_ALPHABLENDENABLE:
		(*pValue) = constants->alphaBlendEnable;
		break;
	case D3DRS_FOGENABLE:
		(*pValue) = constants->fogEnable;
		break;
	case D3DRS_SPECULARENABLE:
		(*pValue) = constants->specularEnable;
		break;
	case D3DRS_FOGCOLOR:
		(*pValue) = constants->fogColor;
		break;
	case D3DRS_FOGTABLEMODE:
		(*pValue) = constants->fogTableMode;
		break;
	case D3DRS_FOGSTART:
		(*pValue) = bit_cast(constants->fogStart);
		break;
	case D3DRS_FOGEND:
		(*pValue) = bit_cast(constants->fogEnd);
		break;
	case D3DRS_FOGDENSITY:
		(*pValue) = bit_cast(constants->fogDensity);
		break;
	case D3DRS_RANGEFOGENABLE:
		(*pValue) = constants->rangeFogEnable;
		break;
	case D3DRS_STENCILENABLE:
		(*pValue) = constants->stencilEnable;
		break;
	case D3DRS_STENCILFAIL:
		(*pValue) = constants->stencilFail;
		break;
	case D3DRS_STENCILZFAIL:
		(*pValue) = constants->stencilZFail;
		break;
	case D3DRS_STENCILPASS:
		(*pValue) = constants->stencilPass;
		break;
	case D3DRS_STENCILFUNC:
		(*pValue) = constants->stencilFunction;
		break;
	case D3DRS_STENCILREF:
		(*pValue) = constants->stencilReference;
		break;
	case D3DRS_STENCILMASK:
		(*pValue) = constants->stencilMask;
		break;
	case D3DRS_STENCILWRITEMASK:
		(*pValue) = constants->stencilWriteMask;
		break;
	case D3DRS_TEXTUREFACTOR:
		(*pValue) = constants->textureFactor;
		break;
	case D3DRS_WRAP0:
		(*pValue) = constants->wrap0;
		break;
	case D3DRS_WRAP1:
		(*pValue) = constants->wrap1;
		break;
	case D3DRS_WRAP2:
		(*pValue) = constants->wrap2;
		break;
	case D3DRS_WRAP3:
		(*pValue) = constants->wrap3;
		break;
	case D3DRS_WRAP4:
		(*pValue) = constants->wrap4;
		break;
	case D3DRS_WRAP5:
		(*pValue) = constants->wrap5;
		break;
	case D3DRS_WRAP6:
		(*pValue) = constants->wrap6;
		break;
	case D3DRS_WRAP7:
		(*pValue) = constants->wrap7;
		break;
	case D3DRS_CLIPPING:
		(*pValue) = constants->clipping;
		break;
	case D3DRS_LIGHTING:
		(*pValue) = constants->lighting;
		break;
	case D3DRS_AMBIENT:
		(*pValue) = constants->ambient;
		break;
	case D3DRS_FOGVERTEXMODE:
		(*pValue) = constants->fogVertexMode;
		break;
	case D3DRS_COLORVERTEX:
		(*pValue) = constants->colorVertex;
		break;
	case D3DRS_LOCALVIEWER:
		(*pValue) = constants->localViewer;
		break;
	case D3DRS_NORMALIZENORMALS:
		(*pValue) = constants->normalizeNormals;
		break;
	case D3DRS_DIFFUSEMATERIALSOURCE:
		(*pValue) = constants->diffuseMaterialSource;
		break;
	case D3DRS_SPECULARMATERIALSOURCE:
		(*pValue) = constants->specularMaterialSource;
		break;
	case D3DRS_AMBIENTMATERIALSOURCE:
		(*pValue) = constants->ambientMaterialSource;
		break;
	case D3DRS_EMISSIVEMATERIALSOURCE:
		(*pValue) = constants->emissiveMaterialSource;
		break;
	case D3DRS_VERTEXBLEND:
		(*pValue) = constants->vertexBlend;
		break;
	case D3DRS_CLIPPLANEENABLE:
		(*pValue) = constants->clipPlaneEnable;
		break;
	case D3DRS_POINTSIZE:
		(*pValue) = constants->pointSize;
		break;
	case D3DRS_POINTSIZE_MIN:
		(*pValue) = bit_cast(constants->pointSizeMinimum);
		break;
	case D3DRS_POINTSPRITEENABLE:
		(*pValue) = constants->pointSpriteEnable;
		break;
	case D3DRS_POINTSCALEENABLE:
		(*pValue) = constants->pointScaleEnable;
		break;
	case D3DRS_POINTSCALE_A:
		(*pValue) = bit_cast(constants->pointScaleA);
		break;
	case D3DRS_POINTSCALE_B:
		(*pValue) = bit_cast(constants->pointScaleB);
		break;
	case D3DRS_POINTSCALE_C:
		(*pValue) = bit_cast(constants->pointScaleC);
		break;
	case D3DRS_MULTISAMPLEANTIALIAS:
		(*pValue) = constants->multisampleAntiAlias;
		break;
	case D3DRS_MULTISAMPLEMASK:
		(*pValue) = constants->multisampleMask;
		break;
	case D3DRS_PATCHEDGESTYLE:
		(*pValue) = constants->patchEdgeStyle;
		break;
	case D3DRS_DEBUGMONITORTOKEN:
		(*pValue) = constants->debugMonitorToken;
		break;
	case D3DRS_POINTSIZE_MAX:
		(*pValue) = bit_cast(constants->pointSizeMaximum);
		break;
	case D3DRS_INDEXEDVERTEXBLENDENABLE:
		(*pValue) = constants->indexedVertexBlendEnable;
		break;
	case D3DRS_COLORWRITEENABLE:
		(*pValue) = constants->colorWriteEnable;
		break;
	case D3DRS_TWEENFACTOR:
		(*pValue) = bit_cast(constants->tweenFactor);
		break;
	case D3DRS_BLENDOP:
		(*pValue) = constants->blendOperation;
		break;
	case D3DRS_POSITIONDEGREE:
		(*pValue) = constants->positionDegree;
		break;
	case D3DRS_NORMALDEGREE:
		(*pValue) = constants->normalDegree;
		break;
	case D3DRS_SCISSORTESTENABLE:
		(*pValue) = constants->scissorTestEnable;
		break;
	case D3DRS_SLOPESCALEDEPTHBIAS:
		(*pValue) = bit_cast(constants->slopeScaleDepthBias);
		break;
	case D3DRS_ANTIALIASEDLINEENABLE:
		(*pValue) = constants->antiAliasedLineEnable;
		break;
	case D3DRS_MINTESSELLATIONLEVEL:
		(*pValue) = bit_cast(constants->minimumTessellationLevel);
		break;
	case D3DRS_MAXTESSELLATIONLEVEL:
		(*pValue) = bit_cast(constants->maximumTessellationLevel);
		break;
	case D3DRS_ADAPTIVETESS_X:
		(*pValue) = bit_cast(constants->adaptivetessX);
		break;
	case D3DRS_ADAPTIVETESS_Y:
		(*pValue) = bit_cast(constants->adaptivetessY);
		break;
	case D3DRS_ADAPTIVETESS_Z:
		(*pValue) = bit_cast(constants->adaptivetessZ);
		break;
	case D3DRS_ADAPTIVETESS_W:
		(*pValue) = bit_cast(constants->adaptivetessW);
		break;
	case D3DRS_ENABLEADAPTIVETESSELLATION:
		(*pValue) = constants->enableAdaptiveTessellation;
		break;
	case D3DRS_TWOSIDEDSTENCILMODE:
		(*pValue) = constants->twoSidedStencilMode;
		break;
	case D3DRS_CCW_STENCILFAIL:
		(*pValue) = constants->ccwStencilFail;
		break;
	case D3DRS_CCW_STENCILZFAIL:
		(*pValue) = constants->ccwStencilZFail;
		break;
	case D3DRS_CCW_STENCILPASS:
		(*pValue) = constants->ccwStencilPass;
		break;
	case D3DRS_CCW_STENCILFUNC:
		(*pValue) = constants->ccwStencilFunction;
		break;
	case D3DRS_COLORWRITEENABLE1:
		(*pValue) = constants->colorWriteEnable1;
		break;
	case D3DRS_COLORWRITEENABLE2:
		(*pValue) = constants->colorWriteEnable2;
		break;
	case D3DRS_COLORWRITEENABLE3:
		(*pValue) = constants->colorWriteEnable3;
		break;
	case D3DRS_BLENDFACTOR:
		(*pValue) = constants->blendFactor;
		break;
	case D3DRS_SRGBWRITEENABLE:
		(*pValue) = constants->srgbWriteEnable;
		break;
	case D3DRS_DEPTHBIAS:
		(*pValue) = bit_cast(constants->depthBias);
		break;
	case D3DRS_WRAP8:
		(*pValue) = constants->wrap8;
		break;
	case D3DRS_WRAP9:
		(*pValue) = constants->wrap9;
		break;
	case D3DRS_WRAP10:
		(*pValue) = constants->wrap10;
		break;
	case D3DRS_WRAP11:
		(*pValue) = constants->wrap11;
		break;
	case D3DRS_WRAP12:
		(*pValue) = constants->wrap12;
		break;
	case D3DRS_WRAP13:
		(*pValue) = constants->wrap13;
		break;
	case D3DRS_WRAP14:
		(*pValue) = constants->wrap14;
		break;
	case D3DRS_WRAP15:
		(*pValue) = constants->wrap15;
		break;
	case D3DRS_SEPARATEALPHABLENDENABLE:
		(*pValue) = constants->separateAlphaBlendEnable;
		break;
	case D3DRS_SRCBLENDALPHA:
		(*pValue) = constants->sourceBlendAlpha;
		break;
	case D3DRS_DESTBLENDALPHA:
		(*pValue) = constants->destinationBlendAlpha;
		break;
	case D3DRS_BLENDOPALPHA:
		(*pValue) = constants->blendOperationAlpha;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRenderState unknown state! " << State;
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 **ppRenderTarget)
{
	(*ppRenderTarget) = mRenderTargets[RenderTargetIndex];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRenderTargetData is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD *pValue)
{
	(*pValue) = mDeviceState.mSamplerStates[Sampler][Type];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT *pRect)
{
	(*pRect) = mDeviceState.m9Scissor;

	return S_OK;
}

BOOL STDMETHODCALLTYPE CDevice9::GetSoftwareVertexProcessing()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetSoftwareVertexProcessing is not implemented!";

	return true;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 **ppStreamData, UINT *pOffsetInBytes, UINT *pStride)
{
	StreamSource& value = mDeviceState.mStreamSources[StreamNumber];

	(*ppStreamData) = (IDirect3DVertexBuffer9*)value.StreamData;
	/*
	Vulkan wants 64bit uint but d3d9 uses 32bit uint. This cast just keeps compiler from complaining.
	This should be safe because only 32bit can be set and d3d9 is x86 only so endianness issues shouldn't come into play.
	*/
	(*pOffsetInBytes) = (UINT)value.OffsetInBytes;
	(*pStride) = value.Stride;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT *pDivider)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetStreamSourceFreq is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSwapChain(UINT  iSwapChain, IDirect3DSwapChain9 **ppSwapChain)
{
	(*ppSwapChain) = (IDirect3DSwapChain9*)mSwapChains[iSwapChain];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9 **ppTexture)
{
	DeviceState* state = NULL;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	auto it = state->mTextures.find(Stage);
	if (it != state->mTextures.end())
	{
		(*ppTexture) = it->second;
	}
	else
	{
		(*ppTexture) = nullptr;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	switch (Type)
	{
	case D3DTSS_COLOROP:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorOperation_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorOperation_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorOperation_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorOperation_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorOperation_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorOperation_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorOperation_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorOperation_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG1:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorArgument1_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorArgument1_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorArgument1_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorArgument1_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorArgument1_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorArgument1_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorArgument1_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorArgument1_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG2:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorArgument2_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorArgument2_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorArgument2_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorArgument2_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorArgument2_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorArgument2_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorArgument2_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorArgument2_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAOP:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaOperation_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaOperation_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaOperation_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaOperation_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaOperation_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaOperation_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaOperation_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaOperation_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG1:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaArgument1_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG2:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaArgument2_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT00:
		switch (Stage)
		{
		case 0:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_0);
			break;
		case 1:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_1);
			break;
		case 2:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_2);
			break;
		case 3:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_3);
			break;
		case 4:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_4);
			break;
		case 5:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_5);
			break;
		case 6:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_6);
			break;
		case 7:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_7);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT01:
		switch (Stage)
		{
		case 0:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_0);
			break;
		case 1:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_1);
			break;
		case 2:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_2);
			break;
		case 3:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_3);
			break;
		case 4:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_4);
			break;
		case 5:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_5);
			break;
		case 6:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_6);
			break;
		case 7:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_7);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT10:
		switch (Stage)
		{
		case 0:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_0);
			break;
		case 1:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_1);
			break;
		case 2:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_2);
			break;
		case 3:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_3);
			break;
		case 4:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_4);
			break;
		case 5:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_5);
			break;
		case 6:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_6);
			break;
		case 7:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_7);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT11:
		switch (Stage)
		{
		case 0:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_0);
			break;
		case 1:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_1);
			break;
		case 2:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_2);
			break;
		case 3:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_3);
			break;
		case 4:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_4);
			break;
		case 5:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_5);
			break;
		case 6:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_6);
			break;
		case 7:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_7);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXCOORDINDEX:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLSCALE:
		switch (Stage)
		{
		case 0:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_0);
			break;
		case 1:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_1);
			break;
		case 2:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_2);
			break;
		case 3:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_3);
			break;
		case 4:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_4);
			break;
		case 5:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_5);
			break;
		case 6:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_6);
			break;
		case 7:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_7);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLOFFSET:
		switch (Stage)
		{
		case 0:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_0);
			break;
		case 1:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_1);
			break;
		case 2:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_2);
			break;
		case 3:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_3);
			break;
		case 4:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_4);
			break;
		case 5:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_5);
			break;
		case 6:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_6);
			break;
		case 7:
			(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_7);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXTURETRANSFORMFLAGS:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.textureTransformationFlags_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG0:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.colorArgument0_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.colorArgument0_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.colorArgument0_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.colorArgument0_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.colorArgument0_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.colorArgument0_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.colorArgument0_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.colorArgument0_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG0:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.alphaArgument0_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_RESULTARG:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.Result_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.Result_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.Result_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.Result_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.Result_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.Result_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.Result_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.Result_7;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_CONSTANT:
		switch (Stage)
		{
		case 0:
			(*pValue) = state->mSpecializationConstants.Constant_0;
			break;
		case 1:
			(*pValue) = state->mSpecializationConstants.Constant_1;
			break;
		case 2:
			(*pValue) = state->mSpecializationConstants.Constant_2;
			break;
		case 3:
			(*pValue) = state->mSpecializationConstants.Constant_3;
			break;
		case 4:
			(*pValue) = state->mSpecializationConstants.Constant_4;
			break;
		case 5:
			(*pValue) = state->mSpecializationConstants.Constant_5;
			break;
		case 6:
			(*pValue) = state->mSpecializationConstants.Constant_6;
			break;
		case 7:
			(*pValue) = state->mSpecializationConstants.Constant_7;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	(*pMatrix) = mDeviceState.mTransforms[State];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
	(*ppDecl) = (IDirect3DVertexDeclaration9*)this->mDeviceState.mVertexDeclaration;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
	(*ppShader) = (IDirect3DVertexShader9*)mDeviceState.mVertexShader;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{
	auto& slots = mDeviceState.mVertexShaderConstantSlots;
	for (size_t i = 0; i < BoolCount; i++)
	{
		pConstantData[i] = slots.BooleanConstants[StartRegister + i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{
	auto& slots = mDeviceState.mVertexShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4fCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		if ((startIndex + i) < 128)
		{
			pConstantData[i] = mDeviceState.mPushConstants[startIndex + i];
		}
		else
		{
			pConstantData[i] = slots.FloatConstants[startIndex + i];
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{
	auto& slots = mDeviceState.mVertexShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4iCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		pConstantData[i] = slots.IntegerConstants[startIndex + i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	(*pViewport) = mDeviceState.m9Viewport;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex, BOOL bEnable)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	Light light = {};

	if (state->mLights.size() == LightIndex)
	{
		light.IsEnabled = bEnable;
		state->mLights.push_back(light);
		state->mAreLightsDirty = true;
	}
	else
	{
		state->mLights[LightIndex].IsEnabled = bEnable;
		state->mAreLightsDirty = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::MultiplyTransform is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9 *pDestBuffer, IDirect3DVertexDeclaration9 *pVertexDecl, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ProcessVertices is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::Reset is not implemented!";

	return S_OK;
}

HRESULT CDevice9::SetClipPlane(DWORD Index, const float *pPlane)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetClipPlane is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetClipStatus is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetCurrentTexturePalette is not implemented!";

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CDevice9::SetCursorPosition(INT X, INT Y, DWORD Flags)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetCursorPosition is not implemented!";

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetCursorProperties is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetDepthStencilSurface is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetDialogBoxMode is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetFVF(DWORD FVF)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mFVF = FVF;
		this->mCurrentStateRecording->mDeviceState.mHasFVF = true;
		this->mCurrentStateRecording->mDeviceState.mHasVertexDeclaration = false;
	}
	else
	{
		mDeviceState.mFVF = FVF;
		mDeviceState.mHasFVF = true;
		mDeviceState.mHasVertexDeclaration = false;
	}

	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain, DWORD Flags, const D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetGammaRamp is not implemented!";
}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	state->mIndexBuffer = (CIndexBuffer9*)pIndexData;
	state->mHasIndexBuffer = true;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index, const D3DLIGHT9 *pLight)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	Light light = {};

	light.Type = pLight->Type;
	light.Diffuse[0] = pLight->Diffuse.r;
	light.Diffuse[1] = pLight->Diffuse.g;
	light.Diffuse[2] = pLight->Diffuse.b;
	light.Diffuse[3] = pLight->Diffuse.a;

	light.Specular[0] = pLight->Specular.r;
	light.Specular[1] = pLight->Specular.g;
	light.Specular[2] = pLight->Specular.b;
	light.Specular[3] = pLight->Specular.a;

	light.Ambient[0] = pLight->Ambient.r;
	light.Ambient[1] = pLight->Ambient.g;
	light.Ambient[2] = pLight->Ambient.b;
	light.Ambient[3] = pLight->Ambient.a;

	light.Position[0] = pLight->Position.x;
	light.Position[1] = pLight->Position.y;
	light.Position[2] = pLight->Position.z;
	//No need to set [3] because structure is init with {} so it's already 0.

	light.Direction[0] = pLight->Direction.x;
	light.Direction[1] = pLight->Direction.y;
	light.Direction[2] = pLight->Direction.z;
	//No need to set [3] because structure is init with {} so it's already 0.

	light.Range = pLight->Range;
	light.Falloff = pLight->Falloff;
	light.Attenuation0 = pLight->Attenuation0;
	light.Attenuation1 = pLight->Attenuation1;
	light.Attenuation2 = pLight->Attenuation2;
	light.Theta = pLight->Theta;
	light.Phi = pLight->Phi;

	if (state->mLights.size() == Index)
	{
		state->mLights.push_back(light);
		state->mAreLightsDirty = true;
	}
	else
	{
		light.IsEnabled = state->mLights[Index].IsEnabled;

		state->mLights[Index] = light;
		state->mAreLightsDirty = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mMaterial = (*pMaterial);
		this->mCurrentStateRecording->mDeviceState.mIsMaterialDirty = true;
	}
	else
	{
		mDeviceState.mMaterial = (*pMaterial);
		mDeviceState.mIsMaterialDirty = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{
	if (nSegments > 0.0f)
	{
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetNPatchMode nPatch greater than zero not supported.";
	}

	if (this->mCurrentStateRecording != nullptr)
	{
		//BOOST_LOG_TRIVIAL(info) << "Recorded NSegments";
		this->mCurrentStateRecording->mDeviceState.mNSegments = nSegments;
	}
	else
	{
		mDeviceState.mNSegments = nSegments;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetPaletteEntries is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
	if (pShader != nullptr)
	{
		pShader->AddRef();
	}

	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mPixelShader = (CPixelShader9*)pShader;
		this->mCurrentStateRecording->mDeviceState.mHasPixelShader = true;
	}
	else
	{
		if (mDeviceState.mPixelShader != nullptr)
		{
			mDeviceState.mPixelShader->Release();
		}

		mDeviceState.mPixelShader = (CPixelShader9*)pShader;
		mDeviceState.mHasPixelShader = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister, const BOOL *pConstantData, UINT BoolCount)
{
	auto& slots = mDeviceState.mPixelShaderConstantSlots;
	for (size_t i = 0; i < BoolCount; i++)
	{
		slots.BooleanConstants[StartRegister + i] = pConstantData[i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{
	auto& slots = mDeviceState.mPixelShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4fCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		slots.FloatConstants[startIndex + i] = pConstantData[i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister, const int *pConstantData, UINT Vector4iCount)
{
	auto& slots = mDeviceState.mPixelShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4iCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		slots.IntegerConstants[startIndex + i] = pConstantData[i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	SpecializationConstants* constants = nullptr;
	DeviceState* state = NULL;

	if (this->mCurrentStateRecording != nullptr)
	{
		constants = &this->mCurrentStateRecording->mDeviceState.mSpecializationConstants;
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		constants = &mDeviceState.mSpecializationConstants;
		state = &mDeviceState;
	}

	switch (State)
	{
	case D3DRS_ZENABLE:
		constants->zEnable = Value;
		state->hasZEnable = true;
		break;
	case D3DRS_FILLMODE:
		constants->fillMode = Value;
		state->hasFillMode = true;
		break;
	case D3DRS_SHADEMODE:
		constants->shadeMode = Value;
		state->hasShadeMode = true;
		break;
	case D3DRS_ZWRITEENABLE:
		constants->zWriteEnable = Value;
		state->hasZWriteEnable = true;
		break;
	case D3DRS_ALPHATESTENABLE:
		constants->alphaTestEnable = Value;
		state->hasAlphaTestEnable = true;
		break;
	case D3DRS_LASTPIXEL:
		constants->lastPixel = Value;
		state->hasLastPixel = true;
		break;
	case D3DRS_SRCBLEND:
		constants->sourceBlend = Value;
		state->hasSourceBlend = true;
		break;
	case D3DRS_DESTBLEND:
		constants->destinationBlend = Value;
		state->hasDestinationBlend = true;
		break;
	case D3DRS_CULLMODE:
		constants->cullMode = Value;
		state->hasCullMode = true;
		break;
	case D3DRS_ZFUNC:
		constants->zFunction = Value;
		state->hasZFunction = true;
		break;
	case D3DRS_ALPHAREF:
		constants->alphaReference = Value;
		state->hasAlphaReference = true;
		break;
	case D3DRS_ALPHAFUNC:
		constants->alphaFunction = Value;
		state->hasAlphaFunction = true;
		break;
	case D3DRS_DITHERENABLE:
		constants->ditherEnable = Value;
		state->hasDitherEnable = true;
		break;
	case D3DRS_ALPHABLENDENABLE:
		constants->alphaBlendEnable = Value;
		state->hasAlphaBlendEnable = true;
		break;
	case D3DRS_FOGENABLE:
		constants->fogEnable = Value;
		state->hasFogEnable = true;
		break;
	case D3DRS_SPECULARENABLE:
		constants->specularEnable = Value;
		state->hasSpecularEnable = true;
		break;
	case D3DRS_FOGCOLOR:
		constants->fogColor = Value;
		state->hasFogColor = true;
		break;
	case D3DRS_FOGTABLEMODE:
		constants->fogTableMode = Value;
		state->hasFogTableMode = true;
		break;
	case D3DRS_FOGSTART:
		constants->fogStart = bit_cast(Value);
		state->hasFogStart = true;
		break;
	case D3DRS_FOGEND:
		constants->fogEnd = bit_cast(Value);
		state->hasFogEnd = true;
		break;
	case D3DRS_FOGDENSITY:
		constants->fogDensity = bit_cast(Value);
		state->hasFogDensity = true;
		break;
	case D3DRS_RANGEFOGENABLE:
		constants->rangeFogEnable = Value;
		state->hasRangeFogEnable = true;
		break;
	case D3DRS_STENCILENABLE:
		constants->stencilEnable = Value;
		state->hasStencilEnable = true;
		break;
	case D3DRS_STENCILFAIL:
		constants->stencilFail = Value;
		state->hasStencilFail = true;
		break;
	case D3DRS_STENCILZFAIL:
		constants->stencilZFail = Value;
		state->hasStencilZFail = true;
		break;
	case D3DRS_STENCILPASS:
		constants->stencilPass = Value;
		state->hasStencilPass = true;
		break;
	case D3DRS_STENCILFUNC:
		constants->stencilFunction = Value;
		state->hasStencilFunction = true;
		break;
	case D3DRS_STENCILREF:
		constants->stencilReference = Value;
		state->hasStencilReference = true;
		break;
	case D3DRS_STENCILMASK:
		constants->stencilMask = Value;
		state->hasStencilMask = true;
		break;
	case D3DRS_STENCILWRITEMASK:
		constants->stencilWriteMask = Value;
		state->hasStencilWriteMask = true;
		break;
	case D3DRS_TEXTUREFACTOR:
		constants->textureFactor = Value;
		state->hasTextureFactor = true;
		break;
	case D3DRS_WRAP0:
		constants->wrap0 = Value;
		state->hasWrap0 = true;
		break;
	case D3DRS_WRAP1:
		constants->wrap1 = Value;
		state->hasWrap1 = true;
		break;
	case D3DRS_WRAP2:
		constants->wrap2 = Value;
		state->hasWrap2 = true;
		break;
	case D3DRS_WRAP3:
		constants->wrap3 = Value;
		state->hasWrap3 = true;
		break;
	case D3DRS_WRAP4:
		constants->wrap4 = Value;
		state->hasWrap4 = true;
		break;
	case D3DRS_WRAP5:
		constants->wrap5 = Value;
		state->hasWrap5 = true;
		break;
	case D3DRS_WRAP6:
		constants->wrap6 = Value;
		state->hasWrap6 = true;
		break;
	case D3DRS_WRAP7:
		constants->wrap7 = Value;
		state->hasWrap7 = true;
		break;
	case D3DRS_CLIPPING:
		constants->clipping = Value;
		state->hasClipping = true;
		break;
	case D3DRS_LIGHTING:
		constants->lighting = Value;
		state->hasLighting = true;
		break;
	case D3DRS_AMBIENT:
		constants->ambient = Value;
		state->hasAmbient = true;
		break;
	case D3DRS_FOGVERTEXMODE:
		constants->fogVertexMode = Value;
		state->hasFogVertexMode = true;
		break;
	case D3DRS_COLORVERTEX:
		constants->colorVertex = Value;
		state->hasColorVertex = true;
		break;
	case D3DRS_LOCALVIEWER:
		constants->localViewer = Value;
		state->hasLocalViewer = true;
		break;
	case D3DRS_NORMALIZENORMALS:
		constants->normalizeNormals = Value;
		state->hasNormalizeNormals = true;
		break;
	case D3DRS_DIFFUSEMATERIALSOURCE:
		constants->diffuseMaterialSource = Value;
		state->hasDiffuseMaterialSource = true;
		break;
	case D3DRS_SPECULARMATERIALSOURCE:
		constants->specularMaterialSource = Value;
		state->hasSpecularMaterialSource = true;
		break;
	case D3DRS_AMBIENTMATERIALSOURCE:
		constants->ambientMaterialSource = Value;
		state->hasAmbientMaterialSource = true;
		break;
	case D3DRS_EMISSIVEMATERIALSOURCE:
		constants->emissiveMaterialSource = Value;
		state->hasEmissiveMaterialSource = true;
		break;
	case D3DRS_VERTEXBLEND:
		constants->vertexBlend = Value;
		state->hasVertexBlend = true;
		break;
	case D3DRS_CLIPPLANEENABLE:
		constants->clipPlaneEnable = Value;
		state->hasClipPlaneEnable = true;
		break;
	case D3DRS_POINTSIZE:
		constants->pointSize = Value;
		state->hasPointSize = true;
		break;
	case D3DRS_POINTSIZE_MIN:
		constants->pointSizeMinimum = bit_cast(Value);
		state->hasPointSizeMinimum = true;
		break;
	case D3DRS_POINTSPRITEENABLE:
		constants->pointSpriteEnable = Value;
		state->hasPointSpriteEnable = true;
		break;
	case D3DRS_POINTSCALEENABLE:
		constants->pointScaleEnable = Value;
		state->hasPointScaleEnable = true;
		break;
	case D3DRS_POINTSCALE_A:
		constants->pointScaleA = bit_cast(Value);
		state->hasPointScaleA = true;
		break;
	case D3DRS_POINTSCALE_B:
		constants->pointScaleB = bit_cast(Value);
		state->hasPointScaleB = true;
		break;
	case D3DRS_POINTSCALE_C:
		constants->pointScaleC = bit_cast(Value);
		state->hasPointScaleC = true;
		break;
	case D3DRS_MULTISAMPLEANTIALIAS:
		constants->multisampleAntiAlias = Value;
		state->hasMultisampleAntiAlias = true;
		break;
	case D3DRS_MULTISAMPLEMASK:
		constants->multisampleMask = Value;
		state->hasMultisampleMask = true;
		break;
	case D3DRS_PATCHEDGESTYLE:
		constants->patchEdgeStyle = Value;
		state->hasPatchEdgeStyle = true;
		break;
	case D3DRS_DEBUGMONITORTOKEN:
		constants->debugMonitorToken = Value;
		state->hasDebugMonitorToken = true;
		break;
	case D3DRS_POINTSIZE_MAX:
		constants->pointSizeMaximum = bit_cast(Value);
		state->hasPointSizeMaximum = true;
		break;
	case D3DRS_INDEXEDVERTEXBLENDENABLE:
		constants->indexedVertexBlendEnable = Value;
		state->hasIndexedVertexBlendEnable = true;
		break;
	case D3DRS_COLORWRITEENABLE:
		constants->colorWriteEnable = Value;
		state->hasColorWriteEnable = true;
		break;
	case D3DRS_TWEENFACTOR:
		constants->tweenFactor = bit_cast(Value);
		state->hasTweenFactor = true;
		break;
	case D3DRS_BLENDOP:
		constants->blendOperation = Value;
		state->hasBlendOperation = true;
		break;
	case D3DRS_POSITIONDEGREE:
		constants->positionDegree = Value;
		state->hasPositionDegree = true;
		break;
	case D3DRS_NORMALDEGREE:
		constants->normalDegree = Value;
		state->hasNormalDegree = true;
		break;
	case D3DRS_SCISSORTESTENABLE:
		constants->scissorTestEnable = Value;
		state->hasScissorTestEnable = true;
		break;
	case D3DRS_SLOPESCALEDEPTHBIAS:
		constants->slopeScaleDepthBias = bit_cast(Value);
		state->hasSlopeScaleDepthBias = true;
		break;
	case D3DRS_ANTIALIASEDLINEENABLE:
		constants->antiAliasedLineEnable = Value;
		state->hasAntiAliasedLineEnable = true;
		break;
	case D3DRS_MINTESSELLATIONLEVEL:
		constants->minimumTessellationLevel = bit_cast(Value);
		state->hasMinimumTessellationLevel = true;
		break;
	case D3DRS_MAXTESSELLATIONLEVEL:
		constants->maximumTessellationLevel = bit_cast(Value);
		state->hasMaximumTessellationLevel = true;
		break;
	case D3DRS_ADAPTIVETESS_X:
		constants->adaptivetessX = bit_cast(Value);
		state->hasAdaptivetessX = true;
		break;
	case D3DRS_ADAPTIVETESS_Y:
		constants->adaptivetessY = bit_cast(Value);
		state->hasAdaptivetessY = true;
		break;
	case D3DRS_ADAPTIVETESS_Z:
		constants->adaptivetessZ = bit_cast(Value);
		state->hasAdaptivetessZ = true;
		break;
	case D3DRS_ADAPTIVETESS_W:
		constants->adaptivetessW = bit_cast(Value);
		state->hasAdaptivetessW = true;
		break;
	case D3DRS_ENABLEADAPTIVETESSELLATION:
		constants->enableAdaptiveTessellation = Value;
		state->hasEnableAdaptiveTessellation = true;
		break;
	case D3DRS_TWOSIDEDSTENCILMODE:
		constants->twoSidedStencilMode = Value;
		state->hasTwoSidedStencilMode = true;
		break;
	case D3DRS_CCW_STENCILFAIL:
		constants->ccwStencilFail = Value;
		state->hasCcwStencilFail = true;
		break;
	case D3DRS_CCW_STENCILZFAIL:
		constants->ccwStencilZFail = Value;
		state->hasCcwStencilZFail = true;
		break;
	case D3DRS_CCW_STENCILPASS:
		constants->ccwStencilPass = Value;
		state->hasCcwStencilPass = true;
		break;
	case D3DRS_CCW_STENCILFUNC:
		constants->ccwStencilFunction = Value;
		state->hasCcwStencilFunction = true;
		break;
	case D3DRS_COLORWRITEENABLE1:
		constants->colorWriteEnable1 = Value;
		state->hasColorWriteEnable1 = true;
		break;
	case D3DRS_COLORWRITEENABLE2:
		constants->colorWriteEnable2 = Value;
		state->hasColorWriteEnable2 = true;
		break;
	case D3DRS_COLORWRITEENABLE3:
		constants->colorWriteEnable3 = Value;
		state->hasColorWriteEnable3 = true;
		break;
	case D3DRS_BLENDFACTOR:
		constants->blendFactor = Value;
		state->hasBlendFactor = true;
		break;
	case D3DRS_SRGBWRITEENABLE:
		constants->srgbWriteEnable = Value;
		state->hasSrgbWriteEnable = true;
		break;
	case D3DRS_DEPTHBIAS:
		constants->depthBias = bit_cast(Value);
		state->hasDepthBias = true;
		break;
	case D3DRS_WRAP8:
		constants->wrap8 = Value;
		state->hasWrap8 = true;
		break;
	case D3DRS_WRAP9:
		constants->wrap9 = Value;
		state->hasWrap9 = true;
		break;
	case D3DRS_WRAP10:
		constants->wrap10 = Value;
		state->hasWrap10 = true;
		break;
	case D3DRS_WRAP11:
		constants->wrap11 = Value;
		state->hasWrap11 = true;
		break;
	case D3DRS_WRAP12:
		constants->wrap12 = Value;
		state->hasWrap12 = true;
		break;
	case D3DRS_WRAP13:
		constants->wrap13 = Value;
		state->hasWrap13 = true;
		break;
	case D3DRS_WRAP14:
		constants->wrap14 = Value;
		state->hasWrap14 = true;
		break;
	case D3DRS_WRAP15:
		constants->wrap15 = Value;
		state->hasWrap15 = true;
		break;
	case D3DRS_SEPARATEALPHABLENDENABLE:
		constants->separateAlphaBlendEnable = Value;
		state->hasSeparateAlphaBlendEnable = true;
		break;
	case D3DRS_SRCBLENDALPHA:
		constants->sourceBlendAlpha = Value;
		state->hasSourceBlendAlpha = true;
		break;
	case D3DRS_DESTBLENDALPHA:
		constants->destinationBlendAlpha = Value;
		state->hasDestinationBlendAlpha = true;
		break;
	case D3DRS_BLENDOPALPHA:
		constants->blendOperationAlpha = Value;
		state->hasBlendOperationAlpha = true;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetRenderState unknown state! " << State;
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget)
{
	mRenderTargets[RenderTargetIndex] = (CRenderTargetSurface9*)pRenderTarget;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	DeviceState* state = NULL;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	state->mSamplerStates[Sampler][Type] = Value;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT *pRect)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.m9Scissor = (*pRect);

		this->mCurrentStateRecording->mDeviceState.mScissor.extent.width = this->mCurrentStateRecording->mDeviceState.m9Scissor.right;
		this->mCurrentStateRecording->mDeviceState.mScissor.extent.height = this->mCurrentStateRecording->mDeviceState.m9Scissor.bottom;
		this->mCurrentStateRecording->mDeviceState.mScissor.offset.x = this->mCurrentStateRecording->mDeviceState.m9Scissor.left;
		this->mCurrentStateRecording->mDeviceState.mScissor.offset.y = this->mCurrentStateRecording->mDeviceState.m9Scissor.top;
	}
	else
	{
		mDeviceState.m9Scissor = (*pRect);

		mDeviceState.mScissor.extent.width = mDeviceState.m9Scissor.right;
		mDeviceState.mScissor.extent.height = mDeviceState.m9Scissor.bottom;
		mDeviceState.mScissor.offset.x = mDeviceState.m9Scissor.left;
		mDeviceState.mScissor.offset.y = mDeviceState.m9Scissor.top;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetSoftwareVertexProcessing is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride)
{
	CVertexBuffer9* streamData = (CVertexBuffer9*)pStreamData;

	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
	}
	else
	{
		mDeviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::SetStreamSourceFreq is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler, IDirect3DBaseTexture9 *pTexture)
{
	DeviceState* state = NULL;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	if (pTexture == nullptr)
	{
		auto it = state->mTextures.find(Sampler);
		if (it != state->mTextures.end())
		{
			state->mTextures.erase(it);
		}
	}
	else
	{
		state->mTextures[Sampler] = pTexture;
		//texture->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	DeviceState* state = nullptr;

	if (this->mCurrentStateRecording != nullptr)
	{
		state = &this->mCurrentStateRecording->mDeviceState;
	}
	else
	{
		state = &mDeviceState;
	}

	switch (Type)
	{
	case D3DTSS_COLOROP:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorOperation_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorOperation_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorOperation_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorOperation_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorOperation_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorOperation_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorOperation_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorOperation_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG1:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorArgument1_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorArgument1_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorArgument1_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorArgument1_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorArgument1_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorArgument1_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorArgument1_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorArgument1_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG2:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorArgument2_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorArgument2_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorArgument2_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorArgument2_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorArgument2_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorArgument2_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorArgument2_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorArgument2_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAOP:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaOperation_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaOperation_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaOperation_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaOperation_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaOperation_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaOperation_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaOperation_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaOperation_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG1:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaArgument1_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaArgument1_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaArgument1_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaArgument1_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaArgument1_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaArgument1_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaArgument1_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaArgument1_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG2:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaArgument2_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaArgument2_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaArgument2_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaArgument2_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaArgument2_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaArgument2_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaArgument2_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaArgument2_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT00:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix00_0 = bit_cast(Value);
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix00_1 = bit_cast(Value);
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix00_2 = bit_cast(Value);
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix00_3 = bit_cast(Value);
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix00_4 = bit_cast(Value);
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix00_5 = bit_cast(Value);
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix00_6 = bit_cast(Value);
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix00_7 = bit_cast(Value);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT01:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix01_0 = bit_cast(Value);
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix01_1 = bit_cast(Value);
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix01_2 = bit_cast(Value);
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix01_3 = bit_cast(Value);
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix01_4 = bit_cast(Value);
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix01_5 = bit_cast(Value);
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix01_6 = bit_cast(Value);
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix01_7 = bit_cast(Value);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT10:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix10_0 = bit_cast(Value);
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix10_1 = bit_cast(Value);
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix10_2 = bit_cast(Value);
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix10_3 = bit_cast(Value);
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix10_4 = bit_cast(Value);
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix10_5 = bit_cast(Value);
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix10_6 = bit_cast(Value);
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix10_7 = bit_cast(Value);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVMAT11:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapMatrix11_0 = bit_cast(Value);
			break;
		case 1:
			state->mSpecializationConstants.bumpMapMatrix11_1 = bit_cast(Value);
			break;
		case 2:
			state->mSpecializationConstants.bumpMapMatrix11_2 = bit_cast(Value);
			break;
		case 3:
			state->mSpecializationConstants.bumpMapMatrix11_3 = bit_cast(Value);
			break;
		case 4:
			state->mSpecializationConstants.bumpMapMatrix11_4 = bit_cast(Value);
			break;
		case 5:
			state->mSpecializationConstants.bumpMapMatrix11_5 = bit_cast(Value);
			break;
		case 6:
			state->mSpecializationConstants.bumpMapMatrix11_6 = bit_cast(Value);
			break;
		case 7:
			state->mSpecializationConstants.bumpMapMatrix11_7 = bit_cast(Value);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXCOORDINDEX:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.texureCoordinateIndex_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.texureCoordinateIndex_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.texureCoordinateIndex_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.texureCoordinateIndex_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.texureCoordinateIndex_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.texureCoordinateIndex_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.texureCoordinateIndex_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.texureCoordinateIndex_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLSCALE:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapScale_0 = bit_cast(Value);
			break;
		case 1:
			state->mSpecializationConstants.bumpMapScale_1 = bit_cast(Value);
			break;
		case 2:
			state->mSpecializationConstants.bumpMapScale_2 = bit_cast(Value);
			break;
		case 3:
			state->mSpecializationConstants.bumpMapScale_3 = bit_cast(Value);
			break;
		case 4:
			state->mSpecializationConstants.bumpMapScale_4 = bit_cast(Value);
			break;
		case 5:
			state->mSpecializationConstants.bumpMapScale_5 = bit_cast(Value);
			break;
		case 6:
			state->mSpecializationConstants.bumpMapScale_6 = bit_cast(Value);
			break;
		case 7:
			state->mSpecializationConstants.bumpMapScale_7 = bit_cast(Value);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_BUMPENVLOFFSET:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.bumpMapOffset_0 = bit_cast(Value);
			break;
		case 1:
			state->mSpecializationConstants.bumpMapOffset_1 = bit_cast(Value);
			break;
		case 2:
			state->mSpecializationConstants.bumpMapOffset_2 = bit_cast(Value);
			break;
		case 3:
			state->mSpecializationConstants.bumpMapOffset_3 = bit_cast(Value);
			break;
		case 4:
			state->mSpecializationConstants.bumpMapOffset_4 = bit_cast(Value);
			break;
		case 5:
			state->mSpecializationConstants.bumpMapOffset_5 = bit_cast(Value);
			break;
		case 6:
			state->mSpecializationConstants.bumpMapOffset_6 = bit_cast(Value);
			break;
		case 7:
			state->mSpecializationConstants.bumpMapOffset_7 = bit_cast(Value);
			break;
		default:
			break;
		}
		break;
	case D3DTSS_TEXTURETRANSFORMFLAGS:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.textureTransformationFlags_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.textureTransformationFlags_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.textureTransformationFlags_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.textureTransformationFlags_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.textureTransformationFlags_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.textureTransformationFlags_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.textureTransformationFlags_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.textureTransformationFlags_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_COLORARG0:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.colorArgument0_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.colorArgument0_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.colorArgument0_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.colorArgument0_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.colorArgument0_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.colorArgument0_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.colorArgument0_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.colorArgument0_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_ALPHAARG0:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.alphaArgument0_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.alphaArgument0_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.alphaArgument0_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.alphaArgument0_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.alphaArgument0_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.alphaArgument0_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.alphaArgument0_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.alphaArgument0_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_RESULTARG:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.Result_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.Result_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.Result_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.Result_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.Result_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.Result_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.Result_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.Result_7 = Value;
			break;
		default:
			break;
		}
		break;
	case D3DTSS_CONSTANT:
		switch (Stage)
		{
		case 0:
			state->mSpecializationConstants.Constant_0 = Value;
			break;
		case 1:
			state->mSpecializationConstants.Constant_1 = Value;
			break;
		case 2:
			state->mSpecializationConstants.Constant_2 = Value;
			break;
		case 3:
			state->mSpecializationConstants.Constant_3 = Value;
			break;
		case 4:
			state->mSpecializationConstants.Constant_4 = Value;
			break;
		case 5:
			state->mSpecializationConstants.Constant_5 = Value;
			break;
		case 6:
			state->mSpecializationConstants.Constant_6 = Value;
			break;
		case 7:
			state->mSpecializationConstants.Constant_7 = Value;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mTransforms[State] = (*pMatrix);
		this->mCurrentStateRecording->mDeviceState.mHasTransformsChanged = true;
	}
	else
	{
		mDeviceState.mTransforms[State] = (*pMatrix);
		mDeviceState.mHasTransformsChanged = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;

		this->mCurrentStateRecording->mDeviceState.mHasVertexDeclaration = true;
		this->mCurrentStateRecording->mDeviceState.mHasFVF = false;
	}
	else
	{
		mDeviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;

		mDeviceState.mHasVertexDeclaration = true;
		mDeviceState.mHasFVF = false;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
	if (pShader != nullptr)
	{
		pShader->AddRef();
	}

	if (this->mCurrentStateRecording != nullptr)
	{
		//BOOST_LOG_TRIVIAL(info) << "Recorded VertexShader";
		this->mCurrentStateRecording->mDeviceState.mVertexShader = (CVertexShader9*)pShader;
		this->mCurrentStateRecording->mDeviceState.mHasVertexShader = true;
	}
	else
	{
		if (mDeviceState.mVertexShader != nullptr)
		{
			mDeviceState.mVertexShader->Release();
		}

		mDeviceState.mVertexShader = (CVertexShader9*)pShader;
		mDeviceState.mHasVertexShader = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister, const BOOL *pConstantData, UINT BoolCount)
{
	auto& slots = mDeviceState.mVertexShaderConstantSlots;
	for (size_t i = 0; i < BoolCount; i++)
	{
		slots.BooleanConstants[StartRegister + i] = pConstantData[i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{
	auto& slots = mDeviceState.mVertexShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4fCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		if ((startIndex + i) < 128)
		{
			mDeviceState.mPushConstants[startIndex + i] = pConstantData[i];
		}
		else
		{
			slots.FloatConstants[startIndex + i] = pConstantData[i];
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister, const int *pConstantData, UINT Vector4iCount)
{
	auto& slots = mDeviceState.mVertexShaderConstantSlots;
	uint32_t startIndex = (StartRegister * 4);
	uint32_t length = (Vector4iCount * 4);
	for (size_t i = 0; i < length; i++)
	{
		slots.IntegerConstants[startIndex + i] = pConstantData[i];
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{
	if (this->mCurrentStateRecording != nullptr)
	{
		this->mCurrentStateRecording->mDeviceState.m9Viewport = (*pViewport);

		//this->mCurrentStateRecording->mDeviceState.mViewport.y = (float)mDeviceState.m9Viewport.Height;
		this->mCurrentStateRecording->mDeviceState.mViewport.width = (float)mDeviceState.m9Viewport.Width;
		//this->mCurrentStateRecording->mDeviceState.mViewport.height = -(float)mDeviceState.m9Viewport.Height;
		this->mCurrentStateRecording->mDeviceState.mViewport.height = (float)mDeviceState.m9Viewport.Height;
		this->mCurrentStateRecording->mDeviceState.mViewport.minDepth = mDeviceState.m9Viewport.MinZ;
		this->mCurrentStateRecording->mDeviceState.mViewport.maxDepth = mDeviceState.m9Viewport.MaxZ;
	}
	else
	{
		mDeviceState.m9Viewport = (*pViewport);

		mDeviceState.mViewport.y = (float)mDeviceState.m9Viewport.Height;
		mDeviceState.mViewport.width = (float)mDeviceState.m9Viewport.Width;
		mDeviceState.mViewport.height = -(float)mDeviceState.m9Viewport.Height;
		mDeviceState.mViewport.minDepth = mDeviceState.m9Viewport.MinZ;
		mDeviceState.mViewport.maxDepth = mDeviceState.m9Viewport.MaxZ;
	}

	return S_OK;
}

BOOL STDMETHODCALLTYPE CDevice9::ShowCursor(BOOL bShow)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ShowCursor is not implemented!";

	return TRUE;
}

HRESULT STDMETHODCALLTYPE CDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect, IDirect3DSurface9 *pDestSurface, const RECT *pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::StretchRect is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::TestCooperativeLevel()
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::TestCooperativeLevel is not implemented!";

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect, IDirect3DSurface9 *pDestinationSurface, const POINT *pDestinationPoint)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::UpdateSurface is not implemented!";

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	if (pSourceTexture == nullptr || pDestinationTexture == nullptr)
	{
		return D3DERR_INVALIDCALL;
	}

	VkCommandBuffer commandBuffer;

	VkCommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = mCommandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	mResult = vkAllocateCommandBuffers(mDevice, &commandBufferInfo, &commandBuffer);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::UpdateTexture vkAllocateCommandBuffers failed with return code of " << GetResultString(mResult);
		return D3DERR_INVALIDCALL;
	}

	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	commandBufferInheritanceInfo.queryFlags = 0;
	commandBufferInheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	mResult = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::UpdateTexture vkBeginCommandBuffer failed with return code of " << GetResultString(mResult);
		return D3DERR_INVALIDCALL;
	}

	//TODO: Handle dirty regions and multiple mip levels.

	if (pSourceTexture->GetType() != D3DRTYPE_CUBETEXTURE)
	{
		CTexture9& source = (*(CTexture9*)pSourceTexture);
		CTexture9& target = (*(CTexture9*)pDestinationTexture);

		ReallySetImageLayout(commandBuffer, source.mImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 0, 1);
		ReallySetImageLayout(commandBuffer, target.mImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 0, 1);
		ReallyCopyImage(commandBuffer, source.mImage, target.mImage, 0, 0, source.mWidth, source.mHeight, 0, 0, 0, 0);
		ReallySetImageLayout(commandBuffer, target.mImage, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 0, 1);
	}
	else
	{
		CCubeTexture9& source = (*(CCubeTexture9*)pSourceTexture);
		CCubeTexture9& target = (*(CCubeTexture9*)pDestinationTexture);

		ReallySetImageLayout(commandBuffer, source.mImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 0, 6);
		ReallySetImageLayout(commandBuffer, target.mImage, 0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 0, 6);
		ReallyCopyImage(commandBuffer, source.mImage, target.mImage, 0, 0, source.mEdgeLength, source.mEdgeLength, 0, 0, 0, 0);
		ReallySetImageLayout(commandBuffer, target.mImage, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 0, 6);
	}

	mResult = vkEndCommandBuffer(commandBuffer);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::UpdateTexture vkEndCommandBuffer failed with return code of " << GetResultString(mResult);
		return D3DERR_INVALIDCALL;
	}

	VkCommandBuffer commandBuffers[] = { commandBuffer };
	VkFence nullFence = VK_NULL_HANDLE;
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	mResult = vkQueueSubmit(mQueue, 1, &submitInfo, nullFence);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::UpdateTexture vkQueueSubmit failed with return code of " << GetResultString(mResult);
		return D3DERR_INVALIDCALL;
	}

	mResult = vkQueueWaitIdle(mQueue);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CDevice9::UpdateTexture vkQueueWaitIdle failed with return code of " << GetResultString(mResult);
		return D3DERR_INVALIDCALL;
	}

	vkFreeCommandBuffers(mDevice, mCommandPool, 1, commandBuffers);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::ValidateDevice(DWORD *pNumPasses)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "CDevice9::ValidateDevice is not implemented!";

	return S_OK;
}

void CDevice9::CopyImage(VkImage srcImage, VkImage dstImage, int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t srcMip, uint32_t dstMip)
{
	VkResult result = VK_SUCCESS;
	VkCommandBuffer commandBuffer;

	VkCommandBufferAllocateInfo commandBufferInfo = {};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.commandPool = mCommandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(mDevice, &commandBufferInfo, &commandBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkAllocateCommandBuffers failed with return code of " << GetResultString(result);
		return;
	}

	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	commandBufferInheritanceInfo.queryFlags = 0;
	commandBufferInheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkBeginCommandBuffer failed with return code of " << GetResultString(result);
		return;
	}

	ReallyCopyImage(commandBuffer, srcImage, dstImage, x, y, width, height, srcMip, dstMip, 0,0);

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkEndCommandBuffer failed with return code of " << GetResultString(result);
		return;
	}

	VkCommandBuffer commandBuffers[] = { commandBuffer };
	VkFence nullFence = VK_NULL_HANDLE;
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	result = vkQueueSubmit(mQueue, 1, &submitInfo, nullFence);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkQueueSubmit failed with return code of " << GetResultString(result);
		return;
	}

	result = vkQueueWaitIdle(mQueue);
	if (result != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "CTexture9::CopyImage vkQueueWaitIdle failed with return code of " << GetResultString(result);
		return;
	}

	vkFreeCommandBuffers(mDevice, mCommandPool, 1, commandBuffers);
}