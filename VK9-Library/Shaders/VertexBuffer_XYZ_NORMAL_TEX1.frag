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

layout(std140,binding = 1) uniform LightBlock
{
	Light lights[lightCount];
};

layout(binding = 2) uniform MaterialBlock
{
	Material material;
};

layout(push_constant) uniform UniformBufferObject {
    mat4 totalTransformation;
	mat4 modelTransformation;
} ubo;

layout(binding = 0) uniform sampler2D textures[1];

layout (location = 0) in vec4 diffuseColor;
layout (location = 1) in vec4 ambientColor;
layout (location = 2) in vec4 specularColor;
layout (location = 3) in vec4 emissiveColor;
layout (location = 4) in vec4 normal;
layout (location = 5) in vec2 texcoord;
layout (location = 6) in vec4 pos;
layout (location = 7) in vec4 globalIllumination;
layout (location = 0) out vec4 uFragColor;

vec2 getTextureCoord(int index)
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

vec4 getStageArgument(int argument,vec4 temp,int constant,vec4 result,sampler2D tex,vec2 texcoord)
{
	switch(argument)
	{
		case D3DTA_CONSTANT:
			return Convert(constant);
		break;
		case D3DTA_CURRENT:
			return result;
		break;
		case D3DTA_DIFFUSE:
			return diffuseColor;
		break;
		case D3DTA_SELECTMASK:
			return vec4(0);
		break;
		case D3DTA_SPECULAR:
			return vec4(0);
		break;
		case D3DTA_TEMP:
			return temp;
		break;
		case D3DTA_TEXTURE:
			return texture(tex, texcoord.xy);
		break;
		case D3DTA_TFACTOR:
			return vec4(0);
		break;
		default:
			return vec4(0);
		break;
	}
}

//https://msdn.microsoft.com/en-us/library/windows/desktop/bb172616(v=vs.85).aspx
vec4 calculateResult(int operation, vec4 argument1, vec4 argument2, vec4 argument0, float alpha, float factorAlpha)
{
	vec4 result = vec4(1.0,1.0,1.0,1.0);

	switch(operation)
	{
		case D3DTOP_DISABLE:
		break;
		case D3DTOP_SELECTARG1:
			result = argument1;
		break;
		case D3DTOP_SELECTARG2:
			result = argument2;
		break;
		case D3DTOP_MODULATE:
			result = argument1 * argument2;
		break;
		case D3DTOP_MODULATE2X:
			result = (argument1 * argument2) * 2;
		break;
		case D3DTOP_MODULATE4X:
			result = (argument1 * argument2) * 4;
		break;
		case D3DTOP_ADD:
			result = argument1 + argument2;
		break;
		case D3DTOP_ADDSIGNED:
			result = argument1 + argument2 - 0.5;
		break;
		case D3DTOP_ADDSIGNED2X:
			result = (argument1 + argument2 - 0.5) * 2;
		break;
		case D3DTOP_SUBTRACT:
			result = argument1 - argument2;
		break;
		case D3DTOP_ADDSMOOTH:
			result = argument1 + argument2 - argument1 * argument2;
		break;
		case D3DTOP_BLENDDIFFUSEALPHA:
			result = argument1 * (alpha) + argument2 * (1 - alpha);
		break;
		case D3DTOP_BLENDTEXTUREALPHA:
			result = argument1 * (alpha) + argument2 * (1 - alpha);
		break;  
		case D3DTOP_BLENDFACTORALPHA:
			result = argument1 * (factorAlpha) + argument2 * (1 - factorAlpha);
		break;
		case D3DTOP_BLENDTEXTUREALPHAPM:
			result = argument1 + argument2 * (1 - alpha);
		break;
		case D3DTOP_BLENDCURRENTALPHA:
			result = argument1 * (alpha) + argument2 * (1 - alpha);
		break;
		case D3DTOP_PREMODULATE:
			result = argument1; //TODO figure out n+1 logic.
		break;
		case D3DTOP_MODULATEALPHA_ADDCOLOR:
			result.rgb = argument1.rgb + argument1.a * argument2.rgb;
		break;
		case D3DTOP_MODULATECOLOR_ADDALPHA:
			result.rgb = argument1.rgb * argument2.rgb + argument1.a;
		break;
		case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
			result.rgb = (1 - argument1.a) * argument2.rgb + argument1.rgb;
		break;
		case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
			result.rgb = (1 - argument1.rgb) * argument2.rgb + argument1.a;
		break;
		case D3DTOP_BUMPENVMAP:
			//TODO: figure out per-pixel bump mapping.
		break;
		case D3DTOP_BUMPENVMAPLUMINANCE:
			//TODO: figure out per-pixel bump mapping.
		break;
		case D3DTOP_DOTPRODUCT3:
			//result.a = (argument1.r * argument2.r) + (argument1.g * argument2.g) + (argument1.b * argument2.b);

			result.a = 4*((argument1.r - 0.5)*(argument2.r - 0.5) + (argument1.g - 0.5)*(argument2.g - 0.5) + (argument1.b - 0.5)*(argument2.b - 0.5));
			result.r = result.a;
			result.g = result.a;
			result.b = result.a;
		break;
		case D3DTOP_MULTIPLYADD:
			result = argument0 + argument1 * argument2;
		break;
		case D3DTOP_LERP:
			result = (argument0) * argument1 + (1 - argument0) * argument2;
		break;
		default:
			//Nothing
		break;
	}

	return result;
}

