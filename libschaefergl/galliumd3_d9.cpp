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
 
#include "galliumd3_d9.h"
#include "galliumd3d_device9.h"

GalliumD3D9::GalliumD3D9()
{
}

GalliumD3D9::~GalliumD3D9()
{
}

HRESULT GalliumD3D9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	GalliumD3DDevice9* device = new GalliumD3DDevice9();
	IDirect3DDevice9* deviceInterface = (IDirect3DDevice9*)device;
	
	
	ppReturnedDeviceInterface = &deviceInterface;
	return 0; //Fix Later
}