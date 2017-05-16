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

#define D3DZB_FALSE 0
#define D3DZB_TRUE 1 // Z buffering
#define D3DZB_USEW 2 // W buffering

#define D3DFILL_POINT 1
#define D3DFILL_WIREFRAME 2
#define D3DFILL_SOLID 3

#define D3DBLEND_ZERO 1
#define D3DBLEND_ONE 2
#define D3DBLEND_SRCCOLOR 3
#define D3DBLEND_INVSRCCOLOR 4
#define D3DBLEND_SRCALPHA 5
#define D3DBLEND_INVSRCALPHA 6
#define D3DBLEND_DESTALPHA 7
#define D3DBLEND_INVDESTALPHA 8
#define D3DBLEND_DESTCOLOR 9
#define D3DBLEND_INVDESTCOLOR 10
#define D3DBLEND_SRCALPHASAT 11
#define D3DBLEND_BOTHSRCALPHA 12
#define D3DBLEND_BOTHINVSRCALPHA 13
#define D3DBLEND_BLENDFACTOR 14
#define D3DBLEND_INVBLENDFACTOR 15
#define D3DBLEND_SRCCOLOR2 16
#define D3DBLEND_INVSRCCOLOR2 17

#define D3DCULL_NONE 1
#define D3DCULL_CW 2
#define D3DCULL_CCW 3

#define D3DCMP_NEVER 1
#define D3DCMP_LESS 2
#define D3DCMP_EQUAL 3
#define D3DCMP_LESSEQUAL 4
#define D3DCMP_GREATER 5
#define D3DCMP_NOTEQUAL 6
#define D3DCMP_GREATEREQUAL 7
#define D3DCMP_ALWAYS 8

#define D3DCMP_NEVER 1
#define D3DCMP_LESS 2
#define D3DCMP_EQUAL 3
#define D3DCMP_LESSEQUAL 4
#define D3DCMP_GREATER 5
#define D3DCMP_NOTEQUAL 6
#define D3DCMP_GREATEREQUAL 7
#define D3DCMP_ALWAYS 8

#define D3DFOG_NONE 0
#define D3DFOG_EXP 1
#define D3DFOG_EXP2 2
#define D3DFOG_LINEAR 3

#define D3DSTENCILOP_KEEP 1
#define D3DSTENCILOP_ZERO 2
#define D3DSTENCILOP_REPLACE 3
#define D3DSTENCILOP_INCRSAT 4
#define D3DSTENCILOP_DECRSAT 5
#define D3DSTENCILOP_INVERT 6
#define D3DSTENCILOP_INCR 7
#define D3DSTENCILOP_DECR 8

#define D3DMCS_MATERIAL 0
#define D3DMCS_COLOR1 1
#define D3DMCS_COLOR2 2

#define D3DVBF_DISABLE 0
#define D3DVBF_1WEIGHTS 1
#define D3DVBF_2WEIGHTS 2
#define D3DVBF_3WEIGHTS 3
#define D3DVBF_TWEENING 255
#define D3DVBF_0WEIGHTS 256

#define D3DPATCHEDGE_DISCRETE 0
#define D3DPATCHEDGE_CONTINUOUS 1

#define D3DDMT_ENABLE 0
#define D3DDMT_DISABLE 1

#define D3DBLENDOP_ADD 1
#define D3DBLENDOP_SUBTRACT 2
#define D3DBLENDOP_REVSUBTRACT 3
#define D3DBLENDOP_MIN 4
#define D3DBLENDOP_MAX 5

#define D3DDEGREE_LINEAR 1
#define D3DDEGREE_QUADRATIC 2
#define D3DDEGREE_CUBIC 3
#define D3DDEGREE_QUINTIC 5

layout(constant_id = 0) const int lightCount = 1;
layout(constant_id = 1) const int reserved1 = 0;
layout(constant_id = 2) const int reserved2 = 0;
layout(constant_id = 3) const int textureCount = 2;

