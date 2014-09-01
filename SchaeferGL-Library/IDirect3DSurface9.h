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
 
#ifndef IDIRECT3DSURFACE9_H
#define IDIRECT3DSURFACE9_H

#include "IDirect3DResource9.h" // Base class: IDirect3DResource9

class IDirect3DSurface9 : public IDirect3DResource9
{
public:
	IDirect3DSurface9();
	~IDirect3DSurface9();

	/*
	 * Provides access to the parent cube texture or texture object if this surface is a cube texture or mipmap child.
	 * This method can also provide access to the parent swap chain if hte surface is a back-buffer child.
	 */
	virtual HRESULT GetContainer(REFIID riid,void **ppContainer);

	/*
	 * Retrieves a device context.
	 */
	virtual HRESULT GetDC(HDC *phdc);
	
	/*
	 * Retrieves a description of the surface.
	 */
	virtual HRESULT GetDesc(D3DSURFACE_DESC *pDesc);
	
	/*
	 * Locks a rectangle on a surface.
	 */
	virtual HRESULT LockRect(D3DLOCKED_RECT *pLockedRect,const RECT *pRect,DWORD Flags);
	
	/*
	 * Release a device context handle.
	 */
	virtual HRESULT ReleaseDC(HDC hdc);
	
	/*
	 * Unlocks a rectangle on a surface.
	 */
	virtual HRESULT UnlockRect();
};

typedef struct IDirect3DSurface9 *LPDIRECT3DSURFACE9, *PDIRECT3DSURFACE9;

#endif // IDIRECT3DSURFACE9_H
