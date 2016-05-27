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

#include "C9.h"
#include "CDevice9.h"
#include "CCubeTexture9.h"
#include "CBaseTexture9.h"
#include "CTexture9.h"
#include "CVolumeTexture9.h"
#include "CSurface9.h"
#include "CVertexDeclaration9.h"
#include "CVertexShader9.h"

#include "Utilities.h"

CDevice9::CDevice9(C9* Instance, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters)
	: mInstance(Instance),
	mAdapter(Adapter),
	mDeviceType(DeviceType),
	mFocusWindow(hFocusWindow),
	mBehaviorFlags(BehaviorFlags),
	mPresentationParameters(pPresentationParameters),
	mQueueCount(0),
	mReferenceCount(0),
	mDisplays(NULL),
	mDisplayCount(0)
{
	mPhysicalDevice = mInstance->mPhysicalDevices[mAdapter]; //pull the selected physical device from the instance.

	//Fetch the queue properties.
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &mQueueCount,NULL);
	mQueueFamilyProperties = new VkQueueFamilyProperties[mQueueCount];
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &mQueueCount,mQueueFamilyProperties);

	/*bool found = false;
	for (unsigned int i = 0; i < mQueueCount; i++) 
	{
		if (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			queue_info.queueFamilyIndex = i;
			found = true;
			break;
		}
	}*/

	float queue_priorities[1] = { 0.0 };
	VkDeviceQueueCreateInfo queue_info = {};	
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = NULL;
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = queue_priorities;

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = NULL;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledExtensionCount = 0;
	device_info.ppEnabledExtensionNames = NULL;
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = NULL;
	device_info.pEnabledFeatures = NULL;

	vkCreateDevice(mPhysicalDevice, &device_info, NULL, &mDevice);

	/*
	Now that the rendering is setup the surface must be created.
	The surface maybe inside of a window or a whole display. (Think SDL)
	*/
	if (!mPresentationParameters->Windowed)
	{
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};

		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = NULL;
		surfaceCreateInfo.flags = 0;
		surfaceCreateInfo.hwnd = hFocusWindow;
		surfaceCreateInfo.hinstance = GetModuleHandle(NULL);

		vkCreateWin32SurfaceKHR(mInstance->mInstance, &surfaceCreateInfo, NULL, &mSurface);
	}
	else
	{
		//TODO: finish full screen support.
		/*vkGetDisplayPlaneSupportedDisplaysKHR(mPhysicalDevice, 0, &mDisplayCount, NULL);
		mDisplays = new VkDisplayKHR[mDisplayCount];
		vkGetDisplayPlaneSupportedDisplaysKHR(mPhysicalDevice, 0, &mDisplayCount, mDisplays);

		//vkGetDisplayModePropertiesKHR(mPhysicalDevice,mDisplays[0])

		VkDisplaySurfaceCreateInfoKHR surfaceCreateInfo = {};

		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = NULL;
		surfaceCreateInfo.flags = 0;



		vkCreateDisplayPlaneSurfaceKHR(mInstance->mInstance,&surfaceCreateInfo,NULL, &mSurface);*/
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &mSurfaceCapabilities);

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = NULL;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = mSurface;
	swapchainCreateInfo.imageFormat = ConvertFormat(mPresentationParameters->BackBufferFormat);

	//TODO: finish swapchain create info including format information.

	vkCreateSwapchainKHR(mDevice, &swapchainCreateInfo, NULL, &mSwapchain);
	

	
}

CDevice9::~CDevice9()
{
	if (mDisplays!=NULL)
	{
		delete[] mDisplays;
	}
	vkDestroySurfaceKHR(mInstance->mInstance, mSurface, NULL);
	vkDestroyDevice(mDevice, NULL);
}

//IUknown

ULONG STDMETHODCALLTYPE CDevice9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE CDevice9::QueryInterface(REFIID riid,void  **ppv)
{
	//TODO: Implement.

	return S_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
}

//Device

HRESULT STDMETHODCALLTYPE CDevice9::BeginScene()
{
	//TODO: Implement.

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::BeginStateBlock()
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	IDirect3DCubeTexture9* obj = new CCubeTexture9(this, EdgeLength,Levels,Usage,Format,Pool,pSharedHandle);

	(*ppCubeTexture) = (IDirect3DCubeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	IDirect3DSurface9* obj = new CSurface9(this,Width,Height,Format,MultiSample,MultisampleQuality,Discard,pSharedHandle);

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DeletePatch(UINT Handle)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndScene()
{
	//TODO: Implement.

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EvictManagedResources()
{
	//TODO: Implement.

	return S_OK;	
}

UINT STDMETHODCALLTYPE CDevice9::GetAvailableTextureMem()
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index,float *pPlane)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	(*ppD3D9) = (IDirect3D9*)mInstance;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{
	//TODO: Implement.

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData) //,UINT *pBaseVertexIndex ?
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index,D3DLIGHT9 *pLight)
{
	//TODO: Implement.

	return E_NOTIMPL;
} 

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index,BOOL *pEnable)
{
	//TODO: Implement.

	return S_OK;		
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	//TODO: Implement.

	return 0;
}

UINT STDMETHODCALLTYPE CDevice9::GetNumberOfSwapChains()
{
	//TODO: Implement.

	return 0;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT *pRect)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

BOOL STDMETHODCALLTYPE CDevice9::GetSoftwareVertexProcessing()
{
	//TODO: Implement.

	return true;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	//TODO: Implement.

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex,BOOL bEnable)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{	
	//TODO: Implement.

	return S_OK;		
}

HRESULT CDevice9::SetClipPlane(DWORD Index,const float *pPlane)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CDevice9::SetCursorPosition(INT X,INT Y,DWORD Flags)
{
	//TODO: Implement.

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetFVF(DWORD FVF)
{
	//TODO: Implement.

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp)
{
	//TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index,const D3DLIGHT9 *pLight)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT *pRect)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride)
{
	//TODO: Implement.
		
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	//TODO: Implement.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{
	//TODO: Implement.

	return S_OK;	
}

BOOL STDMETHODCALLTYPE CDevice9::ShowCursor(BOOL bShow)
{
	//TODO: Implement.

	return TRUE;	
}

HRESULT STDMETHODCALLTYPE CDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::TestCooperativeLevel()
{
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
{
	//TODO: Implement.

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ValidateDevice(DWORD *pNumPasses)
{
	//TODO: Implement.

	return S_OK;	
}