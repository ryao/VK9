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

#ifndef IDIRECT3DDEVICE9_H
#define IDIRECT3DDEVICE9_H

#include "i_unknown.h"
#include "i_direct3d_surface9.h"

class IDirect3DDevice9
 : public IUnknown
{
public:
	IDirect3DDevice9();
	~IDirect3DDevice9();

	/*
	 * Begins a scene.
	 */
	HRESULT BeginScene();
	
	/*
	 * Signals Direct3D to begin recording a device-state block.
	 */
	HRESULT BeginStateBlock();
	
	/*
	 * Clears on or more surfaces such as a render target, multiple render targets, a stencil buffer, or a depth buffer.
	 */
	HRESULT Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	
	/*
	 * Allows an application to fill a rectangular area of a D3DPOOL_DEFAULT surface with a specified color.
	 */
	HRESULT ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color);
};

#endif // IDIRECT3DDEVICE9_H
