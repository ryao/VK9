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


#ifndef IDIRECT3DSTATEBLOCK9_H
#define IDIRECT3DSTATEBLOCK9_H

#include "IUnknown.h" // Base class: IUnknown

class IDirect3DDevice9;

class IDirect3DStateBlock9 : public IUnknown
{
public:
	IDirect3DStateBlock9();
	~IDirect3DStateBlock9();

	/*
	 * Apply the state block to the current device state.
	 */
	virtual HRESULT Apply();
	
	/*
	 * Capture the current value of states that are included in a stateblock.
	 */
	virtual HRESULT Capture();
	
	/*
	 * Gets the device
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **ppDevice);
};

typedef struct IDirect3DStateBlock9 *LPDIRECT3DSTATEBLOCK9, *PDIRECT3DSTATEBLOCK9;

#endif // IDIRECT3DSTATEBLOCK9_H
