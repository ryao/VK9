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

#include <iostream>
#include "wtypes.h"

#include "C9.h"
#include "CDevice9.h"

#include "Utilities.h"

#define APP_SHORT_NAME "VK9"

#ifdef _DEBUG
	#include <vld.h>
#endif

C9::C9()
	: 
	mOptionDescriptions("Allowed options"),
	mMonitors(4)
{
	BOOST_LOG_TRIVIAL(info) << "C9::C9 Started.";
	//Setup configuration & logging.

	mOptionDescriptions.add_options()
		("LogFile", boost::program_options::value<std::string>(), "The location of the log file.");

	boost::program_options::store(boost::program_options::parse_config_file<char>("VK9.conf", mOptionDescriptions), mOptions);
	boost::program_options::notify(mOptions);

	if (mOptions.count("LogFile"))
	{
		boost::log::add_file_log(
			boost::log::keywords::file_name = mOptions["LogFile"].as<std::string>(),
			boost::log::keywords::format = "[%TimeStamp%]: %Message%",
			boost::log::keywords::auto_flush = true
		);
	}
	else
	{
		boost::log::add_file_log(
			boost::log::keywords::file_name = "VK9.log",
			boost::log::keywords::format = "[%TimeStamp%]: %Message%",
			boost::log::keywords::auto_flush = true
		);
	}

#ifndef _DEBUG
	boost::log::core::get()->set_filter(boost::log::trivial::severity > boost::log::trivial::info);
#endif

	//Continue instance setup.
	mResult = vkEnumerateInstanceLayerProperties(&mLayerPropertyCount, nullptr);
	if (mResult != VK_SUCCESS)
	{
		BOOST_LOG_TRIVIAL(fatal) << "C9::C9 vkEnumerateInstanceLayerProperties failed with return code of " << mResult;
		return;
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "C9::C9 vkEnumerateInstanceLayerProperties found " << mLayerPropertyCount << " layers.";
	}
	mLayerProperties = new VkLayerProperties[mLayerPropertyCount];
	mResult = vkEnumerateInstanceLayerProperties(&mLayerPropertyCount, mLayerProperties);
	if (mResult == VK_SUCCESS)
	{
		for (size_t i = 0; i < mLayerPropertyCount; i++)
		{
			if (strcmp(mLayerProperties[i].layerName,"VK_LAYER_LUNARG_standard_validation")==0)
			{
				mValidationPresent = true;
			}
			BOOST_LOG_TRIVIAL(info) << "C9::C9 vkEnumerateInstanceLayerProperties - layerName: " << mLayerProperties[i].layerName;
		}		
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	VkExtensionProperties* extension = new VkExtensionProperties[extensionCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extension);

	for (size_t i = 0; i < extensionCount; i++)
	{
		BOOST_LOG_TRIVIAL(info) << "C9::C9 extension available: " << extension[i].extensionName;
		if (strcmp(extension[i].extensionName, "VK_KHR_display")==0)
		{
			mExtensionNames.push_back("VK_KHR_display");
		}
	}

	delete[] extension;

	mExtensionNames.push_back("VK_KHR_surface");
	mExtensionNames.push_back("VK_KHR_win32_surface");	
#ifdef _DEBUG
	mExtensionNames.push_back("VK_EXT_debug_report");
	mLayerExtensionNames.push_back("VK_LAYER_LUNARG_standard_validation");

	HINSTANCE instnace = LoadLibraryA("renderdoc.dll");
	HMODULE mod = GetModuleHandleA("renderdoc.dll");
	if(mod!=nullptr)
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		if (RENDERDOC_GetAPI!=nullptr)
		{
			int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_1, (void **)&mRenderDocApi);
			if (ret!=1)
			{
				BOOST_LOG_TRIVIAL(warning) << "C9::C9 unable to find RENDERDOC_API_Version_1_1_ !";
			}
			else
			{
				mRenderDocApi->SetLogFilePathTemplate("vk");
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "C9::C9 unable to find RENDERDOC_GetAPI !";
		}
		
	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "C9::C9 unable to find renderdoc.dll !";
	}
#endif // _DEBUG

	// initialize the VkApplicationInfo structure
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = APP_SHORT_NAME;
	app_info.applicationVersion = 1;
	app_info.pEngineName = APP_SHORT_NAME;
	app_info.engineVersion = 1;
	app_info.apiVersion = 0; // VK_API_VERSION;

	// initialize the VkInstanceCreateInfo structure
	VkInstanceCreateInfo inst_info =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,								  // VkStructureType sType;
		NULL,																  // const void* pNext;
		0,																	  // VkInstanceCreateFlags flags;
		&app_info,															  // const VkApplicationInfo* pApplicationInfo;
		mLayerExtensionNames.size(),										  // uint32_t enabledLayerNameCount;
		mLayerExtensionNames.size()>0 ? mLayerExtensionNames.data() : nullptr,// const char* const* ppEnabledLayerNames;
		mExtensionNames.size(),											      // uint32_t enabledExtensionNameCount;
		mExtensionNames.size()>0 ? mExtensionNames.data() : nullptr,		  // const char* const* ppEnabledExtensionNames;
	};

	//Get an instance handle.
	if (VK_SUCCESS == vkCreateInstance(&inst_info, NULL, &mInstance))
	{
		vkGetPhysicalDeviceDisplayPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(vkGetInstanceProcAddr(mInstance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
		vkGetDisplayModePropertiesKHR = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(vkGetInstanceProcAddr(mInstance, "vkGetDisplayModePropertiesKHR"));

#ifdef _DEBUG
		/* Load VK_EXT_debug_report entry points in debug builds */
		vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));
		vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(mInstance, "vkDebugReportMessageEXT"));
		vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT"));

		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		callbackCreateInfo.pNext = nullptr;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = &DebugReportCallback;
		callbackCreateInfo.pUserData = nullptr;

		/* Register the callback */
		mResult = vkCreateDebugReportCallbackEXT(mInstance, &callbackCreateInfo, nullptr, &mCallback);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "C9::C9 vkCreateDebugReportCallbackEXT failed with return code of " << mResult;
			return;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "C9::C9 vkCreateDebugReportCallbackEXT succeeded.";
		}
