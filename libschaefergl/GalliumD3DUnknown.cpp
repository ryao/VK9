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
 
#include "GalliumD3DUnknown.h"

#include "galliumd3d_device9.h"
#include "util/u_atomic.h"

GalliumD3DUnknown::GalliumD3DUnknown(GalliumD3DDevice9* device,GalliumD3DUnknown* container)
: mDevice(device),mContainer(container),mReferenceCount(0)
{
	
}

GalliumD3DUnknown::~GalliumD3DUnknown()
{
	
}

ULONG GalliumD3DUnknown::AddRef()
{
	ULONG count=0;
	
	if(this->mContainer != __null)
	{
		count = this->mContainer->AddRef();
	}
	else
	{
		//count = p_atomic_inc_return(&this->mReferenceCount);
		mReferenceCount++;
		count = mReferenceCount;
	}
	
	if(count==1 && this->mDevice != __null)
	{
		this->mDevice->AddRef();
	}
	
	return count;
}

//HRESULT GalliumD3DUnknown::QueryInterface(REFIID riid, void** ppvObject)
//{
//}

ULONG GalliumD3DUnknown::Release()
{
	ULONG count=0;
	
	if(this->mContainer != __null)
	{
		count = this->mContainer->Release();
	}
	else
	{
		//count = p_atomic_dec_return(&this->mReferenceCount);
		mReferenceCount--;
		count = mReferenceCount;
	}	

	if(count==0)
	{
		if(this->mDevice != __null)
		{
			this->mDevice->Release();
		}
		delete this;
	}
	
}
