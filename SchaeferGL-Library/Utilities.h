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

#include "d3d9.h" // Base class: IDirect3DDevice9
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>

inline VkFormat ConvertFormat(D3DFORMAT format)
{
	/*
	https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkFormat
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172558(v=vs.85).aspx
	*/
	switch (format)
	{
		D3DFMT_UNKNOWN:
	D3DFMT_R8G8B8:
		return VK_FORMAT_R8G8B8_UNORM;
	D3DFMT_A8R8G8B8:
		return VK_FORMAT_B8G8R8A8_UNORM;
	D3DFMT_X8R8G8B8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_R5G6B5:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_X1R5G5B5:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A1R5G5B5:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A4R4G4B4:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_R3G3B2:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A8R3G3B2:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_X4R4G4B4:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A2B10G10R10:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A8B8G8R8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_X8B8G8R8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_G16R16:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A2R10G10B10:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A16B16G16R16:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A8P8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_P8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_L8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A8L8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A4L4:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_V8U8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_L6V5U5:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_X8L8V8U8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_Q8W8V8U8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_V16U16:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A2W10V10U10:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_UYVY:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_R8G8_B8G8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_YUY2:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_G8R8_G8B8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_DXT1:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_DXT2:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_DXT3:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_DXT4:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_DXT5:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D16_LOCKABLE:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D32:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D15S1:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D24S8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D24X8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D24X4S4:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D16:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D32F_LOCKABLE:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_D24FS8:
		return VK_FORMAT_UNDEFINED;
#if !defined(D3D_DISABLE_9EX)
	D3DFMT_D32_LOCKABLE:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_S8_LOCKABLE:
		return VK_FORMAT_UNDEFINED;
#endif // !D3D_DISABLE_9EX
	D3DFMT_L16:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_VERTEXDATA:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_INDEX16:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_INDEX32:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_Q16W16V16U16:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_MULTI2_ARGB8:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_R16F:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_G16R16F:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A16B16G16R16F:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_R32F:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_G32R32F:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_A32B32G32R32F:
		return VK_FORMAT_UNDEFINED;
	D3DFMT_CxV8U8:
		return VK_FORMAT_UNDEFINED;
#if !defined(D3D_DISABLE_9EX)
		D3DFMT_A1:
			return VK_FORMAT_UNDEFINED;
		D3DFMT_A2B10G10R10_XR_BIAS:
			return VK_FORMAT_UNDEFINED;
		D3DFMT_BINARYBUFFER:
			return VK_FORMAT_UNDEFINED;
#endif // !D3D_DISABLE_9EX
		D3DFMT_FORCE_DWORD:
			return VK_FORMAT_UNDEFINED;
	default:
		return VK_FORMAT_UNDEFINED;
	}
}

#endif // UTILITIES_H
