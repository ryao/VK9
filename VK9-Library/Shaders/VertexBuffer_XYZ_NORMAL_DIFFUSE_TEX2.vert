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

#define D3DSHADE_FLAT 1
#define D3DSHADE_GOURAUD 2
#define D3DSHADE_PHONG 3

#define D3DLIGHT_POINT 1
#define D3DLIGHT_SPOT 2
#define D3DLIGHT_DIRECTIONAL 3

#define D3DTOP_DISABLE 1
#define D3DTOP_SELECTARG1 2
#define D3DTOP_SELECTARG2 3
#define D3DTOP_MODULATE 4
#define D3DTOP_MODULATE2X 5
#define D3DTOP_MODULATE4X 6
#define D3DTOP_ADD 7
#define D3DTOP_ADDSIGNED 8
#define D3DTOP_ADDSIGNED2X 9
#define D3DTOP_SUBTRACT 10
#define D3DTOP_ADDSMOOTH 11
#define D3DTOP_BLENDDIFFUSEALPHA 12
#define D3DTOP_BLENDTEXTUREALPHA 13
#define D3DTOP_BLENDFACTORALPHA 14
#define D3DTOP_BLENDTEXTUREALPHAPM 15
#define D3DTOP_BLENDCURRENTALPHA 16
#define D3DTOP_PREMODULATE 17
#define D3DTOP_MODULATEALPHA_ADDCOLOR 18
#define D3DTOP_MODULATECOLOR_ADDALPHA 19
#define D3DTOP_MODULATEINVALPHA_ADDCOLOR 20
#define D3DTOP_MODULATEINVCOLOR_ADDALPHA 21
#define D3DTOP_BUMPENVMAP 22
#define D3DTOP_BUMPENVMAPLUMINANCE 23
#define D3DTOP_DOTPRODUCT3 24
#define D3DTOP_MULTIPLYADD 25
#define D3DTOP_LERP 26

#define D3DTA_SELECTMASK        0x0000000f  // mask for arg selector
#define D3DTA_DIFFUSE           0x00000000  // select diffuse color (read only)
#define D3DTA_CURRENT           0x00000001  // select stage destination register (read/write)
#define D3DTA_TEXTURE           0x00000002  // select texture color (read only)
#define D3DTA_TFACTOR           0x00000003  // select D3DRS_TEXTUREFACTOR (read only)
#define D3DTA_SPECULAR          0x00000004  // select specular color (read only)
#define D3DTA_TEMP              0x00000005  // select temporary register color (read/write)
#define D3DTA_CONSTANT          0x00000006  // select texture stage constant
#define D3DTA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
#define D3DTA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)

#define D3DTTFF_DISABLE 0     // texture coordinates are passed directly
#define D3DTTFF_COUNT1 1      // rasterizer should expect 1-D texture coords
#define D3DTTFF_COUNT2 2      // rasterizer should expect 2-D texture coords
#define D3DTTFF_COUNT3 3      // rasterizer should expect 3-D texture coords
#define D3DTTFF_COUNT4 4      // rasterizer should expect 4-D texture coords
#define D3DTTFF_PROJECTED 256 // texcoords to be divided by COUNTth element

#define D3DTSS_TCI_PASSTHRU                             0x00000000
#define D3DTSS_TCI_CAMERASPACENORMAL                    0x00010000
#define D3DTSS_TCI_CAMERASPACEPOSITION                  0x00020000
#define D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR          0x00030000
#define D3DTSS_TCI_SPHEREMAP                            0x00040000

layout(constant_id = 0) const int lightCount = 1;
layout(constant_id = 1) const int reserved1 = 0;
layout(constant_id = 2) const int reserved2 = 0;
layout(constant_id = 3) const int textureCount = 2;

