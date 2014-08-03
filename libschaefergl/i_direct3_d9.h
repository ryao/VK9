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

};

typedef struct IDirect3D9 *LPDIRECT3D9, *PDIRECT3D9;

#endif // IDIRECT3D9_H
