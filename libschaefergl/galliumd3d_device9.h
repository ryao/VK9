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