//Texture Stage _0
layout(constant_id = 4) const int Constant_0 = 0;
layout(constant_id = 5) const int Result_0 = D3DTA_CURRENT;
layout(constant_id = 6) const int textureTransformationFlags_0 = D3DTTFF_DISABLE;
layout(constant_id = 7) const int texureCoordinateIndex_0 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 8) const int colorOperation_0 = D3DTOP_MODULATE;
layout(constant_id = 9) const int colorArgument0_0 = D3DTA_CURRENT;
layout(constant_id = 10) const int colorArgument1_0 = D3DTA_TEXTURE;
layout(constant_id = 11) const int colorArgument2_0 = D3DTA_CURRENT;
layout(constant_id = 12) const int alphaOperation_0 = D3DTOP_SELECTARG1;
layout(constant_id = 13) const int alphaArgument0_0 = D3DTA_CURRENT;
layout(constant_id = 14) const int alphaArgument1_0 = D3DTA_TEXTURE;
layout(constant_id = 15) const int alphaArgument2_0 = D3DTA_CURRENT;
layout(constant_id = 16) const float bumpMapMatrix00_0 = 0.0;
layout(constant_id = 17) const float bumpMapMatrix01_0 = 0.0;
layout(constant_id = 18) const float bumpMapMatrix10_0 = 0.0;
layout(constant_id = 19) const float bumpMapMatrix11_0 = 0.0;
layout(constant_id = 20) const float bumpMapScale_0 = 0.0;
layout(constant_id = 21) const float bumpMapOffset_0 = 0.0;

//Texture Stage _1
layout(constant_id = 22) const int Constant_1 = 0;
layout(constant_id = 23) const int Result_1 = D3DTA_CURRENT;
layout(constant_id = 24) const int textureTransformationFlags_1 = D3DTTFF_DISABLE;
layout(constant_id = 25) const int texureCoordinateIndex_1 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 26) const int colorOperation_1 = D3DTOP_DISABLE;
layout(constant_id = 27) const int colorArgument0_1 = D3DTA_CURRENT;
layout(constant_id = 28) const int colorArgument1_1 = D3DTA_TEXTURE;
layout(constant_id = 29) const int colorArgument2_1 = D3DTA_CURRENT;
layout(constant_id = 30) const int alphaOperation_1 = D3DTOP_DISABLE;
layout(constant_id = 31) const int alphaArgument0_1 = D3DTA_CURRENT;
layout(constant_id = 32) const int alphaArgument1_1 = D3DTA_TEXTURE;
layout(constant_id = 33) const int alphaArgument2_1 = D3DTA_CURRENT;
layout(constant_id = 34) const float bumpMapMatrix00_1 = 0.0;
layout(constant_id = 35) const float bumpMapMatrix01_1 = 0.0;
layout(constant_id = 36) const float bumpMapMatrix10_1 = 0.0;
layout(constant_id = 37) const float bumpMapMatrix11_1 = 0.0;
layout(constant_id = 38) const float bumpMapScale_1 = 0.0;
layout(constant_id = 39) const float bumpMapOffset_1 = 0.0;

//Texture Stage _2
layout(constant_id = 40) const int Constant_2 = 0;
layout(constant_id = 41) const int Result_2 = D3DTA_CURRENT;
layout(constant_id = 42) const int textureTransformationFlags_2 = D3DTTFF_DISABLE;
layout(constant_id = 43) const int texureCoordinateIndex_2 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 44) const int colorOperation_2 = D3DTOP_DISABLE;
layout(constant_id = 45) const int colorArgument0_2 = D3DTA_CURRENT;
layout(constant_id = 46) const int colorArgument1_2 = D3DTA_TEXTURE;
layout(constant_id = 47) const int colorArgument2_2 = D3DTA_CURRENT;
layout(constant_id = 48) const int alphaOperation_2 = D3DTOP_DISABLE;
layout(constant_id = 49) const int alphaArgument0_2 = D3DTA_CURRENT;
layout(constant_id = 50) const int alphaArgument1_2 = D3DTA_TEXTURE;
layout(constant_id = 51) const int alphaArgument2_2 = D3DTA_CURRENT;
layout(constant_id = 52) const float bumpMapMatrix00_2 = 0.0;
layout(constant_id = 53) const float bumpMapMatrix01_2 = 0.0;
layout(constant_id = 54) const float bumpMapMatrix10_2 = 0.0;
layout(constant_id = 55) const float bumpMapMatrix11_2 = 0.0;
layout(constant_id = 56) const float bumpMapScale_2 = 0.0;
layout(constant_id = 57) const float bumpMapOffset_2 = 0.0;

