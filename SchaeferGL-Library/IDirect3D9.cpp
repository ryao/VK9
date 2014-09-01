//========= Copyright Christopher Joseph Dean Schaefer, All rights reserved. ============//
//                       SchaeferGL CODE LICENSE
//
//  Copyright 2014 Christopher Joseph Dean Schaefer
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
//
//==================================================================================================
 
#include "IDirect3D9.h"
#include <iostream> 

IDirect3D9::IDirect3D9()
{
}

IDirect3D9::~IDirect3D9()
{
}

HRESULT IDirect3D9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	std::cout << "IDirect3D9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)" << std::endl;	

	return E_NOTIMPL;
}

HRESULT IDirect3D9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	std::cout << "IDirect3D9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3D9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	std::cout << "IDirect3D9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)" << std::endl;	

	return E_NOTIMPL;
}

HRESULT IDirect3D9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels)
{
	std::cout << "IDirect3D9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels)" << std::endl;	

	return E_NOTIMPL;
}

HRESULT IDirect3D9::CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)
{
	std::cout << "IDirect3D9::CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3D9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	std::cout << "IDirect3D9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)" << std::endl;	

	return E_NOTIMPL;
}

HRESULT IDirect3D9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode)
{
	std::cout << "IDirect3D9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode)" << std::endl;	

	return E_NOTIMPL;
}

UINT IDirect3D9::GetAdapterCount()
{
	std::cout << "IDirect3D9::GetAdapterCount()" << std::endl;	

	return 0;
}

HRESULT IDirect3D9::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode)
{
	std::cout << "IDirect3D9::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3D9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
	std::cout << "IDirect3D9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier)" << std::endl;	

	return E_NOTIMPL;
}

UINT IDirect3D9::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format)
{
	std::cout << "IDirect3D9::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format)" << std::endl;	

	return 0;
}

HMONITOR IDirect3D9::GetAdapterMonitor(UINT Adapter)
{
	std::cout << "IDirect3D9::GetAdapterMonitor(UINT Adapter)" << std::endl;	

	return 0;
}

HRESULT IDirect3D9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	std::cout << "IDirect3D9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)" << std::endl;

	return E_NOTIMPL;
}

HRESULT IDirect3D9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	std::cout << "IDirect3D9::RegisterSoftwareDevice(void *pInitializeFunction)" << std::endl;

	return E_NOTIMPL;
}