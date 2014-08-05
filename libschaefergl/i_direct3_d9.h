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
 
#ifndef IDIRECT3D9_H
#define IDIRECT3D9_H

#include "i_unknown.h" // Base class: IUnknown
#include "i_direct3d_device9.h"

class IDirect3D9 : public IUnknown
{
public:
	IDirect3D9();
	~IDirect3D9();

	/*
	 * Determines whether a depth-stencil format is compatible with a render-target format in a particular display mode.
	 */
	HRESULT CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
	
	/*
	 * Determines whether a surface format is available as a specified resource type and can be used as a texture, depth-stencil buffer, or render target, or any combination of the three on a device representing this adapter.
	 */
	HRESULT CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
	
	/*
	 * Tests the device to see if it supports conversion from one display format to another.
	 */
	HRESULT CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat);
	
	/*
	 * Determines if a multisampling technique is available on this device.
	 */
	HRESULT CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels);
  
	/*
	 * Verifies whether a hardware accelerated device type can be used on this adapter.
	 */
	HRESULT CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed);
  
	/*
	 * Creates a device to represent the display adapter.
	 */
	HRESULT CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface);
  
	/*
	 * Queries the device to determine whether the specified adapter supports the requested format and display mode.
	 */
	HRESULT EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode);
	
	/*
	 * Returns the number of adapters on the system.
	 */
	UINT GetAdapterCount();
	
	/*
	 * Retrieves the current display mode of the adapter.
	 */
	HRESULT GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode);
	
	/*
	 * Describes the physical display adapters present in hte system when the IDirect3D9 interface was instantiated.
	 */
	HRESULT GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier);
	
	/*
	 * Returns the number of display modes available on this adapter.
	 */
	UINT GetAdapterModeCount(UINT Adapter,D3DFORMAT Format);
	
	/*
	 * Returns the handle of the monitor associated with the Direct3D object.
	 */
	HMONITOR GetAdapterMonitor(UINT Adapter);
	
	/*
	 * Retrieves device-specific information about a device.
	 */
	HRESULT GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps);
	
	/*
	 * Registers a pluggable software device.
	 */
	HRESULT RegisterSoftwareDevice(void *pInitializeFunction);
};

typedef struct IDirect3D9 *LPDIRECT3D9, *PDIRECT3D9;

#endif // IDIRECT3D9_H
