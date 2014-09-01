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
 
#ifndef IDIRECT3DRESOURCE9_H
#define IDIRECT3DRESOURCE9_H

#include "IUnknown.h" // Base class: IUnknown
//#include "i_direct3d_device9.h"
class IDirect3DDevice9;

class IDirect3DResource9 : public IUnknown
{
public:
	IDirect3DResource9();
	~IDirect3DResource9();

	/*
	 * Frees the specified private data associated with this resource.
	 */
	virtual HRESULT FreePrivateData(REFGUID refguid);

	/*
	 * Retrieves the device associated with a resource.
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **ppDevice);
	
	/*
	 * Retrieves the priority for this resource.
	 */
	virtual DWORD GetPriority();
	
	/*
	 * Copies the private data associated with the resource to a provided buffer.
	 */
	virtual HRESULT GetPrivateData(REFGUID refguid,void *pData,DWORD *pSizeOfData);
	
	/*
	 * Returns the type of the resource.
	 */
	virtual D3DRESOURCETYPE GetType();
	
	/*
	 * Preloads a managed resource.
	 */
	virtual void PreLoad();
	
	/*
	 * Assigns the priority of a resource for scheduling purposes.
	 */
	virtual DWORD SetPriority(DWORD PriorityNew);
	
	/*
	 * Associates data with the resource that is intended for use by the application
	 */
	virtual HRESULT SetPrivateData(REFGUID refguid,const void *pData,DWORD SizeOfData,DWORD Flags);
};

#endif // IDIRECT3DRESOURCE9_H
