/*
Copyright(c) 2016 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef CTYPES_H
#define CTYPES_H

#include "d3d9.h"

class StreamSource
{
public:

	UINT StreamNumber;
	IDirect3DVertexBuffer9* StreamData;
	UINT OffsetInBytes;
	UINT Stride;

	StreamSource()
		: StreamSource(0, nullptr, 0, 0)
	{

	}

	StreamSource(const StreamSource& value)
		: StreamSource(value.StreamNumber, value.StreamData, value.OffsetInBytes, value.Stride)
	{

	}

	StreamSource(UINT streamNumber, IDirect3DVertexBuffer9* streamData, UINT offsetInBytes, UINT stride)
		:StreamNumber(streamNumber),
		StreamData(streamData),
		OffsetInBytes(offsetInBytes),
		Stride(stride)
	{
		if (StreamData != nullptr)
		{
			StreamData->AddRef();
		}
	}

	~StreamSource()
	{
		if (StreamData != nullptr)
		{
			StreamData->Release();
		}
	}
};

#endif // CTYPES_H
