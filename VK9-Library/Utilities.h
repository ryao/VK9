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

#ifndef UTILITIES_H
#define UTILITIES_H

#include "resource.h"
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
#include <cstring>

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

#endif // UTILITIES_H
