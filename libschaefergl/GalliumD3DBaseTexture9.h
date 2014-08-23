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
 
#ifndef GALLIUMD3DBASETEXTURE9_H
#define GALLIUMD3DBASETEXTURE9_H

#include "i_direct3d_base_texture9.h" // Base class: IDirect3DBaseTexture9
#include "GalliumD3DResource9.h"

#include "util/u_inlines.h"
#include "util/u_double_list.h"

class GalliumD3DBaseTexture9 : public IDirect3DBaseTexture9, public GalliumD3DResource9
{
protected:


public:
	GalliumD3DBaseTexture9(GalliumD3DDevice9* device,GalliumD3DUnknown* container,BOOL Allocate,D3DRESOURCETYPE Type,D3DPOOL Pool);
	~GalliumD3DBaseTexture9();

	/*
	 * Generate mipmap sublevels
	 */
	virtual VOID GenerateMipSubLevels();
	
	/*
	 * Get the filter type that is used for automatically generated mipmap sublevels.
	 */	
	virtual D3DTEXTUREFILTERTYPE GetAutoGenFilterType();
	
	/*
	 * Returns a value clamped to the maximum level-of-detail set for a managed texture.
	 */	
	virtual DWORD GetLOD();
	
	/*
	 * Returns the number of texture levels in a multilevel texture.
	 */	
	virtual DWORD GetLevelCount();
	
	/*
	 * Set the filter type that is used for automatically generated mipmap sublevels.
	 */	
	virtual HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);
	
	/*
	 * Sets the most detailed level-of-detail for a managed texture.
	 */	
	virtual DWORD SetLOD(DWORD LODNew);
};

#endif // GALLIUMD3DBASETEXTURE9_H
