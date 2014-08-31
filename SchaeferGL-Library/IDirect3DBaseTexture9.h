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
 
#ifndef IDIRECT3DBASETEXTURE9_H
#define IDIRECT3DBASETEXTURE9_H

#include "IDirect3DResource9.h" // Base class: IDirect3DResource9

class IDirect3DBaseTexture9 : public IDirect3DResource9
{
public:
	IDirect3DBaseTexture9();
	~IDirect3DBaseTexture9();

	/*
	 * Generate mipmap sublevels
	 */
	virtual VOID GenerateMipSubLevels();

	/*
	 * Get the filter type that is used for automatically generated mipmap sublevels.
	 */
	virtual D3DTEXTUREFILTERTYPE GetAutoGenFilterType();
	
	/*
	 * Returns the number of texture levels in a multilevel texture.
	 */
	virtual DWORD GetLevelCount();
	
	/*
	 * Returns a value clamped to the maximum level-of-detail set for a managed texture.
	 */
	virtual DWORD GetLOD();
	
	/*
	 * Set the filter type that is used for automatically generated mipmap sublevels.
	 */
	virtual HRESULT SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType);
	
	/*
	 * Sets the most detailed level-of-detail for a managed texture.
	 */
	virtual DWORD SetLOD(DWORD LODNew);
};

typedef struct IDirect3DBaseTexture9 *LPDIRECT3DBASETEXTURE9, *PDIRECT3DBASETEXTURE9;

#endif // IDIRECT3DBASETEXTURE9_H