//Texture Stage _3
layout(constant_id = 58) const int Constant_3 = 0;
layout(constant_id = 59) const int Result_3 = D3DTA_CURRENT;
layout(constant_id = 60) const int textureTransformationFlags_3 = D3DTTFF_DISABLE;
layout(constant_id = 61) const int texureCoordinateIndex_3 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 62) const int colorOperation_3 = D3DTOP_DISABLE;
layout(constant_id = 63) const int colorArgument0_3 = D3DTA_CURRENT;
layout(constant_id = 64) const int colorArgument1_3 = D3DTA_TEXTURE;
layout(constant_id = 65) const int colorArgument2_3 = D3DTA_CURRENT;
layout(constant_id = 66) const int alphaOperation_3 = D3DTOP_DISABLE;
layout(constant_id = 67) const int alphaArgument0_3 = D3DTA_CURRENT;
layout(constant_id = 68) const int alphaArgument1_3 = D3DTA_TEXTURE;
layout(constant_id = 69) const int alphaArgument2_3 = D3DTA_CURRENT;
layout(constant_id = 70) const float bumpMapMatrix00_3 = 0.0;
layout(constant_id = 71) const float bumpMapMatrix01_3 = 0.0;
layout(constant_id = 72) const float bumpMapMatrix10_3 = 0.0;
layout(constant_id = 73) const float bumpMapMatrix11_3 = 0.0;
layout(constant_id = 74) const float bumpMapScale_3 = 0.0;
layout(constant_id = 75) const float bumpMapOffset_3 = 0.0;

//Texture Stage _4
layout(constant_id = 76) const int Constant_4 = 0;
layout(constant_id = 77) const int Result_4 = D3DTA_CURRENT;
layout(constant_id = 78) const int textureTransformationFlags_4 = D3DTTFF_DISABLE;
layout(constant_id = 79) const int texureCoordinateIndex_4 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 80) const int colorOperation_4 = D3DTOP_DISABLE;
layout(constant_id = 81) const int colorArgument0_4 = D3DTA_CURRENT;
layout(constant_id = 82) const int colorArgument1_4 = D3DTA_TEXTURE;
layout(constant_id = 83) const int colorArgument2_4 = D3DTA_CURRENT;
layout(constant_id = 84) const int alphaOperation_4 = D3DTOP_DISABLE;
layout(constant_id = 85) const int alphaArgument0_4 = D3DTA_CURRENT;
layout(constant_id = 86) const int alphaArgument1_4 = D3DTA_TEXTURE;
layout(constant_id = 87) const int alphaArgument2_4 = D3DTA_CURRENT;
layout(constant_id = 88) const float bumpMapMatrix00_4 = 0.0;
layout(constant_id = 89) const float bumpMapMatrix01_4 = 0.0;
layout(constant_id = 90) const float bumpMapMatrix10_4 = 0.0;
layout(constant_id = 91) const float bumpMapMatrix11_4 = 0.0;
layout(constant_id = 92) const float bumpMapScale_4 = 0.0;
layout(constant_id = 93) const float bumpMapOffset_4 = 0.0;

//Texture Stage _5
layout(constant_id = 94 ) const int Constant_5 = 0;
layout(constant_id = 95 ) const int Result_5 = D3DTA_CURRENT;
layout(constant_id = 96 ) const int textureTransformationFlags_5 = D3DTTFF_DISABLE;
layout(constant_id = 97 ) const int texureCoordinateIndex_5 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 98 ) const int colorOperation_5 = D3DTOP_DISABLE;
layout(constant_id = 99 ) const int colorArgument0_5 = D3DTA_CURRENT;
layout(constant_id = 100) const int colorArgument1_5 = D3DTA_TEXTURE;
layout(constant_id = 101) const int colorArgument2_5 = D3DTA_CURRENT;
layout(constant_id = 102) const int alphaOperation_5 = D3DTOP_DISABLE;
layout(constant_id = 103) const int alphaArgument0_5 = D3DTA_CURRENT;
layout(constant_id = 104) const int alphaArgument1_5 = D3DTA_TEXTURE;
layout(constant_id = 105) const int alphaArgument2_5 = D3DTA_CURRENT;
layout(constant_id = 106) const float bumpMapMatrix00_5 = 0.0;
layout(constant_id = 107) const float bumpMapMatrix01_5 = 0.0;
layout(constant_id = 108) const float bumpMapMatrix10_5 = 0.0;
layout(constant_id = 109) const float bumpMapMatrix11_5 = 0.0;
layout(constant_id = 110) const float bumpMapScale_5 = 0.0;
layout(constant_id = 111) const float bumpMapOffset_5 = 0.0;

