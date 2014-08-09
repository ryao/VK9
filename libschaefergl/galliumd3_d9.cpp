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