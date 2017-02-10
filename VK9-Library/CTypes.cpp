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
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "CTypes.h"

#include "CVertexBuffer9.h"

StreamSource::StreamSource()
	: StreamSource(0, nullptr, 0, 0)
{

}

StreamSource& StreamSource::operator =(const StreamSource& value)
{
	this->StreamNumber = value.StreamNumber;
	this->StreamData = value.StreamData;
	this->OffsetInBytes = value.OffsetInBytes;
	this->Stride = value.Stride;

	return *this;
}

StreamSource::StreamSource(const StreamSource& value)
	: StreamSource(value.StreamNumber, value.StreamData, value.OffsetInBytes, value.Stride)
{

}

StreamSource::StreamSource(UINT streamNumber, CVertexBuffer9* streamData, VkDeviceSize offsetInBytes, UINT stride)
	:StreamNumber(streamNumber),
	StreamData(streamData),
	OffsetInBytes(offsetInBytes),
	Stride(stride)
{

}

StreamSource::~StreamSource()
{

}

