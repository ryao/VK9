#ifndef GALLIUMD3D9_H
#define GALLIUMD3D9_H

#include "i_direct3_d9.h" // Base class: IDirect3D9

class GalliumD3D9 : public IDirect3D9
{
public:
	GalliumD3D9();
	~GalliumD3D9();

	/*
	 * Creates a device to represent the display adapter.
	 */
	HRESULT CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface);

};

#endif // GALLIUMD3D9_H
