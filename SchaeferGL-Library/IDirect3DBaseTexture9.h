//========= Copyright Christopher Joseph Dean Schaefer, All rights reserved. ============//
//                       SchaeferGL CODE LICENSE
//
//  Copyright 2014 Christopher Joseph Dean Schaefer
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
//
//==================================================================================================
 
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
