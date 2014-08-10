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
 
#ifndef GALLIUMD3D9_H
#define GALLIUMD3D9_H

#define HAVE_PIPE_LOADER_DRM //DRM support must exist.

#include "i_direct3_d9.h" // Base class: IDirect3D9
#include "pipe-loader/pipe_loader.h"

/*
 * Returns a location used to search for pipe devices to load.
 */
static const char* GetLibrarySearchPath();

class GalliumD3D9 : public IDirect3D9
{
public:
	GalliumD3D9();
	~GalliumD3D9();

	/*
	 * Returns the number of adapters on the system.
	 */
	UINT GetAdapterCount();

	/*
	 * Creates a device to represent the display adapter.
	 */
	HRESULT CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface);

	/*
	 * Retrieves device-specific information about a device.
	 */
	HRESULT GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps);
	
	
private:
	pipe_loader_device* mPipeDevices;
	int mPipeDeviceCount;
	
	/*
	 * Array of pointers.
	 * This is used to track the last screen created for a device so we can pull the caps.
	 * It is assumed that D3D devices will clean up their own screens.
	 */
	pipe_screen** mPipeScreens;
	
	
};

#endif // GALLIUMD3D9_H
