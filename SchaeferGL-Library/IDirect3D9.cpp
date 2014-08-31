/*
 * Copyright (C) 2014 Christopher Joseph Dean Schaefer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
 
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