//Texture Stage _0
layout(constant_id = 4) const int Constant_0 = 0;
layout(constant_id = 5) const int Result_0 = D3DTA_CURRENT;
layout(constant_id = 6) const int textureTransformationFlags_0 = D3DTTFF_DISABLE;
layout(constant_id = 7) const int texureCoordinateIndex_0 = 0;
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
layout(constant_id = 25) const int texureCoordinateIndex_1 = 1;
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
layout(constant_id = 43) const int texureCoordinateIndex_2 = 2;
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
layout(constant_id = 61) const int texureCoordinateIndex_3 = 3;
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
layout(constant_id = 79) const int texureCoordinateIndex_4 = 4;
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
layout(constant_id = 97 ) const int texureCoordinateIndex_5 = 5;
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
layout(constant_id = 115) const int texureCoordinateIndex_6 = 6;
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
layout(constant_id = 133) const int texureCoordinateIndex_7 = 7;
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
layout(constant_id = 148) const int zEnable = D3DZB_FALSE;
layout(constant_id = 149) const int fillMode = D3DFILL_SOLID;
layout(constant_id = 150) const int shadeMode = D3DSHADE_GOURAUD;
layout(constant_id = 151) const bool zWriteEnable = true;
layout(constant_id = 152) const bool alphaTestEnable = false;
layout(constant_id = 153) const bool lastPixel = true;
layout(constant_id = 154) const int sourceBlend = D3DBLEND_ONE;
layout(constant_id = 155) const int destinationBlend = D3DBLEND_ZERO;
layout(constant_id = 156) const int cullMode = D3DCULL_CCW;
layout(constant_id = 157) const int zFunction = D3DCMP_LESSEQUAL;
layout(constant_id = 158) const int alphaReference = 0;
layout(constant_id = 159) const int alphaFunction = D3DCMP_ALWAYS;
layout(constant_id = 160) const bool ditherEnable = false;
layout(constant_id = 161) const bool alphaBlendEnable = false;
layout(constant_id = 162) const bool fogEnable = false;
layout(constant_id = 163) const bool specularEnable = false;
layout(constant_id = 164) const int fogColor = 0;
layout(constant_id = 165) const int fogTableMode = D3DFOG_NONE;
layout(constant_id = 166) const float fogStart = 0.0f;
layout(constant_id = 167) const float fogEnd = 1.0f;
layout(constant_id = 168) const float fogDensity = 1.0f;
layout(constant_id = 169) const bool rangeFogEnable = false;
layout(constant_id = 170) const bool stencilEnable = false;
layout(constant_id = 171) const int stencilFail = D3DSTENCILOP_KEEP;
layout(constant_id = 172) const int stencilZFail = D3DSTENCILOP_KEEP;
layout(constant_id = 173) const int stencilPass = D3DSTENCILOP_KEEP;
layout(constant_id = 174) const int stencilFunction = D3DCMP_ALWAYS;
layout(constant_id = 175) const int stencilReference = 0;
layout(constant_id = 176) const int stencilMask = 0xFFFFFFFF;
layout(constant_id = 177) const int stencilWriteMask = 0xFFFFFFFF;
layout(constant_id = 178) const int textureFactor = 0xFFFFFFFF;
layout(constant_id = 179) const int wrap0 = 0;
layout(constant_id = 180) const int wrap1 = 0;
layout(constant_id = 181) const int wrap2 = 0;
layout(constant_id = 182) const int wrap3 = 0;
layout(constant_id = 183) const int wrap4 = 0;
layout(constant_id = 184) const int wrap5 = 0;
layout(constant_id = 185) const int wrap6 = 0;
layout(constant_id = 186) const int wrap7 = 0;
layout(constant_id = 187) const bool clipping = true;
layout(constant_id = 188) const bool lighting = true;
layout(constant_id = 189) const int ambient = 0;
layout(constant_id = 190) const int fogVertexMode = D3DFOG_NONE;
layout(constant_id = 191) const bool colorVertex = true;
layout(constant_id = 192) const bool localViewer = true;
layout(constant_id = 193) const bool normalizeNormals = false;
layout(constant_id = 194) const int diffuseMaterialSource = D3DMCS_COLOR1;
layout(constant_id = 195) const int specularMaterialSource = D3DMCS_COLOR2;
layout(constant_id = 196) const int ambientMaterialSource = D3DMCS_MATERIAL;
layout(constant_id = 197) const int emissiveMaterialSource = D3DMCS_MATERIAL;
layout(constant_id = 198) const int vertexBlend = D3DVBF_DISABLE;
layout(constant_id = 199) const int clipPlaneEnable = 0;
layout(constant_id = 200) const int pointSize = 64;
layout(constant_id = 201) const float pointSizeMinimum = 1.0f;
layout(constant_id = 202) const bool pointSpriteEnable = false;
layout(constant_id = 203) const bool pointScaleEnable = false;
layout(constant_id = 204) const float pointScaleA = 1.0f;
layout(constant_id = 205) const float pointScaleB = 0.0f;
layout(constant_id = 206) const float pointScaleC = 0.0f;
layout(constant_id = 207) const bool multisampleAntiAlias = true;
layout(constant_id = 208) const int multisampleMask = 0xFFFFFFFF;
layout(constant_id = 209) const int patchEdgeStyle = D3DPATCHEDGE_DISCRETE;
layout(constant_id = 210) const int debugMonitorToken = D3DDMT_ENABLE;
layout(constant_id = 211) const float pointSizeMaximum = 64.0f;
layout(constant_id = 212) const bool indexedVertexBlendEnable = false;
layout(constant_id = 213) const int colorWriteEnable = 0x0000000F;
layout(constant_id = 214) const float tweenFactor = 0.0f;
layout(constant_id = 215) const int blendOperation = D3DBLENDOP_ADD;
layout(constant_id = 216) const int positionDegree = D3DDEGREE_CUBIC;
layout(constant_id = 217) const int normalDegree = D3DDEGREE_LINEAR;
layout(constant_id = 218) const bool scissorTestEnable = false;
layout(constant_id = 219) const int slopeScaleDepthBias = 0;
layout(constant_id = 220) const bool antiAliasedLineEnable = false;
layout(constant_id = 221) const float minimumTessellationLevel = 1.0f;
layout(constant_id = 222) const float maximumTessellationLevel = 1.0f;
layout(constant_id = 223) const float adaptivetessX = 0.0f;
layout(constant_id = 224) const float adaptivetessY = 0.0f;
layout(constant_id = 225) const float adaptivetessZ = 1.0f;
layout(constant_id = 226) const float adaptivetessW = 0.0f;
layout(constant_id = 227) const bool enableAdaptiveTessellation = false;
layout(constant_id = 228) const bool twoSidedStencilMode = false;
layout(constant_id = 229) const int ccwStencilFail = D3DSTENCILOP_KEEP;
layout(constant_id = 230) const int ccwStencilZFail = D3DSTENCILOP_KEEP;
layout(constant_id = 231) const int ccwStencilPass = D3DSTENCILOP_KEEP;
layout(constant_id = 232) const int ccwStencilFunction = D3DCMP_ALWAYS;
layout(constant_id = 233) const int colorWriteEnable1 = 0x0000000f;
layout(constant_id = 234) const int colorWriteEnable2 = 0x0000000f;
layout(constant_id = 235) const int colorWriteEnable3 = 0x0000000f;
layout(constant_id = 236) const int blendFactor = 0xffffffff;
layout(constant_id = 237) const int srgbWriteEnable = 0;
layout(constant_id = 238) const float depthBias = 0;
layout(constant_id = 239) const int wrap8 = 0;
layout(constant_id = 240) const int wrap9 = 0;
layout(constant_id = 241) const int wrap10 = 0;
layout(constant_id = 242) const int wrap11 = 0;
layout(constant_id = 243) const int wrap12 = 0;
layout(constant_id = 244) const int wrap13 = 0;
layout(constant_id = 245) const int wrap14 = 0;
layout(constant_id = 246) const int wrap15 = 0;
layout(constant_id = 247) const bool separateAlphaBlendEnable = false;
layout(constant_id = 248) const int sourceBlendAlpha = D3DBLEND_ONE;
layout(constant_id = 249) const int destinationBlendAlpha = D3DBLEND_ZERO;
layout(constant_id = 250) const int blendOperationAlpha = D3DBLENDOP_ADD;

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

	bool       IsEnabled;
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

