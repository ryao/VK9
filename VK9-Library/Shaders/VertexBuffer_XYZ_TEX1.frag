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

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "Constants"
#include "Structures"
#include "Functions"

layout(std140,binding = 0) uniform ShaderStateBlock0
{
	RenderState renderState;
};

layout(std140,binding = 1) uniform ShaderStateBlock1
{
	TextureStage textureStages[9];
};

layout(std140,binding = 2) uniform ShaderStateBlock2
{
	Light lights[8];
};

layout(std140,binding = 3) uniform ShaderStateBlock3
{
	Material material;
};

layout(binding = 6) uniform sampler2D textures[1];

layout(push_constant) uniform UniformBufferObject 
{
    mat4 totalTransformation;
	mat4 modelTransformation;
} ubo;

layout (location = 0) in vec4 diffuseColor;
layout (location = 1) in vec4 ambientColor;
layout (location = 2) in vec4 specularColor;
layout (location = 3) in vec4 emissiveColor;
layout (location = 4) in vec2 texcoord;
layout (location = 0) out vec4 uFragColor;

vec2 getTextureCoord(uint index)
{
	switch(index)
	{
		case 0:
			return texcoord;
		break;
		case 1:
			return vec2(0,0);
		break;
		default:
			return vec2(0,0);
		break;
	}
}

#include "FixedFunctions"

void main() 
{
	vec4 temp = vec4(1.0,1.0,1.0,1.0);
	vec4 result = vec4(1.0,1.0,1.0,1.0); //On stage 0 CURRENT is the same as DIFFUSE

	processStage(textures[0],textureStages[0].texureCoordinateIndex, textureStages[0].Constant, textureStages[0].Result,
	result, temp, result, temp,
	textureStages[0].colorOperation, textureStages[0].colorArgument1, textureStages[0].colorArgument2, textureStages[0].colorArgument0,
	textureStages[0].alphaOperation, textureStages[0].alphaArgument1, textureStages[0].alphaArgument2, textureStages[0].alphaArgument0);
	
	uFragColor = result;
}
