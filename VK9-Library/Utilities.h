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

#ifndef UTILITIES_H
#define UTILITIES_H

#include "resource.h"
#include "CTypes.h"
#include "d3d9.h" // Base class: IDirect3DDevice9

#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <cstring>
#include <fstream>

#define D3DCOLOR_A(dw) (((float)(((dw) >> 24) & 0xFF)) / 255.0f)
#define D3DCOLOR_R(dw) (((float)(((dw) >> 16) & 0xFF)) / 255.0f)
#define D3DCOLOR_G(dw) (((float)(((dw) >> 8) & 0xFF)) / 255.0f)
#define D3DCOLOR_B(dw) (((float)(((dw) >> 0) & 0xFF)) / 255.0f)


HMODULE GetModule(HMODULE module = 0);

VkShaderModule LoadShaderFromFile(VkDevice device, const char *filename);

VkShaderModule LoadShaderFromResource(VkDevice device, WORD resource);

inline uint32_t FindMemoryType(VkPhysicalDeviceMemoryProperties& memoryProperties, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	return 0;
}

inline bool GetMemoryTypeFromProperties(VkPhysicalDeviceMemoryProperties& memoryProperties, uint32_t typeBits,VkFlags requirements_mask,uint32_t *typeIndex)
{
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
	{
		if ((typeBits & 1) == 1) 
		{
			if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
			{
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}

	return false;
}

//D3DDECLTYPE

inline VkFormat ConvertDeclType(D3DDECLTYPE input)
{
	VkFormat output = VK_FORMAT_UNDEFINED;
	//TODO: finish mapping.
	switch (input)
	{
	case D3DDECLTYPE_FLOAT1: // 1D float expanded to (value, 0., 0., 1.)
		output = VK_FORMAT_R32_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT2:  // 2D float expanded to (value, value, 0., 1.)
		output = VK_FORMAT_R32G32_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT3: // 3D float expanded to (value, value, value, 1.)
		output = VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT4:  // 4D float
		output = VK_FORMAT_R32G32B32A32_SFLOAT;
		break;
	case D3DDECLTYPE_D3DCOLOR:  // 4D packed unsigned bytes mapped to 0. to 1. range
		output = VK_FORMAT_B8G8R8A8_UINT;
		break; // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
	case D3DDECLTYPE_UBYTE4: // 4D unsigned byte
		break;
	case D3DDECLTYPE_SHORT2:  // 2D signed short expanded to (value, value, 0., 1.)
		output = VK_FORMAT_R16G16_SINT;
		break;
	case D3DDECLTYPE_SHORT4:  // 4D signed short
		output = VK_FORMAT_R16G16B16_SINT;
		break;
	case D3DDECLTYPE_UBYTE4N:  // Each of 4 bytes is normalized by dividing to 255.0
		break;
	case D3DDECLTYPE_SHORT2N: // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
		break;
	case D3DDECLTYPE_SHORT4N:  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
		break;
	case D3DDECLTYPE_USHORT2N:  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
		break;
	case D3DDECLTYPE_USHORT4N:  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
		break;
	case D3DDECLTYPE_UDEC3:  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
		break;
	case D3DDECLTYPE_DEC3N:  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
		break;
	case D3DDECLTYPE_FLOAT16_2:  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
		output = VK_FORMAT_R16G16_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT16_4:  // Four 16-bit floating point values
		output = VK_FORMAT_R16G16B16A16_SFLOAT;
		break;
	case D3DDECLTYPE_UNUSED:  // When the type field in a decl is unused.
		output = VK_FORMAT_UNDEFINED;
		break;
	default:
		break;
	}

	return output;
}

inline uint32_t ConvertPrimitiveCountToVertexCount(D3DPRIMITIVETYPE primtiveType, UINT primtiveCount)
{
	uint32_t output;

	switch (primtiveType)
	{
	case D3DPT_POINTLIST:
		output = primtiveCount;
		break;
	case D3DPT_LINELIST:
		output = primtiveCount * 2;
		break;
	case D3DPT_LINESTRIP:
		output = primtiveCount + 1;
		break;
	case D3DPT_TRIANGLELIST:
		output = primtiveCount * 3;
		break;
	case D3DPT_TRIANGLESTRIP:
		output = primtiveCount + 2;
		break;
	case D3DPT_TRIANGLEFAN:
		output = primtiveCount + 2;
		break;
	default:
		output = primtiveCount;
		break;
	}

	return output;
}

inline void SetCulling(VkPipelineRasterizationStateCreateInfo& pipelineRasterizationStateCreateInfo, D3DCULL input)
{
	switch (input)
	{
	case D3DCULL_NONE:
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		break;
	case D3DCULL_CW:
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		break;
	case D3DCULL_CCW:
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	default:
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	}
}

inline VkSamplerAddressMode ConvertTextureAddress(D3DTEXTUREADDRESS input)
{
	VkSamplerAddressMode output;

	switch (input)
	{
	case D3DTADDRESS_WRAP:
		output = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case D3DTADDRESS_MIRROR:
		output = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;
	case D3DTADDRESS_CLAMP:
		output = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	case D3DTADDRESS_BORDER:
		output = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	case D3DTADDRESS_MIRRORONCE:
		output = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		break;
	default:
		output = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	}

	return output;
}

inline VkSamplerMipmapMode ConvertMipmapMode(D3DTEXTUREFILTERTYPE input)
{
	VkSamplerMipmapMode output;

	switch (input)
	{
	case D3DTEXF_NONE:	// filtering disabled (valid for mip filter only)
		output = VK_SAMPLER_MIPMAP_MODE_NEAREST; //revisit
		break;
	case D3DTEXF_POINT:	// nearest
		output = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case D3DTEXF_LINEAR:	// linear interpolation
		output = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case D3DTEXF_ANISOTROPIC:	// anisotropic
		output = VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	case D3DTEXF_PYRAMIDALQUAD:	// 4-sample tent
		output = VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	case D3DTEXF_GAUSSIANQUAD:	// 4-sample Gaussian
		output = VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	default:
		output = VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	}

	return output;
}

inline VkFilter ConvertFilter(D3DTEXTUREFILTERTYPE input)
{
	VkFilter output;

	switch (input)
	{
	case D3DTEXF_NONE:	// filtering disabled (valid for mip filter only)
		output = VK_FILTER_NEAREST; //revisit
		break;
	case D3DTEXF_POINT:	// nearest
		output = VK_FILTER_NEAREST;
		break;
	case D3DTEXF_LINEAR:	// linear interpolation
		output = VK_FILTER_LINEAR;
		break;
	case D3DTEXF_ANISOTROPIC:	// anisotropic
		output = VK_FILTER_CUBIC_IMG; //revisit
		break;
	case D3DTEXF_PYRAMIDALQUAD:	// 4-sample tent
		output = VK_FILTER_CUBIC_IMG; //revisit
		break;
	case D3DTEXF_GAUSSIANQUAD:	// 4-sample Gaussian
		output = VK_FILTER_CUBIC_IMG; //revisit
		break;
	default:
		output = VK_FILTER_NEAREST; //revisit
		break;
	}

	return output;
}

inline VkPolygonMode ConvertFillMode(D3DFILLMODE input)
{
	VkPolygonMode output;

	switch (input)
	{
	case D3DFILL_SOLID:
		output = VK_POLYGON_MODE_FILL;
		break;
	case D3DFILL_POINT:
		output = VK_POLYGON_MODE_POINT;
		break;
	case D3DFILL_WIREFRAME:	
		output = VK_POLYGON_MODE_LINE;
		break;
	default:
		output = VK_POLYGON_MODE_POINT;
		break;
	}

	return output;
}

inline VkPrimitiveTopology ConvertPrimitiveType(D3DPRIMITIVETYPE input)
{
	VkPrimitiveTopology output;

	switch (input)
	{
	case D3DPT_POINTLIST:
		output = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;
	case D3DPT_LINELIST:
		output = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;
	case D3DPT_LINESTRIP:
		output = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		break;
	case D3DPT_TRIANGLELIST:
		output = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	case D3DPT_TRIANGLESTRIP:
		output = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
	case D3DPT_TRIANGLEFAN:
		output = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		break;
	default:
		output = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	}

	/* Types D3d9 doesn't support
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
		VK_PRIMITIVE_TOPOLOGY_PATCH_LIST = 10,
	*/

	return output;
}

inline bool GetMemoryTypeFromProperties(const VkPhysicalDeviceMemoryProperties& deviceMemoryProperties, uint32_t typeBits,VkFlags requirements_mask,uint32_t *typeIndex)
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
			{
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

inline VkFormat ConvertFormat(D3DFORMAT format)
{
	/*
	https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkFormat
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172558(v=vs.85).aspx
	*/
	switch (format)
	{
	case D3DFMT_UNKNOWN:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_R8G8B8:
		return VK_FORMAT_R8G8B8_UNORM;
	case D3DFMT_A8R8G8B8:
		return VK_FORMAT_B8G8R8A8_UNORM;
	case D3DFMT_X8R8G8B8:
		/*
		There is technically no alpha so this may cause problems but I don't see anything that is a 100% match for this format in Vulkan.
		*/
		return VK_FORMAT_B8G8R8A8_UNORM; //B8G8R8X8_UNORM
	case D3DFMT_R5G6B5:
		return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case D3DFMT_X1R5G5B5:
		return VK_FORMAT_UNDEFINED; //B5G5R5X1_UNORM
	case D3DFMT_A1R5G5B5:
		return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case D3DFMT_A4R4G4B4:
		return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case D3DFMT_R3G3B2:
		return VK_FORMAT_UNDEFINED; //B2G3R3_UNORM:
	case D3DFMT_A8:
		return VK_FORMAT_UNDEFINED; //A8_UNORM
	case D3DFMT_A8R3G3B2:
		return VK_FORMAT_UNDEFINED; //B2G3R3A8_UNORM
	case D3DFMT_X4R4G4B4:
		return VK_FORMAT_UNDEFINED; //B4G4R4X4_UNORM
	case D3DFMT_A2B10G10R10:
		return VK_FORMAT_UNDEFINED; //R10G10B10A2_UNORM
	case D3DFMT_A8B8G8R8:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_X8B8G8R8:
		return VK_FORMAT_UNDEFINED; //R8G8B8X8_UNORM
	case D3DFMT_G16R16:
		return VK_FORMAT_R16G16_UNORM;
	case D3DFMT_A2R10G10B10:
		return VK_FORMAT_UNDEFINED; //B10G10R10A2_UNORM
	case D3DFMT_A16B16G16R16:
		return VK_FORMAT_R16G16B16A16_UNORM;
	case D3DFMT_A8P8:
		return VK_FORMAT_UNDEFINED; //P8_UINT_A8_UNORM
	case D3DFMT_P8:
		return VK_FORMAT_UNDEFINED; //P8_UINT
	case D3DFMT_L8:
		return VK_FORMAT_UNDEFINED; //L8_UNORM
	case D3DFMT_A8L8:
		return VK_FORMAT_UNDEFINED; //L8A8_UNORM
	case D3DFMT_A4L4:
		return VK_FORMAT_UNDEFINED; //L4A4_UNORM
	case D3DFMT_V8U8:
		return VK_FORMAT_R8G8_SNORM;
	case D3DFMT_L6V5U5:
		return VK_FORMAT_UNDEFINED; //R5G5_SNORM_L6_UNORM
	case D3DFMT_X8L8V8U8:
		return VK_FORMAT_UNDEFINED; //R8G8_SNORM_L8X8_UNORM
	case D3DFMT_Q8W8V8U8:
		return VK_FORMAT_R8G8B8A8_SNORM;
	case D3DFMT_V16U16:
		return VK_FORMAT_R16G16_SNORM;
	case D3DFMT_A2W10V10U10:
		return VK_FORMAT_UNDEFINED; //R10G10B10_SNORM_A2_UNORM
	case D3DFMT_UYVY:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_R8G8_B8G8:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_YUY2:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_G8R8_G8B8:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_DXT1:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_DXT2:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_DXT3:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_DXT4:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_DXT5:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_D16_LOCKABLE:
		return VK_FORMAT_UNDEFINED; //D16_LOCKABLE
	case D3DFMT_D32:
		return VK_FORMAT_UNDEFINED; //D32_UNORM
	case D3DFMT_D15S1:
		return VK_FORMAT_UNDEFINED; //S1_UINT_D15_UNORM
	case D3DFMT_D24S8:
		return VK_FORMAT_D24_UNORM_S8_UINT;
	case D3DFMT_D24X8:
		return VK_FORMAT_UNDEFINED; //X8D24_UNORM
	case D3DFMT_D24X4S4:
		return VK_FORMAT_UNDEFINED; //S4X4_UINT_D24_UNORM
	case D3DFMT_D16:
		return VK_FORMAT_D16_UNORM;
	case D3DFMT_D32F_LOCKABLE:
		return VK_FORMAT_D32_SFLOAT;
	case D3DFMT_D24FS8:
		return VK_FORMAT_UNDEFINED; //S8_UINT_D24_SFLOAT
#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_D32_LOCKABLE:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_S8_LOCKABLE:
		return VK_FORMAT_UNDEFINED;
#endif // !D3D_DISABLE_9EX
	case D3DFMT_L16:
		return VK_FORMAT_UNDEFINED; //L16_UNORM
	case D3DFMT_VERTEXDATA:
		return VK_FORMAT_UNDEFINED; //VERTEXDATA
	case D3DFMT_INDEX16:
		return VK_FORMAT_R16_UINT;
	case D3DFMT_INDEX32:
		return VK_FORMAT_R32_UINT;
	case D3DFMT_Q16W16V16U16:
		return VK_FORMAT_R16G16B16A16_SNORM;
	case D3DFMT_MULTI2_ARGB8:
		return VK_FORMAT_UNDEFINED;
	case D3DFMT_R16F:
		return VK_FORMAT_R16_SFLOAT;
	case D3DFMT_G16R16F:
		return VK_FORMAT_R16G16_SFLOAT;
	case D3DFMT_A16B16G16R16F:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case D3DFMT_R32F:
		return VK_FORMAT_R32_SFLOAT;
	case D3DFMT_G32R32F:
		return VK_FORMAT_R32G32_SFLOAT;
	case D3DFMT_A32B32G32R32F:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case D3DFMT_CxV8U8:
		return VK_FORMAT_R8G8_SNORM;
#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_A1:
			return VK_FORMAT_UNDEFINED;
	case D3DFMT_A2B10G10R10_XR_BIAS:
			return VK_FORMAT_UNDEFINED;
	case D3DFMT_BINARYBUFFER:
			return VK_FORMAT_UNDEFINED;
#endif // !D3D_DISABLE_9EX
	case D3DFMT_FORCE_DWORD:
			return VK_FORMAT_UNDEFINED;
	default:
		return VK_FORMAT_UNDEFINED;
	}
}


inline D3DFORMAT ConvertFormat(VkFormat format)
{
	/*
	https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkFormat
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172558(v=vs.85).aspx
	*/
	switch (format)
	{
	case VK_FORMAT_UNDEFINED:
		return D3DFMT_UNKNOWN;
	case VK_FORMAT_R8G8B8_UNORM:
		return D3DFMT_R8G8B8;
	case VK_FORMAT_B8G8R8A8_UNORM:
		return D3DFMT_A8R8G8B8;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		return D3DFMT_R5G6B5;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		return D3DFMT_A1R5G5B5;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		return D3DFMT_A4R4G4B4;
	case VK_FORMAT_R8G8B8A8_UNORM:
		return D3DFMT_A8B8G8R8;
	case VK_FORMAT_R16G16_UNORM:
		return D3DFMT_G16R16;
	case VK_FORMAT_R16G16B16A16_UNORM:
		return D3DFMT_A16B16G16R16;
	case VK_FORMAT_R8G8_SNORM:
		return D3DFMT_V8U8;
	case VK_FORMAT_R8G8B8A8_SNORM:
		return D3DFMT_Q8W8V8U8;
	case VK_FORMAT_R16G16_SNORM:
		return D3DFMT_V16U16;
	case VK_FORMAT_D24_UNORM_S8_UINT:
		return D3DFMT_D24S8;
	case VK_FORMAT_D16_UNORM:
		return D3DFMT_D16;
	case VK_FORMAT_D32_SFLOAT:
		return D3DFMT_D32F_LOCKABLE;
	case VK_FORMAT_R16_UINT:
		return D3DFMT_INDEX16;
	case VK_FORMAT_R32_UINT:
		return D3DFMT_INDEX32;
	case VK_FORMAT_R16G16B16A16_SNORM:
		return D3DFMT_Q16W16V16U16;
	case VK_FORMAT_R16_SFLOAT:
		return D3DFMT_R16F;
	case VK_FORMAT_R16G16_SFLOAT:
		return D3DFMT_G16R16F;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return D3DFMT_A16B16G16R16F;
	case VK_FORMAT_R32_SFLOAT:
		return D3DFMT_R32F;
	case VK_FORMAT_R32G32_SFLOAT:
		return D3DFMT_G32R32F;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return D3DFMT_A32B32G32R32F;
	default:
		return D3DFMT_UNKNOWN;
	}
}

inline VkBlendOp ConvertColorOperation(D3DTEXTUREOP  input)
{
	VkBlendOp output; //TODO: review there are only two that match this doesn't seem right.

	switch (input)
	{
	case D3DTOP_DISABLE:
		output = VK_BLEND_OP_MAX_ENUM;
		break;
	case D3DTOP_SELECTARG1:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_SELECTARG2:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MODULATE:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MODULATE2X:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MODULATE4X:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_ADD:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_ADDSIGNED:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_ADDSIGNED2X:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_SUBTRACT:
		output = VK_BLEND_OP_SUBTRACT;
		break;
	case D3DTOP_ADDSMOOTH:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_BLENDDIFFUSEALPHA:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_BLENDTEXTUREALPHA:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_BLENDFACTORALPHA:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_BLENDTEXTUREALPHAPM:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_BLENDCURRENTALPHA:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_PREMODULATE:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MODULATEALPHA_ADDCOLOR:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MODULATECOLOR_ADDALPHA:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_BUMPENVMAP:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_BUMPENVMAPLUMINANCE:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_DOTPRODUCT3:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_MULTIPLYADD:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_LERP:
		output = VK_BLEND_OP_ADD;
		break;
	case D3DTOP_FORCE_DWORD:
		output = VK_BLEND_OP_MAX_ENUM;
		break;
	default:
		output = VK_BLEND_OP_ADD;
		break;
	}

	return output;
}

inline VkBlendOp ConvertColorOperation(DWORD  input)
{
	return ConvertColorOperation((D3DTEXTUREOP)input);
}

inline void Print(std::unordered_map<D3DTRANSFORMSTATETYPE, D3DMATRIX>& transforms)
{
	BOOST_FOREACH(const auto& pair1, transforms)
	{
		BOOST_LOG_TRIVIAL(info) << pair1.first;
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._11 << "," << pair1.second._12 << "," << pair1.second._13 << "," << pair1.second._14 << "}";
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._21 << "," << pair1.second._22 << "," << pair1.second._23 << "," << pair1.second._24 << "}";
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._31 << "," << pair1.second._32 << "," << pair1.second._33 << "," << pair1.second._34 << "}";
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._41 << "," << pair1.second._42 << "," << pair1.second._43 << "," << pair1.second._44 << "}";
		BOOST_LOG_TRIVIAL(info) << "";
	}
}

inline void Print(DeviceState& deviceState)
{
	//IDirect3DDevice9::LightEnable
	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette
	//IDirect3DDevice9::SetFVF
	BOOST_LOG_TRIVIAL(info) << "FVF: " << deviceState.mFVF;
	BOOST_LOG_TRIVIAL(info) << "FVFHasPosition: " << deviceState.mFVFHasPosition;
	BOOST_LOG_TRIVIAL(info) << "FVFHasNormal: " << deviceState.mFVFHasNormal;
	BOOST_LOG_TRIVIAL(info) << "FVFHasColor: " << deviceState.mFVFHasColor;
	BOOST_LOG_TRIVIAL(info) << "FVFTextureCount: " << deviceState.mFVFTextureCount;

	//IDirect3DDevice9::SetIndices
	BOOST_LOG_TRIVIAL(info) << "IndexBuffer: " << deviceState.mIndexBuffer;

	//IDirect3DDevice9::SetLight
	//IDirect3DDevice9::SetMaterial
	//IDirect3DDevice9::SetNPatchMode
	BOOST_LOG_TRIVIAL(info) << "NSegments: " << deviceState.mNSegments;

	//IDirect3DDevice9::SetPixelShader
	BOOST_LOG_TRIVIAL(info) << "PixelShader: " << deviceState.mPixelShader;

	//IDirect3DDevice9::SetPixelShaderConstantB
	//IDirect3DDevice9::SetPixelShaderConstantF
	//IDirect3DDevice9::SetPixelShaderConstantI
	//IDirect3DDevice9::SetRenderState
	BOOST_FOREACH(const auto& pair1, deviceState.mRenderStates)
	{
		BOOST_LOG_TRIVIAL(info) << "RenderState: " << pair1.first << " " << pair1.second;
	}

	//IDirect3DDevice9::SetSamplerState
	BOOST_FOREACH(const auto& pair1, deviceState.mSamplerStates)
	{
		BOOST_FOREACH(const auto& pair2, pair1.second)
		{
			BOOST_LOG_TRIVIAL(info) << "SamplerState: " << pair1.first << " " << pair2.first << " " << pair2.second;
		}
	}

	//IDirect3DDevice9::SetScissorRect
	BOOST_LOG_TRIVIAL(info) << "Scissor X: " << deviceState.mScissor.offset.x;
	BOOST_LOG_TRIVIAL(info) << "Scissor Y: " << deviceState.mScissor.offset.y;
	BOOST_LOG_TRIVIAL(info) << "Scissor Width: " << deviceState.mScissor.extent.width;
	BOOST_LOG_TRIVIAL(info) << "Scissor Height: " << deviceState.mScissor.extent.height;

	//IDirect3DDevice9::SetStreamSource
	BOOST_FOREACH(const auto& pair1, deviceState.mStreamSources)
	{
		const StreamSource& streamSource = pair1.second;
		BOOST_LOG_TRIVIAL(info) << "StreamSource: {StreamNumber: " << streamSource.StreamNumber << ", Stride: " << streamSource.Stride << ", StreamData: " << streamSource.StreamData << ", OffsetInBytes: " << streamSource.OffsetInBytes << "}";
	}

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	for (size_t i = 0; i < 16; i++)
	{
		VkDescriptorImageInfo& sampler = deviceState.mDescriptorImageInfo[i];

		BOOST_LOG_TRIVIAL(info) << "Sampler: {imageLayout: " << sampler.imageLayout << ", imageView: " << sampler.imageView << ", sampler: " << sampler.sampler << "}";
	}

	//IDirect3DDevice9::SetTextureStageState
	BOOST_FOREACH(const auto& pair1, deviceState.mTextureStageStates)
	{
		BOOST_FOREACH(const auto& pair2, pair1.second)
		{
			BOOST_LOG_TRIVIAL(info) << "TextureStageState: " << pair1.first << " " << pair2.first << " " << pair2.second;
		}
	}

	//IDirect3DDevice9::SetTransform
	BOOST_FOREACH(const auto& pair1, deviceState.mTransforms)
	{
		BOOST_LOG_TRIVIAL(info) << pair1.first;
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._11 << "," << pair1.second._12 << "," << pair1.second._13 << "," << pair1.second._14 << "}";
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._21 << "," << pair1.second._22 << "," << pair1.second._23 << "," << pair1.second._24 << "}";
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._31 << "," << pair1.second._32 << "," << pair1.second._33 << "," << pair1.second._34 << "}";
		BOOST_LOG_TRIVIAL(info) << "{" << pair1.second._41 << "," << pair1.second._42 << "," << pair1.second._43 << "," << pair1.second._44 << "}";
		BOOST_LOG_TRIVIAL(info) << "";
	}

	//IDirect3DDevice9::SetViewport
	BOOST_LOG_TRIVIAL(info) << "Viewport X: " << deviceState.mViewport.x;
	BOOST_LOG_TRIVIAL(info) << "Viewport Y: " << deviceState.mViewport.y;
	BOOST_LOG_TRIVIAL(info) << "Viewport Width: " << deviceState.mViewport.width;
	BOOST_LOG_TRIVIAL(info) << "Viewport Height: " << deviceState.mViewport.height;

	//IDirect3DDevice9::SetVertexDeclaration
	BOOST_LOG_TRIVIAL(info) << "VertexDeclaration: " << deviceState.mVertexDeclaration;

	//IDirect3DDevice9::SetVertexShader
	BOOST_LOG_TRIVIAL(info) << "VertexShader: " << deviceState.mVertexShader;

	//IDirect3DDevice9::SetVertexShaderConstantB
	//IDirect3DDevice9::SetVertexShaderConstantF
	//IDirect3DDevice9::SetVertexShaderConstantI
}

/*
d3d9 has a 32bit format where alpha is ignored but so far vulkan does not so to handle that I need to set alpha to be opacity
*/
inline void SetAlpha(char* imageData, uint32_t height, uint32_t width, uint32_t rowPitch)
{
	for (uint32_t y = 0; y < height; y++)
	{
		color_A8R8G8B8 *row = (color_A8R8G8B8*)imageData;
		for (uint32_t x = 0; x < width; x++)
		{
			row->A = 0xFF;

			row++;
		}
		imageData += rowPitch;
	}
}

inline void SaveImage(const char *filename,char* imageData,uint32_t height, uint32_t width, uint32_t rowPitch)
{
	std::ofstream file(filename, std::ios::out | std::ios::binary);

	// Write the header
	file << "P6\n" << width << "\n" << height << "\n" << 255 << "\n";

	// Write the pixel data.
	for (uint32_t y = 0; y < height; y++)
	{
		unsigned int *row = (unsigned int*)imageData;
		for (uint32_t x = 0; x < width; x++)
		{
			file.write((char*)row, 3);
			row++;
		}
		imageData += rowPitch;
	}

	file.close();
}

#endif // UTILITIES_H
