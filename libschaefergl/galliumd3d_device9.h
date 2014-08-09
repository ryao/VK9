#ifndef GALLIUMD3DDEVICE9_H
#define GALLIUMD3DDEVICE9_H

#include "i_direct3d_device9.h" // Base class: IDirect3DDevice9

#include "pipe/p_screen.h"

class GalliumD3DDevice9 : public IDirect3DDevice9
{
public:
	GalliumD3DDevice9();
	~GalliumD3DDevice9();

	pipe_screen* mScreen;
};

#endif // GALLIUMD3DDEVICE9_H
