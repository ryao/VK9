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
 
#ifndef IDIRECT3DVOLUME9_H
#define IDIRECT3DVOLUME9_H

#include "IUnknown.h" // Base class: IUnknown

class IDirect3DDevice9;

class IDirect3DVolume9 : public IUnknown
{
public:
	IDirect3DVolume9();
	~IDirect3DVolume9();

	/*
	 * Frees the specified private data associated with this volume.
	 */
	virtual HRESULT FreePrivateData(REFGUID refguid);
	
	/*
	 * Provides access to the parent volume texture object.
	 */
	virtual HRESULT GetContainer(REFIID riid,void **ppContainer);
	
	/*
	 * Retieves a description of the volume.
	 */
	virtual HRESULT GetDesc(D3DVOLUME_DESC *pDesc);
	
	/*
	 * Retrieves the device associated with a volume.
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **ppDevice);
	
	/*
	 * Copies the private data associated with the volume to a provided buffer.
	 */
	virtual HRESULT GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData);
	
	/*
	 * Locks a box on a volume resource.
	 */
	virtual HRESULT LockBox(D3DLOCKED_BOX *pLockedVolume,const D3DBOX *pBox,DWORD Flags);
	
	/*
	 * Associates data with the volume that is intended for use by the application.
	 */
	virtual HRESULT SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags);
	
	/*
	 * Unlocks a box on a volume resource.
	 */
	virtual HRESULT UnlockBox();
};

typedef struct IDirect3DVolume9 *LPDIRECT3DVOLUME9, *PDIRECT3DVOLUME9;

#endif // IDIRECT3DVOLUME9_H