void getPhongLight( int lightIndex, vec3 position1, vec4 norm, out vec4 ambient, out vec4 diffuse, out vec4 spec )
{
	vec4 temp = ubo.totalTransformation * vec4(lights[lightIndex].Position,1.0) * vec4(1.0,-1.0,1.0,1.0);
	vec3 lightPosition = temp.xyz;

	vec3 n = normalize( norm.xyz );
	vec3 s = normalize( lightPosition - position1 );
	vec3 v = normalize( -position1 );
	vec3 r = reflect( -s, n );
 
	ambient = lights[lightIndex].Ambient * material.Ambient;
 
	float sDotN = max( dot( s, n ), 0.0 );
	diffuse = lights[lightIndex].Diffuse * material.Diffuse * sDotN;
 
 
	spec = lights[lightIndex].Specular * material.Specular * pow( max( dot(r,v) , 0.0 ), material.Power ); 
}

vec4 Convert(int rgba)
{
	vec4 unpacked = vec4(0);

	/*
	unpacked.w = float(rgba.w);
	unpacked.z = float(rgba.z);
	unpacked.y = float(rgba.y);
	unpacked.x = float(rgba.x);

	unpacked.x = unpacked.x / 255;
	unpacked.y = unpacked.y / 255;
	unpacked.z = unpacked.z / 255;
	unpacked.w = unpacked.w / 255;
	*/

	return unpacked;
}

layout(binding = 0) uniform sampler2D textures[2];

layout (location = 0) in vec2 texcoord1;
layout (location = 1) in vec2 texcoord2;
layout (location = 0) out vec4 uFragColor;

vec2 getTextureCoord(int index)
{
	switch(index)
	{
		case 0:
			return texcoord1;
		break;
		case 1:
			return texcoord2;
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
			return vec4(1.0,1.0,1.0,1.0);
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

	if(textureCount>1)
	{
		processStage(textures[1],texureCoordinateIndex_1, Constant_1, Result_1,
		result, temp, result, temp,
		colorOperation_1, colorArgument1_1, colorArgument2_1, colorArgument0_1,
		alphaOperation_1, alphaArgument1_1, alphaArgument2_1, alphaArgument0_1);
	}

	uFragColor = result;
}
