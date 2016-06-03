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
 
#ifndef C9_H
#define C9_H

#include "d3d9.h" // Base class: IDirect3D9
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include <vector>
#include <boost/program_options.hpp>
#include "CTypes.h"

class C9 : public IDirect3D9
{
public:
	C9();
	~C9();

	VkResult mResult;
	VkInstance mInstance;
	VkPhysicalDevice* mPhysicalDevices;
	VkLayerProperties* mLayerProperties;
	uint32_t mLayerPropertyCount;
	uint32_t mGpuCount;
	std::vector<char*> mExtensionNames;
	std::vector<char*> mLayerExtensionNames;
	bool mValidationPresent;

	boost::program_options::variables_map mOptions;
	boost::program_options::options_description mOptionDescriptions;

	int mReferenceCount;

#ifdef _DEBUG
	/* Load VK_EXT_debug_report entry points in debug builds */
	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
	VkDebugReportCallbackEXT mCallback;
#endif

public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3D9
	virtual HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed);
	virtual HRESULT STDMETHODCALLTYPE CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface);
	virtual HRESULT STDMETHODCALLTYPE EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode);
	virtual UINT STDMETHODCALLTYPE GetAdapterCount();
	virtual HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode);
	virtual HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier);
	virtual UINT STDMETHODCALLTYPE GetAdapterModeCount(UINT Adapter,D3DFORMAT Format);
	virtual HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(UINT Adapter);
	virtual HRESULT STDMETHODCALLTYPE GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps);
	virtual HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(void *pInitializeFunction);
};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData);

#endif // C9_H