//Texture Stage _6
layout(constant_id = 112) const int Constant_6 = 0;
layout(constant_id = 113) const int Result_6 = D3DTA_CURRENT;
layout(constant_id = 114) const int textureTransformationFlags_6 = D3DTTFF_DISABLE;
layout(constant_id = 115) const int texureCoordinateIndex_6 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 116) const int colorOperation_6 = D3DTOP_DISABLE;
layout(constant_id = 117) const int colorArgument0_6 = D3DTA_CURRENT;
layout(constant_id = 118) const int colorArgument1_6 = D3DTA_TEXTURE;
layout(constant_id = 119) const int colorArgument2_6 = D3DTA_CURRENT;
layout(constant_id = 120) const int alphaOperation_6 = D3DTOP_DISABLE;
layout(constant_id = 121) const int alphaArgument0_6 = D3DTA_CURRENT;
layout(constant_id = 122) const int alphaArgument1_6 = D3DTA_TEXTURE;
layout(constant_id = 123) const int alphaArgument2_6 = D3DTA_CURRENT;
layout(constant_id = 124) const float bumpMapMatrix00_6 = 0.0;
layout(constant_id = 125) const float bumpMapMatrix01_6 = 0.0;
layout(constant_id = 126) const float bumpMapMatrix10_6 = 0.0;
layout(constant_id = 127) const float bumpMapMatrix11_6 = 0.0;
layout(constant_id = 128) const float bumpMapScale_6 = 0.0;
layout(constant_id = 129) const float bumpMapOffset_6 = 0.0;

//Texture Stage _7
layout(constant_id = 130) const int Constant_7 = 0;
layout(constant_id = 131) const int Result_7 = D3DTA_CURRENT;
layout(constant_id = 132) const int textureTransformationFlags_7 = D3DTTFF_DISABLE;
layout(constant_id = 133) const int texureCoordinateIndex_7 = D3DTSS_TCI_PASSTHRU;
layout(constant_id = 134) const int colorOperation_7 = D3DTOP_DISABLE;
layout(constant_id = 135) const int colorArgument0_7 = D3DTA_CURRENT;
layout(constant_id = 136) const int colorArgument1_7 = D3DTA_TEXTURE;
layout(constant_id = 137) const int colorArgument2_7 = D3DTA_CURRENT;
layout(constant_id = 138) const int alphaOperation_7 = D3DTOP_DISABLE;
layout(constant_id = 139) const int alphaArgument0_7 = D3DTA_CURRENT;
layout(constant_id = 140) const int alphaArgument1_7 = D3DTA_TEXTURE;
layout(constant_id = 141) const int alphaArgument2_7 = D3DTA_CURRENT;
layout(constant_id = 142) const float bumpMapMatrix00_7 = 0.0;
layout(constant_id = 143) const float bumpMapMatrix01_7 = 0.0;
layout(constant_id = 144) const float bumpMapMatrix10_7 = 0.0;
layout(constant_id = 145) const float bumpMapMatrix11_7 = 0.0;
layout(constant_id = 146) const float bumpMapScale_7 = 0.0;
layout(constant_id = 147) const float bumpMapOffset_7 = 0.0;

