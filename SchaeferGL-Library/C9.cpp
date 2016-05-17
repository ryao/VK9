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

#define APP_SHORT_NAME "SchaeferGL"

C9::C9()
	: mGpuCount(0), 
	mPhysicalDevices(NULL),
	mReferenceCount(0)
{
	const char *extensionNames[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };

	// initialize the VkApplicationInfo structure
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = APP_SHORT_NAME;
	app_info.applicationVersion = 1;
	app_info.pEngineName = APP_SHORT_NAME;
	app_info.engineVersion = 1;
	app_info.apiVersion = VK_API_VERSION;

	// initialize the VkInstanceCreateInfo structure
	VkInstanceCreateInfo inst_info =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
		NULL,                                   // const void* pNext;

		0,                                      // VkInstanceCreateFlags flags;

		&app_info,                              // const VkApplicationInfo* pApplicationInfo;

		0,                                      // uint32_t enabledLayerNameCount;
		NULL,                                   // const char* const* ppEnabledLayerNames;

		2,                                      // uint32_t enabledExtensionNameCount;
		extensionNames,                         // const char* const* ppEnabledExtensionNames;
	};

	//Get an instance handle.
	if (VK_SUCCESS == vkCreateInstance(&inst_info, NULL, &mInstance))
	{
		//Fetch an array of available physical devices.
		vkEnumeratePhysicalDevices(mInstance, &mGpuCount, NULL);
		if (mGpuCount > 0)
		{
			mPhysicalDevices = new VkPhysicalDevice[mGpuCount]();
			vkEnumeratePhysicalDevices(mInstance, &mGpuCount, mPhysicalDevices);
		}
		else
		{
			//TODO: no physical device, find a way to fail gracefully.
		}
	}
	else
	{
		//TODO: couldn't get instance, find a way to fail gracefully.
	}
}

C9::~C9()
{
	if (mPhysicalDevices!=NULL)
	{
		delete[]	 mPhysicalDevices;
	}

	vkDestroyInstance(mInstance, NULL);
}

//IUnknown

ULONG STDMETHODCALLTYPE C9::AddRef(void)
{
	mReferenceCount++;

	return mReferenceCount;
}

HRESULT STDMETHODCALLTYPE C9::QueryInterface(REFIID riid,void  **ppv)
{
	
	return S_OK;
}

ULONG STDMETHODCALLTYPE C9::Release(void)
{
	mReferenceCount--;

	if (mReferenceCount <= 0)
	{
		delete this;
	}

	return mReferenceCount;
}

//IDirect3D9

HRESULT STDMETHODCALLTYPE C9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	HRESULT result = D3DERR_NOTAVAILABLE;	// failure
	
	//TODO: Implement.

	return result;	
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	HRESULT result = S_OK;	// failure
	
	//TODO: Implement.
	
	return result;	
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	//TODO: Implement.

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels)
{
	//TODO: Implement.

	return D3DERR_NOTAVAILABLE;	
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)
{		
	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	HRESULT result = S_OK;

	CDevice9* device = new CDevice9(this,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters);

	(*ppReturnedDeviceInterface) = (IDirect3DDevice9*)device;

	return result;	
}

HRESULT STDMETHODCALLTYPE C9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode)
{
	//TODO: Implement.

	return S_OK;		
}

UINT STDMETHODCALLTYPE C9::GetAdapterCount()
{
	return mGpuCount;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode)
{
	//Wanted to use vkGetPhysicalDeviceDisplayPropertiesKHR but it looks like it's an extension that isn't available.

	//Fake it till you make it.
	pMode->Height = 1080;
	pMode->Width = 1920;
	pMode->RefreshRate = 60;
	pMode->Format = D3DFMT_UNKNOWN;

	//TODO: Implement.

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier)
{	
	//TODO: Implement.

	return S_OK;	
}

UINT STDMETHODCALLTYPE C9::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format)
{	
	//TODO: Implement.

	return 0;	
}


HMONITOR STDMETHODCALLTYPE C9::GetAdapterMonitor(UINT Adapter)
{
	//TODO: Implement.

	return 0; //TODO: implement GetAdapterMonitor
}


HRESULT STDMETHODCALLTYPE C9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	//TODO: Implement.

	//Fake it till you make it.
	pCaps->DeviceType = D3DDEVTYPE_FORCE_DWORD;
	pCaps->AdapterOrdinal=Adapter;
	pCaps->Caps=0;
	pCaps->Caps2=0;
	pCaps->Caps3=0;
	pCaps->PresentationIntervals=0;
	pCaps->CursorCaps=0;
	pCaps->DevCaps=0;
	pCaps->PrimitiveMiscCaps=0;
	pCaps->RasterCaps=0;
	pCaps->ZCmpCaps=0;
	pCaps->SrcBlendCaps=0;
	pCaps->DestBlendCaps=0;
	pCaps->AlphaCmpCaps=0;
	pCaps->ShadeCaps=0;
	pCaps->TextureCaps=0;
	pCaps->TextureFilterCaps=0;
	pCaps->CubeTextureFilterCaps=0;
	pCaps->VolumeTextureFilterCaps=0;
	pCaps->TextureAddressCaps=0;
	pCaps->VolumeTextureAddressCaps=0;
	pCaps->LineCaps=0;
	pCaps->MaxTextureWidth=0;
	pCaps->MaxTextureHeight=0;
	pCaps->MaxVolumeExtent=0;
	pCaps->MaxTextureRepeat=0;
	pCaps->MaxTextureAspectRatio=0;
	pCaps->MaxAnisotropy=0;
	pCaps->MaxVertexW=0;
	pCaps->GuardBandLeft=0;
	pCaps->GuardBandTop=0;
	pCaps->GuardBandRight=0;
	pCaps->GuardBandBottom=0;
	pCaps->ExtentsAdjust=0;
	pCaps->StencilCaps=0;
	pCaps->FVFCaps=0;
	pCaps->TextureOpCaps=0;
	pCaps->MaxTextureBlendStages=0;
	pCaps->MaxSimultaneousTextures=0;
	pCaps->VertexProcessingCaps=0;
	pCaps->MaxActiveLights=0;
	pCaps->MaxUserClipPlanes=0;
	pCaps->MaxVertexBlendMatrices=0;
	pCaps->MaxVertexBlendMatrixIndex=0;
	pCaps->MaxPointSize=0;
	pCaps->MaxPrimitiveCount=0;
	pCaps->MaxVertexIndex=0;
	pCaps->MaxStreams=0;
	pCaps->MaxStreamStride=0;
	pCaps->VertexShaderVersion=0;
	pCaps->MaxVertexShaderConst=0;
	pCaps->PixelShaderVersion=0;
	pCaps->PixelShader1xMaxValue=0;
	pCaps->DevCaps2=0;
	pCaps->MasterAdapterOrdinal=0;
	pCaps->AdapterOrdinalInGroup=0;
	pCaps->NumberOfAdaptersInGroup=0;
	pCaps->DeclTypes=0;
	pCaps->NumSimultaneousRTs=0;
	pCaps->StretchRectFilterCaps=0;
	//pCaps->VS20Caps=0;
	//pCaps->PS20Caps=0;
	pCaps->VertexTextureFilterCaps=0;
	pCaps->MaxVShaderInstructionsExecuted=0;
	pCaps->MaxPShaderInstructionsExecuted=0;
	pCaps->MaxVertexShader30InstructionSlots=0;
	pCaps->MaxPixelShader30InstructionSlots=0;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	//TODO: Implement.

	return E_NOTIMPL;
}