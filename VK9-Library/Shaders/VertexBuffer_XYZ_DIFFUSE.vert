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

layout (location = 0) in vec4 position;
layout (location = 1) in uvec4 attr;

layout (location = 0) out vec4 diffuseColor;
layout (location = 1) out vec4 specularColor;

out gl_PerVertex 
{
        vec4 gl_Position;
};

#include "GlobalIllumination"

void main() 
{
	gl_Position = ubo.worldViewProjection * vec4(position.xyz,1.0);
	gl_Position *= vec4(1.0,-1.0,1.0,1.0);

	ColorPair color = CalculateGlobalIllumination(position, vec4(0.0), Convert(attr), vec4(0.0));

	diffuseColor = color.Diffuse;
	specularColor = color.Specular;
}
