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
#include <boost/log/trivial.hpp>

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


struct ShaderHeader
{
	//ShaderType type;
	char major;
	char minor;

	const struct wined3d_sm1_opcode_info *opcode_table;
	const DWORD *start;

	//char fourcc[4];
	//char FileType[4];
	//uint32_t Unknown;
	//uint32_t ShaderSize;
	//uint32_t ChunkCount;
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

inline DWORD ConvertType(sm4_opcode_type value)
{
	switch (value)
	{
	case SM4_OPCODE_TYPE_NA:
		return 0; //TODO: lookup types.
		break;
	case SM4_OPCODE_TYPE_FLOAT:
		return 0; //TODO: lookup types.
		break;
	case SM4_OPCODE_TYPE_DOUBLE:
		return 0; //TODO: lookup types.
		break;
	case SM4_OPCODE_TYPE_INT:
		return 0; //TODO: lookup types.
		break;
	case SM4_OPCODE_TYPE_UINT:
		return 0; //TODO: lookup types.
		break;
	case SM4_OPCODE_TYPE_COUNT:
		return 0; //TODO: lookup types.
		break;
	default:
		return 0;
		break;
	}
}

inline void ConvertOpCode(ShaderConverter* _this,sm4_dcl* instruction)
{
	sm4_opcode opcode = (sm4_opcode)instruction->opcode;
	sm4_opcode_type type;
	DWORD resultId = 0;
	DWORD operand1Id = 0;
	DWORD operand2Id = 0;

	switch (opcode)
	{
	case SM4_OPCODE_DCL_RESOURCE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_RESOURCE is not implemented!";
		break;
	case SM4_OPCODE_DCL_CONSTANT_BUFFER:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_CONSTANT_BUFFER is not implemented!";
		break;
	case SM4_OPCODE_DCL_SAMPLER:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_SAMPLER is not implemented!";

		//type = (sm4_opcode_type)instruction->ops[0]->extended_token.type;
		//resultId = instruction->
		//WIP
		_this->mFunctionDefinitionInstructions.push_back(5); //word size
		_this->mFunctionDefinitionInstructions.push_back(0); //
		_this->mFunctionDefinitionInstructions.push_back(ConvertType(type)); //Result Type TODO: find result type codes.
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id
		_this->mFunctionDefinitionInstructions.push_back(operand1Id); //Image
		_this->mFunctionDefinitionInstructions.push_back(operand2Id); //Sample
		break;
	case SM4_OPCODE_DCL_INDEX_RANGE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INDEX_RANGE is not implemented!";
		break;
	case SM4_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY is not implemented!";
		break;
	case SM4_OPCODE_DCL_GS_INPUT_PRIMITIVE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_GS_INPUT_PRIMITIVE is not implemented!";
		break;
	case SM4_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT is not implemented!";
		break;
	case SM4_OPCODE_DCL_INPUT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INPUT is not implemented!";
		break;
	case SM4_OPCODE_DCL_INPUT_SGV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INPUT_SGV is not implemented!";
		break;
	case SM4_OPCODE_DCL_INPUT_SIV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INPUT_SIV is not implemented!";
		break;
	case SM4_OPCODE_DCL_INPUT_PS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INPUT_PS is not implemented!";
		break;
	case SM4_OPCODE_DCL_INPUT_PS_SGV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INPUT_PS_SGV is not implemented!";
		break;
	case SM4_OPCODE_DCL_INPUT_PS_SIV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INPUT_PS_SIV is not implemented!";
		break;
	case SM4_OPCODE_DCL_OUTPUT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_OUTPUT is not implemented!";
		break;
	case SM4_OPCODE_DCL_OUTPUT_SGV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_OUTPUT_SGV is not implemented!";
		break;
	case SM4_OPCODE_DCL_OUTPUT_SIV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_OUTPUT_SIV is not implemented!";
		break;
	case SM4_OPCODE_DCL_TEMPS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_TEMPS is not implemented!";
		break;
	case SM4_OPCODE_DCL_INDEXABLE_TEMP:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INDEXABLE_TEMP is not implemented!";
		break;
	case SM4_OPCODE_DCL_GLOBAL_FLAGS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_GLOBAL_FLAGS is not implemented!";
		break;
	case SM4_OPCODE_DCL_STREAM:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_STREAM is not implemented!";
		break;
	case SM4_OPCODE_DCL_FUNCTION_BODY:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_FUNCTION_BODY is not implemented!";
		break;
	case SM4_OPCODE_DCL_FUNCTION_TABLE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_FUNCTION_TABLE is not implemented!";
		break;
	case SM4_OPCODE_DCL_INTERFACE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INTERFACE is not implemented!";
		break;
	case SM4_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT is not implemented!";
		break;
	case SM4_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT is not implemented!";
		break;
	case SM4_OPCODE_DCL_TESS_DOMAIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_TESS_DOMAIN is not implemented!";
		break;
	case SM4_OPCODE_DCL_TESS_PARTITIONING:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_TESS_PARTITIONING is not implemented!";
		break;
	case SM4_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE is not implemented!";
		break;
	case SM4_OPCODE_DCL_HS_MAX_TESSFACTOR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_HS_MAX_TESSFACTOR is not implemented!";
		break;
	case SM4_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT is not implemented!";
		break;
	case SM4_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT is not implemented!";
		break;
	case SM4_OPCODE_DCL_THREAD_GROUP:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_THREAD_GROUP is not implemented!";
		break;
	case SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED is not implemented!";
		break;
	case SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW is not implemented!";
		break;
	case SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED is not implemented!";
		break;
	case SM4_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW is not implemented!";
		break;
	case SM4_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED is not implemented!";
		break;
	case SM4_OPCODE_DCL_RESOURCE_RAW:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_RESOURCE_RAW is not implemented!";
		break;
	case SM4_OPCODE_DCL_RESOURCE_STRUCTURED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_RESOURCE_STRUCTURED is not implemented!";
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode " << opcode << " is not implemented!";
		break;
	}
}

inline void ConvertOpCode(ShaderConverter* _this, sm4_insn* instruction)
{
	sm4_opcode opcode = (sm4_opcode)instruction->opcode;
	sm4_opcode_type type;
	DWORD resultId=0;
	DWORD operand1Id=0;
	DWORD operand2Id=0;

	//TODO: figure out how to set the id for operand 1 & 2 as well as result id.

	switch (opcode)
	{
	case SM4_OPCODE_ADD:
		
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ADD is not implemented!";

		type = (sm4_opcode_type)instruction->ops[0]->extended_token.type;
		resultId = instruction->resource_target;

		_this->mFunctionDefinitionInstructions.push_back(5); //word size
		_this->mFunctionDefinitionInstructions.push_back(129); //OpFAdd
		_this->mFunctionDefinitionInstructions.push_back(ConvertType(type)); //Result Type TODO: find result type codes.
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id
		_this->mFunctionDefinitionInstructions.push_back(operand1Id); //Operand 1 Id
		_this->mFunctionDefinitionInstructions.push_back(operand2Id); //Operand 2 Id

		break;
	case SM4_OPCODE_AND:

		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_AND is not implemented!";

		type = (sm4_opcode_type)instruction->ops[0]->extended_token.type;
		resultId = instruction->resource_target;

		_this->mFunctionDefinitionInstructions.push_back(5); //word size
		_this->mFunctionDefinitionInstructions.push_back(167); //OpLogicalAnd
		_this->mFunctionDefinitionInstructions.push_back(ConvertType(type)); //Result Type TODO: find result type codes.
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id
		_this->mFunctionDefinitionInstructions.push_back(operand1Id); //Operand 1 Id
		_this->mFunctionDefinitionInstructions.push_back(operand2Id); //Operand 2 Id

		break;
	case SM4_OPCODE_BREAK:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_BREAK is not implemented!";
		break;
	case SM4_OPCODE_BREAKC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_BREAKC is not implemented!";
		break;
	case SM4_OPCODE_CALL:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CALL is not implemented!";
		break;
	case SM4_OPCODE_CALLC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CALLC is not implemented!";
		break;
	case SM4_OPCODE_CASE:

		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CASE is not implemented!";

		resultId = instruction->resource_target;

		_this->mFunctionDefinitionInstructions.push_back(2); //word size
		_this->mFunctionDefinitionInstructions.push_back(248); //OpLabel
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id

		break;
	case SM4_OPCODE_CONTINUE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CONTINUE is not implemented!";
		break;
	case SM4_OPCODE_CONTINUEC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CONTINUEC is not implemented!";
		break;
	case SM4_OPCODE_CUT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CUT is not implemented!";
		break;
	case SM4_OPCODE_DEFAULT:

		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DEFAULT is not implemented!";

		resultId = instruction->resource_target;

		_this->mFunctionDefinitionInstructions.push_back(2); //word size
		_this->mFunctionDefinitionInstructions.push_back(248); //OpLabel
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id

		break;
	case SM4_OPCODE_DERIV_RTX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DERIV_RTX is not implemented!";
		break;
	case SM4_OPCODE_DERIV_RTY:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DERIV_RTY is not implemented!";
		break;
	case SM4_OPCODE_DISCARD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DISCARD is not implemented!";
		break;
	case SM4_OPCODE_DIV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DIV is not implemented!";
		break;
	case SM4_OPCODE_DP2:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DP2 is not implemented!";
		break;
	case SM4_OPCODE_DP3:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DP3 is not implemented!";
		break;
	case SM4_OPCODE_DP4:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DP4 is not implemented!";
		break;
	case SM4_OPCODE_ELSE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ELSE is not implemented!";
		break;
	case SM4_OPCODE_EMIT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EMIT is not implemented!";
		break;
	case SM4_OPCODE_EMITTHENCUT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EMITTHENCUT is not implemented!";
		break;
	case SM4_OPCODE_ENDIF:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ENDIF is not implemented!";
		break;
	case SM4_OPCODE_ENDLOOP:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ENDLOOP is not implemented!";
		break;
	case SM4_OPCODE_ENDSWITCH:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ENDSWITCH is not implemented!";
		break;
	case SM4_OPCODE_EQ:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EQ is not implemented!";
		break;
	case SM4_OPCODE_EXP:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EXP is not implemented!";
		break;
	case SM4_OPCODE_FRC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_FRC is not implemented!";
		break;
	case SM4_OPCODE_FTOI:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_FTOI is not implemented!";
		break;
	case SM4_OPCODE_FTOU:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_FTOU is not implemented!";
		break;
	case SM4_OPCODE_GE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_GE is not implemented!";
		break;
	case SM4_OPCODE_IADD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IADD is not implemented!";
		break;
	case SM4_OPCODE_IF:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IF is not implemented!";
		break;
	case SM4_OPCODE_IEQ:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IEQ is not implemented!";
		break;
	case SM4_OPCODE_IGE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IGE is not implemented!";
		break;
	case SM4_OPCODE_ILT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ILT is not implemented!";
		break;
	case SM4_OPCODE_IMAD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMAD is not implemented!";
		break;
	case SM4_OPCODE_IMAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMAX is not implemented!";
		break;
	case SM4_OPCODE_IMIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMIN is not implemented!";
		break;
	case SM4_OPCODE_IMUL:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMUL is not implemented!";
		break;
	case SM4_OPCODE_INE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_INE is not implemented!";
		break;
	case SM4_OPCODE_INEG:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_INEG is not implemented!";
		break;
	case SM4_OPCODE_ISHL:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ISHL is not implemented!";
		break;
	case SM4_OPCODE_ISHR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ISHR is not implemented!";
		break;
	case SM4_OPCODE_ITOF:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ITOF is not implemented!";
		break;
	case SM4_OPCODE_LABEL:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LABEL is not implemented!";
		break;
	case SM4_OPCODE_LD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LD is not implemented!";
		break;
	case SM4_OPCODE_LD_MS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LD_MS is not implemented!";
		break;
	case SM4_OPCODE_LOG:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LOG is not implemented!";
		break;
	case SM4_OPCODE_LOOP:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LOOP is not implemented!";
		break;
	case SM4_OPCODE_LT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LT is not implemented!";
		break;
	case SM4_OPCODE_MAD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_MAD is not implemented!";
		break;
	case SM4_OPCODE_MIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_MIN is not implemented!";
		break;
	case SM4_OPCODE_MAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_MAX is not implemented!";
		break;
	case SM4_OPCODE_CUSTOMDATA:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CUSTOMDATA is not implemented!";
		break;
	case SM4_OPCODE_MOV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_MOV is not implemented!";
		break;
	case SM4_OPCODE_MOVC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_MOVC is not implemented!";
		break;
	case SM4_OPCODE_MUL:

		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_MUL is not implemented!";

		type = (sm4_opcode_type)instruction->ops[0]->extended_token.type;
		resultId = instruction->resource_target;

		_this->mFunctionDefinitionInstructions.push_back(5); //word size
		_this->mFunctionDefinitionInstructions.push_back(133); //OpFMul
		_this->mFunctionDefinitionInstructions.push_back(ConvertType(type)); //Result Type TODO: find result type codes.
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id
		_this->mFunctionDefinitionInstructions.push_back(operand1Id); //Operand 1 Id
		_this->mFunctionDefinitionInstructions.push_back(operand2Id); //Operand 2 Id

		break;
	case SM4_OPCODE_NE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_NE is not implemented!";
		break;
	case SM4_OPCODE_NOP:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_NOP is not implemented!";
		break;
	case SM4_OPCODE_NOT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_NOT is not implemented!";
		break;
	case SM4_OPCODE_OR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_OR is not implemented!";
		break;
	case SM4_OPCODE_RESINFO:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_RESINFO is not implemented!";
		break;
	case SM4_OPCODE_RET:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_RET is not implemented!";
		break;
	case SM4_OPCODE_RETC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_RETC is not implemented!";
		break;
	case SM4_OPCODE_ROUND_NE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ROUND_NE is not implemented!";
		break;
	case SM4_OPCODE_ROUND_NI:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ROUND_NI is not implemented!";
		break;
	case SM4_OPCODE_ROUND_PI:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ROUND_PI is not implemented!";
		break;
	case SM4_OPCODE_ROUND_Z:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ROUND_Z is not implemented!";
		break;
	case SM4_OPCODE_RSQ:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_RSQ is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE_C:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE_C is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE_C_LZ:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE_C_LZ is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE_L:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE_L is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE_D:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE_D is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE_B:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE_B is not implemented!";
		break;
	case SM4_OPCODE_SQRT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SQRT is not implemented!";
		break;
	case SM4_OPCODE_SWITCH:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SWITCH is not implemented!";
		break;
	case SM4_OPCODE_SINCOS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SINCOS is not implemented!";
		break;
	case SM4_OPCODE_UDIV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UDIV is not implemented!";
		break;
	case SM4_OPCODE_ULT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ULT is not implemented!";
		break;
	case SM4_OPCODE_UGE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UGE is not implemented!";
		break;
	case SM4_OPCODE_UMUL:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UMUL is not implemented!";
		break;
	case SM4_OPCODE_UMAD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UMAD is not implemented!";
		break;
	case SM4_OPCODE_UMAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UMAX is not implemented!";
		break;
	case SM4_OPCODE_UMIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UMIN is not implemented!";
		break;
	case SM4_OPCODE_USHR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_USHR is not implemented!";
		break;
	case SM4_OPCODE_UTOF:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UTOF is not implemented!";
		break;
	case SM4_OPCODE_XOR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_XOR is not implemented!";
		break;
	case SM4_OPCODE_D3D10_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_D3D10_COUNT is not implemented!";
		break;
	case SM4_OPCODE_LOD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LOD is not implemented!";
		break;
	case SM4_OPCODE_GATHER4:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_GATHER4 is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE_POS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE_POS is not implemented!";
		break;
	case SM4_OPCODE_SAMPLE_INFO:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SAMPLE_INFO is not implemented!";
		break;
	case SM4_OPCODE_D3D10_1_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_D3D10_1_COUNT is not implemented!";
		break;
	case SM4_OPCODE_HS_DECLS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_HS_DECLS is not implemented!";
		break;
	case SM4_OPCODE_HS_CONTROL_POINT_PHASE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_HS_CONTROL_POINT_PHASE is not implemented!";
		break;
	case SM4_OPCODE_HS_FORK_PHASE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_HS_FORK_PHASE is not implemented!";
		break;
	case SM4_OPCODE_HS_JOIN_PHASE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_HS_JOIN_PHASE is not implemented!";
		break;
	case SM4_OPCODE_EMIT_STREAM:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EMIT_STREAM is not implemented!";
		break;
	case SM4_OPCODE_CUT_STREAM:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_CUT_STREAM is not implemented!";
		break;
	case SM4_OPCODE_EMITTHENCUT_STREAM:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EMITTHENCUT_STREAM is not implemented!";
		break;
	case SM4_OPCODE_INTERFACE_CALL:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_INTERFACE_CALL is not implemented!";
		break;
	case SM4_OPCODE_BUFINFO:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_BUFINFO is not implemented!";
		break;
	case SM4_OPCODE_DERIV_RTX_COARSE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DERIV_RTX_COARSE is not implemented!";
		break;
	case SM4_OPCODE_DERIV_RTX_FINE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DERIV_RTX_FINE is not implemented!";
		break;
	case SM4_OPCODE_DERIV_RTY_COARSE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DERIV_RTY_COARSE is not implemented!";
		break;
	case SM4_OPCODE_DERIV_RTY_FINE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DERIV_RTY_FINE is not implemented!";
		break;
	case SM4_OPCODE_GATHER4_C:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_GATHER4_C is not implemented!";
		break;
	case SM4_OPCODE_GATHER4_PO:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_GATHER4_PO is not implemented!";
		break;
	case SM4_OPCODE_GATHER4_PO_C:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_GATHER4_PO_C is not implemented!";
		break;
	case SM4_OPCODE_RCP:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_RCP is not implemented!";
		break;
	case SM4_OPCODE_F32TOF16:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_F32TOF16 is not implemented!";
		break;
	case SM4_OPCODE_F16TOF32:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_F16TOF32 is not implemented!";
		break;
	case SM4_OPCODE_UADDC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UADDC is not implemented!";
		break;
	case SM4_OPCODE_USUBB:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_USUBB is not implemented!";
		break;
	case SM4_OPCODE_COUNTBITS:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_COUNTBITS is not implemented!";
		break;
	case SM4_OPCODE_FIRSTBIT_HI:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_FIRSTBIT_HI is not implemented!";
		break;
	case SM4_OPCODE_FIRSTBIT_LO:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_FIRSTBIT_LO is not implemented!";
		break;
	case SM4_OPCODE_FIRSTBIT_SHI:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_FIRSTBIT_SHI is not implemented!";
		break;
	case SM4_OPCODE_UBFE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_UBFE is not implemented!";
		break;
	case SM4_OPCODE_IBFE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IBFE is not implemented!";
		break;
	case SM4_OPCODE_BFI:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_BFI is not implemented!";
		break;
	case SM4_OPCODE_BFREV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_BFREV is not implemented!";
		break;
	case SM4_OPCODE_SWAPC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SWAPC is not implemented!";
		break;
	case SM4_OPCODE_LD_UAV_TYPED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LD_UAV_TYPED is not implemented!";
		break;
	case SM4_OPCODE_STORE_UAV_TYPED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_STORE_UAV_TYPED is not implemented!";
		break;
	case SM4_OPCODE_LD_RAW:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LD_RAW is not implemented!";
		break;
	case SM4_OPCODE_STORE_RAW:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_STORE_RAW is not implemented!";
		break;
	case SM4_OPCODE_LD_STRUCTURED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_LD_STRUCTURED is not implemented!";
		break;
	case SM4_OPCODE_STORE_STRUCTURED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_STORE_STRUCTURED is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_AND:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_AND is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_OR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_OR is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_XOR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_XOR is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_CMP_STORE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_CMP_STORE is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_IADD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_IADD is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_IMAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_IMAX is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_IMIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_IMIN is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_UMAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_UMAX is not implemented!";
		break;
	case SM4_OPCODE_ATOMIC_UMIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_ATOMIC_UMIN is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_ALLOC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_ALLOC is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_CONSUME:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_CONSUME is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_IADD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_IADD is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_AND:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_AND is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_OR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_OR is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_XOR:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_XOR is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_EXCH:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_EXCH is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_CMP_EXCH:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_CMP_EXCH is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_IMAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_IMAX is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_IMIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_IMIN is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_UMAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_UMAX is not implemented!";
		break;
	case SM4_OPCODE_IMM_ATOMIC_UMIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_IMM_ATOMIC_UMIN is not implemented!";
		break;
	case SM4_OPCODE_SYNC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_SYNC is not implemented!";
		break;
	case SM4_OPCODE_DADD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DADD is not implemented!";
		break;
	case SM4_OPCODE_DMAX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DMAX is not implemented!";
		break;
	case SM4_OPCODE_DMIN:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DMIN is not implemented!";
		break;
	case SM4_OPCODE_DMUL:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DMUL is not implemented!";
		break;
	case SM4_OPCODE_DEQ:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DEQ is not implemented!";
		break;
	case SM4_OPCODE_DGE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DGE is not implemented!";
		break;
	case SM4_OPCODE_DLT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DLT is not implemented!";
		break;
	case SM4_OPCODE_DNE:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DNE is not implemented!";
		break;
	case SM4_OPCODE_DMOV:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DMOV is not implemented!";
		break;
	case SM4_OPCODE_DMOVC:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DMOVC is not implemented!";
		break;
	case SM4_OPCODE_DTOF:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DTOF is not implemented!";
		break;
	case SM4_OPCODE_FTOD:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_FTOD is not implemented!";
		break;
	case SM4_OPCODE_EVAL_SNAPPED:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EVAL_SNAPPED is not implemented!";
		break;
	case SM4_OPCODE_EVAL_SAMPLE_INDEX:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EVAL_SAMPLE_INDEX is not implemented!";
		break;
	case SM4_OPCODE_EVAL_CENTROID:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_EVAL_CENTROID is not implemented!";
		break;
	case SM4_OPCODE_DCL_GS_INSTANCE_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_DCL_GS_INSTANCE_COUNT is not implemented!";
		break;
	case SM4_OPCODE_COUNT:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode SM4_OPCODE_COUNT is not implemented!";
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "ConvertOpCode " << opcode << " is not implemented!";
		break;
	}
}

ShaderConverter::ShaderConverter(VkDevice device)
	: mDevice(device)
{

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
	data += sizeof(ShaderHeader);

	ConvertedShader output;
	//output.Size = header.ShaderSize; //The size is not a parameter to the d3d9 functions so I have to take it from the header.

	//dxbc_container* dxbc = dxbc_parse(shader, 0);
	//if (dxbc) //!= nullptr
	//{
	//	dxbc_chunk_header* sm4_chunk = dxbc_find_shader_bytecode(shader, header.ShaderSize);
	//	if (sm4_chunk) //!= nullptr
	//	{
	//		sm4_program* sm4 = sm4_parse(sm4_chunk + 1, bswap_le32(sm4_chunk->size));
	//		if (sm4) //!= nullptr
	//		{
	//			for (size_t i = 0; i < sm4->num_params_in; i++)
	//			{
	//				auto& parameter = sm4->params_in[i];
	//				auto& attributeDescription = output.mVertexInputAttributeDescription[i];

	//				attributeDescription.binding = parameter.Stream;
	//				attributeDescription.location = i;
	//				attributeDescription.format = ConvertType(parameter.ComponentType);
	//				attributeDescription.offset = parameter.Register*4;
	//			}

	//			for (size_t i = 0; i < sm4->dcls.size(); i++)
	//			{
	//				ConvertOpCode(this,sm4->dcls[i]);
	//			}

	//			for (size_t i = 0; i < sm4->insns.size(); i++)
	//			{
	//				ConvertOpCode(this,sm4->insns[i]);
	//			}
	//			
	//			delete sm4;
	//		}		
	//	}
	//	delete dxbc;
	//}

	//uint32_t chunkType;
	//char* chunkPointer = nullptr;

	//std::vector<uint32_t> mOutputInstructions;

	//The header is followed by a list of chunk offsets so loop over that and process each chunk.
	//uint32_t* indexPointer = (uint32_t*)data;
	//for (size_t i = 0; i < header.ChunkCount; i++)
	//{
	//	indexPointer += i; //sizeof is 4 bytes or one uint32.
	//	uint32_t chunkOffset = (*indexPointer);
	//	
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

	mInstructions.insert(std::end(mInstructions), std::begin(mCapabilityInstructions), std::end(mCapabilityInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mExtensionInstructions), std::end(mExtensionInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mImportExtendedInstructions), std::end(mImportExtendedInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mMemoryModelInstructions), std::end(mMemoryModelInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mEntryPointInstructions), std::end(mEntryPointInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mExecutionModeInstructions), std::end(mExecutionModeInstructions));

	mInstructions.insert(std::end(mInstructions), std::begin(mStringInstructions), std::end(mStringInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mSourceExtensionInstructions), std::end(mSourceExtensionInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mSourceInstructions), std::end(mSourceInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mSourceContinuedInstructions), std::end(mSourceContinuedInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mNameInstructions), std::end(mNameInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mMemberNameInstructions), std::end(mMemberNameInstructions));

	mInstructions.insert(std::end(mInstructions), std::begin(mDecorateInstructions), std::end(mDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mMemberDecorateInstructions), std::end(mMemberDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mGroupDecorateInstructions), std::end(mGroupDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mGroupMemberDecorateInstructions), std::end(mGroupMemberDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mDecorationGroupInstructions), std::end(mDecorationGroupInstructions));

	mInstructions.insert(std::end(mInstructions), std::begin(mTypeInstructions), std::end(mTypeInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mFunctionDeclarationInstructions), std::end(mFunctionDeclarationInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mFunctionDefinitionInstructions), std::end(mFunctionDefinitionInstructions));

	mCapabilityInstructions.clear();
	mExtensionInstructions.clear();
	mImportExtendedInstructions.clear();
	mMemoryModelInstructions.clear();
	mEntryPointInstructions.clear();
	mExecutionModeInstructions.clear();

	mStringInstructions.clear();
	mSourceExtensionInstructions.clear();
	mSourceInstructions.clear();
	mSourceContinuedInstructions.clear();
	mNameInstructions.clear();
	mMemberNameInstructions.clear();

	mDecorateInstructions.clear();
	mMemberDecorateInstructions.clear();
	mGroupDecorateInstructions.clear();
	mGroupMemberDecorateInstructions.clear();
	mDecorationGroupInstructions.clear();

	mTypeInstructions.clear();
	mFunctionDeclarationInstructions.clear();
	mFunctionDefinitionInstructions.clear();

	VkResult result = VK_SUCCESS;
	VkShaderModuleCreateInfo moduleCreateInfo = {};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;
	moduleCreateInfo.codeSize = mInstructions.size() * sizeof(uint32_t);
	moduleCreateInfo.pCode = (uint32_t*)mInstructions.data(); //Why is this uint32_t* if the size is in bytes?
	moduleCreateInfo.flags = 0;
	result = vkCreateShaderModule(mDevice, &moduleCreateInfo, NULL, &output.ShaderModule);

	mInstructions.clear();

	return output; //Return value optimization don't fail me now.
}