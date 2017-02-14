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

#ifndef SHADERCONVERTER_H
#define SHADERCONVERTER_H

//#include "d3d9.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include "d3d9types.h"

const uint32_t mEndToken = 0x0000FFFF;
const uint16_t mShaderTypePixel = 0xFFFF;
const uint16_t mShaderTypeVertex = 0xFFFE;

struct ConvertedShader
{
	UINT Size = 0;
	VkShaderModule ShaderModule = VK_NULL_HANDLE;
};

struct VersionToken
{
	char MinorVersion=0;
	char MajorVersion=0;
	uint16_t ShaderType; //Pixel 0xFFFF & Vertex 0xFFFE
};



class ShaderConverter
{

public:
	ConvertedShader Convert(DWORD* shader);
private:

};

#endif //SHADERCONVERTER_H
//