#endif

		//Fetch an array of available physical devices.
		mResult = vkEnumeratePhysicalDevices(mInstance, &mGpuCount, NULL);
		if (mResult != VK_SUCCESS)
		{
			BOOST_LOG_TRIVIAL(fatal) << "C9::C9 vkEnumeratePhysicalDevices failed with return code of " << mResult;
			return;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "There were " << mGpuCount << " physical devices found.";
		}

		if (mGpuCount > 0)
		{
			mPhysicalDevices = new VkPhysicalDevice[mGpuCount]();
			mResult = vkEnumeratePhysicalDevices(mInstance, &mGpuCount, mPhysicalDevices);
			if (mResult != VK_SUCCESS)
			{
				BOOST_LOG_TRIVIAL(fatal) << "C9::C9 vkEnumeratePhysicalDevices failed with return code of " << mResult;
				return;
			}

			if (vkGetPhysicalDeviceDisplayPropertiesKHR!=nullptr)
			{
				mResult = vkGetPhysicalDeviceDisplayPropertiesKHR(mPhysicalDevices[0], &mDisplayCount, nullptr);
				if (mResult != VK_SUCCESS)
				{
					BOOST_LOG_TRIVIAL(fatal) << "C9::C9 vkGetPhysicalDeviceDisplayPropertiesKHR failed with return code of " << mResult;
					return;
				}

				mDisplayProperties = new VkDisplayPropertiesKHR[mDisplayCount]();

				mResult = vkGetPhysicalDeviceDisplayPropertiesKHR(mPhysicalDevices[0], &mDisplayCount, mDisplayProperties);
				if (mResult != VK_SUCCESS)
				{
					BOOST_LOG_TRIVIAL(fatal) << "C9::C9 vkGetPhysicalDeviceDisplayPropertiesKHR failed with return code of " << mResult;
					return;
				}
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "No phyuscial devices were found.";
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "C9::C9 vkCreateInstance failed.";
		return;
	}

	//WINAPI to get monitor info
	EnumDisplayMonitors(GetDC(NULL), NULL, MonitorEnumProc, (LPARAM)&mMonitors);

	BOOST_LOG_TRIVIAL(info) << "C9::C9 Finished.";
}

