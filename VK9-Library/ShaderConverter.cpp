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

#include "ShaderConverter.h"
#include "Utilities.h"

ConvertedShader ShaderConverter::Convert(DWORD* shader)
{
	/*
	http://timjones.io/blog/archive/2015/09/02/parsing-direct3d-shader-bytecode
	https://msdn.microsoft.com/en-us/library/bb219840(VS.85).aspx#Shader_Binary_Format
	http://stackoverflow.com/questions/2545704/format-of-compiled-directx9-shader-files
	https://msdn.microsoft.com/en-us/library/windows/hardware/ff552891(v=vs.85).aspx
	*/

	ConvertedShader output;
	ShaderHeader header;
	uint32_t chunkType;
	char* data = (char*)shader;
	char* chunkPointer = nullptr;

	memcpy(&header, (void*)data, sizeof(ShaderHeader));
	data += sizeof(ShaderHeader);

	//The header is followed by a list of chunk offsets so loop over that and process each chunk.
	for (size_t i = 0; i < header.ChunkCount; i++)
	{
		uint32_t chunkOffset = (*(uint32_t*)data);
		data += sizeof(uint32_t);

		chunkPointer = (char*)shader;
		chunkPointer += chunkOffset;

		memcpy(&chunkType, (void*)chunkPointer, sizeof(char[4]));
		switch (chunkType)
		{
		case ICFE:
		{
			ICFEChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(ICFEChunk));
		}
		break;
		case ISGN:
		{
			ISGNChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(ISGNChunk));
		}
		break;
		case OSG5:
		{
			OSG5Chunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(OSG5Chunk));
		}
		break;
		case OSGN:
		{
			OSGNChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(OSGNChunk));
		}
		break;
		case PCSG:
		{
			PCSGChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(PCSGChunk));
		}
		break;
		case RDEF:
		{
			RDEFChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(RDEFChunk));
		}
		break;
		case SDGB:
		{
			SDGBChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(SDGBChunk));
		}
		break;
		case SFI0:
		{
			SFI0Chunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(SFI0Chunk));
		}
		break;
		case SHDR:
		{
			SHDRChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(SHDRChunk));
		}
		break;
		case SHEX:
		{
			SHEXChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(SHEXChunk));
		}
		break;
		case SPDB:
		{
			SPDBChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(SPDBChunk));
		}
		break;
		case STAT:
		{
			STATChunk chunk;
			memcpy(&chunk, chunkPointer, sizeof(STATChunk));
		}
		break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::Convert unknown chunk type " << chunkType;
			break;
		}
	}

	//DWORD token;
	//VersionToken versionToken;
	//int32_t size = sizeof(ShaderHeader);

	//versionToken = (*(ShaderHeader*)shader);
	//shader += 1;
	//token = (*shader);

	//BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Convert shader version: " << versionToken.MajorVersion << "." << versionToken.MinorVersion;

	//switch (versionToken.ShaderType)
	//{
	//case mShaderTypePixel:
	//	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Convert shader type: Pixel";
	//	break;
	//case mShaderTypeVertex:
	//	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Convert shader type: Vertex";
	//	break;
	//default:
	//	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Convert shader type: Unknown";
	//	break;
	//}

	//while (token != mEndToken)
	//{
	//	uint16_t opcode = (*(uint16_t*)shader);

	//	switch (opcode)
	//	{

	//	default:
	//		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::Convert OP code " << opcode;
	//		break;
	//	}

	//	shader += 1;
	//	token = (*shader);
	//	size += 1;
	//}

	return output; //Return value optimization don't fail me now.
}