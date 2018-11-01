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

layout(std140,binding = 0) uniform ShaderStateBlock
{
	ShaderState shaderState;
};

layout(push_constant) uniform UniformBufferObject {
    mat4 totalTransformation;
	mat4 modelTransformation;
} ubo;

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 attr1;
layout (location = 2) in vec4 attr2;
layout (location = 0) out vec4 diffuseColor;
layout (location = 1) out vec4 ambientColor;
layout (location = 2) out vec4 specularColor;
layout (location = 3) out vec4 emissiveColor;
layout (location = 4) out vec2 texcoord1;
layout (location = 5) out vec2 texcoord2;

out gl_PerVertex 
{
        vec4 gl_Position;
};

void main() 
{
	gl_Position = ubo.totalTransformation * vec4(position.xyz,1.0);
	gl_Position *= vec4(1.0,-1.0,1.0,1.0);

	if(shaderState.mTextureStages[0].textureTransformationFlags == D3DTTFF_DISABLE)
	{
		texcoord1 = attr1.xy;
	}
	else
	{
		texcoord1 = (attr1 * shaderState.mTextureStages[0].textureTransformationMatrix).xy;
	}
	
	if(shaderState.mTextureStages[1].textureTransformationFlags == D3DTTFF_DISABLE)
	{
		texcoord2 = attr2.xy;
	}
	else
	{
		texcoord2 = (attr2 * shaderState.mTextureStages[1].textureTransformationMatrix).xy;
	}

	if(shaderState.mRenderState.colorVertex==1)
	{
		switch(shaderState.mRenderState.diffuseMaterialSource)
		{
			case D3DMCS_MATERIAL:
				diffuseColor = shaderState.mMaterial.Diffuse;
			break;
			case D3DMCS_COLOR1:
				diffuseColor = vec4(1.0);
			break;
			case D3DMCS_COLOR2:
				diffuseColor = vec4(1.0);
			break;
			default:
				diffuseColor = vec4(1.0);
			break;
		}
		
		switch(shaderState.mRenderState.ambientMaterialSource)
		{
			case D3DMCS_MATERIAL:
				ambientColor = shaderState.mMaterial.Ambient;
			break;
			case D3DMCS_COLOR1:
				ambientColor = vec4(1.0);
			break;
			case D3DMCS_COLOR2:
				ambientColor = vec4(1.0);
			break;
			default:
				ambientColor = vec4(1.0);
			break;
		}

		switch(shaderState.mRenderState.specularMaterialSource)
		{
			case D3DMCS_MATERIAL:
				specularColor = shaderState.mMaterial.Specular;
			break;
			case D3DMCS_COLOR1:
				specularColor = vec4(1.0);
			break;
			case D3DMCS_COLOR2:
				specularColor = vec4(1.0);
			break;
			default:
				specularColor = vec4(1.0);
			break;
		}

		switch(shaderState.mRenderState.emissiveMaterialSource)
		{
			case D3DMCS_MATERIAL:
				emissiveColor = shaderState.mMaterial.Emissive;
			break;
			case D3DMCS_COLOR1:
				emissiveColor = vec4(1.0);
			break;
			case D3DMCS_COLOR2:
				emissiveColor = vec4(1.0);
			break;
			default:
				emissiveColor = vec4(1.0);
			break;
		}		
	}
	else
	{
		diffuseColor = shaderState.mMaterial.Diffuse;
		ambientColor = shaderState.mMaterial.Ambient;
		specularColor = shaderState.mMaterial.Specular;
		emissiveColor = shaderState.mMaterial.Emissive;
	}
}