C9::~C9()
{
	BOOST_LOG_TRIVIAL(info) << "C9::~C9";

	delete[] mDisplayProperties;
	delete[] mPhysicalDevices;
	delete[] mLayerProperties;

#ifdef _DEBUG
	if(mCallback != VK_NULL_HANDLE)
	{
		vkDestroyDebugReportCallbackEXT(mInstance, mCallback, nullptr);
	}
#endif

	if (mInstance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(mInstance, nullptr);
	}	
}

//IUnknown

ULONG STDMETHODCALLTYPE C9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE C9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3D9))
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

	(*ppv) = nullptr;

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE C9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

//IDirect3D9

HRESULT STDMETHODCALLTYPE C9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	HRESULT result = S_OK;
	
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::CheckDepthStencilMatch is not implemented!";

	return result;	
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	//TODO: Implement.
	
	BOOST_LOG_TRIVIAL(warning) << "C9::CheckDeviceFormat is not implemented!" << AdapterFormat << " " << CheckFormat;

	return S_OK;
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	HRESULT result = S_OK;

	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::CheckDeviceFormatConversion is not implemented!";

	return result;
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::CheckDeviceMultiSampleType is not implemented!";

	return D3DERR_NOTAVAILABLE;	
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)
{		
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::CheckDeviceType is not implemented!";

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	HRESULT result = S_OK;

	//TODO: create different code path for D3DCREATE_MULTITHREADED. It's not recommended so I'll probably do that after the thread unsafe version is working correctly.

	CDevice9* obj = new CDevice9(this,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters);

	if (obj->mResult != VK_SUCCESS)
	{
		delete obj;
		obj = nullptr;
		result = D3DERR_INVALIDCALL;
	}

	(*ppReturnedDeviceInterface) = (IDirect3DDevice9*)obj;

	return result;	
}

HRESULT STDMETHODCALLTYPE C9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::EnumAdapterModes is not implemented!";

	return S_OK;		
}

UINT STDMETHODCALLTYPE C9::GetAdapterCount()
{
	return mGpuCount;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode)
{
	Monitor& monitor = mMonitors[Adapter];

	pMode->Height = monitor.Height;
	pMode->Width = monitor.Width;
	pMode->RefreshRate = monitor.RefreshRate;
	pMode->Format = D3DFMT_X8R8G8B8;

	if (monitor.PixelBits != 32)
	{
		BOOST_LOG_TRIVIAL(info) << "C9::GetAdapterDisplayMode Unknown pixel bit format : " << monitor.PixelBits; //Revisit
	}

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier)
{	
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(mPhysicalDevices[Adapter], &deviceProperties);
	
	(*pIdentifier) = {}; //zero it out.

	strcpy(pIdentifier->DeviceName, deviceProperties.deviceName); //256 to 32 revisit
	strcpy(pIdentifier->Driver, ""); //revisit
	strcpy(pIdentifier->Description, ""); //revisit
	pIdentifier->VendorId = deviceProperties.vendorID;
	pIdentifier->DeviceId = deviceProperties.deviceID;
	pIdentifier->DriverVersion.QuadPart = deviceProperties.driverVersion;
	
	//pIdentifier->SubSysId = 0;
	//pIdentifier->Revision = 0;
	//pIdentifier->DeviceIdentifier = 0;
	//pIdentifier->WHQLLevel = 0;

	//this->AddRef();

	return S_OK;	
}

UINT STDMETHODCALLTYPE C9::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format)
{	
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::GetAdapterModeCount is not implemented!";

	return 0;	
}


HMONITOR STDMETHODCALLTYPE C9::GetAdapterMonitor(UINT Adapter)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::GetAdapterMonitor is not implemented!";

	return 0; //TODO: implement GetAdapterMonitor
}


