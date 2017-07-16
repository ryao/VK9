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

#include <vulkan/vk_sdk_platform.h>
//#include "d3d9types.h"
#include "d3d9.h"
#include "fxdis/dxbc.h"
#include "fxdis/sm4.h"
#include "D3D11Shader.h"
#include "ShaderConverter.h"
//#include "Utilities.h"

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

const uint32_t ICFE = PACK('I', 'C', 'F', 'E');
const uint32_t IFCE = PACK('I', 'F', 'C', 'E');
const uint32_t ISGN = PACK('I', 'S', 'G', 'N');
const uint32_t OSG5 = PACK('O', 'S', 'G', '5');
const uint32_t OSGN = PACK('O', 'S', 'G', 'N');
const uint32_t PCSG = PACK('P', 'C', 'S', 'G');
const uint32_t RDEF = PACK('R', 'D', 'E', 'F');
const uint32_t SDGB = PACK('S', 'D', 'G', 'B');
const uint32_t SFI0 = PACK('S', 'F', 'I', '0');
const uint32_t SHDR = PACK('S', 'H', 'D', 'R');
const uint32_t SHEX = PACK('S', 'H', 'E', 'X');
const uint32_t SPDB = PACK('S', 'P', 'D', 'B');
const uint32_t STAT = PACK('S', 'T', 'A', 'T');

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
	unsigned MinorVersion : 4;
	unsigned MajorVersion : 4;
	uint16_t ProgramType; //1 means vertex shader.
	uint32_t DWORDCount; //Number of DWORDs in the chunk.
};

struct SHEXChunk //maybe wrong?
{
	uint32_t ChunkType;
	uint32_t ChunkLength;
	unsigned MinorVersion : 4;
	unsigned MajorVersion : 4;
	uint16_t ProgramType; //1 means vertex shader.
	uint32_t DWORDCount; //Number of DWORDs in the chunk.
};

struct OperationCodeHeader //OPCODE
{
	uint32_t OperationCodeType : 11;
	uint32_t Unknown : 13;
	uint32_t OperationCodeLength : 7;
	uint32_t IsExtended : 1;
};

inline VkFormat ConvertType(D3D_REGISTER_COMPONENT_TYPE value)
{
	switch (value)
	{
	case D3D_REGISTER_COMPONENT_UNKNOWN:
		return VK_FORMAT_R32_SFLOAT;
		break;
	case D3D_REGISTER_COMPONENT_UINT32:
		return VK_FORMAT_R32_UINT;
		break;
	case D3D_REGISTER_COMPONENT_SINT32:
		return VK_FORMAT_R32_SINT;
		break;
	case D3D_REGISTER_COMPONENT_FLOAT32:
		return VK_FORMAT_R32_SFLOAT;
		break;
	default:
		return VK_FORMAT_R32_SFLOAT;
		break;
	}
}

ConvertedShader ShaderConverter::Convert(DWORD* shader)
{
	/*
	http://timjones.io/blog/archive/2015/09/02/parsing-direct3d-shader-bytecode
	https://msdn.microsoft.com/en-us/library/bb219840(VS.85).aspx#Shader_Binary_Format
	http://stackoverflow.com/questions/2545704/format-of-compiled-directx9-shader-files
	https://msdn.microsoft.com/en-us/library/windows/hardware/ff552891(v=vs.85).aspx
	*/

	ShaderHeader header;
	char* data = (char*)shader;
	memcpy(&header, (void*)data, sizeof(ShaderHeader));
	
	ConvertedShader output;
	output.Size = header.ShaderSize; //The size is not a parameter to the d3d9 functions so I have to take it from the header.

	dxbc_container* dxbc = dxbc_parse(shader, header.ShaderSize);
	if (dxbc) //!= nullptr
	{
		dxbc_chunk_header* sm4_chunk = dxbc_find_shader_bytecode(shader, header.ShaderSize);
		if (sm4_chunk) //!= nullptr
		{
			sm4_program* sm4 = sm4_parse(sm4_chunk + 1, bswap_le32(sm4_chunk->size));
			if (sm4) //!= nullptr
			{
				for (size_t i = 0; i < sm4->num_params_in; i++)
				{
					auto& parameter = sm4->params_in[i];
					auto& attributeDescription = output.mVertexInputAttributeDescription[i];

					attributeDescription.binding = parameter.Stream;
					attributeDescription.location = i;
					attributeDescription.format = ConvertType(parameter.ComponentType);
					attributeDescription.offset = parameter.Register*4;
				}

				
				delete sm4;
			}		
		}
		delete dxbc;
	}

	//uint32_t chunkType;
	//char* chunkPointer = nullptr;

	//std::vector<uint32_t> mOutputInstructions;

	//The header is followed by a list of chunk offsets so loop over that and process each chunk.
	//for (size_t i = 0; i < header.ChunkCount; i++)
	//{
	//	uint32_t chunkOffset = (*(uint32_t*)data);
	//	data += sizeof(uint32_t);

	//	chunkPointer = (char*)shader;
	//	chunkPointer += chunkOffset;

	//	memcpy(&chunkType, (void*)chunkPointer, sizeof(char[4]));
	//	switch (chunkType)
	//	{
	//	case ICFE:
	//	{
	//		ICFEChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(ICFEChunk));
	//	}
	//	break;
	//	case IFCE:
	//	{
	//		IFCEChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(IFCEChunk));
	//	}
	//	break;
	//	case ISGN:
	//	{
	//		ISGNChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(ISGNChunk));
	//	}
	//	break;
	//	case OSG5:
	//	{
	//		OSG5Chunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(OSG5Chunk));
	//	}
	//	break;
	//	case OSGN:
	//	{
	//		OSGNChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(OSGNChunk));
	//	}
	//	break;
	//	case PCSG:
	//	{
	//		PCSGChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(PCSGChunk));
	//	}
	//	break;
	//	case RDEF:
	//	{
	//		RDEFChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(RDEFChunk));
	//	}
	//	break;
	//	case SDGB:
	//	{
	//		SDGBChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(SDGBChunk));
	//	}
	//	break;
	//	case SFI0:
	//	{
	//		SFI0Chunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(SFI0Chunk));
	//	}
	//	break;
	//	case SHDR:
	//	{
	//		SHDRChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(SHDRChunk));
	//	}
	//	break;
	//	case SHEX:
	//	{
	//		SHEXChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(SHEXChunk));
	//	}
	//	break;
	//	case SPDB:
	//	{
	//		SPDBChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(SPDBChunk));
	//	}
	//	break;
	//	case STAT:
	//	{
	//		STATChunk chunk;
	//		memcpy(&chunk, chunkPointer, sizeof(STATChunk));
	//	}
	//	break;
	//	default:
	//		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::Convert unknown chunk type " << chunkType;
	//		break;
	//	}
	//}

	return output; //Return value optimization don't fail me now.
}