//Render State
layout(constant_id = 148) const int zEnable = 7;
layout(constant_id = 149) const int fillMode = 8;
layout(constant_id = 150) const int shadeMode = 9;
layout(constant_id = 151) const int zWriteEnable = 14;
layout(constant_id = 152) const int alphaTestEnable = 15;
layout(constant_id = 153) const int lastPixel = 16;
layout(constant_id = 154) const int sourceBlend = 19;
layout(constant_id = 155) const int destinationBlend = 20;
layout(constant_id = 156) const int cullMode = 22;
layout(constant_id = 157) const int zFunction = 23;
layout(constant_id = 158) const int alphaReference = 24;
layout(constant_id = 159) const int alphaFunction = 25;
layout(constant_id = 160) const int ditherEnable = 26;
layout(constant_id = 161) const int alphaBlendEnable = 27;
layout(constant_id = 162) const int fogEnable = 28;
layout(constant_id = 163) const int specularEnable = 29;
layout(constant_id = 164) const int fogColor = 34;
layout(constant_id = 165) const int fogTableMode = 35;
layout(constant_id = 166) const int fogStart = 36;
layout(constant_id = 167) const int fogEnd = 37;
layout(constant_id = 168) const int fogDensity = 38;
layout(constant_id = 169) const int rangeFogEnable = 48;
layout(constant_id = 170) const int stencilEnable = 52;
layout(constant_id = 171) const int stencilFail = 53;
layout(constant_id = 172) const int stencilZFail = 54;
layout(constant_id = 173) const int stencilPass = 55;
layout(constant_id = 174) const int stencilFunction = 56;
layout(constant_id = 175) const int stencilReference = 57;
layout(constant_id = 176) const int stencilMask = 58;
layout(constant_id = 177) const int stencilWriteMask = 59;
layout(constant_id = 178) const int textureFactor = 60;
layout(constant_id = 179) const int wrap0 = 128;
layout(constant_id = 180) const int wrap1 = 129;
layout(constant_id = 181) const int wrap2 = 130;
layout(constant_id = 182) const int wrap3 = 131;
layout(constant_id = 183) const int wrap4 = 132;
layout(constant_id = 184) const int wrap5 = 133;
layout(constant_id = 185) const int wrap6 = 134;
layout(constant_id = 186) const int wrap7 = 135;
layout(constant_id = 187) const int clipping = 136;
layout(constant_id = 188) const bool lighting = true;
layout(constant_id = 189) const int ambient = 139;
layout(constant_id = 190) const int fogVertexMode = 140;
layout(constant_id = 191) const int colorVertex = 141;
layout(constant_id = 192) const int localViewer = 142;
layout(constant_id = 193) const int normalizeNormals = 143;
layout(constant_id = 194) const int diffuseMaterialSource = 145;
layout(constant_id = 195) const int specularMaterialSource = 146;
layout(constant_id = 196) const int ambientMaterialSource = 147;
layout(constant_id = 197) const int emissiveMaterialSource = 148;
layout(constant_id = 198) const int vertexBlend = 151;
layout(constant_id = 199) const int clipPlaneEnable = 152;
layout(constant_id = 200) const int pointSize = 154;
layout(constant_id = 201) const int pointSizeMinimum = 155;
layout(constant_id = 202) const int pointSpriteEnable = 156;
layout(constant_id = 203) const int pointScaleEnable = 157;
layout(constant_id = 204) const int pointScaleA = 158;
layout(constant_id = 205) const int pointScaleB = 159;
layout(constant_id = 206) const int pointScaleC = 160;
layout(constant_id = 207) const int multisampleAntiAlias = 161;
layout(constant_id = 208) const int multisampleMask = 162;
layout(constant_id = 209) const int patchEdgeStyle = 163;
layout(constant_id = 210) const int debugMonitorToken = 165;
layout(constant_id = 211) const int pointSizeMaximum = 166;
layout(constant_id = 212) const int indexedVertexBlendEnable = 167;
layout(constant_id = 213) const int colorWriteEnable = 168;
layout(constant_id = 214) const int tweenFactor = 170;
layout(constant_id = 215) const int blendOperation = 171;
layout(constant_id = 216) const int positionDegree = 172;
layout(constant_id = 217) const int normalDegree = 173;
layout(constant_id = 218) const int scissorTestEnable = 174;
layout(constant_id = 219) const int slopeScaleDepthBias = 175;
layout(constant_id = 220) const int antiAliasedLineEnable = 176;
layout(constant_id = 221) const int minimumTessellationLevel = 178;
layout(constant_id = 222) const int maximumTessellationLevel = 179;
layout(constant_id = 223) const int adaptivetessX = 180;
layout(constant_id = 224) const int adaptivetessY = 181;
layout(constant_id = 225) const int adaptivetessZ = 182;
layout(constant_id = 226) const int adaptivetessW = 183;
layout(constant_id = 227) const int enableAdaptiveTessellation = 184;
layout(constant_id = 228) const int twoSidedStencilMode = 185;
layout(constant_id = 229) const int ccwStencilFail = 186;
layout(constant_id = 230) const int ccwStencilZFail = 187;
layout(constant_id = 231) const int ccwStencilPass = 188;
layout(constant_id = 232) const int ccwStencilFunction = 189;
layout(constant_id = 233) const int colorWriteEnable1 = 190;
layout(constant_id = 234) const int colorWriteEnable2 = 191;
layout(constant_id = 235) const int colorWriteEnable3 = 192;
layout(constant_id = 236) const int blendFactor = 193;
layout(constant_id = 237) const int srgbWriteEnable = 194;
layout(constant_id = 238) const int depthBias = 195;
layout(constant_id = 239) const int wrap8 = 198;
layout(constant_id = 240) const int wrap9 = 199;
layout(constant_id = 241) const int wrap10 = 200;
layout(constant_id = 242) const int wrap11 = 201;
layout(constant_id = 243) const int wrap12 = 202;
layout(constant_id = 244) const int wrap13 = 203;
layout(constant_id = 245) const int wrap14 = 204;
layout(constant_id = 246) const int wrap15 = 205;
layout(constant_id = 247) const int separateAlphaBlendEnable = 206;
layout(constant_id = 248) const int sourceBlendAlpha = 207;
layout(constant_id = 249) const int destinationBlendAlpha = 208;
layout(constant_id = 250) const int blendOperationAlpha = 209;

