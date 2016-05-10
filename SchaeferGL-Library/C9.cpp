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
	: mGpuCount(0), mPhysicalDevices(NULL)
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

	vkCreateInstance(&inst_info, NULL, &mInstance);

	//Fetch an array of available physical devices.
	vkEnumeratePhysicalDevices(mInstance, &mGpuCount, NULL);
	mPhysicalDevices = new VkPhysicalDevice[mGpuCount]();
	vkEnumeratePhysicalDevices(mInstance, &mGpuCount, mPhysicalDevices);
}

C9::~C9()
{
	delete[]	 mPhysicalDevices;
	vkDestroyInstance(mInstance, NULL);
}

//IUnknown

ULONG STDMETHODCALLTYPE C9::AddRef(void)
{
	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE C9::QueryInterface(REFIID riid,void  **ppv)
{
	
	return S_OK;
}

ULONG STDMETHODCALLTYPE C9::Release(void)
{
	return this->Release(0);
}

//IDirect3D9

HRESULT STDMETHODCALLTYPE C9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	HRESULT result = D3DERR_NOTAVAILABLE;	// failure
	


	return result;	
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	HRESULT result = D3DERR_NOTAVAILABLE;	// failure
	

	
	return result;	
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels)
{
	return D3DERR_NOTAVAILABLE;	
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)
{		
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
	return S_OK;		
}

UINT STDMETHODCALLTYPE C9::GetAdapterCount()
{
	return mGpuCount;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode)
{
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier)
{	
	return S_OK;	
}

UINT STDMETHODCALLTYPE C9::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format)
{	
	return 0;	
}


HMONITOR STDMETHODCALLTYPE C9::GetAdapterMonitor(UINT Adapter)
{
	return 0; //TODO: implement GetAdapterMonitor
}


HRESULT STDMETHODCALLTYPE C9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	return S_OK;	
}


HRESULT STDMETHODCALLTYPE C9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE C9::AddRef( int which, char *comment)
{
	return 0;
}

ULONG STDMETHODCALLTYPE	C9::Release( int which, char *comment)
{
	return 0;
}