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
 
#ifndef IDIRECT3DQUERY9_H
#define IDIRECT3DQUERY9_H

#include "IUnknown.h" // Base class: IUnknown

class IDirect3DDevice9;

class IDirect3DQuery9 : public IUnknown
{
public:
	IDirect3DQuery9();
	~IDirect3DQuery9();

	/*
	 * Polls a queried resource to get the query state or a query result.
	 */
	virtual HRESULT GetData(void *pData,DWORD dwSize,DWORD dwGetDataFlags);
	
	/*
	 * Gets the number of bytes in the query data.
	 */
	virtual DWORD GetDataSize();

	/*
	 * Gets the device that is being queried.
	 */
	virtual HRESULT GetDevice(IDirect3DDevice9 **pDevice);

	/*
	 * Gets the query type.
	 */
	virtual D3DQUERYTYPE GetType();

	/*
	 * Issue a query.
	 */
	virtual HRESULT Issue(DWORD dwIssueFlags);
	
};

typedef struct IDirect3DQuery9 *LPDIRECT3DQUERY9, *PDIRECT3DQUERY9;

#endif // IDIRECT3DQUERY9_H
