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

#include "util/u_memory.h"
#include "util/u_inlines.h"

#include <stdlib.h> //for getenv
#include <new>  //for nothrow

static const char* GetLibrarySearchPath()
{
	const char* search_path = NULL;
   
   /*
	* I've seen several examples with this check indicating that it is to prevent "set uid apps" from using GBM_BACKENDS_PATH.
	* TODO determine reason for check and if possible add alternate (non-static) method for finding pipe path.
	*/
	if (geteuid() == getuid())
	{
		search_path = getenv("GBM_BACKENDS_PATH");		  
	}

	if (search_path == NULL)
	{
		#ifdef PIPE_SEARCH_DIR 
		search_path = PIPE_SEARCH_DIR; //This would normally be set by the build scripts.
		#endif
	}
    
	return search_path;	
}

GalliumD3D9::GalliumD3D9()
{
	this->mPipeDeviceCount = pipe_loader_drm_probe(&mPipeDevices,20);
}

GalliumD3D9::~GalliumD3D9()
{
	pipe_loader_release(&mPipeDevices,mPipeDeviceCount);
}

UINT GalliumD3D9::GetAdapterCount()
{
	return this->mPipeDeviceCount;
}

HRESULT GalliumD3D9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	int returnValue = 0;
	pipe_screen* screen = NULL;
	pipe_loader_device* pipeDevice = NULL;
	GalliumD3DDevice9* device = NULL;
	IDirect3DDevice9* deviceInterface = NULL;
	D3DCAPS9 capabilities;
	
	if(Adapter<mPipeDeviceCount)
	{
		pipeDevice = &mPipeDevices[Adapter];
	}
	else
	{
		return D3DERR_INVALIDCALL;
	}
	
	switch(DeviceType)
	{
        case D3DDEVTYPE_HAL:
				screen = pipe_loader_create_screen(pipeDevice, GetLibrarySearchPath());
            break;
        case D3DDEVTYPE_REF:
			return D3DERR_NOTAVAILABLE;
        case D3DDEVTYPE_NULLREF:
				return D3DERR_NOTAVAILABLE;
        case D3DDEVTYPE_SW:
            return D3DERR_NOTAVAILABLE;
        default:
            return D3DERR_INVALIDCALL;		
	}
	
	if(screen==NULL)
	{
		return D3DERR_NOTAVAILABLE;
	}
	
	if(this->GetDeviceCaps(Adapter,DeviceType,&capabilities)!=D3D_OK)
	{
		return E_FAIL;
	}
	
	device = new (std::nothrow) GalliumD3DDevice9(screen);
	
	if(device==NULL)
	{
		return E_FAIL;
	}
	
	deviceInterface = (IDirect3DDevice9*)device;
	ppReturnedDeviceInterface = &deviceInterface;
	
	return D3D_OK;
}

HRESULT GalliumD3D9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	//Misc setup.
	pCaps->DeviceType = DeviceType;
	pCaps->AdapterOrdinal = Adapter;
	
	
	//Assumed capabilities.
	pCaps->Caps = 0;
	
	pCaps->Caps2 = D3DCAPS2_CANMANAGERESOURCE |
			/* D3DCAPS2_CANSHARERESOURCE | */
			/* D3DCAPS2_CANCALIBRATEGAMMA | */
			   D3DCAPS2_DYNAMICTEXTURES |
			   D3DCAPS2_FULLSCREENGAMMA |
			   D3DCAPS2_CANAUTOGENMIPMAP;
			   
	pCaps->Caps3 =
			/* D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD | */
			   D3DCAPS3_COPY_TO_VIDMEM |
			   D3DCAPS3_COPY_TO_SYSTEMMEM |
			   D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION |
			   D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD;
		
	//Bit mask of values representing what presentation swap intervals are available.
    pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_DEFAULT |
                                   D3DPRESENT_INTERVAL_ONE |
                                   /*D3DPRESENT_INTERVAL_TWO |*/
                                   /*D3DPRESENT_INTERVAL_THREE |*/
                                   /*D3DPRESENT_INTERVAL_FOUR |*/
                                   D3DPRESENT_INTERVAL_IMMEDIATE;
	
	//capability checks.
}