HRESULT STDMETHODCALLTYPE C9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	/*
		https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkPhysicalDeviceProperties
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb172513(v=vs.85).aspx
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb172635(v=vs.85).aspx
		https://msdn.microsoft.com/en-us/library/windows/desktop/bb172591(v=vs.85).aspx
	*/

	//Fetch the properties & features from the physical device.
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceProperties(this->mPhysicalDevices[Adapter], &properties);
	vkGetPhysicalDeviceFeatures(this->mPhysicalDevices[Adapter], &features);

	//Translate the vulkan properties & features into D3D9 capabilities.
	pCaps->DeviceType = DeviceType;
	pCaps->AdapterOrdinal=0;
	pCaps->Caps=0;
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
	pCaps->MaxTextureWidth = properties.limits.maxImageDimension2D; //Revisit
	pCaps->MaxTextureHeight = properties.limits.maxImageDimension2D; //Revisit
	pCaps->MaxVolumeExtent= properties.limits.maxImageDimensionCube; //Revisit
	pCaps->MaxTextureRepeat= 32768; //revisit
	pCaps->MaxTextureAspectRatio = pCaps->MaxTextureWidth;
	pCaps->MaxAnisotropy= features.samplerAnisotropy;
	pCaps->MaxVertexW = 1e10f; //revisit
	pCaps->GuardBandLeft = -1e9f; //revisit
	pCaps->GuardBandTop = -1e9f; //revisit
	pCaps->GuardBandRight = 1e9f; //revisit
	pCaps->GuardBandBottom = 1e9f; //revisit
	pCaps->ExtentsAdjust = 0.0f; //revisit
	pCaps->StencilCaps = D3DSTENCILCAPS_KEEP | D3DSTENCILCAPS_ZERO | D3DSTENCILCAPS_REPLACE | D3DSTENCILCAPS_INCRSAT | D3DSTENCILCAPS_DECRSAT | D3DSTENCILCAPS_INVERT | D3DSTENCILCAPS_INCR | D3DSTENCILCAPS_DECR | D3DSTENCILCAPS_TWOSIDED;
	pCaps->FVFCaps = D3DFVFCAPS_PSIZE;
	pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_MODULATE | D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE4X | D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_ADDSIGNED | D3DTEXOPCAPS_ADDSIGNED2X | D3DTEXOPCAPS_SUBTRACT | D3DTEXOPCAPS_ADDSMOOTH | D3DTEXOPCAPS_BLENDDIFFUSEALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHA | D3DTEXOPCAPS_BLENDFACTORALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHAPM | D3DTEXOPCAPS_BLENDCURRENTALPHA | D3DTEXOPCAPS_PREMODULATE | D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA | D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA | D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE | D3DTEXOPCAPS_DOTPRODUCT3 | D3DTEXOPCAPS_MULTIPLYADD | D3DTEXOPCAPS_LERP;
	pCaps->MaxTextureBlendStages = properties.limits.maxDescriptorSetSamplers; //revisit
	/*
	I'm setting this to 16 so I can make my array 16 and be safe ish	
	*/
	pCaps->MaxSimultaneousTextures = 16; // properties.limits.maxDescriptorSetSampledImages; //revisit

	pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_MATERIALSOURCE7 | D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_LOCALVIEWER | D3DVTXPCAPS_TWEENING;
	pCaps->MaxActiveLights=0;  //Revsit should be infinite but games may not read it that way.
	pCaps->MaxUserClipPlanes = 8; //revisit
	pCaps->MaxVertexBlendMatrices = 4; //revisit
	pCaps->MaxVertexBlendMatrixIndex = 7; //revisit
	pCaps->MaxPointSize= properties.limits.pointSizeRange[1]; //revisit
	pCaps->MaxPrimitiveCount = 0xFFFFFFFF; //revisit
	pCaps->MaxVertexIndex = 0xFFFFFFFF; //revisit
	pCaps->MaxStreams = properties.limits.maxVertexInputBindings; //revisit
	pCaps->MaxStreamStride= properties.limits.maxVertexInputBindingStride; //revisit
	pCaps->VertexShaderVersion= D3DVS_VERSION(3, 0);
	pCaps->MaxVertexShaderConst=256; //revisit
	pCaps->PixelShaderVersion = D3DPS_VERSION(3, 0);
	pCaps->PixelShader1xMaxValue = 65504.f;
	pCaps->DevCaps2 = D3DDEVCAPS2_STREAMOFFSET | D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET | D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES;
	pCaps->MasterAdapterOrdinal=0;
	pCaps->AdapterOrdinalInGroup=0;
	pCaps->NumberOfAdaptersInGroup = 1;
	pCaps->DeclTypes= D3DDTCAPS_UBYTE4 | D3DDTCAPS_UBYTE4N | D3DDTCAPS_SHORT2N | D3DDTCAPS_SHORT4N | D3DDTCAPS_USHORT2N | D3DDTCAPS_USHORT4N | D3DDTCAPS_UDEC3 | D3DDTCAPS_DEC3N | D3DDTCAPS_FLOAT16_2 | D3DDTCAPS_FLOAT16_4;
	pCaps->NumSimultaneousRTs = 4; //revisit
	pCaps->StretchRectFilterCaps= D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR;
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
	pCaps->MaxVShaderInstructionsExecuted = 65535 < pCaps->MaxVertexShader30InstructionSlots * 32 ? pCaps->MaxVertexShader30InstructionSlots * 32: 65535; //revisit
	pCaps->MaxPShaderInstructionsExecuted = 65535 < pCaps->MaxPixelShader30InstructionSlots * 32 ? pCaps->MaxPixelShader30InstructionSlots * 32 : 65535; //revisit


	pCaps->MaxNpatchTessellationLevel = 0.0f;
	pCaps->Reserved5 = 0;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	//TODO: Implement.

	BOOST_LOG_TRIVIAL(warning) << "C9::RegisterSoftwareDevice is not implemented!";

	return E_NOTIMPL;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData)
{
	switch (flags)
	{
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		BOOST_LOG_TRIVIAL(info) << "DebugReport(Info): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "DebugReport(Warn): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "DebugReport(Perf): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		BOOST_LOG_TRIVIAL(error) << "DebugReport(Error): " << message << " " << objectType;
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		BOOST_LOG_TRIVIAL(warning) << "DebugReport(Debug): " << message << " " << objectType;
		break;
	default:
		BOOST_LOG_TRIVIAL(error) << "DebugReport(?): " << message << " " << objectType;
		break;
	}
	
	return VK_FALSE;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	Monitor monitor;
	
	monitor.hMonitor = hMonitor;
	monitor.hdcMonitor = hdcMonitor;

	monitor.Height = ::GetDeviceCaps(monitor.hdcMonitor, HORZRES);
	monitor.Width = ::GetDeviceCaps(monitor.hdcMonitor, VERTRES);
	monitor.RefreshRate = ::GetDeviceCaps(monitor.hdcMonitor, VREFRESH);
	monitor.PixelBits = ::GetDeviceCaps(monitor.hdcMonitor, BITSPIXEL);
	monitor.ColorPlanes = ::GetDeviceCaps(monitor.hdcMonitor, PLANES);

	if (dwData != NULL)
	{
		std::vector<Monitor>* monitors;
		monitors = (std::vector<Monitor>*)dwData;
		monitors->push_back(monitor);

		BOOST_LOG_TRIVIAL(info) << "MonitorEnumProc HMONITOR: " << monitor.hMonitor;
		BOOST_LOG_TRIVIAL(info) << "MonitorEnumProc HDC: " << monitor.hdcMonitor;

		BOOST_LOG_TRIVIAL(info) << "MonitorEnumProc Height: " << monitor.Height;
		BOOST_LOG_TRIVIAL(info) << "MonitorEnumProc Width: " << monitor.Width;
		BOOST_LOG_TRIVIAL(info) << "MonitorEnumProc RefreshRate: " << monitor.RefreshRate;

		BOOST_LOG_TRIVIAL(info) << "MonitorEnumProc PixelBits: " << monitor.PixelBits;
		BOOST_LOG_TRIVIAL(info) << "MonitorEnumProc ColorPlanes: " << monitor.ColorPlanes;

		return true;
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "MonitorEnumProc: monitor vector is null.";
		return false;
	}	
}