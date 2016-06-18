#include "CTypes.h"

#include "CVertexBuffer9.h"

StreamSource::StreamSource()
	: StreamSource(0, nullptr, 0, 0)
{

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