struct Light
{
	int        Type;            /* Type of light source */
	vec4       Diffuse;         /* Diffuse color of light */
	vec4       Specular;        /* Specular color of light */
	vec4       Ambient;         /* Ambient color of light */
	vec3       Position;        /* Position in world space */
	vec3       Direction;       /* Direction in world space */
	float      Range;           /* Cutoff range */
	float      Falloff;         /* Falloff */
	float      Attenuation0;    /* Constant attenuation */
	float      Attenuation1;    /* Linear attenuation */
	float      Attenuation2;    /* Quadratic attenuation */
	float      Theta;           /* Inner angle of spotlight cone */
	float      Phi;             /* Outer angle of spotlight cone */
};
 
struct Material
{
	vec4   Diffuse;        /* Diffuse color RGBA */
	vec4   Ambient;        /* Ambient color RGB */
	vec4   Specular;       /* Specular 'shininess' */
	vec4   Emissive;       /* Emissive color RGB */
	float  Power;          /* Sharpness if specular highlight */
};

layout(binding = 1) uniform LightBlock
{
	Light lights[lightCount];
};

layout(binding = 2) uniform MaterialBlock
{
	Material material;
};

layout(push_constant) uniform UniformBufferObject {
    mat4 totalTransformation;
} ubo;

vec3 getGouradLight( int lightIndex, vec3 position1, vec3 norm )
{
	vec3 s = normalize( vec3( lights[lightIndex].Position - position1 ) );
	vec3 v = normalize( -position1.xyz );
	vec3 r = reflect( -s, norm );
 
	vec3 ambient = lights[lightIndex].Ambient.xyz * material.Ambient.xyz;
 
	float sDotN = max( dot( s, norm ), 0.0 );
	vec3 diffuse = lights[lightIndex].Diffuse.xyz * material.Diffuse.xyz * sDotN;
 
	vec3 spec = vec3( 0.0 );
	if ( sDotN > 0.0 )
		spec = lights[lightIndex].Specular.xyz * material.Specular.xyz * pow( max( dot(r,v) , 0.0 ), material.Power );
 
	return ambient + diffuse + spec;
}

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 attr1; //normal
layout (location = 2) in uvec4 attr2; //color
layout (location = 3) in vec2 attr3; //tex1
layout (location = 4) in vec2 attr4; //tex2

layout (location = 0) out vec4 normal;
layout (location = 1) out vec4 frontColor;
layout (location = 2) out vec4 backColor;
layout (location = 3) out vec2 texcoord1;
layout (location = 4) out vec2 texcoord2;
layout (location = 5) out vec4 pos;

out gl_PerVertex 
{
	vec4 gl_Position;
};

vec4 Convert(uvec4 rgba)
{
	vec4 unpacked;

	unpacked.w = float(rgba.w);
	unpacked.z = float(rgba.z);
	unpacked.y = float(rgba.y);
	unpacked.x = float(rgba.x);

	unpacked.x = unpacked.x / 255;
	unpacked.y = unpacked.y / 255;
	unpacked.z = unpacked.z / 255;
	unpacked.w = unpacked.w / 255;	

	return unpacked;
}

void main() 
{
	vec3 frontLightColor = vec3(0);
	vec3 backLightColor = vec3(0);

	pos = ubo.totalTransformation * position * vec4(1.0,-1.0,1.0,1.0);

	gl_Position = pos;
	frontColor = Convert(attr2);
	backColor = frontColor;
	normal = attr1;
	texcoord1 = attr3;
	texcoord2 = attr4;

	if(lighting)
	{
		if(shadeMode == D3DSHADE_GOURAUD)
		{
			for( int i=0; i<lightCount; ++i )
			{
				frontLightColor += getGouradLight( i, position.xyz, normal.xyz);
				backLightColor += getGouradLight( i, position.xyz, -normal.xyz);
			}
			frontColor *= vec4(frontLightColor,1);
			backColor *= vec4(backLightColor,1);
		}
	}

}
