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

layout(std140,binding = 0) uniform LightBlock
{
	Light lights[lightCount];
};

layout(binding = 1) uniform MaterialBlock
{
	Material material;
};

layout(push_constant) uniform UniformBufferObject {
    mat4 totalTransformation;
	mat4 modelTransformation;
} ubo;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 attr1; //normal
layout (location = 2) in uvec4 attr2; //color

layout (location = 0) out vec4 diffuseColor;
layout (location = 1) out vec4 ambientColor;
layout (location = 2) out vec4 specularColor;
layout (location = 3) out vec4 emissiveColor;
layout (location = 4) out vec4 normal;
layout (location = 7) out vec4 pos;
layout (location = 8) out vec4 globalIllumination;

out gl_PerVertex 
{
	vec4 gl_Position;
};

/*
https://msdn.microsoft.com/en-us/library/windows/desktop/bb172256(v=vs.85).aspx
*/
vec4 GetGlobalIllumination()
{
	vec4 ambient = vec4(0);
	vec4 diffuse = vec4(0);
	vec4 specular = vec4(0);
	vec4 emissive = vec4(0);	
	vec4 attenuationTemp = vec4(0);
	vec4 diffuseTemp = vec4(0);
	vec4 specularTemp = vec4(0);
	vec3 cameraPosition = vec3(0);

	float lightDistance = 0;
	vec4 vectorPosition = vec4(0);
	vec4 lightPosition = vec4(0);
	vec4 lightDirection = vec4(0);
	float attenuation = 0;
	vec4 ldir = vec4(0);
	float rho = 0;
	float spot = 0;

	normal = ubo.modelTransformation * vec4(attr1,0);
	normal = normalize(normal);
	//normal *= vec4(1.0,-1.0,1.0,1.0);

	vectorPosition = ubo.modelTransformation * vec4(position,1.0);
	vectorPosition *= vec4(1.0,-1.0,1.0,1.0);

	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb172279(v=vs.85).aspx
	for( int i=0; i<lightCount; ++i )
	{		
		if(lights[i].IsEnabled)
		{		
			lightPosition = ubo.modelTransformation * lights[i].Position;
			lightPosition *= vec4(1.0,-1.0,1.0,1.0);

			lightDirection = lights[i].Direction;
			//lightDirection *= vec4(1.0,-1.0,1.0,1.0);

			lightDistance = abs(distance(pos.xyz,lightPosition.xyz));

			if(lights[i].Type == D3DLIGHT_DIRECTIONAL)
			{
				ldir = normalize(lightDirection * vec4(-1.0,-1.0,-1.0,-1.0));
			}
			else
			{
				ldir = normalize(lightPosition - vectorPosition);
			}

			if(lights[i].Type == D3DLIGHT_DIRECTIONAL)
			{
				attenuation = 1;
			}
			else if(lights[i].Range < lightDistance)
			{
				attenuation = 0;
			}
			else
			{
				attenuation = 1/( lights[i].Attenuation0 + lights[i].Attenuation1 * lightDistance + lights[i].Attenuation2 * pow(lightDistance,2));	
			}

			rho = dot(normalize(lightDirection.xyz),normalize(lightDirection.xyz));

			if(lights[i].Type != D3DLIGHT_SPOT || rho > cos(lights[i].Theta/2))
			{
				spot = 1;
			}
			else if(rho < cos(lights[i].Phi/2))
			{
				spot = 0;
			}
			else
			{
				spot = ((rho - cos(lights[i].Phi / 2)) / (cos(lights[i].Theta / 2) - cos(lights[i].Phi / 2))) * lights[i].Falloff;
			}

			attenuationTemp += (attenuation * spot * lights[i].Ambient);
			diffuseTemp += (diffuseColor * lights[i].Diffuse * max(dot(normal,ldir),0.0) * attenuation * spot);

			if(specularEnable)
			{
				specularTemp += (lights[i].Specular * pow(max(dot(normal.xyz, normalize(normalize(cameraPosition - pos.xyz) + ldir.xyz)),0.0),material.Power) * attenuation * spot);
			}
		}
	}

	ambient = material.Ambient * (Convert(globalAmbient) + attenuationTemp);
	diffuse = diffuseTemp;
	emissive = material.Emissive;

	if(specularEnable)
	{
		specular = specularColor * specularTemp;
	}


	return (ambient + diffuse + specular + emissive);
}

void main() 
{
	gl_Position = ubo.totalTransformation * vec4(position,1.0);
	gl_Position *= vec4(1.0,-1.0,1.0,1.0);
	pos = gl_Position;

	switch(diffuseMaterialSource)
	{
		case D3DMCS_MATERIAL:
			diffuseColor = material.Diffuse;
		break;
		case D3DMCS_COLOR1:
			diffuseColor = Convert(attr2);
		break;
		case D3DMCS_COLOR2:
			diffuseColor = vec4(0);
		break;
		default:
			diffuseColor = vec4(0);
		break;
	}

	switch(ambientMaterialSource)
	{
		case D3DMCS_MATERIAL:
			ambientColor = material.Ambient;
		break;
		case D3DMCS_COLOR1:
			ambientColor = material.Ambient;
		break;
		case D3DMCS_COLOR2:
			ambientColor = material.Ambient;
		break;
		default:
			ambientColor = material.Ambient;
		break;
	}

	switch(specularMaterialSource)
	{
		case D3DMCS_MATERIAL:
			specularColor = material.Specular;
		break;
		case D3DMCS_COLOR1:
			specularColor = material.Specular;
		break;
		case D3DMCS_COLOR2:
			specularColor = material.Specular;
		break;
		default:
			specularColor = material.Specular;
		break;
	}

	switch(emissiveMaterialSource)
	{
		case D3DMCS_MATERIAL:
			emissiveColor = material.Emissive;
		break;
		case D3DMCS_COLOR1:
			emissiveColor = material.Emissive;
		break;
		case D3DMCS_COLOR2:
			emissiveColor = material.Emissive;
		break;
		default:
			emissiveColor = material.Emissive;
		break;
	}

	globalIllumination = GetGlobalIllumination();
}
