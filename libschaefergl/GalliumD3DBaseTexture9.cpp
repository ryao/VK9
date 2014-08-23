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
 
#include "GalliumD3DBaseTexture9.h"

#include "pipe/p_state.h"
#include "pipe/p_context.h"
#include "pipe/p_screen.h"
#include "util/u_inlines.h"
#include "util/u_resource.h"

GalliumD3DBaseTexture9::GalliumD3DBaseTexture9(GalliumD3DDevice9* device,GalliumD3DUnknown* container,BOOL Allocate,D3DRESOURCETYPE Type,D3DPOOL Pool)
: GalliumD3DResource9(device,container,Allocate,Type,Pool)
{
	
}

GalliumD3DBaseTexture9::~GalliumD3DBaseTexture9()
{
	
}

VOID GalliumD3DBaseTexture9::GenerateMipSubLevels()
{
	
}

D3DTEXTUREFILTERTYPE GalliumD3DBaseTexture9::GetAutoGenFilterType()
{
	
}

DWORD GalliumD3DBaseTexture9::GetLOD()
{
	
}

DWORD GalliumD3DBaseTexture9::GetLevelCount()
{
	
}
HRESULT GalliumD3DBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	
}

DWORD GalliumD3DBaseTexture9::SetLOD(DWORD LODNew)
{
	
}
