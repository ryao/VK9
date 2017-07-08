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

/*
http://timjones.io/blog/archive/2015/09/02/parsing-direct3d-shader-bytecode
https://msdn.microsoft.com/en-us/library/bb219840(VS.85).aspx#Shader_Binary_Format
http://stackoverflow.com/questions/2545704/format-of-compiled-directx9-shader-files
https://msdn.microsoft.com/en-us/library/windows/hardware/ff552891(v=vs.85).aspx
*/

const uint32_t mEndToken = 0x0000FFFF;
const uint16_t mShaderTypePixel = 0xFFFF;
const uint16_t mShaderTypeVertex = 0xFFFE;

#define PACK(c0, c1, c2, c3) \
    (((uint32_t)(uint8_t)(c0) << 24) | \
    ((uint32_t)(uint8_t)(c1) << 16) | \
    ((uint32_t)(uint8_t)(c2) << 8) | \
    ((uint32_t)(uint8_t)(c3)))

const uint32_t ICFE = PACK('I','C','F','E');
const uint32_t IFCE = PACK('I', 'F', 'C', 'E');
const uint32_t ISGN = PACK('I','S','G','N');
const uint32_t OSG5 = PACK('O','S','G','5');
const uint32_t OSGN = PACK('O','S','G','N');
const uint32_t PCSG = PACK('P','C','S','G');
const uint32_t RDEF = PACK('R','D','E','F');
const uint32_t SDGB = PACK('S','D','G','B');
const uint32_t SFI0 = PACK('S','F','I','0');
const uint32_t SHDR = PACK('S','H','D','R');
const uint32_t SHEX = PACK('S','H','E','X');
const uint32_t SPDB = PACK('S','P','D','B');
const uint32_t STAT = PACK('S','T','A','T');

struct ConvertedShader
{
	UINT Size = 0;
	VkShaderModule ShaderModule = VK_NULL_HANDLE;
};

/*
 The structures read from the file don't have initializers because they are set from file data so it would be a waste of CPU cycles.
 Not all of these structures are documented so some of these may be wrong.
 */

struct ShaderHeader
{
	char FileType[4];
	uint64_t Checksum;
	uint32_t Unknown;
	uint32_t ShaderSize;
	uint32_t ChunkCount;
};

//ICFE and IFCE might be the same thing but I'm not sure. 

struct ICFEChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	uint32_t ClassInstanceCount;
	uint32_t ClassTypeCount;
	uint32_t InterfaceSlotRecordCount;
	uint32_t InterfaceSlotCount;
	uint32_t Unknown;
	uint32_t ClassTypeOffset;
};

struct IFCEChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	uint32_t ClassInstanceCount;
	uint32_t ClassTypeCount;
	uint32_t InterfaceSlotRecordCount;
	uint32_t InterfaceSlotCount;
	uint32_t Unknown;
	uint32_t ClassTypeOffset;
};

struct ISGNChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	uint32_t ElementCount;
	uint32_t Unknown;
};

struct OSG5Chunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	//??
};

struct OSGNChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	uint32_t ElementCount;
	uint32_t Unknown;
};

struct PCSGChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	//??????????
};

struct RDEFChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	uint32_t ConstantBufferCount;
	uint32_t ConstantBufferOffset;
	uint32_t ResourceBindingCount;
	uint32_t ResourceBindingOffset;
	char MinorVersionNumber;
	char MajorVersionNumber;
	uint16_t ProgramType;
	uint32_t Flag;
	uint32_t CreatorOffset;
};

struct SDGBChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	//???
};

struct SFI0Chunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	uint32_t Flags;  //if 1 then double precision floating point is required.
};

struct SPDBChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	//??????
};

struct STATChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	uint32_t InstructionCount;
	uint32_t TempRegisterCount;
	uint32_t DefineCount;
	uint32_t DeclarationCount;
	uint32_t FloatInstructionCount;
	uint32_t IntegerInstructionCount;
	uint32_t UnsignedIntegerInstructionCount;
	uint32_t StaticFlowControlCount;
	uint32_t DynamicFlowControlCount;
	uint32_t MacroInstructionCount; //might be wrong.
	uint32_t TempArrayCount;
	uint32_t ArrayInstructionCount;
	uint32_t CutInstructionCount;
	uint32_t EmitInstructionCount;
	uint32_t TextureNormalInstructionCount;
	uint32_t TextureLoadInstructionCount;
	uint32_t TextureBiasInstructionCount;
	uint32_t TextureGradientInstructionCount;
	uint32_t MovInstructionCount;
	uint32_t MovcInstructionCount;
	uint32_t ConversionInstructionCount;
	uint32_t Unknown;
	uint32_t InputPrimitiveForGeometryShaderCount;
	uint32_t PrimitiveTopologyForGemotryShaderCount;
	uint32_t MaximumOutputVertexCountForGeometryShaderCount;
	uint32_t Unknown2;
	uint32_t Unknown3;
	uint32_t IsSampleFrequencyShader; //Might be more than a switch but I only know about 1 and 0.
};

struct SHDRChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	char Version; //Minor low 4 bits //Major high 4 bits.
	uint16_t ProgramType; //1 means vertex shader.
	uint32_t DWORDCount; //Number of DWORDs in the chunk.
};

struct SHEXChunk
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	//??????
};

struct OperationCodeHeader //OPCODE
{
	uint32_t OperationCodeType : 11;
	uint32_t Unknown : 13;
	uint32_t OperationCodeLength : 7;
	uint32_t IsExtended : 1;
};

class ShaderConverter
{

public:
	ConvertedShader Convert(DWORD* shader);
private:

};

#endif //SHADERCONVERTER_H
//
