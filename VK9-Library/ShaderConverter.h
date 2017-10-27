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

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <spirv.hpp>
#include "CTypes.h"

/*
http://timjones.io/blog/archive/2015/09/02/parsing-direct3d-shader-bytecode
https://msdn.microsoft.com/en-us/library/bb219840(VS.85).aspx#Shader_Binary_Format
http://stackoverflow.com/questions/2545704/format-of-compiled-directx9-shader-files
https://msdn.microsoft.com/en-us/library/windows/hardware/ff552891(v=vs.85).aspx
https://github.com/ValveSoftware/ToGL
*/

#define PACK(c0, c1, c2, c3) \
    (((uint32_t)(uint8_t)(c0) << 24) | \
    ((uint32_t)(uint8_t)(c1) << 16) | \
    ((uint32_t)(uint8_t)(c2) << 8) | \
    ((uint32_t)(uint8_t)(c3)))

struct ConvertedShader
{
	//Information about layout
	//VkVertexInputBindingDescription mVertexInputBindingDescription[16] = {};
	uint32_t mVertexInputAttributeDescriptionCount = 0;
	VkVertexInputAttributeDescription mVertexInputAttributeDescription[32] = {};
	uint32_t mDescriptorSetLayoutBindingCount = 0;
	VkDescriptorSetLayoutBinding mDescriptorSetLayoutBinding[16] = {};

	//Actual Payload
	UINT Size = 0;
	VkShaderModule ShaderModule = VK_NULL_HANDLE;
	PushConstants mPushConstants;
};

//https://msdn.microsoft.com/en-us/library/windows/hardware/ff552738(v=vs.85).aspx
union DestinationParameterToken
{
	struct
	{
		uint32_t RegisterNumber : 11;
		uint32_t RegisterType2 : 2;
		bool UseRelativeAddressing : 1;
		uint32_t Reserved : 2;
		uint32_t WriteMask : 4;
		uint32_t ResultModifier : 4;
		uint32_t ResultShiftScale : 4;
		uint32_t RegisterType1 : 3;
		uint32_t FinalBit : 1;
	};
	uint32_t AllField;
};

union OpcodeDescription
{
	struct
	{
		spv::Op Opcode : 16;
		uint32_t WordCount : 16;
	};
	uint32_t Word;
};

//In DXBC can have float's in the DWORD stream so it was either this or break alias rules.
union Token
{
	float f;
	uint32_t i;
	DestinationParameterToken DestinationParameterToken;
};

struct TypeDescription
{
	spv::Op PrimaryType = spv::OpTypeVoid;
	spv::Op SecondaryType = spv::OpTypeVoid;
	spv::Op TernaryType = spv::OpTypeVoid;
	uint32_t ComponentCount = 0;
	std::vector<uint32_t> Arguments;

	bool operator ==(const TypeDescription &value) const
	{
		return this->PrimaryType == value.PrimaryType && this->SecondaryType == value.SecondaryType && this->TernaryType == value.TernaryType && this->ComponentCount == value.ComponentCount;
	}

	bool operator <(const TypeDescription &value) const
	{
		return this->PrimaryType < value.PrimaryType || this->SecondaryType < value.SecondaryType || this->TernaryType < value.TernaryType || this->ComponentCount < value.ComponentCount;
	}
};

inline uint32_t Pack(uint32_t wordCount, spv::Op opcode)
{
	OpcodeDescription opcodeDescription;

	opcodeDescription.WordCount = wordCount;
	opcodeDescription.Opcode = opcode;

	return opcodeDescription.Word;
}


inline void PutStringInVector(std::string& text, std::vector<uint32_t>& words)
{
	for (size_t i = 0; i < text.length(); i+=4)
	{
		uint32_t difference = text.length() - (i);
		const char* value = text.c_str();

		switch (difference)
		{
		case 0:
			break;
		case 1:
			words.push_back(PACK('\0', '\0', '\0', value[i]));
			break;
		case 2:
			words.push_back(PACK('\0', '\0', value[i + 1], value[i]));
			break;
		case 3:
			words.push_back(PACK('\0', value[i + 2], value[i + 1], value[i]));
			break;
		default:
			words.push_back(PACK(value[i+3], value[i + 2], value[i + 1], value[i]));
			break;
		}	
	}

	if (text.length() % 4 == 0)
	{
		words.push_back(0); //null terminator if all words have characters.
	}

}

class ShaderConverter
{
protected:
	VkDevice mDevice;
	ConvertedShader mConvertedShader;
public:
	ShaderConverter(VkDevice device);
	ConvertedShader Convert(uint32_t* shader);
private:	
	std::vector<uint32_t> mInstructions; //used to store the combined instructions for creating a module.