void processStage(sampler2D tex,int textureIndex, int constant, int resultArgument,
vec4 resultIn, vec4 tempIn, out vec4 resultOut, out vec4 tempOut,
int colorOperation, int colorArgument1, int colorArgument2, int colorArgument0,
int alphaOperation, int alphaArgument1, int alphaArgument2, int alphaArgument0)
{
	vec4 temp = tempIn;
	vec4 result = resultIn;
	vec4 tempResult = vec4(1); //This is the result regardless if selected target.
	vec2 texcoord = getTextureCoord(textureIndex);

	vec4 colorArg1 = getStageArgument(colorArgument1,tempIn,constant,resultIn, tex, texcoord);
	vec4 colorArg2 = getStageArgument(colorArgument2,tempIn,constant,resultIn, tex, texcoord);
	vec4 colorArg0 = getStageArgument(colorArgument0,tempIn,constant,resultIn, tex, texcoord);

	vec4 alphaArg1 = getStageArgument(alphaArgument1,tempIn,constant,resultIn, tex, texcoord);
	vec4 alphaArg2 = getStageArgument(alphaArgument2,tempIn,constant,resultIn, tex, texcoord);
	vec4 alphaArg0 = getStageArgument(alphaArgument0,tempIn,constant,resultIn, tex, texcoord);

	if(alphaBlendEnable)
	{
		//TODO: review alpha factor logic.
		tempResult.a = calculateResult(alphaOperation,alphaArg1,alphaArg2,alphaArg0,1.0,1.0).a;
	}
	else
	{
		tempResult.a = 1.0;
	}

	if(colorOperation != D3DTOP_DISABLE)
	{
		tempResult.rgb = calculateResult(colorOperation,colorArg1,colorArg2,colorArg0,tempResult.a,tempResult.a).rgb;	
	}	

	switch(resultArgument)
	{
		case D3DTA_CURRENT:
			result = tempResult;
		break;
		case D3DTA_TEMP:
			temp = tempResult;
		break;
		default:
			result = tempResult;
		break;
	}

	resultOut = result;
	tempOut = temp;
}

void main() 
{
	vec4 temp;
	vec4 result = vec4(1.0,1.0,1.0,1.0); //On stage 0 CURRENT is the same as DIFFUSE

	if(textureCount>0)
	{
		processStage(textures[0],texureCoordinateIndex_0, Constant_0, Result_0,
		result, temp, result, temp,
		colorOperation_0, colorArgument1_0, colorArgument2_0, colorArgument0_0,
		alphaOperation_0, alphaArgument1_0, alphaArgument2_0, alphaArgument0_0);
	}

	uFragColor = result;
	
	if(lighting)
	{	
		if(shadeMode == D3DSHADE_GOURAUD)
		{
			uFragColor.rgb *= globalIllumination.rgb;
		}
	}
}
