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
 
#ifndef IDIRECT3D9_H
#define IDIRECT3D9_H

#include "IUnknown.h" // Base class: IUnknown
#include "IDirect3DDevice9.h"

class IDirect3D9 : public IUnknown
{
public:
	IDirect3D9();
	~IDirect3D9();

	/*
	 * Determines whether a depth-stencil format is compatible with a render-target format in a particular display mode.
	 */
	virtual HRESULT CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
	
	/*
	 * Determines whether a surface format is available as a specified resource type and can be used as a texture, depth-stencil buffer, or render target, or any combination of the three on a device representing this adapter.
	 */
	virtual HRESULT CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
	
	/*
	 * Tests the device to see if it supports conversion from one display format to another.
	 */
	virtual HRESULT CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat);
	
	/*
	 * Determines if a multisampling technique is available on this device.
	 */
	virtual HRESULT CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels);
  
	/*
	 * Verifies whether a hardware accelerated device type can be used on this adapter.
	 */
	virtual HRESULT CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed);
  
	/*
	 * Creates a device to represent the display adapter.
	 */
	virtual HRESULT CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface);
  
	/*
	 * Queries the device to determine whether the specified adapter supports the requested format and display mode.
	 */
	virtual HRESULT EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode);
	
	/*
	 * Returns the number of adapters on the system.
	 */
	virtual UINT GetAdapterCount();
	
	/*
	 * Retrieves the current display mode of the adapter.
	 */
	virtual HRESULT GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode);
	
	/*
	 * Describes the physical display adapters present in hte system when the IDirect3D9 interface was instantiated.
	 */
	virtual HRESULT GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier);
	
	/*
	 * Returns the number of display modes available on this adapter.
	 */
	virtual UINT GetAdapterModeCount(UINT Adapter,D3DFORMAT Format);
	
	/*
	 * Returns the handle of the monitor associated with the Direct3D object.
	 */
	virtual HMONITOR GetAdapterMonitor(UINT Adapter);
	
	/*
	 * Retrieves device-specific information about a device.
	 */
	virtual HRESULT GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps);
	
	/*
	 * Registers a pluggable software device.
	 */
	virtual HRESULT RegisterSoftwareDevice(void *pInitializeFunction);
};

typedef struct IDirect3D9 *LPDIRECT3D9, *PDIRECT3D9;

#endif // IDIRECT3D9_H