	boost::container::flat_map<D3DSHADER_PARAM_REGISTER_TYPE, boost::container::flat_map<uint32_t, uint32_t> > mRegistersById;
	boost::container::flat_map<D3DSHADER_PARAM_REGISTER_TYPE, boost::container::flat_map<uint32_t, uint32_t> > mIdsByRegister;
	
	boost::container::flat_map<uint32_t, uint32_t> mInputRegisters;
	boost::container::flat_map<uint32_t, uint32_t> mOutputRegisters;
	boost::container::flat_map<_D3DDECLUSAGE, uint32_t> mOutputRegisterUsages;

	boost::container::flat_map<TypeDescription, uint32_t> mTypeIdPairs;
	boost::container::flat_map<uint32_t, TypeDescription> mIdTypePairs;

	std::vector<uint32_t> mCapabilityInstructions;
	std::vector<uint32_t> mExtensionInstructions;
	std::vector<uint32_t> mImportExtendedInstructions;
	std::vector<uint32_t> mMemoryModelInstructions;
	std::vector<uint32_t> mEntryPointInstructions;
	std::vector<uint32_t> mExecutionModeInstructions;

	std::vector<uint32_t> mStringInstructions;
	std::vector<uint32_t> mSourceExtensionInstructions;
	std::vector<uint32_t> mSourceInstructions;
	std::vector<uint32_t> mSourceContinuedInstructions;
	std::vector<uint32_t> mNameInstructions;
	std::vector<uint32_t> mMemberNameInstructions;

	std::vector<uint32_t> mDecorateInstructions;
	std::vector<uint32_t> mMemberDecorateInstructions;
	std::vector<uint32_t> mGroupDecorateInstructions;
	std::vector<uint32_t> mGroupMemberDecorateInstructions;
	std::vector<uint32_t> mDecorationGroupInstructions;

	std::vector<uint32_t> mTypeInstructions;
	std::vector<uint32_t> mFunctionDeclarationInstructions;
	std::vector<uint32_t> mFunctionDefinitionInstructions;

	uint32_t* mBaseToken = nullptr;
	uint32_t* mNextToken = nullptr;
	uint32_t mBaseId = 1;
	uint32_t mNextId = 1;
	uint32_t mTokenOffset = 0;
	uint32_t mMinorVersion = 0;
	uint32_t mMajorVersion = 0;
	uint32_t mPositionRegister = -1;
	uint32_t mEntryPointTypeId = -1;
	uint32_t mEntryPointId = -1;
	bool mIsVertexShader = false;

	Token GetNextToken();
	void SkipTokens(uint32_t numberToSkip);
	uint32_t GetNextId();
	void SkipIds(uint32_t numberToSkip);
	uint32_t GetOpcode(uint32_t token);
	uint32_t GetOpcodeData(uint32_t token);
	uint32_t GetTextureType(uint32_t token);
	_D3DSHADER_PARAM_REGISTER_TYPE GetRegisterType(uint32_t token);
	uint32_t GetRegisterNumber(const Token& token);
	uint32_t GetUsage(uint32_t token);
	uint32_t GetUsageIndex(uint32_t token);
	uint32_t GetSpirVTypeId(spv::Op registerType);
	uint32_t GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2);
	uint32_t GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2,uint32_t componentCount);
	uint32_t GetSpirVTypeId(TypeDescription& registerType);
	uint32_t GetNextVersionId(const Token& token);
	uint32_t GetIdByRegister(const Token& token);
	void SetIdByRegister(const Token& token, uint32_t id);
	TypeDescription GetTypeByRegister(const Token& token);
	uint32_t GetSwizzledId(const Token& token, uint32_t inputId = UINT_MAX);
	uint32_t ApplyWriteMask(const Token& token, uint32_t inputId);
	void GenerateStore(const Token& token, uint32_t inputId);
	void GenerateDecoration(uint32_t registerNumber, uint32_t inputId, uint32_t usage=-1);
	void CombineSpirVOpCodes();
	void CreateSpirVModule();

	//declare
	void Process_DCL_Pixel();
	void Process_DCL_Vertex();
	void Process_DCL();
	void Process_DEF();
	void Process_DEFI();
	void Process_DEFB();

	//Unary Operators
	void Process_MOV();

	//Binary Operators
	void Process_MUL();
	void Process_ADD();
	void Process_SUB();
	void Process_MIN();
	void Process_MAX();
	void Process_DP3();
	void Process_DP4();
	void Process_TEX();

	//ternary operators
	void Process_MAD();
};

#endif //SHADERCONVERTER_H
//
