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
		break;
	}
}

inline void ConvertOpCode(ShaderConverter* _this,sm4_dcl* instruction)
{
	sm4_opcode opcode = (sm4_opcode)instruction->opcode;

	switch (opcode)
	{
	default:
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
		//TODO: BREAK
		break;
	case SM4_OPCODE_BREAKC:
		//TODO: BREAKC
		break;
	case SM4_OPCODE_CALL:
		//TODO: CALL
		break;
	case SM4_OPCODE_CALLC:
		//TODO: CALLC
		break;
	case SM4_OPCODE_CASE:

		resultId = instruction->resource_target;

		_this->mFunctionDefinitionInstructions.push_back(2); //word size
		_this->mFunctionDefinitionInstructions.push_back(248); //OpLabel
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id

		break;
	case SM4_OPCODE_CONTINUE:
		//TODO: CONTINUE
		break;
	case SM4_OPCODE_CONTINUEC:
		//TODO: CONTINUEC
		break;
	case SM4_OPCODE_CUT:
		//TODO: CUT
		break;
	case SM4_OPCODE_DEFAULT:

		resultId = instruction->resource_target;

		_this->mFunctionDefinitionInstructions.push_back(2); //word size
		_this->mFunctionDefinitionInstructions.push_back(248); //OpLabel
		_this->mFunctionDefinitionInstructions.push_back(resultId); //Result Id

		break;
	case SM4_OPCODE_DERIV_RTX:
		break;
	case SM4_OPCODE_DERIV_RTY:
		break;
	case SM4_OPCODE_DISCARD:
		break;
	case SM4_OPCODE_DIV:
		break;
	case SM4_OPCODE_DP2:
		break;
	case SM4_OPCODE_DP3:
		break;
	case SM4_OPCODE_DP4:
		break;
	case SM4_OPCODE_ELSE:
		break;
	case SM4_OPCODE_EMIT:
		break;
	case SM4_OPCODE_EMITTHENCUT:
		break;
	case SM4_OPCODE_ENDIF:
		break;
	case SM4_OPCODE_ENDLOOP:
		break;
	case SM4_OPCODE_ENDSWITCH:
		break;
	case SM4_OPCODE_EQ:
		break;
	case SM4_OPCODE_EXP:
		break;
	case SM4_OPCODE_FRC:
		break;
	case SM4_OPCODE_FTOI:
		break;
	case SM4_OPCODE_FTOU:
		break;
	case SM4_OPCODE_GE:
		break;
	case SM4_OPCODE_IADD:
		break;
	case SM4_OPCODE_IF:
		break;
	case SM4_OPCODE_IEQ:
		break;
	case SM4_OPCODE_IGE:
		break;
	case SM4_OPCODE_ILT:
		break;
	case SM4_OPCODE_IMAD:
		break;
	case SM4_OPCODE_IMAX:
		break;
	case SM4_OPCODE_IMIN:
		break;
	case SM4_OPCODE_IMUL:
		break;
	case SM4_OPCODE_INE:
		break;
	case SM4_OPCODE_INEG:
		break;
	case SM4_OPCODE_ISHL:
		break;
	case SM4_OPCODE_ISHR:
		break;
	case SM4_OPCODE_ITOF:
		break;
	case SM4_OPCODE_LABEL:
		break;
	case SM4_OPCODE_LD:
		break;
	case SM4_OPCODE_LD_MS:
		break;
	case SM4_OPCODE_LOG:
		break;
	case SM4_OPCODE_LOOP:
		break;
	case SM4_OPCODE_LT:
		break;
	case SM4_OPCODE_MAD:
		break;
	case SM4_OPCODE_MIN:
		break;
	case SM4_OPCODE_MAX:
		break;
	case SM4_OPCODE_CUSTOMDATA:
		break;
	case SM4_OPCODE_MOV:
		break;
	case SM4_OPCODE_MOVC:
		break;
	case SM4_OPCODE_MUL:
		break;
	case SM4_OPCODE_NE:
		break;
	case SM4_OPCODE_NOP:
		break;
	case SM4_OPCODE_NOT:
		break;
	case SM4_OPCODE_OR:
		break;
	case SM4_OPCODE_RESINFO:
		break;
	case SM4_OPCODE_RET:
		break;
	case SM4_OPCODE_RETC:
		break;
	case SM4_OPCODE_ROUND_NE:
		break;
	case SM4_OPCODE_ROUND_NI:
		break;
	case SM4_OPCODE_ROUND_PI:
		break;
	case SM4_OPCODE_ROUND_Z:
		break;
	case SM4_OPCODE_RSQ:
		break;
	case SM4_OPCODE_SAMPLE:
		break;
	case SM4_OPCODE_SAMPLE_C:
		break;
	case SM4_OPCODE_SAMPLE_C_LZ:
		break;
	case SM4_OPCODE_SAMPLE_L:
		break;
	case SM4_OPCODE_SAMPLE_D:
		break;
	case SM4_OPCODE_SAMPLE_B:
		break;
	case SM4_OPCODE_SQRT:
		break;
	case SM4_OPCODE_SWITCH:
		break;
	case SM4_OPCODE_SINCOS:
		break;
	case SM4_OPCODE_UDIV:
		break;
	case SM4_OPCODE_ULT:
		break;
	case SM4_OPCODE_UGE:
		break;
	case SM4_OPCODE_UMUL:
		break;
	case SM4_OPCODE_UMAD:
		break;
	case SM4_OPCODE_UMAX:
		break;
	case SM4_OPCODE_UMIN:
		break;
	case SM4_OPCODE_USHR:
		break;
	case SM4_OPCODE_UTOF:
		break;
	case SM4_OPCODE_XOR:
		break;
	case SM4_OPCODE_DCL_RESOURCE:
		break;
	case SM4_OPCODE_DCL_CONSTANT_BUFFER:
		break;
	case SM4_OPCODE_DCL_SAMPLER:
		break;
	case SM4_OPCODE_DCL_INDEX_RANGE:
		break;
	case SM4_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY:
		break;
	case SM4_OPCODE_DCL_GS_INPUT_PRIMITIVE:
		break;
	case SM4_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT:
		break;
	case SM4_OPCODE_DCL_INPUT:
		break;
	case SM4_OPCODE_DCL_INPUT_SGV:
		break;
	case SM4_OPCODE_DCL_INPUT_SIV:
		break;
	case SM4_OPCODE_DCL_INPUT_PS:
		break;
	case SM4_OPCODE_DCL_INPUT_PS_SGV:
		break;
	case SM4_OPCODE_DCL_INPUT_PS_SIV:
		break;
	case SM4_OPCODE_DCL_OUTPUT:
		break;
	case SM4_OPCODE_DCL_OUTPUT_SGV:
		break;
	case SM4_OPCODE_DCL_OUTPUT_SIV:
		break;
	case SM4_OPCODE_DCL_TEMPS:
		break;
	case SM4_OPCODE_DCL_INDEXABLE_TEMP:
		break;
	case SM4_OPCODE_DCL_GLOBAL_FLAGS:
		break;
	case SM4_OPCODE_D3D10_COUNT:
		break;
	case SM4_OPCODE_LOD:
		break;
	case SM4_OPCODE_GATHER4:
		break;
	case SM4_OPCODE_SAMPLE_POS:
		break;
	case SM4_OPCODE_SAMPLE_INFO:
		break;
	case SM4_OPCODE_D3D10_1_COUNT:
		break;
	case SM4_OPCODE_HS_DECLS:
		break;
	case SM4_OPCODE_HS_CONTROL_POINT_PHASE:
		break;
	case SM4_OPCODE_HS_FORK_PHASE:
		break;
	case SM4_OPCODE_HS_JOIN_PHASE:
		break;
	case SM4_OPCODE_EMIT_STREAM:
		break;
	case SM4_OPCODE_CUT_STREAM:
		break;
	case SM4_OPCODE_EMITTHENCUT_STREAM:
		break;
	case SM4_OPCODE_INTERFACE_CALL:
		break;
	case SM4_OPCODE_BUFINFO:
		break;
	case SM4_OPCODE_DERIV_RTX_COARSE:
		break;
	case SM4_OPCODE_DERIV_RTX_FINE:
		break;
	case SM4_OPCODE_DERIV_RTY_COARSE:
		break;
	case SM4_OPCODE_DERIV_RTY_FINE:
		break;
	case SM4_OPCODE_GATHER4_C:
		break;
	case SM4_OPCODE_GATHER4_PO:
		break;
	case SM4_OPCODE_GATHER4_PO_C:
		break;
	case SM4_OPCODE_RCP:
		break;
	case SM4_OPCODE_F32TOF16:
		break;
	case SM4_OPCODE_F16TOF32:
		break;
	case SM4_OPCODE_UADDC:
		break;
	case SM4_OPCODE_USUBB:
		break;
	case SM4_OPCODE_COUNTBITS:
		break;
	case SM4_OPCODE_FIRSTBIT_HI:
		break;
	case SM4_OPCODE_FIRSTBIT_LO:
		break;
	case SM4_OPCODE_FIRSTBIT_SHI:
		break;
	case SM4_OPCODE_UBFE:
		break;
	case SM4_OPCODE_IBFE:
		break;
	case SM4_OPCODE_BFI:
		break;
	case SM4_OPCODE_BFREV:
		break;
	case SM4_OPCODE_SWAPC:
		break;
	case SM4_OPCODE_DCL_STREAM:
		break;
	case SM4_OPCODE_DCL_FUNCTION_BODY:
		break;
	case SM4_OPCODE_DCL_FUNCTION_TABLE:
		break;
	case SM4_OPCODE_DCL_INTERFACE:
		break;
	case SM4_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT:
		break;
	case SM4_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT:
		break;
	case SM4_OPCODE_DCL_TESS_DOMAIN:
		break;
	case SM4_OPCODE_DCL_TESS_PARTITIONING:
		break;
	case SM4_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE:
		break;
	case SM4_OPCODE_DCL_HS_MAX_TESSFACTOR:
		break;
	case SM4_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT:
		break;
	case SM4_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT:
		break;
	case SM4_OPCODE_DCL_THREAD_GROUP:
		break;
	case SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED:
		break;
	case SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW:
		break;
	case SM4_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED:
		break;
	case SM4_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW:
		break;
	case SM4_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED:
		break;
	case SM4_OPCODE_DCL_RESOURCE_RAW:
		break;
	case SM4_OPCODE_DCL_RESOURCE_STRUCTURED:
		break;
	case SM4_OPCODE_LD_UAV_TYPED:
		break;
	case SM4_OPCODE_STORE_UAV_TYPED:
		break;
	case SM4_OPCODE_LD_RAW:
		break;
	case SM4_OPCODE_STORE_RAW:
		break;
	case SM4_OPCODE_LD_STRUCTURED:
		break;
	case SM4_OPCODE_STORE_STRUCTURED:
		break;
	case SM4_OPCODE_ATOMIC_AND:
		break;
	case SM4_OPCODE_ATOMIC_OR:
		break;
	case SM4_OPCODE_ATOMIC_XOR:
		break;
	case SM4_OPCODE_ATOMIC_CMP_STORE:
		break;
	case SM4_OPCODE_ATOMIC_IADD:
		break;
	case SM4_OPCODE_ATOMIC_IMAX:
		break;
	case SM4_OPCODE_ATOMIC_IMIN:
		break;
	case SM4_OPCODE_ATOMIC_UMAX:
		break;
	case SM4_OPCODE_ATOMIC_UMIN:
		break;
	case SM4_OPCODE_IMM_ATOMIC_ALLOC:
		break;
	case SM4_OPCODE_IMM_ATOMIC_CONSUME:
		break;
	case SM4_OPCODE_IMM_ATOMIC_IADD:
		break;
	case SM4_OPCODE_IMM_ATOMIC_AND:
		break;
	case SM4_OPCODE_IMM_ATOMIC_OR:
		break;
	case SM4_OPCODE_IMM_ATOMIC_XOR:
		break;
	case SM4_OPCODE_IMM_ATOMIC_EXCH:
		break;
	case SM4_OPCODE_IMM_ATOMIC_CMP_EXCH:
		break;
	case SM4_OPCODE_IMM_ATOMIC_IMAX:
		break;
	case SM4_OPCODE_IMM_ATOMIC_IMIN:
		break;
	case SM4_OPCODE_IMM_ATOMIC_UMAX:
		break;
	case SM4_OPCODE_IMM_ATOMIC_UMIN:
		break;
	case SM4_OPCODE_SYNC:
		break;
	case SM4_OPCODE_DADD:
		break;
	case SM4_OPCODE_DMAX:
		break;
	case SM4_OPCODE_DMIN:
		break;
	case SM4_OPCODE_DMUL:
		break;
	case SM4_OPCODE_DEQ:
		break;
	case SM4_OPCODE_DGE:
		break;
	case SM4_OPCODE_DLT:
		break;
	case SM4_OPCODE_DNE:
		break;
	case SM4_OPCODE_DMOV:
		break;
	case SM4_OPCODE_DMOVC:
		break;
	case SM4_OPCODE_DTOF:
		break;
	case SM4_OPCODE_FTOD:
		break;
	case SM4_OPCODE_EVAL_SNAPPED:
		break;
	case SM4_OPCODE_EVAL_SAMPLE_INDEX:
		break;
	case SM4_OPCODE_EVAL_CENTROID:
		break;
	case SM4_OPCODE_DCL_GS_INSTANCE_COUNT:
		break;
	case SM4_OPCODE_COUNT:
		break;
	default:
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

				for (size_t i = 0; i < sm4->dcls.size(); i++)
				{
					ConvertOpCode(this,sm4->dcls[i]);
				}

				for (size_t i = 0; i < sm4->insns.size(); i++)
				{
					ConvertOpCode(this,sm4->insns[i]);
				}
				
				delete sm4;
			}		
		}
		delete dxbc;
	}

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
	moduleCreateInfo.codeSize = mInstructions.size()*sizeof(uint32_t);
	moduleCreateInfo.pCode = (uint32_t*)mInstructions.data(); //Why is this uint32_t* if the size is in bytes?
	moduleCreateInfo.flags = 0;
	result = vkCreateShaderModule(mDevice, &moduleCreateInfo, NULL, &output.ShaderModule);

	mInstructions.clear();

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