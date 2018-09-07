// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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
#include "d3d9.h"
#include "d3d11shader.h"
#include "ShaderConverter.h"
#include <boost/log/trivial.hpp>
#include <boost/foreach.hpp>
#include <fstream>

#include "CDevice9.h"
#include "Utilities.h"

/*
http://timjones.io/blog/archive/2015/09/02/parsing-direct3d-shader-bytecode
https://msdn.microsoft.com/en-us/library/bb219840(VS.85).aspx#Shader_Binary_Format
http://stackoverflow.com/questions/2545704/format-of-compiled-directx9-shader-files
https://msdn.microsoft.com/en-us/library/windows/hardware/ff552891(v=vs.85).aspx
https://github.com/ValveSoftware/ToGL
https://www.khronos.org/registry/spir-v/specs/1.2/SPIRV.html
*/

const uint32_t mEndToken = 0x0000FFFF;
const uint16_t mShaderTypePixel = 0xFFFF;
const uint16_t mShaderTypeVertex = 0xFFFE;

#define PACK(c0, c1, c2, c3) \
    (((uint32_t)(uint8_t)(c0) << 24) | \
    ((uint32_t)(uint8_t)(c1) << 16) | \
    ((uint32_t)(uint8_t)(c2) << 8) | \
    ((uint32_t)(uint8_t)(c3)))

boost::log::basic_record_ostream<char>& operator<< (boost::log::basic_record_ostream<char>& os, GLSLstd450 code)
{
	switch (code)
	{
	case GLSLstd450Bad: return os << "GLSLstd450Bad";
	case GLSLstd450Round: return os << "GLSLstd450Round";
	case GLSLstd450RoundEven: return os << "GLSLstd450RoundEven";
	case GLSLstd450Trunc: return os << "GLSLstd450Trunc";
	case GLSLstd450FAbs: return os << "GLSLstd450FAbs";
	case GLSLstd450SAbs: return os << "GLSLstd450SAbs";
	case GLSLstd450FSign: return os << "GLSLstd450FSign";
	case GLSLstd450SSign: return os << "GLSLstd450SSign";
	case GLSLstd450Floor: return os << "GLSLstd450Floor";
	case GLSLstd450Ceil: return os << "GLSLstd450Ceil";
	case GLSLstd450Fract: return os << "GLSLstd450Fract";
	case GLSLstd450Radians: return os << "GLSLstd450Radians";
	case GLSLstd450Degrees: return os << "GLSLstd450Degrees";
	case GLSLstd450Sin: return os << "GLSLstd450Sin";
	case GLSLstd450Cos: return os << "GLSLstd450Cos";
	case GLSLstd450Tan: return os << "GLSLstd450Tan";
	case GLSLstd450Asin: return os << "GLSLstd450Asin";
	case GLSLstd450Acos: return os << "GLSLstd450Acos";
	case GLSLstd450Atan: return os << "GLSLstd450Atan";
	case GLSLstd450Sinh: return os << "GLSLstd450Sinh";
	case GLSLstd450Cosh: return os << "GLSLstd450Cosh";
	case GLSLstd450Tanh: return os << "GLSLstd450Tanh";
	case GLSLstd450Asinh: return os << "GLSLstd450Asinh";
	case GLSLstd450Acosh: return os << "GLSLstd450Acosh";
	case GLSLstd450Atanh: return os << "GLSLstd450Atanh";
	case GLSLstd450Atan2: return os << "GLSLstd450Atan2";
	case GLSLstd450Pow: return os << "GLSLstd450Pow";
	case GLSLstd450Exp: return os << "GLSLstd450Exp";
	case GLSLstd450Log: return os << "GLSLstd450Log";
	case GLSLstd450Exp2: return os << "GLSLstd450Exp2";
	case GLSLstd450Log2: return os << "GLSLstd450Log2";
	case GLSLstd450Sqrt: return os << "GLSLstd450Sqrt";
	case GLSLstd450InverseSqrt: return os << "GLSLstd450InverseSqrt";
	case GLSLstd450Determinant: return os << "GLSLstd450Determinant";
	case GLSLstd450MatrixInverse: return os << "GLSLstd450MatrixInverse";
	case GLSLstd450Modf: return os << "GLSLstd450Modf";
	case GLSLstd450ModfStruct: return os << "GLSLstd450ModfStruct";
	case GLSLstd450FMin: return os << "GLSLstd450FMin";
	case GLSLstd450UMin: return os << "GLSLstd450UMin";
	case GLSLstd450SMin: return os << "GLSLstd450SMin";
	case GLSLstd450FMax: return os << "GLSLstd450FMax";
	case GLSLstd450UMax: return os << "GLSLstd450UMax";
	case GLSLstd450SMax: return os << "GLSLstd450SMax";
	case GLSLstd450FClamp: return os << "GLSLstd450FClamp";
	case GLSLstd450UClamp: return os << "GLSLstd450UClamp";
	case GLSLstd450SClamp: return os << "GLSLstd450SClamp";
	case GLSLstd450FMix: return os << "GLSLstd450FMix";
	case GLSLstd450IMix: return os << "GLSLstd450IMix";
	case GLSLstd450Step: return os << "GLSLstd450Step";
	case GLSLstd450SmoothStep: return os << "GLSLstd450SmoothStep";
	case GLSLstd450Fma: return os << "GLSLstd450Fma";
	case GLSLstd450Frexp: return os << "GLSLstd450Frexp";
	case GLSLstd450FrexpStruct: return os << "GLSLstd450FrexpStruct";
	case GLSLstd450Ldexp: return os << "GLSLstd450Ldexp";
	case GLSLstd450PackSnorm4x8: return os << "GLSLstd450PackSnorm4x8";
	case GLSLstd450PackUnorm4x8: return os << "GLSLstd450PackUnorm4x8";
	case GLSLstd450PackSnorm2x16: return os << "GLSLstd450PackSnorm2x16";
	case GLSLstd450PackUnorm2x16: return os << "GLSLstd450PackUnorm2x16";
	case GLSLstd450PackHalf2x16: return os << "GLSLstd450PackHalf2x16";
	case GLSLstd450PackDouble2x32: return os << "GLSLstd450PackDouble2x32";
	case GLSLstd450UnpackSnorm2x16: return os << "GLSLstd450UnpackSnorm2x16";
	case GLSLstd450UnpackUnorm2x16: return os << "GLSLstd450UnpackUnorm2x16";
	case GLSLstd450UnpackHalf2x16: return os << "GLSLstd450UnpackHalf2x16";
	case GLSLstd450UnpackSnorm4x8: return os << "GLSLstd450UnpackSnorm4x8";
	case GLSLstd450UnpackUnorm4x8: return os << "GLSLstd450UnpackUnorm4x8";
	case GLSLstd450UnpackDouble2x32: return os << "GLSLstd450UnpackDouble2x32";
	case GLSLstd450Length: return os << "GLSLstd450Length";
	case GLSLstd450Distance: return os << "GLSLstd450Distance";
	case GLSLstd450Cross: return os << "GLSLstd450Cross";
	case GLSLstd450Normalize: return os << "GLSLstd450Normalize";
	case GLSLstd450FaceForward: return os << "GLSLstd450FaceForward";
	case GLSLstd450Reflect: return os << "GLSLstd450Reflect";
	case GLSLstd450Refract: return os << "GLSLstd450Refract";
	case GLSLstd450FindILsb: return os << "GLSLstd450FindILsb";
	case GLSLstd450FindSMsb: return os << "GLSLstd450FindSMsb";
	case GLSLstd450FindUMsb: return os << "GLSLstd450FindUMsb";
	case GLSLstd450InterpolateAtCentroid: return os << "GLSLstd450InterpolateAtCentroid";
	case GLSLstd450InterpolateAtSample: return os << "GLSLstd450InterpolateAtSample";
	case GLSLstd450InterpolateAtOffset: return os << "GLSLstd450InterpolateAtOffset";
	case GLSLstd450NMin: return os << "GLSLstd450NMin";
	case GLSLstd450NMax: return os << "GLSLstd450NMax";
	case GLSLstd450NClamp: return os << "GLSLstd450NClamp";
	};
	return os << static_cast<std::uint32_t>(code);
}

boost::log::basic_record_ostream<char>& operator<< (boost::log::basic_record_ostream<char>& os, spv::Op code)
{
	switch (code)
	{
	case spv::OpNop: return os << "OpNop";
	case spv::OpUndef: return os << "OpUndef";
	case spv::OpSourceContinued: return os << "OpSourceContinued";
	case spv::OpSource: return os << "OpSource";
	case spv::OpSourceExtension: return os << "OpSourceExtension";
	case spv::OpName: return os << "OpName";
	case spv::OpMemberName: return os << "OpMemberName";
	case spv::OpString: return os << "OpString";
	case spv::OpLine: return os << "OpLine";
	case spv::OpExtension: return os << "OpExtension";
	case spv::OpExtInstImport: return os << "OpExtInstImport";
	case spv::OpExtInst: return os << "OpExtInst";
	case spv::OpMemoryModel: return os << "OpMemoryModel";
	case spv::OpEntryPoint: return os << "OpEntryPoint";
	case spv::OpExecutionMode: return os << "OpExecutionMode";
	case spv::OpCapability: return os << "OpCapability";
	case spv::OpTypeVoid: return os << "OpTypeVoid";
	case spv::OpTypeBool: return os << "OpTypeBool";
	case spv::OpTypeInt: return os << "OpTypeInt";
	case spv::OpTypeFloat: return os << "OpTypeFloat";
	case spv::OpTypeVector: return os << "OpTypeVector";
	case spv::OpTypeMatrix: return os << "OpTypeMatrix";
	case spv::OpTypeImage: return os << "OpTypeImage";
	case spv::OpTypeSampler: return os << "OpTypeSampler";
	case spv::OpTypeSampledImage: return os << "OpTypeSampledImage";
	case spv::OpTypeArray: return os << "OpTypeArray";
	case spv::OpTypeRuntimeArray: return os << "OpTypeRuntimeArray";
	case spv::OpTypeStruct: return os << "OpTypeStruct";
	case spv::OpTypeOpaque: return os << "OpTypeOpaque";
	case spv::OpTypePointer: return os << "OpTypePointer";
	case spv::OpTypeFunction: return os << "OpTypeFunction";
	case spv::OpTypeEvent: return os << "OpTypeEvent";
	case spv::OpTypeDeviceEvent: return os << "OpTypeDeviceEvent";
	case spv::OpTypeReserveId: return os << "OpTypeReserveId";
	case spv::OpTypeQueue: return os << "OpTypeQueue";
	case spv::OpTypePipe: return os << "OpTypePipe";
	case spv::OpTypeForwardPointer: return os << "OpTypeForwardPointer";
	case spv::OpConstantTrue: return os << "OpConstantTrue";
	case spv::OpConstantFalse: return os << "OpConstantFalse";
	case spv::OpConstant: return os << "OpConstant";
	case spv::OpConstantComposite: return os << "OpConstantComposite";
	case spv::OpConstantSampler: return os << "OpConstantSampler";
	case spv::OpConstantNull: return os << "OpConstantNull";
	case spv::OpSpecConstantTrue: return os << "OpSpecConstantTrue";
	case spv::OpSpecConstantFalse: return os << "OpSpecConstantFalse";
	case spv::OpSpecConstant: return os << "OpSpecConstant";
	case spv::OpSpecConstantComposite: return os << "OpSpecConstantComposite";
	case spv::OpSpecConstantOp: return os << "OpSpecConstantOp";
	case spv::OpFunction: return os << "OpFunction";
	case spv::OpFunctionParameter: return os << "OpFunctionParameter";
	case spv::OpFunctionEnd: return os << "OpFunctionEnd";
	case spv::OpFunctionCall: return os << "OpFunctionCall";
	case spv::OpVariable: return os << "OpVariable";
	case spv::OpImageTexelPointer: return os << "OpImageTexelPointer";
	case spv::OpLoad: return os << "OpLoad";
	case spv::OpStore: return os << "OpStore";
	case spv::OpCopyMemory: return os << "OpCopyMemory";
	case spv::OpCopyMemorySized: return os << "OpCopyMemorySized";
	case spv::OpAccessChain: return os << "OpAccessChain";
	case spv::OpInBoundsAccessChain: return os << "OpInBoundsAccessChain";
	case spv::OpPtrAccessChain: return os << "OpPtrAccessChain";
	case spv::OpArrayLength: return os << "OpArrayLength";
	case spv::OpGenericPtrMemSemantics: return os << "OpGenericPtrMemSemantics";
	case spv::OpInBoundsPtrAccessChain: return os << "OpInBoundsPtrAccessChain";
	case spv::OpDecorate: return os << "OpDecorate";
	case spv::OpMemberDecorate: return os << "OpMemberDecorate";
	case spv::OpDecorationGroup: return os << "OpDecorationGroup";
	case spv::OpGroupDecorate: return os << "OpGroupDecorate";
	case spv::OpGroupMemberDecorate: return os << "OpGroupMemberDecorate";
	case spv::OpVectorExtractDynamic: return os << "OpVectorExtractDynamic";
	case spv::OpVectorInsertDynamic: return os << "OpVectorInsertDynamic";
	case spv::OpVectorShuffle: return os << "OpVectorShuffle";
	case spv::OpCompositeConstruct: return os << "OpCompositeConstruct";
	case spv::OpCompositeExtract: return os << "OpCompositeExtract";
	case spv::OpCompositeInsert: return os << "OpCompositeInsert";
	case spv::OpCopyObject: return os << "OpCopyObject";
	case spv::OpTranspose: return os << "OpTranspose";
	case spv::OpSampledImage: return os << "OpSampledImage";
	case spv::OpImageSampleImplicitLod: return os << "OpImageSampleImplicitLod";
	case spv::OpImageSampleExplicitLod: return os << "OpImageSampleExplicitLod";
	case spv::OpImageSampleDrefImplicitLod: return os << "OpImageSampleDrefImplicitLod";
	case spv::OpImageSampleDrefExplicitLod: return os << "OpImageSampleDrefExplicitLod";
	case spv::OpImageSampleProjImplicitLod: return os << "OpImageSampleProjImplicitLod";
	case spv::OpImageSampleProjExplicitLod: return os << "OpImageSampleProjExplicitLod";
	case spv::OpImageSampleProjDrefImplicitLod: return os << "OpImageSampleProjDrefImplicitLod";
	case spv::OpImageSampleProjDrefExplicitLod: return os << "OpImageSampleProjDrefExplicitLod";
	case spv::OpImageFetch: return os << "OpImageFetch";
	case spv::OpImageGather: return os << "OpImageGather";
	case spv::OpImageDrefGather: return os << "OpImageDrefGather";
	case spv::OpImageRead: return os << "OpImageRead";
	case spv::OpImageWrite: return os << "OpImageWrite";
	case spv::OpImage: return os << "OpImage";
	case spv::OpImageQueryFormat: return os << "OpImageQueryFormat";
	case spv::OpImageQueryOrder: return os << "OpImageQueryOrder";
	case spv::OpImageQuerySizeLod: return os << "OpImageQuerySizeLod";
	case spv::OpImageQuerySize: return os << "OpImageQuerySize";
	case spv::OpImageQueryLod: return os << "OpImageQueryLod";
	case spv::OpImageQueryLevels: return os << "OpImageQueryLevels";
	case spv::OpImageQuerySamples: return os << "OpImageQuerySamples";
	case spv::OpConvertFToU: return os << "OpConvertFToU";
	case spv::OpConvertFToS: return os << "OpConvertFToS";
	case spv::OpConvertSToF: return os << "OpConvertSToF";
	case spv::OpConvertUToF: return os << "OpConvertUToF";
	case spv::OpUConvert: return os << "OpUConvert";
	case spv::OpSConvert: return os << "OpSConvert";
	case spv::OpFConvert: return os << "OpFConvert";
	case spv::OpQuantizeToF16: return os << "OpQuantizeToF16";
	case spv::OpConvertPtrToU: return os << "OpConvertPtrToU";
	case spv::OpSatConvertSToU: return os << "OpSatConvertSToU";
	case spv::OpSatConvertUToS: return os << "OpSatConvertUToS";
	case spv::OpConvertUToPtr: return os << "OpConvertUToPtr";
	case spv::OpPtrCastToGeneric: return os << "OpPtrCastToGeneric";
	case spv::OpGenericCastToPtr: return os << "OpGenericCastToPtr";
	case spv::OpGenericCastToPtrExplicit: return os << "OpGenericCastToPtrExplicit";
	case spv::OpBitcast: return os << "OpBitcast";
	case spv::OpSNegate: return os << "OpSNegate";
	case spv::OpFNegate: return os << "OpFNegate";
	case spv::OpIAdd: return os << "OpIAdd";
	case spv::OpFAdd: return os << "OpFAdd";
	case spv::OpISub: return os << "OpISub";
	case spv::OpFSub: return os << "OpFSub";
	case spv::OpIMul: return os << "OpIMul";
	case spv::OpFMul: return os << "OpFMul";
	case spv::OpUDiv: return os << "OpUDiv";
	case spv::OpSDiv: return os << "OpSDiv";
	case spv::OpFDiv: return os << "OpFDiv";
	case spv::OpUMod: return os << "OpUMod";
	case spv::OpSRem: return os << "OpSRem";
	case spv::OpSMod: return os << "OpSMod";
	case spv::OpFRem: return os << "OpFRem";
	case spv::OpFMod: return os << "OpFMod";
	case spv::OpVectorTimesScalar: return os << "OpVectorTimesScalar";
	case spv::OpMatrixTimesScalar: return os << "OpMatrixTimesScalar";
	case spv::OpVectorTimesMatrix: return os << "OpVectorTimesMatrix";
	case spv::OpMatrixTimesVector: return os << "OpMatrixTimesVector";
	case spv::OpMatrixTimesMatrix: return os << "OpMatrixTimesMatrix";
	case spv::OpOuterProduct: return os << "OpOuterProduct";
	case spv::OpDot: return os << "OpDot";
	case spv::OpIAddCarry: return os << "OpIAddCarry";
	case spv::OpISubBorrow: return os << "OpISubBorrow";
	case spv::OpUMulExtended: return os << "OpUMulExtended";
	case spv::OpSMulExtended: return os << "OpSMulExtended";
	case spv::OpAny: return os << "OpAny";
	case spv::OpAll: return os << "OpAll";
	case spv::OpIsNan: return os << "OpIsNan";
	case spv::OpIsInf: return os << "OpIsInf";
	case spv::OpIsFinite: return os << "OpIsFinite";
	case spv::OpIsNormal: return os << "OpIsNormal";
	case spv::OpSignBitSet: return os << "OpSignBitSet";
	case spv::OpLessOrGreater: return os << "OpLessOrGreater";
	case spv::OpOrdered: return os << "OpOrdered";
	case spv::OpUnordered: return os << "OpUnordered";
	case spv::OpLogicalEqual: return os << "OpLogicalEqual";
	case spv::OpLogicalNotEqual: return os << "OpLogicalNotEqual";
	case spv::OpLogicalOr: return os << "OpLogicalOr";
	case spv::OpLogicalAnd: return os << "OpLogicalAnd";
	case spv::OpLogicalNot: return os << "OpLogicalNot";
	case spv::OpSelect: return os << "OpSelect";
	case spv::OpIEqual: return os << "OpIEqual";
	case spv::OpINotEqual: return os << "OpINotEqual";
	case spv::OpUGreaterThan: return os << "OpUGreaterThan";
	case spv::OpSGreaterThan: return os << "OpSGreaterThan";
	case spv::OpUGreaterThanEqual: return os << "OpUGreaterThanEqual";
	case spv::OpSGreaterThanEqual: return os << "OpSGreaterThanEqual";
	case spv::OpULessThan: return os << "OpULessThan";
	case spv::OpSLessThan: return os << "OpSLessThan";
	case spv::OpULessThanEqual: return os << "OpULessThanEqual";
	case spv::OpSLessThanEqual: return os << "OpSLessThanEqual";
	case spv::OpFOrdEqual: return os << "OpFOrdEqual";
	case spv::OpFUnordEqual: return os << "OpFUnordEqual";
	case spv::OpFOrdNotEqual: return os << "OpFOrdNotEqual";
	case spv::OpFUnordNotEqual: return os << "OpFUnordNotEqual";
	case spv::OpFOrdLessThan: return os << "OpFOrdLessThan";
	case spv::OpFUnordLessThan: return os << "OpFUnordLessThan";
	case spv::OpFOrdGreaterThan: return os << "OpFOrdGreaterThan";
	case spv::OpFUnordGreaterThan: return os << "OpFUnordGreaterThan";
	case spv::OpFOrdLessThanEqual: return os << "OpFOrdLessThanEqual";
	case spv::OpFUnordLessThanEqual: return os << "OpFUnordLessThanEqual";
	case spv::OpFOrdGreaterThanEqual: return os << "OpFOrdGreaterThanEqual";
	case spv::OpFUnordGreaterThanEqual: return os << "OpFUnordGreaterThanEqual";
	case spv::OpShiftRightLogical: return os << "OpShiftRightLogical";
	case spv::OpShiftRightArithmetic: return os << "OpShiftRightArithmetic";
	case spv::OpShiftLeftLogical: return os << "OpShiftLeftLogical";
	case spv::OpBitwiseOr: return os << "OpBitwiseOr";
	case spv::OpBitwiseXor: return os << "OpBitwiseXor";
	case spv::OpBitwiseAnd: return os << "OpBitwiseAnd";
	case spv::OpNot: return os << "OpNot";
	case spv::OpBitFieldInsert: return os << "OpBitFieldInsert";
	case spv::OpBitFieldSExtract: return os << "OpBitFieldSExtract";
	case spv::OpBitFieldUExtract: return os << "OpBitFieldUExtract";
	case spv::OpBitReverse: return os << "OpBitReverse";
	case spv::OpBitCount: return os << "OpBitCount";
	case spv::OpDPdx: return os << "OpDPdx";
	case spv::OpDPdy: return os << "OpDPdy";
	case spv::OpFwidth: return os << "OpFwidth";
	case spv::OpDPdxFine: return os << "OpDPdxFine";
	case spv::OpDPdyFine: return os << "OpDPdyFine";
	case spv::OpFwidthFine: return os << "OpFwidthFine";
	case spv::OpDPdxCoarse: return os << "OpDPdxCoarse";
	case spv::OpDPdyCoarse: return os << "OpDPdyCoarse";
	case spv::OpFwidthCoarse: return os << "OpFwidthCoarse";
	case spv::OpEmitVertex: return os << "OpEmitVertex";
	case spv::OpEndPrimitive: return os << "OpEndPrimitive";
	case spv::OpEmitStreamVertex: return os << "OpEmitStreamVertex";
	case spv::OpEndStreamPrimitive: return os << "OpEndStreamPrimitive";
	case spv::OpControlBarrier: return os << "OpControlBarrier";
	case spv::OpMemoryBarrier: return os << "OpMemoryBarrier";
	case spv::OpAtomicLoad: return os << "OpAtomicLoad";
	case spv::OpAtomicStore: return os << "OpAtomicStore";
	case spv::OpAtomicExchange: return os << "OpAtomicExchange";
	case spv::OpAtomicCompareExchange: return os << "OpAtomicCompareExchange";
	case spv::OpAtomicCompareExchangeWeak: return os << "OpAtomicCompareExchangeWeak";
	case spv::OpAtomicIIncrement: return os << "OpAtomicIIncrement";
	case spv::OpAtomicIDecrement: return os << "OpAtomicIDecrement";
	case spv::OpAtomicIAdd: return os << "OpAtomicIAdd";
	case spv::OpAtomicISub: return os << "OpAtomicISub";
	case spv::OpAtomicSMin: return os << "OpAtomicSMin";
	case spv::OpAtomicUMin: return os << "OpAtomicUMin";
	case spv::OpAtomicSMax: return os << "OpAtomicSMax";
	case spv::OpAtomicUMax: return os << "OpAtomicUMax";
	case spv::OpAtomicAnd: return os << "OpAtomicAnd";
	case spv::OpAtomicOr: return os << "OpAtomicOr";
	case spv::OpAtomicXor: return os << "OpAtomicXor";
	case spv::OpPhi: return os << "OpPhi";
	case spv::OpLoopMerge: return os << "OpLoopMerge";
	case spv::OpSelectionMerge: return os << "OpSelectionMerge";
	case spv::OpLabel: return os << "OpLabel";
	case spv::OpBranch: return os << "OpBranch";
	case spv::OpBranchConditional: return os << "OpBranchConditional";
	case spv::OpSwitch: return os << "OpSwitch";
	case spv::OpKill: return os << "OpKill";
	case spv::OpReturn: return os << "OpReturn";
	case spv::OpReturnValue: return os << "OpReturnValue";
	case spv::OpUnreachable: return os << "OpUnreachable";
	case spv::OpLifetimeStart: return os << "OpLifetimeStart";
	case spv::OpLifetimeStop: return os << "OpLifetimeStop";
	case spv::OpGroupAsyncCopy: return os << "OpGroupAsyncCopy";
	case spv::OpGroupWaitEvents: return os << "OpGroupWaitEvents";
	case spv::OpGroupAll: return os << "OpGroupAll";
	case spv::OpGroupAny: return os << "OpGroupAny";
	case spv::OpGroupBroadcast: return os << "OpGroupBroadcast";
	case spv::OpGroupIAdd: return os << "OpGroupIAdd";
	case spv::OpGroupFAdd: return os << "OpGroupFAdd";
	case spv::OpGroupFMin: return os << "OpGroupFMin";
	case spv::OpGroupUMin: return os << "OpGroupUMin";
	case spv::OpGroupSMin: return os << "OpGroupSMin";
	case spv::OpGroupFMax: return os << "OpGroupFMax";
	case spv::OpGroupUMax: return os << "OpGroupUMax";
	case spv::OpGroupSMax: return os << "OpGroupSMax";
	case spv::OpReadPipe: return os << "OpReadPipe";
	case spv::OpWritePipe: return os << "OpWritePipe";
	case spv::OpReservedReadPipe: return os << "OpReservedReadPipe";
	case spv::OpReservedWritePipe: return os << "OpReservedWritePipe";
	case spv::OpReserveReadPipePackets: return os << "OpReserveReadPipePackets";
	case spv::OpReserveWritePipePackets: return os << "OpReserveWritePipePackets";
	case spv::OpCommitReadPipe: return os << "OpCommitReadPipe";
	case spv::OpCommitWritePipe: return os << "OpCommitWritePipe";
	case spv::OpIsValidReserveId: return os << "OpIsValidReserveId";
	case spv::OpGetNumPipePackets: return os << "OpGetNumPipePackets";
	case spv::OpGetMaxPipePackets: return os << "OpGetMaxPipePackets";
	case spv::OpGroupReserveReadPipePackets: return os << "OpGroupReserveReadPipePackets";
	case spv::OpGroupReserveWritePipePackets: return os << "OpGroupReserveWritePipePackets";
	case spv::OpGroupCommitReadPipe: return os << "OpGroupCommitReadPipe";
	case spv::OpGroupCommitWritePipe: return os << "OpGroupCommitWritePipe";
	case spv::OpEnqueueMarker: return os << "OpEnqueueMarker";
	case spv::OpEnqueueKernel: return os << "OpEnqueueKernel";
	case spv::OpGetKernelNDrangeSubGroupCount: return os << "OpGetKernelNDrangeSubGroupCount";
	case spv::OpGetKernelNDrangeMaxSubGroupSize: return os << "OpGetKernelNDrangeMaxSubGroupSize";
	case spv::OpGetKernelWorkGroupSize: return os << "OpGetKernelWorkGroupSize";
	case spv::OpGetKernelPreferredWorkGroupSizeMultiple: return os << "OpGetKernelPreferredWorkGroupSizeMultiple";
	case spv::OpRetainEvent: return os << "OpRetainEvent";
	case spv::OpReleaseEvent: return os << "OpReleaseEvent";
	case spv::OpCreateUserEvent: return os << "OpCreateUserEvent";
	case spv::OpIsValidEvent: return os << "OpIsValidEvent";
	case spv::OpSetUserEventStatus: return os << "OpSetUserEventStatus";
	case spv::OpCaptureEventProfilingInfo: return os << "OpCaptureEventProfilingInfo";
	case spv::OpGetDefaultQueue: return os << "OpGetDefaultQueue";
	case spv::OpBuildNDRange: return os << "OpBuildNDRange";
	case spv::OpImageSparseSampleImplicitLod: return os << "OpImageSparseSampleImplicitLod";
	case spv::OpImageSparseSampleExplicitLod: return os << "OpImageSparseSampleExplicitLod";
	case spv::OpImageSparseSampleDrefImplicitLod: return os << "OpImageSparseSampleDrefImplicitLod";
	case spv::OpImageSparseSampleDrefExplicitLod: return os << "OpImageSparseSampleDrefExplicitLod";
	case spv::OpImageSparseSampleProjImplicitLod: return os << "OpImageSparseSampleProjImplicitLod";
	case spv::OpImageSparseSampleProjExplicitLod: return os << "OpImageSparseSampleProjExplicitLod";
	case spv::OpImageSparseSampleProjDrefImplicitLod: return os << "OpImageSparseSampleProjDrefImplicitLod";
	case spv::OpImageSparseSampleProjDrefExplicitLod: return os << "OpImageSparseSampleProjDrefExplicitLod";
	case spv::OpImageSparseFetch: return os << "OpImageSparseFetch";
	case spv::OpImageSparseGather: return os << "OpImageSparseGather";
	case spv::OpImageSparseDrefGather: return os << "OpImageSparseDrefGather";
	case spv::OpImageSparseTexelsResident: return os << "OpImageSparseTexelsResident";
	case spv::OpNoLine: return os << "OpNoLine";
	case spv::OpAtomicFlagTestAndSet: return os << "OpAtomicFlagTestAndSet";
	case spv::OpAtomicFlagClear: return os << "OpAtomicFlagClear";
	case spv::OpImageSparseRead: return os << "OpImageSparseRead";
	case spv::OpSizeOf: return os << "OpSizeOf";
	case spv::OpTypePipeStorage: return os << "OpTypePipeStorage";
	case spv::OpConstantPipeStorage: return os << "OpConstantPipeStorage";
	case spv::OpCreatePipeFromPipeStorage: return os << "OpCreatePipeFromPipeStorage";
	case spv::OpGetKernelLocalSizeForSubgroupCount: return os << "OpGetKernelLocalSizeForSubgroupCount";
	case spv::OpGetKernelMaxNumSubgroups: return os << "OpGetKernelMaxNumSubgroups";
	case spv::OpTypeNamedBarrier: return os << "OpTypeNamedBarrier";
	case spv::OpNamedBarrierInitialize: return os << "OpNamedBarrierInitialize";
	case spv::OpMemoryNamedBarrier: return os << "OpMemoryNamedBarrier";
	case spv::OpModuleProcessed: return os << "OpModuleProcessed";
	case spv::OpExecutionModeId: return os << "OpExecutionModeId";
	case spv::OpDecorateId: return os << "OpDecorateId";
	case spv::OpSubgroupBallotKHR: return os << "OpSubgroupBallotKHR";
	case spv::OpSubgroupFirstInvocationKHR: return os << "OpSubgroupFirstInvocationKHR";
	case spv::OpSubgroupAllKHR: return os << "OpSubgroupAllKHR";
	case spv::OpSubgroupAnyKHR: return os << "OpSubgroupAnyKHR";
	case spv::OpSubgroupAllEqualKHR: return os << "OpSubgroupAllEqualKHR";
	case spv::OpSubgroupReadInvocationKHR: return os << "OpSubgroupReadInvocationKHR";
	case spv::OpGroupIAddNonUniformAMD: return os << "OpGroupIAddNonUniformAMD";
	case spv::OpGroupFAddNonUniformAMD: return os << "OpGroupFAddNonUniformAMD";
	case spv::OpGroupFMinNonUniformAMD: return os << "OpGroupFMinNonUniformAMD";
	case spv::OpGroupUMinNonUniformAMD: return os << "OpGroupUMinNonUniformAMD";
	case spv::OpGroupSMinNonUniformAMD: return os << "OpGroupSMinNonUniformAMD";
	case spv::OpGroupFMaxNonUniformAMD: return os << "OpGroupFMaxNonUniformAMD";
	case spv::OpGroupUMaxNonUniformAMD: return os << "OpGroupUMaxNonUniformAMD";
	case spv::OpGroupSMaxNonUniformAMD: return os << "OpGroupSMaxNonUniformAMD";
	case spv::OpFragmentMaskFetchAMD: return os << "OpFragmentMaskFetchAMD";
	case spv::OpFragmentFetchAMD: return os << "OpFragmentFetchAMD";
	case spv::OpSubgroupShuffleINTEL: return os << "OpSubgroupShuffleINTEL";
	case spv::OpSubgroupShuffleDownINTEL: return os << "OpSubgroupShuffleDownINTEL";
	case spv::OpSubgroupShuffleUpINTEL: return os << "OpSubgroupShuffleUpINTEL";
	case spv::OpSubgroupShuffleXorINTEL: return os << "OpSubgroupShuffleXorINTEL";
	case spv::OpSubgroupBlockReadINTEL: return os << "OpSubgroupBlockReadINTEL";
	case spv::OpSubgroupBlockWriteINTEL: return os << "OpSubgroupBlockWriteINTEL";
	case spv::OpSubgroupImageBlockReadINTEL: return os << "OpSubgroupImageBlockReadINTEL";
	case spv::OpSubgroupImageBlockWriteINTEL: return os << "OpSubgroupImageBlockWriteINTEL";
	case spv::OpDecorateStringGOOGLE: return os << "OpDecorateStringGOOGLE";
	case spv::OpMemberDecorateStringGOOGLE: return os << "OpMemberDecorateStringGOOGLE";
	case spv::OpMax: return os << "OpMax";
	};
	return os << static_cast<std::uint32_t>(code);
}

boost::log::basic_record_ostream<char>& operator<< (boost::log::basic_record_ostream<char>& os, TypeDescription& typeDescription)
{
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		switch (typeDescription.SecondaryType)
		{
		case spv::OpTypeVector:
			return os << "*vec" << typeDescription.ComponentCount;
		case spv::OpTypeMatrix:
			return os << "*mat" << typeDescription.ComponentCount;
		case spv::OpTypeFloat:
			return os << "*float";
		case spv::OpTypeInt:
			return os << "*int";
		case spv::OpTypeBool:
			return os << "*bool";
		case spv::OpTypeImage:
			return os << "*image";
		default:
			return os << "*void";
		}
	}
	else
	{
		switch (typeDescription.PrimaryType)
		{
		case spv::OpTypeVector:
			return os << "vec" << typeDescription.ComponentCount;
		case spv::OpTypeMatrix:
			return os << "mat" << typeDescription.ComponentCount;
		case spv::OpTypeFloat:
			return os << "float";
		case spv::OpTypeInt:
			return os << "int";
		case spv::OpTypeBool:
			return os << "bool";
		case spv::OpTypeImage:
			return os << "image";
		default:
			return os << "void";
		}
	}
}

/*
Generator's magic number. It is associated with the tool that generated
the module. Its value does not affect any semantics, and is allowed to be 0.
Using a non-0 value is encouraged, and can be registered with
Khronos at https://www.khronos.org/registry/spir-v/api/spir-v.xml.
*/
#define SPIR_V_GENERATORS_NUMBER 0x00000000

ShaderConverter::ShaderConverter(vk::Device& device, ShaderConstantSlots& shaderConstantSlots)
	: mDevice(device), 
	mShaderConstantSlots(shaderConstantSlots),
	mFalseLabelCount(0)
{

}

ShaderConverter::~ShaderConverter()
{
	if (mConvertedShader.ShaderModule != vk::ShaderModule())
	{
		mDevice.destroyShaderModule(mConvertedShader.ShaderModule, nullptr);
		mConvertedShader.ShaderModule = nullptr;
	}
}

Token ShaderConverter::GetNextToken()
{
	Token token;

	mPreviousToken++;
	token.i = *(mNextToken++);

	return token;
}

void ShaderConverter::SkipTokens(uint32_t numberToSkip)
{
	mNextToken += numberToSkip;
}

uint32_t ShaderConverter::GetNextId()
{
	return mNextId++;
}

void ShaderConverter::SkipIds(uint32_t numberToSkip)
{
	mNextId += numberToSkip;
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType, uint32_t id)
{
	TypeDescription description;

	description.PrimaryType = registerType;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description, id);
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2)
{
	TypeDescription description;

	description.PrimaryType = registerType1;
	description.SecondaryType = registerType2;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description);
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2, uint32_t componentCount)
{
	TypeDescription description;

	description.PrimaryType = registerType1;
	description.SecondaryType = registerType2;
	description.ComponentCount = componentCount;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description);
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2, spv::Op registerType3, uint32_t componentCount)
{
	TypeDescription description;

	description.PrimaryType = registerType1;
	description.SecondaryType = registerType2;
	description.TernaryType = registerType3;
	description.ComponentCount = componentCount;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description);
}

uint32_t ShaderConverter::GetSpirVTypeId(TypeDescription& registerType, uint32_t id)
{
	uint32_t columnTypeId = 0;
	uint32_t pointerTypeId = 0;
	uint32_t returnTypeId = 0;
	uint32_t sampledTypeId = 0;
	uint32_t id2 = 0;

	for (const auto& type : mTypeIdPairs)
	{
		if (type.first == registerType)
		{
			return type.second;
		}
	}

	if (id == UINT_MAX)
	{
		id = GetNextId();
	}

	mTypeIdPairs[registerType] = id;
	mIdTypePairs[id] = registerType;

	switch (registerType.PrimaryType)
	{
	case spv::OpTypeBool:
		mTypeInstructions.push_back(Pack(2, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		break;
	case spv::OpTypeInt:
		mTypeInstructions.push_back(Pack(4, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(32); //Number of bits.
		mTypeInstructions.push_back(0); //Signedness (0 = unsigned,1 = signed)
		break;
	case spv::OpTypeFloat:
		mTypeInstructions.push_back(Pack(3, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(32); //Number of bits.
		break;
	case spv::OpTypeMatrix:
		columnTypeId = GetSpirVTypeId(spv::OpTypeVector, spv::OpTypeFloat, registerType.ComponentCount);

		mTypeInstructions.push_back(Pack(4, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(columnTypeId); //Component/Column Type
		mTypeInstructions.push_back(registerType.ComponentCount);

		mDecorateInstructions.push_back(Pack(3, spv::OpDecorate)); //size,Type
		mDecorateInstructions.push_back(id); //target (Id)
		mDecorateInstructions.push_back(spv::DecorationColMajor); //Decoration Type (Id)	
		//mDecorateInstructions.push_back(spv::DecorationRowMajor); //Decoration Type (Id)	
		break;
	case spv::OpTypeVector:
		columnTypeId = GetSpirVTypeId(registerType.SecondaryType);

		mTypeInstructions.push_back(Pack(4, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(columnTypeId); //Component/Column Type
		mTypeInstructions.push_back(registerType.ComponentCount);
		break;
	case spv::OpTypePointer:
		pointerTypeId = GetSpirVTypeId(registerType.SecondaryType, registerType.TernaryType, registerType.ComponentCount);

		mTypeInstructions.push_back(Pack(4, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		if (registerType.SecondaryType == spv::OpTypeSampledImage || registerType.SecondaryType == spv::OpTypeImage)
		{
			mTypeInstructions.push_back(spv::StorageClassUniformConstant); //Storage Class
		}
		else
		{
			mTypeInstructions.push_back(registerType.StorageClass); //Storage Class
		}
		mTypeInstructions.push_back(pointerTypeId); // Type
		break;
	case spv::OpTypeSampler:
		mTypeInstructions.push_back(Pack(2, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		break;
	case spv::OpTypeSampledImage:
	case spv::OpTypeImage:
		id2 = id;
		id = GetNextId();

		sampledTypeId = GetSpirVTypeId(spv::OpTypeFloat);
		mTypeInstructions.push_back(Pack(9, spv::OpTypeImage)); //size,Type
		mTypeInstructions.push_back(id2); //Result (Id)
		mTypeInstructions.push_back(sampledTypeId); //Sampled Type (Id)
		mTypeInstructions.push_back(spv::Dim2D); //dimensionality
		mTypeInstructions.push_back(0); //Depth
		mTypeInstructions.push_back(0); //Arrayed
		mTypeInstructions.push_back(0); //MS
		mTypeInstructions.push_back(1); //Sampled
		mTypeInstructions.push_back(spv::ImageFormatUnknown); //Sampled

		mTypeInstructions.push_back(Pack(3, spv::OpTypeSampledImage)); //size,Type
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(id2); //Type (Id)
		break;
	case spv::OpTypeFunction:
	{
		returnTypeId = GetSpirVTypeId(registerType.SecondaryType);

		mTypeInstructions.push_back(Pack(3 + registerType.Arguments.size(), registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(returnTypeId); //Return Type (Id)

		//Right now there is no comparison on arguments so we are assuming that functions with the same return type are the same. This will need to be expanded later when we're using functions other than the default entry point.
		for (size_t i = 0; i < registerType.Arguments.size(); i++)
		{
			mTypeInstructions.push_back(registerType.Arguments[i]); //Argument Id
		}
	}
	break;
	case spv::OpTypeVoid:
		mTypeInstructions.push_back(Pack(2, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id		
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "GetSpirVTypeId - Unsupported data type " << registerType.PrimaryType;
		break;
	}

	mTypeIdPairs[registerType] = id;
	mIdTypePairs[id] = registerType;

	return id;
}

/*
SPIR-V is SSA so this method will generate a new id with the type of the old one when a new "register" is needed.
To handle this result registers will get a new Id each type. The result Id can be used as an input to other operations so this will work fine.
To make sure each call gets the latest id number the lookups must be updated.
*/
uint32_t ShaderConverter::GetNextVersionId(const Token& token)
{
	uint32_t id = GetNextId();

	SetIdByRegister(token, id);

	return id;
}

uint32_t ShaderConverter::GetIdByRegister(const Token& token, _D3DSHADER_PARAM_REGISTER_TYPE type, _D3DDECLUSAGE usage)
{
	D3DSHADER_PARAM_REGISTER_TYPE registerType;
	uint32_t registerNumber = 0;
	TypeDescription description;
	uint32_t id = 0;
	uint32_t typeId = 0;
	std::string registerName;

	if (type == D3DSPR_FORCE_DWORD)
	{
		registerType = GetRegisterType(token.i);
	}
	else
	{
		registerType = type;
	}

	switch (registerType)
	{
	case D3DSPR_CONST2:
		registerNumber = GetRegisterNumber(token) + 2048;
		break;
	case D3DSPR_CONST3:
		registerNumber = GetRegisterNumber(token) + 4096;
		break;
	case D3DSPR_CONST4:
		registerNumber = GetRegisterNumber(token) + 6144;
		break;
	case D3DSPR_TEXTURE:
		registerNumber = GetRegisterNumber(token);

		if (mTextures[registerNumber])
		{
			//TODO: Revisit
			registerNumber = mTextures[registerNumber];
		}

		break;
	default:
		registerNumber = GetRegisterNumber(token);
		break;
	}

	//If a register has already be declared just return the id.
	boost::container::flat_map<D3DSHADER_PARAM_REGISTER_TYPE, boost::container::flat_map<uint32_t, uint32_t> >::iterator it1 = mIdsByRegister.find(registerType);
	if (it1 != mIdsByRegister.end())
	{
		boost::container::flat_map<uint32_t, uint32_t>::iterator it2 = it1->second.find(registerNumber);
		if (it2 != it1->second.end())
		{
			return it2->second;
		}
	}

	/*
	Registers can be defined simply by using them so anything past this point was used before it was declared.
	We'll be missing some usage information and some other bits so we'll try to piece together what it should be from context. (also guess some stuff)

	The rules are different between ps and vs and even between shader models 1, 2, and 3.
	*/

	switch (registerType)
	{
	case D3DSPR_TEXTURE: //Texture could be texcoord
	case D3DSPR_INPUT:
		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeVector;

		//TODO: find a way to tell if this is an integer or float.
		if (usage == D3DDECLUSAGE_COLOR)
		{
			description.TernaryType = spv::OpTypeInt; //Hint says this is color so it should be a single DWORD that Vulkan splits into a uvec4.
		}
		else
		{
			description.TernaryType = spv::OpTypeFloat;
		}
		description.ComponentCount = 4;
		description.StorageClass = spv::StorageClassInput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class

		mInputRegisters.push_back(id);

		if (this->mMajorVersion == 3)
		{
			registerName = "v" + std::to_string(registerNumber);
			PushName(id, registerName);

			GenerateDecoration(registerNumber, id, usage, true);
		}
		else
		{
			if (registerType == D3DSPR_INPUT)
			{
				registerName = "oD" + std::to_string(registerNumber);
				PushName(id, registerName);

				GenerateDecoration(registerNumber, id, D3DDECLUSAGE_COLOR, true);
			}
			else
			{
				registerName = "oT" + std::to_string(registerNumber);
				PushName(id, registerName);

				GenerateDecoration(registerNumber, id, D3DDECLUSAGE_TEXCOORD, true);
			}
		}

		if (this->mIsVertexShader)
		{
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].binding = 0;//element.Stream; TODO:REVISIT
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].location = registerNumber;  //mConvertedShader.mVertexInputAttributeDescriptionCount;
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].offset = 0;//element.Offset;
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32B32A32Sfloat;
			mConvertedShader.mVertexInputAttributeDescriptionCount++;
		}

		break;
	case D3DSPR_RASTOUT:
	case D3DSPR_ATTROUT:
	case D3DSPR_COLOROUT:
	case D3DSPR_DEPTHOUT:
	case D3DSPR_OUTPUT:
	case D3DSPR_TEMP:
		/*
		D3DSPR_TEMP is included with the outputs because for pixel shaders r0 is the color output.
		So rather than duplicate everything I put some logic here and there to decide if it's an output or a temp.
		*/

		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeVector;
		description.TernaryType = spv::OpTypeFloat; //TODO: find a way to tell if this is an integer or float.
		description.ComponentCount = 4;
		description.StorageClass = spv::StorageClassOutput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)

		if (this->mIsVertexShader && registerType != D3DSPR_TEMP)
		{
			mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
		}
		else if (!this->mIsVertexShader && registerType == D3DSPR_TEMP) //r0 is used for pixel shader color output because reasons.
		{
			mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
		}
		else
		{
			mTypeInstructions.push_back(spv::StorageClassPrivate); //Storage Class
		}

		/*
			D3DDECLUSAGE_FOG:
			registerName = "oFog" + std::to_string(registerNumber);
			break;

			D3DDECLUSAGE_PSIZE:
			registerName = "oPts" + std::to_string(registerNumber);
			break;
		*/

		switch (registerType)
		{
		case D3DSPR_RASTOUT:
			mPositionId = id;
			registerName = "oPos" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_POSITION;
			break;
		case D3DSPR_ATTROUT:
		case D3DSPR_COLOROUT:

			if (registerNumber)
			{
				mColor2Id = id;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor2XId = GetNextId();
				mIdTypePairs[mColor2XId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2XId, mColor2Id, m0Id);

				mColor2YId = GetNextId();
				mIdTypePairs[mColor2YId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2YId, mColor2Id, m1Id);

				mColor2ZId = GetNextId();
				mIdTypePairs[mColor2ZId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2ZId, mColor2Id, m2Id);

				mColor2WId = GetNextId();
				mIdTypePairs[mColor2WId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2WId, mColor2Id, m3Id);
			}
			else
			{
				mColor1Id = id;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor1XId = GetNextId();
				mIdTypePairs[mColor1XId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1XId, mColor1Id, m0Id);

				mColor1YId = GetNextId();
				mIdTypePairs[mColor1YId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1YId, mColor1Id, m1Id);

				mColor1ZId = GetNextId();
				mIdTypePairs[mColor1ZId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1ZId, mColor1Id, m2Id);

				mColor1WId = GetNextId();
				mIdTypePairs[mColor1WId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1WId, mColor1Id, m3Id);
			}

			registerName = "oD" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_COLOR;
			break;
		case D3DSPR_TEXCRDOUT:
			registerName = "oT" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_TEXCOORD;
			break;
		case D3DSPR_TEMP:
			registerName = "r" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_COLOR;
			break;
		default:
			registerName = "o" + std::to_string(registerNumber);
			break;
		}

		PushName(id, registerName);

		//mOutputRegisterUsages[(_D3DDECLUSAGE)GetUsage(token.i)] = id;
		if (this->mIsVertexShader && registerType != D3DSPR_TEMP)
		{
			mOutputRegisters.push_back(id);

			GenerateDecoration(registerNumber, id, usage, false);
		}
		else if (!this->mIsVertexShader && registerType == D3DSPR_TEMP) //r0 is used for pixel shader color output because reasons.
		{
			mOutputRegisters.push_back(id);

			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(id); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
			mDecorateInstructions.push_back(0); //Location offset
		}

		break;
	case D3DSPR_CONST:
	case D3DSPR_CONST2:
	case D3DSPR_CONST3:
	case D3DSPR_CONST4:
		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeVector;
		description.TernaryType = spv::OpTypeFloat; //TODO: find a way to tell if this is an integer or float.
		description.ComponentCount = 4;
		description.StorageClass = spv::StorageClassInput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassPushConstant); //Storage Class
		break;

	case D3DSPR_SAMPLER:
		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeImage;
		description.StorageClass = spv::StorageClassInput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassUniformConstant); //Storage Class

		mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
		mDecorateInstructions.push_back(id); //target (Id)
		mDecorateInstructions.push_back(spv::DecorationBinding); //Decoration Type (Id)
		mDecorateInstructions.push_back(registerNumber); //Location offset

		mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
		mDecorateInstructions.push_back(id); //target (Id)
		mDecorateInstructions.push_back(spv::DecorationDescriptorSet); //Decoration Type (Id)
		mDecorateInstructions.push_back(0); //Location offset

		registerName = "s" + std::to_string(registerNumber);
		PushName(id, registerName);

		if (!this->mIsVertexShader)
		{
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].binding = registerNumber; //mConvertedShader.mDescriptorSetLayoutBindingCount;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorCount = 1;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].stageFlags = vk::ShaderStageFlagBits::eFragment;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].pImmutableSamplers = NULL;

			mConvertedShader.mDescriptorSetLayoutBindingCount++;
		}

		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "GetIdByRegister - Id not found register " << registerNumber << " (" << registerType << ")";
		break;
	}


	if (!id)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetIdByRegister - Id was zero!";
	}

	return id;
}

void ShaderConverter::SetIdByRegister(const Token& token, uint32_t id)
{
	D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	uint32_t registerNumber = 0;
	TypeDescription description;
	uint32_t typeId = 0;

	switch (registerType)
	{
	case D3DSPR_CONST2:
		registerNumber = GetRegisterNumber(token) + 2048;
		break;
	case D3DSPR_CONST3:
		registerNumber = GetRegisterNumber(token) + 4096;
		break;
	case D3DSPR_CONST4:
		registerNumber = GetRegisterNumber(token) + 6144;
		break;
	default:
		registerNumber = GetRegisterNumber(token);
		break;
	}

	mIdsByRegister[registerType][registerNumber] = id;
	mRegistersById[registerType][id] = registerNumber;
}

TypeDescription ShaderConverter::GetTypeByRegister(const Token& token, _D3DDECLUSAGE usage)
{
	TypeDescription dataType;

	uint32_t id = GetIdByRegister(token, D3DSPR_FORCE_DWORD, usage);

	boost::container::flat_map<uint32_t, TypeDescription>::iterator it2 = mIdTypePairs.find(id);
	if (it2 != mIdTypePairs.end())
	{
		dataType = it2->second;
	}
	else
	{
		dataType.PrimaryType = spv::OpTypeVector;
		if (usage == D3DDECLUSAGE_COLOR)
		{
			dataType.SecondaryType = spv::OpTypeInt;
		}
		else
		{
			dataType.SecondaryType = spv::OpTypeFloat;
		}
		dataType.ComponentCount = 4;
	}

	return dataType;
}

/*
This function gets the id for the token and if it's a pointer loads it.
It this checks to see if there is a swizzle and if so shuffle or extract to get the right values out.
The swizzle values for sources are different from the write mask on a result which is why we need a seperate function for each. :-(
*/
uint32_t ShaderConverter::GetSwizzledId(const Token& token, uint32_t lookingFor)
{
	uint32_t originalId = 0;	
	uint32_t outputComponentCount = 4;

	if (lookingFor == GIVE_ME_SAMPLER)
	{
		originalId = GetIdByRegister(token, D3DSPR_SAMPLER);
	}
	else if (lookingFor == GIVE_ME_VECTOR_2)
	{
		originalId = GetIdByRegister(token, D3DSPR_TEXTURE, D3DDECLUSAGE_TEXCOORD);
		outputComponentCount = 2;
	}	
	else if (lookingFor == GIVE_ME_MATRIX_4X4)
	{
		originalId = GetIdByRegister(token);

		uint32_t matrixId = mVector4Matrix4X4Pairs[originalId];
		if (matrixId)
		{
			originalId = matrixId;
		}
	}
	else if (lookingFor == GIVE_ME_MATRIX_3X3)
	{
		originalId = GetIdByRegister(token);

		uint32_t matrixId = mVector4Matrix3X3Pairs[originalId];
		if (matrixId)
		{
			originalId = matrixId;
		}
	}
	else if (lookingFor == GIVE_ME_VECTOR_3)
	{
		originalId = GetIdByRegister(token);

		uint32_t vectorId = mVector4Vector3Pairs[originalId];
		if (vectorId)
		{
			originalId = vectorId;
		}
		else
		{
			//Couldn't find vec3 version so create one quick and return that instead.
			originalId = ConvertVec4ToVec3(originalId);
		}

		outputComponentCount = 3;
	}
	else
	{
		originalId = GetIdByRegister(token);
	}

	TypeDescription originalType = mIdTypePairs[originalId];
	uint32_t originalTypeId = GetSpirVTypeId(originalType);

	uint32_t loadedId = 0;
	TypeDescription loadedType;
	uint32_t loadedTypeId = 0;

	if (originalType.PrimaryType == spv::OpTypePointer)
	{
		
		loadedType.PrimaryType = originalType.SecondaryType;
		loadedType.SecondaryType = originalType.TernaryType;
		loadedType.TernaryType = spv::OpTypeVoid;
		loadedType.ComponentCount = originalType.ComponentCount;
		uint32_t loadedTypeId = GetSpirVTypeId(loadedType);

		loadedId = GetNextId();
		mIdTypePairs[loadedId] = loadedType;
		PushLoad(loadedTypeId, loadedId, originalId);	
	}
	else
	{
		loadedType = originalType;
		loadedId = originalId;
		loadedTypeId = originalTypeId;
	}

	/*
	Check for modifiers and if found apply them to the interim result.
	*/
	uint32_t modifier = token.i & D3DSP_SRCMOD_MASK;

	switch (modifier)
	{
	case D3DSPSM_NONE:
		//Nothing to do here.
		break;
	case D3DSPSM_NEG:
		if (loadedType.PrimaryType == spv::OpTypeFloat || loadedType.SecondaryType == spv::OpTypeFloat)
		{
			uint32_t negatedId = GetNextId();
			mIdTypePairs[negatedId] = loadedType;
			Push(spv::OpFNegate,loadedTypeId, negatedId, loadedId);
			loadedId = negatedId;
		}
		else
		{
			uint32_t negatedId = GetNextId();
			mIdTypePairs[negatedId] = loadedType;
			Push(spv::OpSNegate,loadedTypeId, negatedId, loadedId);
			loadedId = negatedId;
		}
		break;
	case D3DSPSM_BIAS:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_BIAS";
		break;
	case D3DSPSM_BIASNEG:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_BIASNEG";
		break;
	case D3DSPSM_SIGN:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_SIGN";
		break;
	case D3DSPSM_SIGNNEG:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_SIGNNEG";
		break;
	case D3DSPSM_COMP:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_COMP";
		break;
	case D3DSPSM_X2:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_X2";
		break;
	case D3DSPSM_X2NEG:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_X2NEG";
		break;
	case D3DSPSM_DZ:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_DZ";
		break;
	case D3DSPSM_DW:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_DW";
		break;
	case D3DSPSM_ABS:
		if (loadedType.PrimaryType == spv::OpTypeFloat || loadedType.SecondaryType == spv::OpTypeFloat)
		{
			uint32_t absoluteId = GetNextId();
			mIdTypePairs[absoluteId] = loadedType;
			Push(spv::OpExtInst, loadedTypeId, absoluteId, mGlslExtensionId, GLSLstd450::GLSLstd450FAbs, loadedId);
			loadedId = absoluteId;
		}
		else
		{
			uint32_t absoluteId = GetNextId();
			mIdTypePairs[absoluteId] = loadedType;
			Push(spv::OpExtInst, loadedTypeId, absoluteId, mGlslExtensionId, GLSLstd450::GLSLstd450SAbs, loadedId);
			loadedId = absoluteId;
		}
		break;
	case D3DSPSM_ABSNEG:
		if (loadedType.PrimaryType == spv::OpTypeFloat || loadedType.SecondaryType == spv::OpTypeFloat)
		{
			uint32_t absoluteId = GetNextId();
			mIdTypePairs[absoluteId] = loadedType;
			Push(spv::OpExtInst, loadedTypeId, absoluteId, mGlslExtensionId, GLSLstd450::GLSLstd450FAbs, loadedId);
			loadedId = absoluteId;

			uint32_t negatedId = GetNextId();
			mIdTypePairs[negatedId] = loadedType;
			Push(spv::OpFNegate, loadedTypeId, negatedId, loadedId);
			loadedId = negatedId;
		}
		else
		{
			uint32_t absoluteId = GetNextId();
			mIdTypePairs[absoluteId] = loadedType;
			Push(spv::OpExtInst, loadedTypeId, absoluteId, mGlslExtensionId, GLSLstd450::GLSLstd450SAbs, loadedId);
			loadedId = absoluteId;

			uint32_t negatedId = GetNextId();
			mIdTypePairs[negatedId] = loadedType;
			Push(spv::OpSNegate, loadedTypeId, negatedId, loadedId);
			loadedId = negatedId;
		}
		break;
	case D3DSPSM_NOT:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type D3DSPSM_NOT";
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetSwizzledId - Unsupported modifier type " << modifier;
		break;
	}

	/*
	Use either shuffle or extract to get the component(s) indicated by the swizzle mask.
	There are some special cases where we need to ignore the mask such as texcoord.
	*/
	uint32_t swizzle = token.i & D3DVS_SWIZZLE_MASK;

	if (swizzle == 0 || swizzle == D3DVS_NOSWIZZLE || outputComponentCount == 0 || lookingFor == GIVE_ME_SAMPLER)
	{
		return loadedId; //No swizzle no op.
	}

	if (lookingFor == GIVE_ME_MATRIX_4X4 || lookingFor == GIVE_ME_MATRIX_3X3)
	{
		//TODO: figure out what to do if we need a 3x3 but have a 4x4 or the other way around.
		return loadedId; //No swizzle no op.
	}

	uint32_t xSource = swizzle & D3DVS_X_W;
	uint32_t ySource = swizzle & D3DVS_Y_W;
	uint32_t zSource = swizzle & D3DVS_Z_W;
	uint32_t wSource = swizzle & D3DVS_W_W;
	uint32_t outputId = GetNextId();

	//For some reason the mask comes through as WW for text even though it needs to be XY.
	if (lookingFor == GIVE_ME_VECTOR_2 && mMajorVersion == 1)
	{
		xSource = D3DVS_X_X;
		ySource = D3DVS_Y_Y;
	}

	//OpVectorShuffle must return a vector and vectors must have at least 2 elements so OpCompositeExtract must be used for a single component swizzle operation.
	if
		(
		((xSource >> D3DVS_SWIZZLE_SHIFT) == (ySource >> (D3DVS_SWIZZLE_SHIFT + 2))) &&
			((xSource >> D3DVS_SWIZZLE_SHIFT) == (zSource >> (D3DVS_SWIZZLE_SHIFT + 4))) &&
			((xSource >> D3DVS_SWIZZLE_SHIFT) == (wSource >> (D3DVS_SWIZZLE_SHIFT + 6)))
			)
	{
		TypeDescription outputType = mIdTypePairs[loadedId];
		outputType.PrimaryType = outputType.SecondaryType;
		outputType.SecondaryType = outputType.TernaryType;
		outputType.TernaryType = spv::OpTypeVoid;
		outputType.ComponentCount = 0;

		if (loadedType.PrimaryType != spv::OpTypeVoid)
		{
			uint32_t outputTypeId = GetSpirVTypeId(outputType);

			switch (xSource)
			{
			case D3DVS_X_X:
				mIdTypePairs[outputId] = outputType;
				PushCompositeExtract(outputTypeId, outputId, loadedId, 0);			
				break;
			case D3DVS_X_Y:
				mIdTypePairs[outputId] = outputType;
				PushCompositeExtract(outputTypeId, outputId, loadedId, 1);		
				break;
			case D3DVS_X_Z:
				mIdTypePairs[outputId] = outputType;
				PushCompositeExtract(outputTypeId, outputId, loadedId, 2);		
				break;
			case D3DVS_X_W:
				mIdTypePairs[outputId] = outputType;
				PushCompositeExtract(outputTypeId, outputId, loadedId, 3);		
				break;
			}
		}
		else
		{
			outputId = loadedId;
		}
	}
	else //vector
	{
		std::string registerName;

		TypeDescription vectorType;
		vectorType.PrimaryType = spv::OpTypeVector;
		vectorType.SecondaryType = spv::OpTypeFloat;
		vectorType.ComponentCount = outputComponentCount;
		vectorType.StorageClass = spv::StorageClassInput;

		uint32_t vectorTypeId = GetSpirVTypeId(vectorType); //Revisit may not be a float

		mIdTypePairs[outputId] = vectorType;

		registerName = mNameIdPairs[loadedId];
		if (registerName.size())
		{
			//registerName += ".r";
			PushName(outputId, registerName);
		}

		uint32_t xIndex = 0;
		uint32_t yIndex = 0;
		uint32_t zIndex = 0;
		uint32_t wIndex = 0;

		switch (xSource)
		{
		case D3DVS_X_X:
			(xIndex = 0); //Component Literal
			break;
		case D3DVS_X_Y:
			(xIndex = 1); //Component Literal
			break;
		case D3DVS_X_Z:
			(xIndex = 2); //Component Literal
			break;
		case D3DVS_X_W:
			(xIndex = 3); //Component Literal
			break;
		}

		switch (ySource)
		{
		case D3DVS_Y_X:
			(yIndex = 0); //Component Literal
			break;
		case D3DVS_Y_Y:
			(yIndex = 1); //Component Literal
			break;
		case D3DVS_Y_Z:
			(yIndex = 2); //Component Literal
			break;
		case D3DVS_Y_W:
			(yIndex = 3); //Component Literal
			break;
		}

		switch (zSource)
		{
		case D3DVS_Z_X:
			(zIndex = 0); //Component Literal
			break;
		case D3DVS_Z_Y:
			(zIndex = 1); //Component Literal
			break;
		case D3DVS_Z_Z:
			(zIndex = 2); //Component Literal
			break;
		case D3DVS_Z_W:
			(zIndex = 3); //Component Literal
			break;
		}

		switch (wSource)
		{
		case D3DVS_W_X:
			(wIndex = 0); //Component Literal
			break;
		case D3DVS_W_Y:
			(wIndex = 1); //Component Literal
			break;
		case D3DVS_W_Z:
			(wIndex = 2); //Component Literal
			break;
		case D3DVS_W_W:
			(wIndex = 3); //Component Literal
			break;
		}

		switch (outputComponentCount)
		{ //1 should be covered by the other branch.
		case 2:
			Push(spv::OpVectorShuffle, vectorTypeId, outputId, loadedId, loadedId, xIndex, yIndex);
			break;
		case 3:
			Push(spv::OpVectorShuffle, vectorTypeId, outputId, loadedId, loadedId, xIndex, yIndex, zIndex);
			break;
		case 4:
			Push(spv::OpVectorShuffle, vectorTypeId, outputId, loadedId, loadedId, xIndex, yIndex, zIndex, wIndex);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "GetSwizzledId - Unsupported component count  " << outputComponentCount;
			break;
		}
	}

	return outputId;
}

/*
If we are for example setting xyz on a vec4 we need an access chain to store the correct values in the right spot.
We use the write mask on the result token to figure out out many components we are storing and which ones.
*/
uint32_t ShaderConverter::SwizzlePointer(const Token& token)
{
	uint32_t originalId = GetIdByRegister(token);
	TypeDescription originalType = GetTypeByRegister(token);
	//TypeDescription inputType = mIdTypePairs[originalId];
	TypeDescription outputType;
	uint32_t outputTypeId;
	uint32_t outputId = 0;
	uint32_t outputComponentCount = 0;

	if (token.i & D3DSP_WRITEMASK_0)
	{
		outputComponentCount++;
	}

	if (token.i & D3DSP_WRITEMASK_1)
	{
		outputComponentCount++;
	}

	if (token.i & D3DSP_WRITEMASK_2)
	{
		outputComponentCount++;
	}

	if (token.i & D3DSP_WRITEMASK_3)
	{
		outputComponentCount++;
	}

	if (originalType.SecondaryType != spv::OpTypeVector && outputComponentCount == 1)
	{
		return originalId;
	}

	if (originalType.SecondaryType == spv::OpTypeVector && originalType.ComponentCount == outputComponentCount)
	{
		return originalId;
	}

	if (outputComponentCount > 1)
	{
		return originalId;
	}
	else
	{
		outputType.PrimaryType = spv::OpTypePointer;
		outputType.SecondaryType = originalType.TernaryType;
		outputTypeId = GetSpirVTypeId(outputType);
	}

	if (originalType == outputType)
	{
		return originalId;
	}

	outputId = GetNextId();

	if (token.i & D3DSP_WRITEMASK_0)
	{
		PushAccessChain(outputTypeId, outputId, originalId, m0Id);
	}
	else if (token.i & D3DSP_WRITEMASK_1)
	{
		PushAccessChain(outputTypeId, outputId, originalId, m1Id);
	}
	else if (token.i & D3DSP_WRITEMASK_2)
	{
		PushAccessChain(outputTypeId, outputId, originalId, m2Id);
	}
	else if (token.i & D3DSP_WRITEMASK_3)
	{
		PushAccessChain(outputTypeId, outputId, originalId, m3Id);
	}

	mIdTypePairs[outputId] = outputType;

	return outputId;
}

/*
Take a vector and shuffle or extract it to get the right data type.
To figure out what the right type is we use the write mask on the result token.
*/
uint32_t ShaderConverter::SwizzleValue(const Token& token, uint32_t inputId)
{
	auto inputType = mIdTypePairs[inputId];

	if (inputType.PrimaryType != spv::OpTypeVector)
	{
		return inputId;
	}

	TypeDescription outputType;
	uint32_t outputTypeId = 0;
	uint32_t outputId = 0;
	uint32_t outputComponentCount = 0;
	uint32_t index[4] = {};
	size_t indexIndex = 0;

	if (token.i & D3DSP_WRITEMASK_0)
	{
		index[indexIndex] = 0;
		indexIndex++;

		outputComponentCount++;
	}

	if (token.i & D3DSP_WRITEMASK_1)
	{
		index[indexIndex] = 1;
		indexIndex++;

		outputComponentCount++;
	}

	if (token.i & D3DSP_WRITEMASK_2)
	{
		index[indexIndex] = 2;
		indexIndex++;

		outputComponentCount++;
	}

	if (token.i & D3DSP_WRITEMASK_3)
	{
		index[indexIndex] = 3;
		indexIndex++;

		outputComponentCount++;
	}

	outputId = GetNextId();

	if (outputComponentCount > 1)
	{
		outputType = inputType;
		outputType.ComponentCount = outputComponentCount;
		outputTypeId = GetSpirVTypeId(outputType);

		mIdTypePairs[outputId] = outputType;

		std::string registerName = mNameIdPairs[inputId];
		if (registerName.size())
		{
			//registerName += ".r";
			PushName(outputId, registerName);
		}

		switch (outputComponentCount)
		{ //1 should be covered by the other branch.
		case 2:
			Push(spv::OpVectorShuffle, outputTypeId, outputId, inputId, inputId, index[0], index[1]);
			break;
		case 3:
			Push(spv::OpVectorShuffle, outputTypeId, outputId, inputId, inputId, index[0], index[1], index[2]);
			break;
		case 4:
			Push(spv::OpVectorShuffle, outputTypeId, outputId, inputId, inputId, index[0], index[1], index[2], index[3]);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "GetSwizzledId - Unsupported component count  " << outputComponentCount;
			break;
		}
	}
	else
	{
		outputType.PrimaryType = inputType.SecondaryType;
		outputTypeId = GetSpirVTypeId(outputType);

		std::string registerName = mNameIdPairs[inputId];
		if (registerName.size())
		{
			//registerName += ".r";
			PushName(outputId, registerName);
		}

		mIdTypePairs[outputId] = outputType;

		if (token.i & D3DSP_WRITEMASK_0)
		{
			PushCompositeExtract(outputTypeId, outputId, inputId, 0);
		}

		if (token.i & D3DSP_WRITEMASK_1)
		{
			PushCompositeExtract(outputTypeId, outputId, inputId, 1);
		}

		if (token.i & D3DSP_WRITEMASK_2)
		{
			PushCompositeExtract(outputTypeId, outputId, inputId, 2);
		}

		if (token.i & D3DSP_WRITEMASK_3)
		{
			PushCompositeExtract(outputTypeId, outputId, inputId, 3);
		}
	}

	return outputId;
}

/*
Colors can come in as a vector of UINT. In this case values are 0 to 255 instead of 0.0 to 1.0
With that being the case I just split out the components and divide by 255.
I also check the result token write mask so I know which ones to actually store to the output pointer.
*/
void ShaderConverter::HandleColor(const Token& token, uint32_t inputId, uint32_t outputId, uint32_t originalId)
{
	TypeDescription inputType = mIdTypePairs[inputId];
	TypeDescription outputType = mIdTypePairs[outputId];

	TypeDescription intType;
	intType.PrimaryType = spv::OpTypeInt;
	uint32_t intTypeId = GetSpirVTypeId(intType);

	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	if (inputType.PrimaryType == spv::OpTypeVector)
	{
		uint32_t r2Id = GetNextId();
		uint32_t g2Id = GetNextId();
		uint32_t b2Id = GetNextId();
		uint32_t a2Id = GetNextId();

		if (token.i & D3DSP_WRITEMASK_0)
		{
			if (inputType.SecondaryType == spv::OpTypeInt)
			{
				uint32_t rId = GetNextId();
				mIdTypePairs[rId] = intType;
				PushCompositeExtract(intTypeId, rId, inputId, 0);

				mIdTypePairs[r2Id] = floatType;
				Push(spv::OpConvertUToF, floatTypeId, r2Id, rId);			
			}
			else
			{
				mIdTypePairs[r2Id] = floatType;
				PushCompositeExtract(floatTypeId, r2Id, inputId, 0);
				
			}

			uint32_t rDividedId = GetNextId();
			mIdTypePairs[rDividedId] = floatType;
			Push(spv::OpFDiv, floatTypeId, rDividedId, r2Id, m255FloatId);

			if (originalId == mColor1Id)
			{
				PushStore(mColor1XId, rDividedId);
			}
			else
			{
				PushStore(mColor2XId, rDividedId);
			}
		}

		if (token.i & D3DSP_WRITEMASK_1)
		{
			if (inputType.SecondaryType == spv::OpTypeInt)
			{
				uint32_t gId = GetNextId();
				mIdTypePairs[gId] = intType;
				PushCompositeExtract(intTypeId, gId, inputId, 1);

				mIdTypePairs[g2Id] = floatType;
				Push(spv::OpConvertUToF, floatTypeId, g2Id, gId);			
			}
			else
			{
				mIdTypePairs[g2Id] = floatType;
				PushCompositeExtract(floatTypeId, g2Id, inputId, 1);		
			}

			uint32_t gDividedId = GetNextId();
			mIdTypePairs[gDividedId] = floatType;
			Push(spv::OpFDiv, floatTypeId, gDividedId, g2Id, m255FloatId);

			if (originalId == mColor1Id)
			{
				PushStore(mColor1YId, gDividedId);
			}
			else
			{
				PushStore(mColor2YId, gDividedId);
			}
		}

		if (token.i & D3DSP_WRITEMASK_2)
		{
			if (inputType.SecondaryType == spv::OpTypeInt)
			{
				uint32_t bId = GetNextId();
				mIdTypePairs[bId] = intType;
				PushCompositeExtract(intTypeId, bId, inputId, 2);

				mIdTypePairs[b2Id] = floatType;
				Push(spv::OpConvertUToF, floatTypeId, b2Id, bId);			
			}
			else
			{
				mIdTypePairs[b2Id] = floatType;
				PushCompositeExtract(floatTypeId, b2Id, inputId, 2);
			}

			uint32_t bDividedId = GetNextId();
			mIdTypePairs[bDividedId] = floatType;
			Push(spv::OpFDiv, floatTypeId, bDividedId, b2Id, m255FloatId);

			if (originalId == mColor1Id)
			{
				PushStore(mColor1ZId, bDividedId);
			}
			else
			{
				PushStore(mColor2ZId, bDividedId);
			}
		}

		if (token.i & D3DSP_WRITEMASK_3)
		{
			if (inputType.SecondaryType == spv::OpTypeInt)
			{
				uint32_t aId = GetNextId();
				mIdTypePairs[aId] = intType;
				PushCompositeExtract(intTypeId, aId, inputId, 3);

				mIdTypePairs[a2Id] = floatType;
				Push(spv::OpConvertUToF, floatTypeId, a2Id, aId);
			}
			else
			{
				mIdTypePairs[a2Id] = floatType;
				PushCompositeExtract(floatTypeId, a2Id, inputId, 3);
			}

			uint32_t aDividedId = GetNextId();
			mIdTypePairs[aDividedId] = floatType;
			Push(spv::OpFDiv, floatTypeId, aDividedId, a2Id, m255FloatId);

			if (originalId == mColor1Id)
			{
				PushStore(mColor1WId, aDividedId);
			}
			else
			{
				PushStore(mColor2WId, aDividedId);
			}
		}
	}
	else
	{
		uint32_t r2Id;

		if (inputType.PrimaryType == spv::OpTypeInt)
		{
			r2Id = GetNextId();
			mIdTypePairs[r2Id] = floatType;
			Push(spv::OpConvertUToF, floatTypeId, r2Id, inputId);	
		}
		else
		{
			r2Id = inputId;
		}	

		uint32_t rDividedId = GetNextId();
		mIdTypePairs[rDividedId] = floatType;
		Push(spv::OpFDiv, floatTypeId, rDividedId, r2Id, m255FloatId);

		if (originalId == mColor1Id)
		{
			if (token.i & D3DSP_WRITEMASK_0)
			{
				PushStore(mColor1XId, rDividedId);
			}

			if (token.i & D3DSP_WRITEMASK_1)
			{
				PushStore(mColor1YId, rDividedId);
			}

			if (token.i & D3DSP_WRITEMASK_2)
			{
				PushStore(mColor1ZId, rDividedId);
			}

			if (token.i & D3DSP_WRITEMASK_3)
			{
				PushStore(mColor1WId, rDividedId);
			}
		}
		else
		{
			if (token.i & D3DSP_WRITEMASK_0)
			{
				PushStore(mColor2XId, rDividedId);
			}

			if (token.i & D3DSP_WRITEMASK_1)
			{
				PushStore(mColor2YId, rDividedId);
			}

			if (token.i & D3DSP_WRITEMASK_2)
			{
				PushStore(mColor2ZId, rDividedId);
			}

			if (token.i & D3DSP_WRITEMASK_3)
			{
				PushStore(mColor2WId, rDividedId);
			}
		}
	}
}

/*
This method takes the result of an operation and does something with it.
If it's just a register it will replace the id in the register dictionary so later calls will get this result instead of an earlier generation.
If it's a pointer though we'll need to do a store to save the result. If it's a color we do some special magic.
*/
uint32_t ShaderConverter::ApplyWriteMask(const Token& token, uint32_t modifiedId, _D3DDECLUSAGE usage)
{
	TypeDescription modifiedType = mIdTypePairs[modifiedId];
	TypeDescription originalType = GetTypeByRegister(token);
	uint32_t originalId = GetIdByRegister(token);
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(token.i);

	uint32_t inputId = 0;
	uint32_t outputId = 0;

	//TypeDescription intType;
	//intType.PrimaryType = spv::OpTypeInt;
	//uint32_t intTypeId = GetSpirVTypeId(intType);

	//TypeDescription intPointerType;
	//intPointerType.PrimaryType = spv::OpTypePointer;
	//intPointerType.SecondaryType = spv::OpTypeInt;
	//uint32_t intPointerTypeId = GetSpirVTypeId(intPointerType);

	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	TypeDescription floatPointerType;
	floatPointerType.PrimaryType = spv::OpTypePointer;
	floatPointerType.SecondaryType = spv::OpTypeFloat;
	uint32_t floatPointerTypeId = GetSpirVTypeId(floatPointerType);

	//If the input is a pointer go ahead and load it. Otherwise we'll just roll with what was passed in.
	if (modifiedType.PrimaryType == spv::OpTypePointer)
	{
		TypeDescription loadedModifiedType;
		loadedModifiedType.PrimaryType = modifiedType.SecondaryType;
		loadedModifiedType.SecondaryType = modifiedType.TernaryType;
		loadedModifiedType.TernaryType = spv::OpTypeVoid;
		loadedModifiedType.ComponentCount = modifiedType.ComponentCount;
		uint32_t loadedModifiedTypeId = GetSpirVTypeId(loadedModifiedType);

		inputId = GetNextId();
		mIdTypePairs[inputId] = loadedModifiedType;
		PushLoad(loadedModifiedTypeId, inputId, modifiedId);	
	}
	else
	{
		inputId = modifiedId;
	}

	/*
	Sometimes we have to go from vec4 to vec3 or vec4 to float.
	The result token tells us what the swizzle needs to be but we'll have to emit some code to convert it to the right type.
	*/
	inputId = SwizzleValue(token, inputId);
	
	/*
	If this token has the _sat modifier then do a clamp before we store the result.
	*/
	if (token.i & D3DSPDM_SATURATE)
	{
		uint32_t saturatedInputId  = GetNextId();
		TypeDescription saturatedInputType = mIdTypePairs[inputId];
		uint32_t saturatedInputTypeId = GetSpirVTypeId(saturatedInputType);
		mIdTypePairs[saturatedInputId] = saturatedInputType;

		if (saturatedInputType.PrimaryType == spv::OpTypeFloat || saturatedInputType.SecondaryType == spv::OpTypeFloat)
		{
			Push(spv::OpExtInst, saturatedInputTypeId, saturatedInputId, mGlslExtensionId, GLSLstd450::GLSLstd450FClamp, inputId, m0fId, m1fId);
		}
		else
		{
			Push(spv::OpExtInst, saturatedInputTypeId, saturatedInputId, mGlslExtensionId, GLSLstd450::GLSLstd450UClamp, inputId, m0fId, m1fId);
		}

		inputId = saturatedInputId;
	}

	if (token.i & D3DSPDM_PARTIALPRECISION)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::ApplyWriteMask - D3DSPDM_PARTIALPRECISION is not supported!";
	}

	if (token.i & D3DSPDM_MSAMPCENTROID)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::ApplyWriteMask - D3DSPDM_MSAMPCENTROID is not supported!";
	}

	/*
	For some reason you can write the result of TEX back to the texture register in DXBC.
	This makes Spir-V angry because input registers are read-only. 
	So my work around is to make sure that if the target is an input register to just replace the id with the new one so later calls will pick up the result.
	*/
	if (originalType.StorageClass == spv::StorageClassInput)
	{
		outputId = inputId;
		SetIdByRegister(token, outputId);
	}
	else if (originalType.PrimaryType == spv::OpTypePointer)
	{
		outputId = SwizzlePointer(token);
		if ((originalId == mColor1Id || originalId == mColor2Id))
		{
			HandleColor(token, inputId, outputId, originalId);
		}
		else
		{
			if (((token.i & D3DSP_WRITEMASK_ALL) == D3DSP_WRITEMASK_ALL) && (originalType.ComponentCount == modifiedType.ComponentCount))
			{
				PushStore(outputId, inputId);
			}
			else
			{
				TypeDescription swizzledType = mIdTypePairs[inputId];
				if (swizzledType.PrimaryType == spv::OpTypeVector)
				{
					/*
					Based on a glsl bug report doing a shuffle and then storing it like I used to do is some kind of race condition.
					To avoid any UB I've switched to the recommended method of storing each compoenent.
					https://github.com/KhronosGroup/glslang/issues/94
					*/
					if (token.i & D3DSP_WRITEMASK_0)
					{
						uint32_t objectId1 = GetNextId();
						uint32_t pointerId1 = GetNextId();

						mIdTypePairs[objectId1] = floatType;
						PushCompositeExtract(floatTypeId, objectId1, inputId, 0);
						
						mIdTypePairs[pointerId1] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId1, originalId, m0Id);
						
						PushStore(pointerId1, objectId1);
					}

					if (token.i & D3DSP_WRITEMASK_1)
					{
						uint32_t objectId2 = GetNextId();
						uint32_t pointerId2 = GetNextId();
						
						mIdTypePairs[objectId2] = floatType;
						PushCompositeExtract(floatTypeId, objectId2, inputId, 1);
						
						mIdTypePairs[pointerId2] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId2, originalId, m1Id);
						
						PushStore(pointerId2, objectId2);
					}

					if (token.i & D3DSP_WRITEMASK_2)
					{
						uint32_t objectId3 = GetNextId();
						uint32_t pointerId3 = GetNextId();

						mIdTypePairs[objectId3] = floatType;
						PushCompositeExtract(floatTypeId, objectId3, inputId, 2);
						
						mIdTypePairs[pointerId3] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId3, originalId, m2Id);
						
						PushStore(pointerId3, objectId3);
					}

					if (token.i & D3DSP_WRITEMASK_3)
					{
						uint32_t objectId4 = GetNextId();
						uint32_t pointerId4 = GetNextId();
						
						mIdTypePairs[objectId4] = floatType;
						PushCompositeExtract(floatTypeId, objectId4, inputId, 3);
						
						mIdTypePairs[pointerId4] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId4, originalId, m3Id);
						
						PushStore(pointerId4, objectId4);
					}
				}
				else
				{
					uint32_t objectId1 = inputId;

					if (token.i & D3DSP_WRITEMASK_0)
					{
						uint32_t pointerId1 = GetNextId();

						mIdTypePairs[pointerId1] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId1, originalId, m0Id);
						
						PushStore(pointerId1, objectId1);
					}

					if (token.i & D3DSP_WRITEMASK_1)
					{
						uint32_t pointerId2 = GetNextId();

						mIdTypePairs[pointerId2] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId2, originalId, m1Id);
						
						PushStore(pointerId2, objectId1);
					}

					if (token.i & D3DSP_WRITEMASK_2)
					{
						uint32_t pointerId3 = GetNextId();

						mIdTypePairs[pointerId3] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId3, originalId, m2Id);
						
						PushStore(pointerId3, objectId1);
					}

					if (token.i & D3DSP_WRITEMASK_3)
					{
						uint32_t pointerId4 = GetNextId();

						mIdTypePairs[pointerId4] = floatPointerType;
						PushAccessChain(floatPointerTypeId, pointerId4, originalId, m3Id);
						
						PushStore(pointerId4, objectId1);
					}
				}
			}
		}
	}
	else
	{
		outputId = originalId;
		SetIdByRegister(token, outputId);
	}

	return outputId;
}

void ShaderConverter::GenerateYFlip()
{
	if (!mPositionYId)
	{
		return;
	}

	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	uint32_t negativeId = GetNextId();
	mIdTypePairs[negativeId] = floatType;
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(floatTypeId); //Result Type (Id)
	mTypeInstructions.push_back(negativeId); //Result (Id)
	mTypeInstructions.push_back(bit_cast(-1.0f)); //Literal Value

	uint32_t positionYId = GetNextId();
	mIdTypePairs[positionYId] = floatType;
	PushLoad(floatTypeId, positionYId, mPositionYId);

	uint32_t resultId = GetNextId();
	mIdTypePairs[resultId] = floatType;
	Push(spv::OpFMul, floatTypeId, resultId, positionYId, negativeId);
	PushStore(mPositionYId, resultId);
}

void ShaderConverter::GeneratePushConstant()
{
	std::string registerName;
	uint32_t stringWordSize = 0;

	TypeDescription matrixPointerType;
	matrixPointerType.PrimaryType = spv::OpTypePointer;
	matrixPointerType.SecondaryType = spv::OpTypeMatrix;
	matrixPointerType.TernaryType = spv::OpTypeVector;
	matrixPointerType.ComponentCount = 4;
	matrixPointerType.StorageClass = spv::StorageClassPushConstant;

	TypeDescription vectorPointerType;
	vectorPointerType.PrimaryType = spv::OpTypePointer;
	vectorPointerType.SecondaryType = spv::OpTypeVector;
	vectorPointerType.TernaryType = spv::OpTypeFloat;
	vectorPointerType.ComponentCount = 4;
	vectorPointerType.StorageClass = spv::StorageClassPushConstant;

	TypeDescription matrixType;
	matrixType.PrimaryType = spv::OpTypeMatrix;
	matrixType.SecondaryType = spv::OpTypeVector;
	matrixType.ComponentCount = 4;

	TypeDescription vectorType;
	vectorType.PrimaryType = spv::OpTypeVector;
	vectorType.SecondaryType = spv::OpTypeFloat;
	vectorType.ComponentCount = 4;

	uint32_t matrixPointerTypeId = GetSpirVTypeId(matrixPointerType);
	uint32_t vectorPointerTypeId = GetSpirVTypeId(vectorPointerType);
	uint32_t matrixTypeId = GetSpirVTypeId(matrixType);
	uint32_t vectorTypeId = GetSpirVTypeId(vectorType);

	uint32_t pushConstantTypeId = GetNextId();
	uint32_t pushConstantPointerTypeId = GetNextId();
	uint32_t pushConstantPointerId = GetNextId();

	mTypeInstructions.push_back(Pack(2 + 1, spv::OpTypeStruct)); //size,Type
	mTypeInstructions.push_back(pushConstantTypeId); //Result (Id)
	mTypeInstructions.push_back(matrixTypeId); //Member 0 type (Id)

	//Name and decorate push constant structure type.
	registerName = "PushConstants";
	PushName(pushConstantTypeId, registerName);

	mDecorateInstructions.push_back(Pack(3, spv::OpDecorate)); //size,Type
	mDecorateInstructions.push_back(pushConstantTypeId); //target (Id)
	mDecorateInstructions.push_back(spv::DecorationBlock); //Decoration Type (Id)

	mDecorateInstructions.push_back(Pack(4, spv::OpDecorate)); //size,Type
	mDecorateInstructions.push_back(matrixTypeId); //target (Id)
	mDecorateInstructions.push_back(spv::DecorationMatrixStride); //Decoration Type (Id)
	mDecorateInstructions.push_back(16); //stride

	mDecorateInstructions.push_back(Pack(3, spv::OpDecorate)); //size,Type
	mDecorateInstructions.push_back(matrixTypeId); //target (Id)
	mDecorateInstructions.push_back(spv::DecorationColMajor); //Decoration Type (Id)

	//Name Members
	registerName = "c0";
	PushMemberName(pushConstantTypeId, registerName, 0);

	//Set member offsets
	mDecorateInstructions.push_back(Pack(4 + 1, spv::OpMemberDecorate)); //size,Type
	mDecorateInstructions.push_back(pushConstantTypeId); //target (Id)
	mDecorateInstructions.push_back(0); //Member (Literal)
	mDecorateInstructions.push_back(spv::DecorationOffset); //Decoration Type (Id)
	mDecorateInstructions.push_back(0);

	//Create Pointer type and variable
	mTypeInstructions.push_back(Pack(4, spv::OpTypePointer)); //size,Type
	mTypeInstructions.push_back(pushConstantPointerTypeId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassPushConstant); //Storage Class
	mTypeInstructions.push_back(pushConstantTypeId); //type (Id)

	mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
	mTypeInstructions.push_back(pushConstantPointerTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
	mTypeInstructions.push_back(pushConstantPointerId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassPushConstant); //Storage Class

	registerName = "PC";
	PushName(pushConstantPointerId, registerName);

	//Create Access Chains
	uint32_t matrixPointerId = GetNextId();
	mIdTypePairs[matrixPointerId] = matrixPointerType;
	PushAccessChain(matrixPointerTypeId, matrixPointerId, pushConstantPointerId, m0Id);

	uint32_t c0 = GetNextId();
	mIdTypePairs[c0] = vectorPointerType;
	PushAccessChain(vectorPointerTypeId, c0, matrixPointerId, m0Id);

	uint32_t c1 = GetNextId();
	mIdTypePairs[c1] = vectorPointerType;
	PushAccessChain(vectorPointerTypeId, c1, matrixPointerId, m1Id);

	uint32_t c2 = GetNextId();
	mIdTypePairs[c2] = vectorPointerType;
	PushAccessChain(vectorPointerTypeId, c2, matrixPointerId, m2Id);

	uint32_t c3 = GetNextId();
	mIdTypePairs[c3] = vectorPointerType;
	PushAccessChain(vectorPointerTypeId, c3, matrixPointerId, m3Id);

	//Load Access Chains
	uint32_t matrix_loaded = GetNextId();
	mIdTypePairs[matrix_loaded] = matrixType;
	PushLoad(matrixTypeId, matrix_loaded, matrixPointerId);

	uint32_t c0_loaded = GetNextId();
	mIdTypePairs[c0_loaded] = vectorType;
	PushLoad(vectorTypeId, c0_loaded, c0);
	mVector4Vector3Pairs[c0_loaded] = ConvertVec4ToVec3(c0_loaded);

	uint32_t c1_loaded = GetNextId();
	mIdTypePairs[c1_loaded] = vectorType;
	PushLoad(vectorTypeId, c1_loaded, c1);
	mVector4Vector3Pairs[c1_loaded] = ConvertVec4ToVec3(c1_loaded);

	uint32_t c2_loaded = GetNextId();
	mIdTypePairs[c2_loaded] = vectorType;
	PushLoad(vectorTypeId, c2_loaded, c2);
	mVector4Vector3Pairs[c2_loaded] = ConvertVec4ToVec3(c2_loaded);

	uint32_t c3_loaded = GetNextId();
	mIdTypePairs[c3_loaded] = vectorType;
	PushLoad(vectorTypeId, c3_loaded, c3);
	mVector4Vector3Pairs[c3_loaded] = ConvertVec4ToVec3(c3_loaded);

	mVector4Matrix4X4Pairs[c0_loaded] = matrix_loaded;
	mVector4Matrix3X3Pairs[c0_loaded] = ConvertMat4ToMat3(matrix_loaded);

	//Remap c0-c3 to push constant.
	mIdsByRegister[(_D3DSHADER_PARAM_REGISTER_TYPE)1337][0] = matrix_loaded;
	mRegistersById[(_D3DSHADER_PARAM_REGISTER_TYPE)1337][matrix_loaded] = 0;

	mIdsByRegister[D3DSPR_CONST][0] = c0_loaded;
	mRegistersById[D3DSPR_CONST][c0_loaded] = 0;

	mIdsByRegister[D3DSPR_CONST][1] = c1_loaded;
	mRegistersById[D3DSPR_CONST][c1_loaded] = 1;

	mIdsByRegister[D3DSPR_CONST][2] = c2_loaded;
	mRegistersById[D3DSPR_CONST][c2_loaded] = 2;

	mIdsByRegister[D3DSPR_CONST][3] = c3_loaded;
	mRegistersById[D3DSPR_CONST][c3_loaded] = 3;
}

void ShaderConverter::GenerateConstantIndices()
{
	std::string registerName;
	uint32_t stringWordSize = 0;

	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	m0fId = GetNextId();
	mIdTypePairs[m0fId] = floatType;
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(floatTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m0fId); //Result (Id)
	mTypeInstructions.push_back(0); //Literal Value
	registerName = "float_0";
	PushName(m0fId, registerName);

	m1fId = GetNextId();
	mIdTypePairs[m1fId] = floatType;
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(floatTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m1fId); //Result (Id)
	mTypeInstructions.push_back(1); //Literal Value
	registerName = "float_1";
	PushName(m1fId, registerName);

	TypeDescription intType;
	intType.PrimaryType = spv::OpTypeInt;
	uint32_t intTypeId = GetSpirVTypeId(intType);

	m0Id = GetNextId();
	mIdTypePairs[m0Id] = intType;
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m0Id); //Result (Id)
	mTypeInstructions.push_back(0); //Literal Value
	registerName = "int_0";
	PushName(m0Id, registerName);

	m1Id = GetNextId();
	mIdTypePairs[m1Id] = intType;
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m1Id); //Result (Id)
	mTypeInstructions.push_back(1); //Literal Value
	registerName = "int_1";
	PushName(m1Id, registerName);

	m2Id = GetNextId();
	mIdTypePairs[m2Id] = intType;
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m2Id); //Result (Id)
	mTypeInstructions.push_back(2); //Literal Value
	registerName = "int_2";
	PushName(m2Id, registerName);

	m3Id = GetNextId();
	mIdTypePairs[m3Id] = intType;
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m3Id); //Result (Id)
	mTypeInstructions.push_back(3); //Literal Value
	registerName = "int_3";
	PushName(m3Id, registerName);
}

void ShaderConverter::GeneratePostition()
{
	TypeDescription positionType;
	positionType.PrimaryType = spv::OpTypeVector;
	positionType.SecondaryType = spv::OpTypeFloat;
	positionType.ComponentCount = 4;

	TypeDescription positionPointerType;
	positionPointerType.PrimaryType = spv::OpTypePointer;
	positionPointerType.SecondaryType = spv::OpTypeVector;
	positionPointerType.TernaryType = spv::OpTypeFloat;
	positionPointerType.ComponentCount = 4;
	positionPointerType.StorageClass = spv::StorageClassOutput;

	TypeDescription floatPointerType;
	floatPointerType.PrimaryType = spv::OpTypePointer;
	floatPointerType.SecondaryType = spv::OpTypeFloat;
	uint32_t floatPointerTypeId = GetSpirVTypeId(floatPointerType);

	uint32_t floatTypeId = GetSpirVTypeId(spv::OpTypeFloat);

	uint32_t positionTypeId = GetSpirVTypeId(positionType);
	uint32_t positionPointerTypeId = GetSpirVTypeId(positionPointerType);
	uint32_t positionStructureTypeId = GetNextId();
	uint32_t positionStructurePointerTypeId = GetNextId();
	uint32_t positionStructurePointerId = GetNextId();
	uint32_t positionPointerId = GetNextId();
	std::string registerName;
	uint32_t stringWordSize = 0;

	mTypeInstructions.push_back(Pack(2 + 1, spv::OpTypeStruct)); //size,Type
	mTypeInstructions.push_back(positionStructureTypeId); //Result (Id)
	mTypeInstructions.push_back(positionTypeId); //Member 0 type (Id)

	mTypeInstructions.push_back(Pack(4, spv::OpTypePointer)); //size,Type
	mTypeInstructions.push_back(positionStructurePointerTypeId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
	mTypeInstructions.push_back(positionStructureTypeId); //type (Id)

	mDecorateInstructions.push_back(Pack(3, spv::OpDecorate)); //size,Type
	mDecorateInstructions.push_back(positionStructureTypeId); //target (Id)
	mDecorateInstructions.push_back(spv::DecorationBlock); //Decoration Type (Id)

	mDecorateInstructions.push_back(Pack(4 + 1, spv::OpMemberDecorate)); //size,Type
	mDecorateInstructions.push_back(positionStructureTypeId); //target (Id)
	mDecorateInstructions.push_back(0); //Member (Literal)
	mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
	mDecorateInstructions.push_back(spv::BuiltInPosition);

	mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
	mTypeInstructions.push_back(positionStructurePointerTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
	mTypeInstructions.push_back(positionStructurePointerId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class

	registerName = "gl_PerVertex";
	PushName(positionStructurePointerId, registerName);

	mIdTypePairs[positionPointerId] = positionPointerType;
	PushAccessChain(positionPointerTypeId, positionPointerId, positionStructurePointerId, m0Id);

	//Updated tracking structures
	mPositionId = positionPointerId;
	mIdsByRegister[D3DSPR_RASTOUT][0] = positionPointerId;
	mRegistersById[D3DSPR_RASTOUT][positionPointerId] = 0;
	mIdTypePairs[positionPointerId] = positionPointerType;

	mOutputRegisters.push_back(positionStructurePointerId);
	mOutputRegisterUsages[D3DDECLUSAGE_POSITION] = positionPointerId;

	registerName = "gl_Position";
	PushName(positionPointerId, registerName);

	//Add an access chain for later flipping.
	mPositionYId = GetNextId();
	mIdTypePairs[mPositionYId] = floatPointerType;
	PushAccessChain(floatPointerTypeId, mPositionYId, positionPointerId, m1Id);
}

void ShaderConverter::GenerateStore(const Token& token, uint32_t inputId)
{
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(token.i);
	uint32_t resultId = GetNextVersionId(token);
	uint32_t argumentId1 = inputId;

	switch (resultRegisterType)
	{
	case D3DSPR_RASTOUT:
	case D3DSPR_ATTROUT:
	case D3DSPR_COLOROUT:
	case D3DSPR_DEPTHOUT:
	case D3DSPR_OUTPUT:
		PushStore(resultId, argumentId1);
		break;
	default:
		break;
	}
}

void ShaderConverter::GenerateDecoration(uint32_t registerNumber, uint32_t inputId, _D3DDECLUSAGE usage, bool isInput)
{
	if (this->mMajorVersion == 3)
	{
		switch (usage)
		{
		case D3DDECLUSAGE_POSITION:
			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(inputId); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
			mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
			break;
		default:
			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(inputId); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
			mDecorateInstructions.push_back(registerNumber); //Location offset
			break;
		}
	}
	else
	{
		if (isInput)
		{
			if (this->mIsVertexShader)
			{
				mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
				mDecorateInstructions.push_back(inputId); //target (Id)
				mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
				mDecorateInstructions.push_back(registerNumber); //Location offset
			}
			else
			{
				switch (usage)
				{
				case D3DDECLUSAGE_POSITION:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
					mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
					break;
				case D3DDECLUSAGE_COLOR:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 2); //Location offset
					break;
				default:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 4); //Location offset
					break;
				}
			}
		}
		else
		{
			if (this->mIsVertexShader)
			{
				switch (usage)
				{
				case D3DDECLUSAGE_POSITION:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
					mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
					break;
				case D3DDECLUSAGE_FOG:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(0); //Location offset
					break;
				case D3DDECLUSAGE_PSIZE:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(1); //Location offset
					break;
				case D3DDECLUSAGE_COLOR:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 2); //Location offset
					break;
				default:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 4); //Location offset
					break;
				}
			}
			else
			{
				switch (usage)
				{
				case D3DDECLUSAGE_POSITION:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
					mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
					break;
				default:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber); //Location offset
					break;
				}
			}
		}
	}
}

void ShaderConverter::Generate255Constants()
{	
	uint32_t id = GetNextId();
	TypeDescription type;
	type.PrimaryType = spv::OpTypeFloat;
	uint32_t typeId = GetSpirVTypeId(type);

	mIdTypePairs[id] = type;

	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(typeId); //Result Type (Id)
	mTypeInstructions.push_back(id); //Result (Id)
	mTypeInstructions.push_back(bit_cast(255.0f)); //Literal Value

	uint32_t compositeId = GetNextId();
	TypeDescription compositeType;
	compositeType.PrimaryType = spv::OpTypeVector;
	compositeType.SecondaryType = spv::OpTypeFloat;
	compositeType.ComponentCount = 4;
	uint32_t compositeTypeId = GetSpirVTypeId(compositeType);

	mIdTypePairs[compositeId] = compositeType;

	mTypeInstructions.push_back(Pack(3 + 4, spv::OpConstantComposite)); //size,Type
	mTypeInstructions.push_back(compositeTypeId); //Result Type (Id)
	mTypeInstructions.push_back(compositeId); //Result (Id)
	mTypeInstructions.push_back(id); // Value (Id)
	mTypeInstructions.push_back(id); // Value (Id)
	mTypeInstructions.push_back(id); // Value (Id)
	mTypeInstructions.push_back(id); // Value (Id)

	m255FloatId = id;
	m255VectorId = compositeId;
}

void ShaderConverter::GenerateConstantBlock()
{
	TypeDescription typeDescription; //OpTypeVoid isn't 0 so ={} borks things.
	TypeDescription componentTypeDescription; //OpTypeVoid isn't 0 so ={} borks things.
	uint32_t typeId = 0;
	uint32_t componentTypeId = 0;
	uint32_t specId = 0;

	//--------------Integer-----------------------------
	typeDescription.PrimaryType = spv::OpTypeVector;
	typeDescription.SecondaryType = spv::OpTypeInt;
	typeDescription.ComponentCount = 4;
	typeId = GetSpirVTypeId(typeDescription);

	componentTypeDescription.PrimaryType = spv::OpTypeInt;
	componentTypeDescription.SecondaryType = spv::OpTypeVoid;
	componentTypeDescription.ComponentCount = 0; //default is 0 so using 1 will mess up compare.
	componentTypeId = GetSpirVTypeId(componentTypeDescription);

	for (size_t i = 0; i < 16; i++)
	{
		uint32_t id;
		uint32_t ids[4];

		id = GetNextId();
		for (size_t j = 0; j < 4; j++)
		{
			ids[j] = GetNextId();
			mTypeInstructions.push_back(Pack(3 + 1, spv::OpSpecConstant)); //size,Type
			mTypeInstructions.push_back(componentTypeId); //Result Type (Id)
			mTypeInstructions.push_back(ids[j]); //Result (Id)
			mTypeInstructions.push_back(0); //Literal Value

			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(ids[j]); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationSpecId); //Decoration Type (Id)
			mDecorateInstructions.push_back(specId++);
		}

		mTypeInstructions.push_back(Pack(3 + 4, spv::OpSpecConstantComposite)); //size,Type
		mTypeInstructions.push_back(typeId); //Result Type (Id)
		mTypeInstructions.push_back(id); //Result (Id)
		for (size_t j = 0; j < 4; j++)
		{
			mTypeInstructions.push_back(ids[j]); //Constituents
		}

		std::string registerName = "i" + std::to_string(i);
		PushName(id, registerName);

		mIdTypePairs[id] = componentTypeDescription;

		mIdsByRegister[D3DSPR_CONSTINT][i] = id;
		mRegistersById[D3DSPR_CONSTINT][id] = i;
	}

	//---------------Boolean------------------------------------
	typeDescription.PrimaryType = spv::OpTypeBool;
	typeDescription.SecondaryType = spv::OpTypeVoid;
	typeDescription.ComponentCount = 0; //default is 0 so using 1 will mess up compare.
	typeId = GetSpirVTypeId(typeDescription);

	for (size_t i = 0; i < 16; i++)
	{
		uint32_t id;

		id = GetNextId();

		mTypeInstructions.push_back(Pack(3 + 1, spv::OpSpecConstant)); //size,Type
		//This is setup as a int because Spir-V doesn't like Bool for some reason.
		mTypeInstructions.push_back(componentTypeId); //Result Type (Id) 
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(0); //Literal Value

		std::string registerName = "b" + std::to_string(i);
		PushName(id, registerName);

		mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
		mDecorateInstructions.push_back(id); //target (Id)
		mDecorateInstructions.push_back(spv::DecorationSpecId); //Decoration Type (Id)
		mDecorateInstructions.push_back(specId++);

		mIdTypePairs[id] = componentTypeDescription;

		mIdsByRegister[D3DSPR_CONSTBOOL][i] = id;
		mRegistersById[D3DSPR_CONSTBOOL][id] = i;
	}

	//--------------Float-----------------------------
	typeDescription.PrimaryType = spv::OpTypeVector;
	typeDescription.SecondaryType = spv::OpTypeFloat;
	
	typeDescription.ComponentCount = 3;
	TypeDescription vector3Type = typeDescription;
	uint32_t vector3TypeId = GetSpirVTypeId(vector3Type);
	
	typeDescription.ComponentCount = 4;
	TypeDescription vector4Type = typeDescription;
	uint32_t vector4TypeId = GetSpirVTypeId(vector4Type);

	componentTypeDescription.PrimaryType = spv::OpTypeFloat;
	componentTypeDescription.SecondaryType = spv::OpTypeVoid;
	componentTypeDescription.ComponentCount = 0; //default is 0 so using 1 will mess up compare.
	componentTypeId = GetSpirVTypeId(componentTypeDescription);

	for (size_t i = 0; i < 256; i++)
	{
		uint32_t id;
		uint32_t ids[4];
		std::string registerName;

		for (size_t j = 0; j < 4; j++)
		{
			ids[j] = GetNextId();
			mTypeInstructions.push_back(Pack(3 + 1, spv::OpSpecConstant)); //size,Type
			mTypeInstructions.push_back(componentTypeId); //Result Type (Id)
			mTypeInstructions.push_back(ids[j]); //Result (Id)
			//mTypeInstructions.push_back(bit_cast(1.0f)); //Literal Value
			mTypeInstructions.push_back(bit_cast(0.0f)); //Literal Value

			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(ids[j]); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationSpecId); //Decoration Type (Id)
			mDecorateInstructions.push_back(specId++);
		}

		//vec3
		uint32_t vector3Id = GetNextId();
		mTypeInstructions.push_back(Pack(3 + 3, spv::OpSpecConstantComposite)); //size,Type
		mTypeInstructions.push_back(vector3TypeId); //Result Type (Id)
		mTypeInstructions.push_back(vector3Id); //Result (Id)
		for (size_t j = 0; j < 3; j++)
		{
			mTypeInstructions.push_back(ids[j]); //Constituents
		}

		registerName = "c_vec3_" + std::to_string(i);
		PushName(vector3Id, registerName);

		//vec4
		uint32_t vector4Id = GetNextId();	
		mTypeInstructions.push_back(Pack(3 + 4, spv::OpSpecConstantComposite)); //size,Type
		mTypeInstructions.push_back(vector4TypeId); //Result Type (Id)
		mTypeInstructions.push_back(vector4Id); //Result (Id)
		for (size_t j = 0; j < 4; j++)
		{
			mTypeInstructions.push_back(ids[j]); //Constituents
		}

		registerName = "c_vec4_" + std::to_string(i);
		PushName(vector4Id, registerName);

		id = vector4Id;

		mIdTypePairs[vector3Id] = vector3Type;
		mIdTypePairs[vector4Id] = vector4Type;

		mIdsByRegister[D3DSPR_CONST][i] = id;
		mRegistersById[D3DSPR_CONST][id] = i;

		mVector4Vector3Pairs[vector4Id] = vector3Id;
	}

	typeDescription.PrimaryType = spv::OpTypeMatrix;
	typeDescription.SecondaryType = spv::OpTypeVector;
	typeDescription.TernaryType = spv::OpTypeFloat;

	//mat3
	typeDescription.ComponentCount = 3;
	TypeDescription matrix3Type = typeDescription;
	uint32_t matrix3TypeId = GetSpirVTypeId(matrix3Type);

	//mat4
	typeDescription.ComponentCount = 4;
	TypeDescription matrix4Type = typeDescription;
	uint32_t matrix4TypeId = GetSpirVTypeId(matrix4Type);

	for (size_t i = 0; i < 64; i++)
	{
		std::string registerName;

		uint32_t matrix3Id = GetNextId();
		uint32_t matrix4Id = GetNextId();
		uint32_t id = matrix4Id;

		//mat3
		mTypeInstructions.push_back(Pack(3 + 3, spv::OpSpecConstantComposite)); //size,Type
		mTypeInstructions.push_back(matrix3TypeId); //Result Type (Id)
		mTypeInstructions.push_back(matrix3Id); //Result (Id)
		for (size_t j = 0; j < 3; j++)
		{
			//keep * 4 because vectors are still laid out the same.
			//vec3 is one id below vec4
			mTypeInstructions.push_back(mIdsByRegister[D3DSPR_CONST][i * 4 + j]-1); //Constituents
		}

		registerName = "c_mat3_" + std::to_string(i);
		PushName(matrix3Id, registerName);

		//mat4
		mTypeInstructions.push_back(Pack(3 + 4, spv::OpSpecConstantComposite)); //size,Type
		mTypeInstructions.push_back(matrix4TypeId); //Result Type (Id)
		mTypeInstructions.push_back(matrix4Id); //Result (Id)
		for (size_t j = 0; j < 4; j++)
		{
			mTypeInstructions.push_back(mIdsByRegister[D3DSPR_CONST][i * 4 + j]); //Constituents
		}

		registerName = "c_mat4_" + std::to_string(i);
		PushName(matrix4Id, registerName);

		mVector4Matrix3X3Pairs[mIdsByRegister[D3DSPR_CONST][i * 4]] = matrix3Id;
		mVector4Matrix4X4Pairs[mIdsByRegister[D3DSPR_CONST][i * 4]] = matrix4Id;

		mIdTypePairs[matrix3Id] = matrix3Type;
		mIdTypePairs[matrix4Id] = matrix4Type;

		mIdsByRegister[(_D3DSHADER_PARAM_REGISTER_TYPE)1337][i * 4] = id;
		mRegistersById[(_D3DSHADER_PARAM_REGISTER_TYPE)1337][id] = i * 4;
	}
}

void ShaderConverter::CombineSpirVOpCodes()
{
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
}

void ShaderConverter::CreateSpirVModule()
{
#ifdef _EXTRA_SHADER_DEBUG_INFO
	if (!mIsVertexShader)
	{
		std::ofstream outFile("fragment_" + std::to_string((uint32_t)mInstructions.data()) + ".spv", std::ios::out | std::ios::binary);
		outFile.write((char*)mInstructions.data(), mInstructions.size() * sizeof(uint32_t));
	}
	else
	{
		std::ofstream outFile("vertex_" + std::to_string((uint32_t)mInstructions.data()) + ".spv", std::ios::out | std::ios::binary);
		outFile.write((char*)mInstructions.data(), mInstructions.size() * sizeof(uint32_t));
	}
#endif

	vk::Result result;
	vk::ShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.codeSize = mInstructions.size() * sizeof(uint32_t);
	moduleCreateInfo.pCode = mInstructions.data(); //Why is this uint32_t* if the size is in bytes?
	//moduleCreateInfo.flags = 0;
	result = mDevice.createShaderModule(&moduleCreateInfo, nullptr, &mConvertedShader.ShaderModule);

	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::CreateSpirVModule vkCreateShaderModule failed with return code of " << GetResultString((VkResult)result);
		return;
	}
}

/*
To match GLSL behavior we will extract the components and construct a new composite from the pieces.
*/

uint32_t ShaderConverter::ConvertVec4ToVec3(uint32_t id)
{
	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	TypeDescription vectorType;
	vectorType.PrimaryType = spv::OpTypeVector;
	vectorType.SecondaryType = spv::OpTypeFloat;
	vectorType.ComponentCount = 3;
	uint32_t vectorTypeId = GetSpirVTypeId(vectorType);

	//
	uint32_t xId = GetNextId();
	mIdTypePairs[xId] = floatType;
	PushCompositeExtract(floatTypeId, xId, id, 0);

	uint32_t yId = GetNextId();
	mIdTypePairs[yId] = floatType;
	PushCompositeExtract(floatTypeId, yId, id, 1);

	uint32_t zId = GetNextId();
	mIdTypePairs[zId] = floatType;
	PushCompositeExtract(floatTypeId, zId, id, 1);



	uint32_t resultId = GetNextId();
	mIdTypePairs[resultId] = vectorType;
	Push(spv::OpCompositeConstruct, vectorTypeId, resultId, xId, yId, zId);

	return resultId;
}

uint32_t ShaderConverter::ConvertMat4ToMat3(uint32_t id)
{
	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	TypeDescription vectorType;
	vectorType.PrimaryType = spv::OpTypeVector;
	vectorType.SecondaryType = spv::OpTypeFloat;
	vectorType.ComponentCount = 3;
	uint32_t vectorTypeId = GetSpirVTypeId(vectorType);

	TypeDescription matrixType;
	matrixType.PrimaryType = spv::OpTypeMatrix;
	matrixType.SecondaryType = spv::OpTypeVector;
	matrixType.ComponentCount = 3;
	uint32_t matrixTypeId = GetSpirVTypeId(matrixType);

	//
	uint32_t x1Id = GetNextId();
	mIdTypePairs[x1Id] = floatType;
	PushCompositeExtract(floatTypeId, x1Id, id, 0,0);

	uint32_t y1Id = GetNextId();
	mIdTypePairs[y1Id] = floatType;
	PushCompositeExtract(floatTypeId, y1Id, id, 0,1);

	uint32_t z1Id = GetNextId();
	mIdTypePairs[z1Id] = floatType;
	PushCompositeExtract(floatTypeId, z1Id, id, 0,2);

	uint32_t v1Id = GetNextId();
	mIdTypePairs[v1Id] = vectorType;
	Push(spv::OpCompositeConstruct, vectorTypeId, v1Id, x1Id, y1Id, z1Id);

	//
	uint32_t x2Id = GetNextId();
	mIdTypePairs[x2Id] = floatType;
	PushCompositeExtract(floatTypeId, x2Id, id, 1, 0);

	uint32_t y2Id = GetNextId();
	mIdTypePairs[y2Id] = floatType;
	PushCompositeExtract(floatTypeId, y2Id, id, 1, 1);

	uint32_t z2Id = GetNextId();
	mIdTypePairs[z2Id] = floatType;
	PushCompositeExtract(floatTypeId, z2Id, id, 1, 2);

	uint32_t v2Id = GetNextId();
	mIdTypePairs[v2Id] = vectorType;
	Push(spv::OpCompositeConstruct, vectorTypeId, v2Id, x2Id, y2Id, z2Id);

	//
	uint32_t x3Id = GetNextId();
	mIdTypePairs[x3Id] = floatType;
	PushCompositeExtract(floatTypeId, x3Id, id, 2, 0);

	uint32_t y3Id = GetNextId();
	mIdTypePairs[y3Id] = floatType;
	PushCompositeExtract(floatTypeId, y3Id, id, 2, 1);

	uint32_t z3Id = GetNextId();
	mIdTypePairs[z3Id] = floatType;
	PushCompositeExtract(floatTypeId, z3Id, id, 2, 2);

	uint32_t v3Id = GetNextId();
	mIdTypePairs[v3Id] = vectorType;
	Push(spv::OpCompositeConstruct, vectorTypeId, v3Id, x3Id, y3Id, z3Id);



	uint32_t resultId = GetNextId();
	mIdTypePairs[resultId] = matrixType;
	Push(spv::OpCompositeConstruct, matrixTypeId, resultId, v1Id, v2Id, v3Id);

	return resultId;
}

void ShaderConverter::PushMemberName(uint32_t id, std::string& registerName, uint32_t index)
{
	std::vector<uint32_t> nameInstructions;
	PutStringInVector(registerName, nameInstructions); //Literal

	mNameInstructions.push_back(Pack(nameInstructions.size() + 3, spv::OpMemberName));
	mNameInstructions.push_back(id); //target (Id)
	mNameInstructions.push_back(index);
	mNameInstructions.insert(mNameInstructions.end(), nameInstructions.begin(), nameInstructions.end());

	mNameIdPairs[id] = registerName;
}

void ShaderConverter::PushName(uint32_t id, std::string& registerName)
{
	std::vector<uint32_t> nameInstructions;
	PutStringInVector(registerName, nameInstructions); //Literal

	mNameInstructions.push_back(Pack(nameInstructions.size() + 2, spv::OpName));
	mNameInstructions.push_back(id); //target (Id)
	mNameInstructions.insert(mNameInstructions.end(), nameInstructions.begin(), nameInstructions.end());	

	mNameIdPairs[id] = registerName;
}

void ShaderConverter::PushCompositeExtract(uint32_t resultTypeId, uint32_t resultId, uint32_t baseId, uint32_t index)
{
	std::string registerName = mNameIdPairs[baseId];
	if (registerName.size())
	{
		if (index == 0)
		{
			registerName += ".x";
		}
		else if (index == 1)
		{
			registerName += ".y";
		}
		else if (index == 2)
		{
			registerName += ".z";
		}
		else if (index == 3)
		{
			registerName += ".w";
		}

		PushName(resultId, registerName);
	}

	Push(spv::OpCompositeExtract, resultTypeId, resultId, baseId, index);
}

void ShaderConverter::PushCompositeExtract(uint32_t resultTypeId, uint32_t resultId, uint32_t baseId, uint32_t index1, uint32_t index2)
{
	std::string registerName = mNameIdPairs[baseId];
	if (registerName.size())
	{
		registerName += "[" + std::to_string(index1) + "][" + std::to_string(index2) + "]";

		PushName(resultId, registerName);
	}

	Push(spv::OpCompositeExtract, resultTypeId, resultId, baseId, index1, index2);
}

void ShaderConverter::PushAccessChain(uint32_t resultTypeId, uint32_t resultId, uint32_t baseId, uint32_t indexId)
{
	std::string registerName = mNameIdPairs[baseId];
	if (registerName.size())
	{
		if (baseId == mColor1Id || baseId == mColor2Id)
		{
			if (indexId == m0Id)
			{
				registerName += ".r";
			}
			else if (indexId == m1Id)
			{
				registerName += ".g";
			}
			else if (indexId == m2Id)
			{
				registerName += ".b";
			}
			else if (indexId == m3Id)
			{
				registerName += ".a";
			}
		}
		else
		{
			if (indexId == m0Id)
			{
				registerName += "[0]";
			}
			else if (indexId == m1Id)
			{
				registerName += "[1]";
			}
			else if (indexId == m2Id)
			{
				registerName += "[2]";
			}
			else if (indexId == m3Id)
			{
				registerName += "[3]";
			}
		}
		
		PushName(resultId, registerName);
	}

	Push(spv::OpAccessChain, resultTypeId, resultId, baseId, indexId);
}

void ShaderConverter::PushInverseSqrt(uint32_t resultTypeId, uint32_t resultId, uint32_t argumentId)
{
#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType = mIdTypePairs[argumentId];
	auto& resultType = mIdTypePairs[resultTypeId];

	if (resultType.PrimaryType != argumentType.PrimaryType)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushInverseSqrt PrimaryType mismatch " << resultType.PrimaryType << " != " << argumentType.PrimaryType;
	}

	if (resultType.SecondaryType != argumentType.SecondaryType)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushInverseSqrt SecondaryType mismatch " << resultType.SecondaryType << " != " << argumentType.SecondaryType;
	}

	if (resultType.TernaryType != argumentType.TernaryType)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushInverseSqrt TernaryType mismatch " << resultType.TernaryType << " != " << argumentType.TernaryType;
	}

	if (resultType.ComponentCount != argumentType.ComponentCount)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushInverseSqrt ComponentCount mismatch " << resultType.ComponentCount << " != " << argumentType.ComponentCount;
	}
#endif

	Push(spv::OpExtInst, resultTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450InverseSqrt, argumentId);
}

void ShaderConverter::PushLoad(uint32_t resultTypeId, uint32_t resultId, uint32_t pointerId)
{
#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& resultType = mIdTypePairs[resultTypeId];
	auto& pointerType = mIdTypePairs[pointerId];

	if (pointerType.PrimaryType != spv::OpTypePointer)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushLoad PrimaryType is not OpPointer " << pointerType.PrimaryType;
	}

	if (pointerType.SecondaryType != resultType.PrimaryType)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushLoad PrimaryType mismatch " << pointerType.SecondaryType << " != " << resultType.PrimaryType;
	}

	if (pointerType.TernaryType != resultType.SecondaryType)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushLoad SecondaryType mismatch " << pointerType.TernaryType << " != " << resultType.SecondaryType;
	}

	if (pointerType.ComponentCount != resultType.ComponentCount)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushLoad ComponentCount mismatch " << pointerType.ComponentCount << " != " << resultType.ComponentCount;
	}
#endif

	std::string registerName = mNameIdPairs[pointerId];
	if (registerName.size())
	{
		registerName += "_loaded";
		PushName(resultId, registerName);
	}

	Push(spv::OpLoad, resultTypeId, resultId, pointerId);
}

void ShaderConverter::PushStore(uint32_t pointerId, uint32_t objectId)
{
#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& objectType = mIdTypePairs[objectId];
	auto& pointerType = mIdTypePairs[pointerId];

	if (pointerType.PrimaryType != spv::OpTypePointer)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushStore PrimaryType is not OpPointer " << pointerType.PrimaryType;
	}

	if (pointerType.SecondaryType != objectType.PrimaryType)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushStore PrimaryType mismatch " << pointerType.SecondaryType << " != " << objectType.PrimaryType;
	}

	if (pointerType.TernaryType != objectType.SecondaryType)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushStore SecondaryType mismatch " << pointerType.TernaryType << " != " << objectType.SecondaryType;
	}

	if (pointerType.ComponentCount != objectType.ComponentCount)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::PushStore ComponentCount mismatch " << pointerType.ComponentCount << " != " << objectType.ComponentCount;
	}
#endif

	Push(spv::OpStore, pointerId, objectId);
}

void ShaderConverter::Push(spv::Op code)
{
	mFunctionDefinitionInstructions.push_back(Pack(1, code)); //size,Type

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code;
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1)
{
	mFunctionDefinitionInstructions.push_back(Pack(2, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2)
{
	mFunctionDefinitionInstructions.push_back(Pack(3, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")";
#endif	
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3)
{
	mFunctionDefinitionInstructions.push_back(Pack(4, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")";
#endif	
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4)
{
	mFunctionDefinitionInstructions.push_back(Pack(5, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5)
{
	mFunctionDefinitionInstructions.push_back(Pack(6, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, GLSLstd450 argument4, uint32_t argument5)
{
	mFunctionDefinitionInstructions.push_back(Pack(6, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6)
{
	mFunctionDefinitionInstructions.push_back(Pack(7, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);
	mFunctionDefinitionInstructions.push_back(argument6);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];
	auto& argumentType6 = mIdTypePairs[argument6];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")" << ", " << argument6 << "(" << argumentType6 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, GLSLstd450 argument4, uint32_t argument5, uint32_t argument6)
{
	mFunctionDefinitionInstructions.push_back(Pack(7, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);
	mFunctionDefinitionInstructions.push_back(argument6);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];
	auto& argumentType6 = mIdTypePairs[argument6];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")" << ", " << argument6 << "(" << argumentType6 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7)
{
	mFunctionDefinitionInstructions.push_back(Pack(8, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);
	mFunctionDefinitionInstructions.push_back(argument6);
	mFunctionDefinitionInstructions.push_back(argument7);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];
	auto& argumentType6 = mIdTypePairs[argument6];
	auto& argumentType7 = mIdTypePairs[argument7];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")" << ", " << argument6 << "(" << argumentType6 << ")" << ", " << argument7 << "(" << argumentType7 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8)
{
	mFunctionDefinitionInstructions.push_back(Pack(9, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);
	mFunctionDefinitionInstructions.push_back(argument6);
	mFunctionDefinitionInstructions.push_back(argument7);
	mFunctionDefinitionInstructions.push_back(argument8);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];
	auto& argumentType6 = mIdTypePairs[argument6];
	auto& argumentType7 = mIdTypePairs[argument7];
	auto& argumentType8 = mIdTypePairs[argument8];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")" << ", " << argument6 << "(" << argumentType6 << ")" << ", " << argument7 << "(" << argumentType7 << ")" << ", " << argument8 << "(" << argumentType8 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8, uint32_t argument9)
{
	mFunctionDefinitionInstructions.push_back(Pack(10, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);
	mFunctionDefinitionInstructions.push_back(argument6);
	mFunctionDefinitionInstructions.push_back(argument7);
	mFunctionDefinitionInstructions.push_back(argument8);
	mFunctionDefinitionInstructions.push_back(argument9);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];
	auto& argumentType6 = mIdTypePairs[argument6];
	auto& argumentType7 = mIdTypePairs[argument7];
	auto& argumentType8 = mIdTypePairs[argument8];
	auto& argumentType9 = mIdTypePairs[argument9];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")" << ", " << argument6 << "(" << argumentType6 << ")" << ", " << argument7 << "(" << argumentType7 << ")" << ", " << argument8 << "(" << argumentType8 << ")" << ", " << argument9 << "(" << argumentType9 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8, uint32_t argument9, uint32_t argument10)
{
	mFunctionDefinitionInstructions.push_back(Pack(11, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);
	mFunctionDefinitionInstructions.push_back(argument6);
	mFunctionDefinitionInstructions.push_back(argument7);
	mFunctionDefinitionInstructions.push_back(argument8);
	mFunctionDefinitionInstructions.push_back(argument9);
	mFunctionDefinitionInstructions.push_back(argument10);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];
	auto& argumentType6 = mIdTypePairs[argument6];
	auto& argumentType7 = mIdTypePairs[argument7];
	auto& argumentType8 = mIdTypePairs[argument8];
	auto& argumentType9 = mIdTypePairs[argument9];
	auto& argumentType10 = mIdTypePairs[argument10];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")" << ", " << argument6 << "(" << argumentType6 << ")" << ", " << argument7 << "(" << argumentType7 << ")" << ", " << argument8 << "(" << argumentType8 << ")" << ", " << argument9 << "(" << argumentType9 << ")" << ", " << argument10 << "(" << argumentType10 << ")";
#endif
}

void ShaderConverter::Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8, uint32_t argument9, uint32_t argument10, uint32_t argument11)
{
	mFunctionDefinitionInstructions.push_back(Pack(12, code)); //size,Type
	mFunctionDefinitionInstructions.push_back(argument1);
	mFunctionDefinitionInstructions.push_back(argument2);
	mFunctionDefinitionInstructions.push_back(argument3);
	mFunctionDefinitionInstructions.push_back(argument4);
	mFunctionDefinitionInstructions.push_back(argument5);
	mFunctionDefinitionInstructions.push_back(argument6);
	mFunctionDefinitionInstructions.push_back(argument7);
	mFunctionDefinitionInstructions.push_back(argument8);
	mFunctionDefinitionInstructions.push_back(argument9);
	mFunctionDefinitionInstructions.push_back(argument10);
	mFunctionDefinitionInstructions.push_back(argument11);

#ifdef _EXTRA_SHADER_DEBUG_INFO
	auto& argumentType1 = mIdTypePairs[argument1];
	auto& argumentType2 = mIdTypePairs[argument2];
	auto& argumentType3 = mIdTypePairs[argument3];
	auto& argumentType4 = mIdTypePairs[argument4];
	auto& argumentType5 = mIdTypePairs[argument5];
	auto& argumentType6 = mIdTypePairs[argument6];
	auto& argumentType7 = mIdTypePairs[argument7];
	auto& argumentType8 = mIdTypePairs[argument8];
	auto& argumentType9 = mIdTypePairs[argument9];
	auto& argumentType10 = mIdTypePairs[argument10];
	auto& argumentType11 = mIdTypePairs[argument11];

	BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Push " << code << " " << argument1 << "(" << argumentType1 << ")" << ", " << argument2 << "(" << argumentType2 << ")" << ", " << argument3 << "(" << argumentType3 << ")" << ", " << argument4 << "(" << argumentType4 << ")" << ", " << argument5 << "(" << argumentType5 << ")" << ", " << argument6 << "(" << argumentType6 << ")" << ", " << argument7 << "(" << argumentType7 << ")" << ", " << argument8 << "(" << argumentType8 << ")" << ", " << argument9 << "(" << argumentType9 << ")" << ", " << argument10 << "(" << argumentType10 << ")" << ", " << argument11 << "(" << argumentType11 << ")";
#endif
}

void ShaderConverter::Process_DCL_Pixel()
{
	Token token = GetNextToken();
	Token registerToken = GetNextToken();
	uint32_t usage = GetUsage(token.i);
	uint32_t usageIndex = GetUsageIndex(token.i);
	uint32_t registerNumber = GetRegisterNumber(registerToken);
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(registerToken.i);
	uint32_t tokenId = GetNextVersionId(registerToken);
	TypeDescription typeDescription;
	uint32_t registerComponents = (registerToken.i & D3DSP_WRITEMASK_ALL) >> 16;
	uint32_t resultTypeId;
	uint32_t textureType;
	std::string registerName;

	typeDescription.PrimaryType = spv::OpTypePointer;
	typeDescription.SecondaryType = spv::OpTypeVector;
	typeDescription.TernaryType = spv::OpTypeFloat;
	typeDescription.StorageClass = spv::StorageClassInput;

	//Magic numbers from ToGL (no whole numbers?)
	switch (registerComponents)
	{
	case 1: //float
		typeDescription.SecondaryType = spv::OpTypeFloat;
		typeDescription.TernaryType = spv::OpTypeVoid;
		typeDescription.ComponentCount = 1;
		break;
	case 3: //vec2
		typeDescription.ComponentCount = 2;
		break;
	case 7: //vec3
		//This is really a vec3 but I'm going to declare it has a vec4 to make later code easier.
		//typeDescription.ComponentCount = 3;
		typeDescription.ComponentCount = 4;
		break;
	case 0xF: //vec4
		typeDescription.ComponentCount = 4;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_DCL - Unsupported component type " << registerComponents;
		break;
	}

	mIdTypePairs[tokenId] = typeDescription;

	switch (registerType)
	{
	case D3DSPR_INPUT:
		typeDescription.StorageClass = spv::StorageClassInput;
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class
		//Optional initializer

		mInputRegisters.push_back(tokenId);

		registerName = "v" + std::to_string(registerNumber);
		PushName(tokenId, registerName);

		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, true);
		break;
	case D3DSPR_TEXTURE:
		typeDescription.StorageClass = spv::StorageClassInput;
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class
															 //Optional initializer

		mInputRegisters.push_back(tokenId);

		registerName = "T" + std::to_string(registerNumber);
		PushName(tokenId, registerName);

		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, true);
		break;
	case D3DSPR_SAMPLER:
		textureType = GetTextureType(token.i);

		//switch (textureType)
		//{
		//case D3DSTT_2D:
		//	break;
		//case D3DSTT_CUBE:
		//	break;
		//case D3DSTT_VOLUME:
		//	break;
		//case D3DSTT_UNKNOWN:
		//	break;
		//default:
		//	break;
		//}	

		resultTypeId = GetSpirVTypeId(spv::OpTypePointer, spv::OpTypeImage);
		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassUniformConstant); //Storage Class
		//Optional initializer

		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].binding = registerNumber; //mConvertedShader.mDescriptorSetLayoutBindingCount;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorCount = 1;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].stageFlags = vk::ShaderStageFlagBits::eFragment;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].pImmutableSamplers = nullptr;

		mConvertedShader.mDescriptorSetLayoutBindingCount++;


		/*
		resultTypeId = GetSpirVTypeId(spv::OpTypePointer, spv::OpTypeSampler);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassUniform);
		//Optional initializer
		*/

		break;
	case D3DSPR_TEMP:
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassPrivate); //Storage Class
		//Optional initializer
		break;
	default:
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::Process_DCL_Pixel unsupported register type " << registerType;
		break;
	}

	BOOST_LOG_TRIVIAL(info) << "DCL " << GetUsageName((_D3DDECLUSAGE)usage) << " - " << registerNumber << "(" << GetRegisterTypeName(registerType) << ") ";
}

void ShaderConverter::Process_DCL_Vertex()
{
	Token token = GetNextToken();
	Token registerToken = GetNextToken();
	uint32_t usage = GetUsage(token.i);
	uint32_t usageIndex = GetUsageIndex(token.i);
	uint32_t registerNumber = GetRegisterNumber(registerToken);
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(registerToken.i);
	uint32_t tokenId = GetNextVersionId(registerToken);
	TypeDescription typeDescription;
	uint32_t registerComponents = (registerToken.i & D3DSP_WRITEMASK_ALL) >> 16;
	uint32_t resultTypeId;
	std::string registerName;
	uint32_t stringWordSize;

	typeDescription.PrimaryType = spv::OpTypePointer;
	typeDescription.SecondaryType = spv::OpTypeVector;
	if (usage == D3DDECLUSAGE_COLOR)
	{
		typeDescription.TernaryType = spv::OpTypeInt;
	}
	else
	{
		typeDescription.TernaryType = spv::OpTypeFloat;
	}

	//Magic numbers from ToGL (no whole numbers?)
	switch (registerComponents)
	{
	case 1: //float
		if (usage == D3DDECLUSAGE_COLOR)
		{
			typeDescription.SecondaryType = spv::OpTypeInt;
		}
		else
		{
			typeDescription.SecondaryType = spv::OpTypeFloat;
		}

		typeDescription.TernaryType = spv::OpTypeVoid;
		typeDescription.ComponentCount = 1;
		break;
	case 3: //vec2
		typeDescription.ComponentCount = 2;
		break;
	case 7: //vec3
			//This is really a vec3 but I'm going to declare it has a vec4 to make later code easier.
			//typeDescription.ComponentCount = 3;
		typeDescription.ComponentCount = 4;
		break;
	case 0xF: //vec4
		typeDescription.ComponentCount = 4;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_DCL - Unsupported component type " << registerComponents;
		break;
	}

	mIdTypePairs[tokenId] = typeDescription;

	switch (registerType)
	{
	case D3DSPR_INPUT:
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class
															 //Optional initializer

		mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].binding = 0;//element.Stream; TODO:REVISIT
		mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].location = mConvertedShader.mVertexInputAttributeDescriptionCount;
		mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].offset = 0;//element.Offset;

		switch (typeDescription.ComponentCount)
		{
		case 1: // 1D float expanded to (value, 0., 0., 1.)
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32Sfloat;
			break;
		case 2:  // 2D float expanded to (value, value, 0., 1.)
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32Sfloat;
			break;
		case 3: // 3D float expanded to (value, value, value, 1.)
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32B32Sfloat;
			break;
		case 4:  // 4D float
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32B32A32Sfloat;
		default:
			break;
		}

		mConvertedShader.mVertexInputAttributeDescriptionCount++;

		mInputRegisters.push_back(tokenId);

		registerName = "v" + std::to_string(registerNumber);
		PushName(tokenId, registerName);

		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, true);
		break;
	case D3DSPR_RASTOUT:
	case D3DSPR_ATTROUT:
	case D3DSPR_COLOROUT:
	case D3DSPR_DEPTHOUT:
	case D3DSPR_OUTPUT:
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
		//Optional initializer

		switch (usage)
		{
		case D3DDECLUSAGE_POSITION:
			mPositionRegister = usageIndex; //might need this later.
			mPositionId = tokenId;
			registerName = "oPos" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_FOG:
			registerName = "oFog" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_PSIZE:
			registerName = "oPts" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_COLOR:

			if (registerNumber)
			{
				mColor2Id = tokenId;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor2XId = GetNextId();
				mIdTypePairs[mColor2XId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2XId, mColor2Id, m0Id);

				mColor2YId = GetNextId();
				mIdTypePairs[mColor2YId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2YId, mColor2Id, m1Id);

				mColor2ZId = GetNextId();
				mIdTypePairs[mColor2ZId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2ZId, mColor2Id, m2Id);

				mColor2WId = GetNextId();
				mIdTypePairs[mColor2WId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor2WId, mColor2Id, m3Id);
			}
			else
			{
				mColor1Id = tokenId;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor1XId = GetNextId();
				mIdTypePairs[mColor1XId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1XId, mColor1Id, m0Id);

				mColor1YId = GetNextId();
				mIdTypePairs[mColor1YId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1YId, mColor1Id, m1Id);

				mColor1ZId = GetNextId();
				mIdTypePairs[mColor1ZId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1ZId, mColor1Id, m2Id);

				mColor1WId = GetNextId();
				mIdTypePairs[mColor1WId] = pointerFloatType;
				PushAccessChain(floatTypeId, mColor1WId, mColor1Id, m3Id);
			}

			registerName = "oD" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_TEXCOORD:
			registerName = "oT" + std::to_string(registerNumber);
			break;
		default:
			registerName = "o" + std::to_string(registerNumber);
			break;
		}

		stringWordSize = 2 + std::max(registerName.length() / 4, (size_t)1);
		PushName(tokenId, registerName);

		mOutputRegisters.push_back(tokenId);
		mOutputRegisterUsages[(_D3DDECLUSAGE)usage] = tokenId;
		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, false);
		break;
	case D3DSPR_TEMP:
		typeDescription.StorageClass = spv::StorageClassPrivate;
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassPrivate); //Storage Class
		//Optional initializer
		break;
	default:
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::Process_DCL_Vertex unsupported register type " << registerType;
		break;
	}

	BOOST_LOG_TRIVIAL(info) << "DCL " << GetUsageName((_D3DDECLUSAGE)usage) << " - " << registerNumber << "(" << GetRegisterTypeName(registerType) << ") ";
}

void ShaderConverter::Process_DCL()
{
	if (mIsVertexShader)
	{
		if (mMajorVersion >= 3)
		{
			Process_DCL_Vertex();
		}
		else if (mMajorVersion >= 2)
		{
			Process_DCL_Vertex();
		}
		else
		{
			Process_DCL_Vertex();
			//BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::Process_DCL unsupported shader version " << mMajorVersion;
		}
	}
	else
	{
		if (mMajorVersion >= 3)
		{
			Process_DCL_Pixel();
		}
		else if (mMajorVersion >= 2)
		{
			Process_DCL_Pixel();
		}
		else
		{
			Process_DCL_Pixel();
			//BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::Process_DCL unsupported shader version " << mMajorVersion;
		}
	}
}

void ShaderConverter::Process_DEF()
{
	DWORD literalValue;
	Token token = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	DestinationParameterToken  destinationParameterToken = token.DestinationParameterToken;

	for (size_t i = 0; i < 4; i++)
	{
		literalValue = GetNextToken().i;
		mShaderConstantSlots.FloatConstants[token.DestinationParameterToken.RegisterNumber * 4 + i] = bit_cast(literalValue);
	}

	PrintTokenInformation("DEF", token, token, token);
}

void ShaderConverter::Process_DEFI()
{
	DWORD literalValue;
	Token token = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	DestinationParameterToken  destinationParameterToken = token.DestinationParameterToken;

	for (size_t i = 0; i < 4; i++)
	{
		literalValue = GetNextToken().i;
		mShaderConstantSlots.IntegerConstants[token.DestinationParameterToken.RegisterNumber * 4 + i] = literalValue;
	}

	PrintTokenInformation("DEFI", token, token, token);
}

void ShaderConverter::Process_DEFB()
{
	DWORD literalValue;
	Token token = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	DestinationParameterToken  destinationParameterToken = token.DestinationParameterToken;

	literalValue = GetNextToken().i;
	mShaderConstantSlots.BooleanConstants[token.DestinationParameterToken.RegisterNumber * 4] = literalValue;

	PrintTokenInformation("DEFB", token, token, token);
}

void ShaderConverter::Process_IFC(uint32_t extraInfo)
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;
	uint32_t trueLabelId;
	uint32_t falseLabelId;
	uint32_t endIfLabelId;

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);
	argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4); //, UINT_MAX, D3DSPR_INPUT
	resultId = GetNextId();

	mIdTypePairs[resultId] = typeDescription;

	switch (extraInfo)
	{
	case 1:
		switch (dataType)
		{
		case spv::OpTypeBool:
			Push(spv::OpUGreaterThan, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		case spv::OpTypeInt:
			Push(spv::OpUGreaterThan, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported data type " << dataType;
			break;
		}
		break;
	case 2:
		switch (dataType)
		{
		case spv::OpTypeBool:
			Push(spv::OpIEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		case spv::OpTypeInt:
			Push(spv::OpIEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported data type " << dataType;
			break;
		}
		break;
	case 3:
		switch (dataType)
		{
		case spv::OpTypeBool:
			Push(spv::OpUGreaterThanEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		case spv::OpTypeInt:
			Push(spv::OpUGreaterThanEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported data type " << dataType;
			break;
		}
		break;
	case 4:
		switch (dataType)
		{
		case spv::OpTypeBool:
			Push(spv::OpULessThan, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		case spv::OpTypeInt:
			Push(spv::OpULessThan, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported data type " << dataType;
			break;
		}
		break;
	case 5:
		switch (dataType)
		{
		case spv::OpTypeBool:
			Push(spv::OpINotEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		case spv::OpTypeInt:
			Push(spv::OpINotEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported data type " << dataType;
			break;
		}
		break;
	case 6:
		switch (dataType)
		{
		case spv::OpTypeBool:
			Push(spv::OpULessThanEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		case spv::OpTypeInt:
			Push(spv::OpULessThanEqual, dataTypeId, resultId, argumentId1, argumentId2);
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported data type " << dataType;
			break;
		}
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported compare type " << extraInfo;
		break;
	}

	TypeDescription labelType;
	labelType.PrimaryType = spv::OpLabel;

	trueLabelId = GetNextId();
	falseLabelId = GetNextId();
	endIfLabelId = GetNextId();

	mIdTypePairs[trueLabelId] = labelType;
	mIdTypePairs[falseLabelId] = labelType;
	mIdTypePairs[endIfLabelId] = labelType;

	mFalseLabels.push(falseLabelId);
	mFalseLabelCount++;

	mEndIfLabels.push(endIfLabelId);

	Push(spv::OpSelectionMerge, endIfLabelId, 0);
	Push(spv::OpBranchConditional, resultId, trueLabelId, falseLabelId);
	Push(spv::OpLabel, trueLabelId);
	PrintTokenInformation("IFC", argumentToken1, argumentToken2);
}

void ShaderConverter::Process_IF()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;
	uint32_t trueLabelId;
	uint32_t falseLabelId;
	uint32_t endIfLabelId;

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);
	resultId = argumentId1;

	TypeDescription labelType;
	labelType.PrimaryType = spv::OpLabel;

	trueLabelId = GetNextId();
	falseLabelId = GetNextId();
	endIfLabelId = GetNextId();

	mIdTypePairs[trueLabelId] = labelType;
	mIdTypePairs[falseLabelId] = labelType;
	mIdTypePairs[endIfLabelId] = labelType;

	mFalseLabels.push(falseLabelId);
	mFalseLabelCount++;

	mEndIfLabels.push(endIfLabelId);

	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpSelectionMerge, endIfLabelId, 0);
		Push(spv::OpBranchConditional, resultId, trueLabelId, falseLabelId);
		Push(spv::OpLabel, trueLabelId);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_IF - Unsupported data type " << dataType;
		break;
	}

	PrintTokenInformation("IF", argumentToken1);
}

void ShaderConverter::Process_ELSE()
{
	uint32_t falseLabelId = mFalseLabels.top();
	uint32_t endIfLabelId = mEndIfLabels.top();

	mFalseLabels.pop();

	Push(spv::OpBranch, endIfLabelId);
	Push(spv::OpLabel, falseLabelId);
	PrintTokenInformation("ELSE");
}

void ShaderConverter::Process_ENDIF()
{
	if (mFalseLabels.size() == mFalseLabelCount)
	{
		Process_ELSE();
	}
	mFalseLabelCount--;

	uint32_t endIfLabelId = mEndIfLabels.top();
	mEndIfLabels.pop();

	Push(spv::OpBranch, endIfLabelId);
	Push(spv::OpLabel, endIfLabelId);

	PrintTokenInformation("ENDIF");
}

void ShaderConverter::Process_NRM()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Normalize, argumentId1);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Normalize, argumentId1);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Normalize, argumentId1);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_NRM - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("NRM", resultToken, argumentToken1);
}

void ShaderConverter::Process_MOV()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	//Push(spv::OpCopyObject, dataTypeId, resultId, argumentId1);

	resultId = argumentId1;

	mIdTypePairs[resultId] = typeDescription;

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MOV", resultToken, resultId, argumentToken1, argumentId1);
}

void ShaderConverter::Process_MOVA()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = spv::OpTypeInt;
	}
	else
	{
		typeDescription.PrimaryType = spv::OpTypeInt;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);
	resultId = GetNextId();

	Push(spv::OpConvertFToS, dataTypeId, resultId, argumentId1);

	mIdTypePairs[resultId] = typeDescription;

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MOVA", resultToken, argumentToken1);
}

void ShaderConverter::Process_RSQ()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	/*
	The operand x must be a scalar or vector whose component type is floating-point.

	Result Type and the type of x must be the same type. Results are computed per component.
	*/
	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeFloat:
		PushInverseSqrt(dataTypeId, resultId, argumentId1);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_RSQ - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("RSQ", resultToken, argumentToken1);
}

/*
If x, y, or z are less than zero then kill the pixel.
*/
void ShaderConverter::Process_TEXKILL()
{
	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	TypeDescription boolType;
	boolType.PrimaryType = spv::OpTypeBool;
	uint32_t boolTypeId = GetSpirVTypeId(boolType);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	//X
	uint32_t xId = GetNextId();
	mIdTypePairs[xId] = floatType;
	PushCompositeExtract(floatTypeId, xId, argumentId1, 0);

	uint32_t xConditionalId = GetNextId();
	mIdTypePairs[xConditionalId] = boolType;
	Push(spv::OpFOrdLessThan, boolTypeId, xConditionalId, xId, m0fId);
	uint32_t xMergeBlockId = GetNextId();
	Push(spv::OpSelectionMerge, xMergeBlockId, 0);
	uint32_t xTrueLabelId = GetNextId();
	uint32_t xFalseLabelId = GetNextId();
	Push(spv::OpBranchConditional, xConditionalId, xTrueLabelId, xFalseLabelId);
	Push(spv::OpLabel, xTrueLabelId);
	Push(spv::OpKill);
	Push(spv::OpLabel, xFalseLabelId);

	//Y
	uint32_t yId = GetNextId();
	mIdTypePairs[yId] = floatType;
	PushCompositeExtract(floatTypeId, yId, argumentId1, 1);

	uint32_t yConditionalId = GetNextId();
	mIdTypePairs[yConditionalId] = boolType;
	Push(spv::OpFOrdLessThan, boolTypeId, yConditionalId, yId, m0fId);
	uint32_t yMergeBlockId = GetNextId();
	Push(spv::OpSelectionMerge, yMergeBlockId, 0);
	uint32_t yTrueLabelId = GetNextId();
	uint32_t yFalseLabelId = GetNextId();
	Push(spv::OpBranchConditional, yConditionalId, yTrueLabelId, yFalseLabelId);
	Push(spv::OpLabel, yTrueLabelId);
	Push(spv::OpKill);
	Push(spv::OpLabel, yFalseLabelId);

	//Z
	uint32_t zId = GetNextId();
	mIdTypePairs[zId] = floatType;
	PushCompositeExtract(floatTypeId, zId, argumentId1, 1);

	uint32_t zConditionalId = GetNextId();
	mIdTypePairs[zConditionalId] = boolType;
	Push(spv::OpFOrdLessThan, boolTypeId, zConditionalId, zId, m0fId);
	uint32_t zMergeBlockId = GetNextId();
	Push(spv::OpSelectionMerge, zMergeBlockId, 0);
	uint32_t zTrueLabelId = GetNextId();
	uint32_t zFalseLabelId = GetNextId();
	Push(spv::OpBranchConditional, zConditionalId, zTrueLabelId, zFalseLabelId);
	Push(spv::OpLabel, zTrueLabelId);
	Push(spv::OpKill);
	Push(spv::OpLabel, zFalseLabelId);

	PrintTokenInformation("TEXKILL", argumentToken1);
}

void ShaderConverter::Process_DST()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Distance, argumentId1, argumentId2);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Distance, argumentId1, argumentId2);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Distance, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_DST - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("DST", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_CRS()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Cross, argumentId1, argumentId2);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Cross, argumentId1, argumentId2);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Cross, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_CRS - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("CRS", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_POW()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Pow, argumentId1, argumentId2);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Pow, argumentId1, argumentId2);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Pow, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_POW - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("POW", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_MUL()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	//Grab type info if there is any.
	TypeDescription argumentType1 = mIdTypePairs[argumentId1];
	TypeDescription argumentType2 = mIdTypePairs[argumentId2];
	TypeDescription typeDescription;

	//Shift the result type so we get a register instead of a pointer as the output type.
	if (argumentType1.PrimaryType == spv::OpTypePointer)
	{

		argumentType1.PrimaryType = argumentType1.SecondaryType;
		argumentType1.SecondaryType = argumentType1.TernaryType;
		argumentType1.TernaryType = spv::OpTypeVoid;
	}

	if (argumentType2.PrimaryType == spv::OpTypePointer)
	{
		argumentType2.PrimaryType = argumentType2.SecondaryType;
		argumentType2.SecondaryType = argumentType2.TernaryType;
		argumentType2.TernaryType = spv::OpTypeVoid;
	}

	//Figure out which OpCode to used based on the argument types.
	//Unforunately DXBC doesn't care about argument types for multiply but SpirV does.
	if (argumentType1.PrimaryType == spv::OpTypeFloat && argumentType2.PrimaryType == spv::OpTypeFloat)
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpFMul, dataTypeId, resultId, argumentId1, argumentId2);
	}
	else if (argumentType1.PrimaryType == spv::OpTypeVector && argumentType2.PrimaryType == spv::OpTypeVector
		&& argumentType1.SecondaryType == spv::OpTypeFloat && argumentType2.SecondaryType == spv::OpTypeFloat)
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpFMul, dataTypeId, resultId, argumentId1, argumentId2);
	}
	else if (argumentType1.PrimaryType == spv::OpTypeMatrix && argumentType2.PrimaryType == spv::OpTypeMatrix)
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpMatrixTimesMatrix, dataTypeId, resultId, argumentId1, argumentId2);
	}
	else if (argumentType1.PrimaryType == spv::OpTypeVector && argumentType2.PrimaryType == spv::OpTypeFloat)
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpVectorTimesScalar, dataTypeId, resultId, argumentId1, argumentId2);
	}
	else if (argumentType1.PrimaryType == spv::OpTypeMatrix && argumentType2.PrimaryType == spv::OpTypeFloat)
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpMatrixTimesScalar, dataTypeId, resultId, argumentId1, argumentId2);
	}
	else if (argumentType1.PrimaryType == spv::OpTypeInt && argumentType2.PrimaryType == spv::OpTypeInt)
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpIMul, dataTypeId, resultId, argumentId1, argumentId2);
	}
	else if (argumentType1.PrimaryType == spv::OpTypeBool && argumentType2.PrimaryType == spv::OpTypeBool)
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpIMul, dataTypeId, resultId, argumentId1, argumentId2);
	}
	else
	{
		typeDescription = argumentType1;
		uint32_t dataTypeId = GetSpirVTypeId(typeDescription);
		mIdTypePairs[resultId] = typeDescription;
		Push(spv::OpFMul, dataTypeId, resultId, argumentId1, argumentId2);

		BOOST_LOG_TRIVIAL(warning) << "Process_MUL - Unsupported data types " << argumentType1.PrimaryType << " " << argumentType2.PrimaryType;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MUL", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_EXP()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Exp2, argumentId1);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Exp2, argumentId1);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Exp2, argumentId1);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_EXP - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("EXP", resultToken, argumentToken1);
}

void ShaderConverter::Process_EXPP()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Exp2, argumentId1);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Exp2, argumentId1);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Exp2, argumentId1);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_EXPP - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("EXPP", resultToken, argumentToken1);
}

void ShaderConverter::Process_LOG()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Log2, argumentId1);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Log2, argumentId1);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Log2, argumentId1);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_LOG - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("LOG", resultToken, argumentToken1);
}

void ShaderConverter::Process_LOGP()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Log2, argumentId1);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Log2, argumentId1);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Log2, argumentId1);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_LOGP - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("LOGP", resultToken, argumentToken1);
}

void ShaderConverter::Process_FRC()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2 = 0;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	//TODO: create a integer pointer to store the whole part.
	BOOST_LOG_TRIVIAL(warning) << "Process_FRC - Not currently handling integer pointer argument.";

	resultId = GetNextId();

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450Modf, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_FRC - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("FRC", resultToken, argumentToken1);
}


void ShaderConverter::Process_ABS()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450SAbs, argumentId1);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450SAbs, argumentId1);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450FAbs, argumentId1);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_ABS - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("ABS", resultToken, argumentToken1);
}

void ShaderConverter::Process_ADD()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpIAdd, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	case spv::OpTypeInt:
		Push(spv::OpIAdd, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpFAdd, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_ADD - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("ADD", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_SUB()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpISub, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	case spv::OpTypeInt:
		Push(spv::OpISub, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpFSub, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_SUB - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("SUB", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_MIN()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450UMin, argumentId1, argumentId2);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450SMin, argumentId1, argumentId2);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450FMin, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MIN - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MIN", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_MAX()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450UMax, argumentId1, argumentId2);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450SMax, argumentId1, argumentId2);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450FMax, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MAX - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MAX", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_DP3()
{
	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	//DP output is float even though input is a vec.
	mIdTypePairs[resultId] = floatType;
	Push(spv::OpDot, floatTypeId, resultId, argumentId1, argumentId2);

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("DP3", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_DP4()
{
	TypeDescription floatType;
	floatType.PrimaryType = spv::OpTypeFloat;
	uint32_t floatTypeId = GetSpirVTypeId(floatType);

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	//DP output is float even though input is a vec.
	mIdTypePairs[resultId] = floatType;
	Push(spv::OpDot, floatTypeId, resultId, argumentId1, argumentId2);

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("DP4", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_TEX()
{
	TypeDescription vectorType;
	vectorType.PrimaryType = spv::OpTypeVector;
	vectorType.SecondaryType = spv::OpTypeFloat;
	vectorType.ComponentCount = 4;
	uint32_t vectorTypeId = GetSpirVTypeId(vectorType);

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = (mMajorVersion > 1 || mMinorVersion >= 4) ? GetNextToken() : resultToken;
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_SAMPLER);

	Token argumentToken2 = (mMajorVersion > 1) ? GetNextToken() : argumentToken1;
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_2);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	//TEX output is vec4 even though input is a vec2 and sampler.
	mIdTypePairs[resultId] = vectorType;
	Push(spv::OpImageSampleImplicitLod, vectorTypeId, resultId, argumentId1, argumentId2);

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("TEX", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_TEXCOORD()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = (mMajorVersion > 1 || mMinorVersion >= 4) ? GetNextToken() : resultToken;
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	//Push(spv::OpCopyObject, dataTypeId, resultId, argumentId1);

	resultId = argumentId1;

	mIdTypePairs[resultId] = typeDescription;

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("TEXCOORD", resultToken, argumentToken1);
}

void ShaderConverter::Process_M4x4()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_MATRIX_4X4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeFloat:
		Push(spv::OpVectorTimesMatrix, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_M4x4 - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("M4x4", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_M4x3()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_MATRIX_4X4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeFloat:
		Push(spv::OpVectorTimesMatrix, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_M4x3 - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("M4x3", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_M3x4()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_3);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_MATRIX_3X3);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeFloat:
		Push(spv::OpVectorTimesMatrix, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_M3x4 - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("M3x4", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_M3x3()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_3);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_MATRIX_3X3);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeFloat:
		Push(spv::OpVectorTimesMatrix, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_M3x3 - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("M3x3", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_M3x2()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_3);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_MATRIX_3X3);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeFloat:
		Push(spv::OpVectorTimesMatrix, dataTypeId, resultId, argumentId1, argumentId2);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_M3x2 - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("M3x2", resultToken, argumentToken1, argumentToken2);
}

/*
I don't know why SPIR-V doesn't have a MAD instruction.
*/
void ShaderConverter::Process_MAD()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();
	uint32_t resultId2 = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	Token argumentToken3 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType3 = GetRegisterType(argumentToken3.i);
	uint32_t argumentId3 = GetSwizzledId(argumentToken3, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	if (typeDescription.PrimaryType == spv::OpTypeVoid)
	{
		typeDescription = mIdTypePairs[argumentId2];
	}

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;
	mIdTypePairs[resultId2] = typeDescription;
	switch (dataType)
	{
	case spv::OpTypeBool:
		//Write out multiply
		Push(spv::OpIMul, dataTypeId, resultId, argumentId1, argumentId2);
		//Write out add
		Push(spv::OpIAdd, dataTypeId, resultId2, resultId, argumentId3);
		break;
	case spv::OpTypeInt:
		//Write out multiply
		Push(spv::OpIMul, dataTypeId, resultId, argumentId1, argumentId2);
		//Write out add
		Push(spv::OpIAdd, dataTypeId, resultId2, resultId, argumentId3);
		break;
	case spv::OpTypeFloat:
		//Write out multiply
		Push(spv::OpFMul, dataTypeId, resultId, argumentId1, argumentId2);
		//Write out add
		Push(spv::OpFAdd, dataTypeId, resultId2, resultId, argumentId3);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MAD - Unsupported data type " << dataType;
		break;
	}

	resultId2 = ApplyWriteMask(resultToken, resultId2);

	PrintTokenInformation("MAD", resultToken, argumentToken1, argumentToken2, argumentToken3);
}

void ShaderConverter::Process_LRP()
{
	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);
	uint32_t resultId = GetNextId();

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);
	uint32_t argumentId1 = GetSwizzledId(argumentToken1, GIVE_ME_VECTOR_4);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);
	uint32_t argumentId2 = GetSwizzledId(argumentToken2, GIVE_ME_VECTOR_4);

	Token argumentToken3 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType3 = GetRegisterType(argumentToken3.i);
	uint32_t argumentId3 = GetSwizzledId(argumentToken3, GIVE_ME_VECTOR_4);

	TypeDescription typeDescription = mIdTypePairs[argumentId1];

	spv::Op dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	uint32_t dataTypeId = GetSpirVTypeId(typeDescription);

	mIdTypePairs[resultId] = typeDescription;

	switch (dataType)
	{
	case spv::OpTypeBool:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450IMix, argumentId1, argumentId2, argumentId3);
		break;
	case spv::OpTypeInt:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450IMix, argumentId1, argumentId2, argumentId3);
		break;
	case spv::OpTypeFloat:
		Push(spv::OpExtInst, dataTypeId, resultId, mGlslExtensionId, GLSLstd450::GLSLstd450FMix, argumentId1, argumentId2, argumentId3);
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MAD - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("LRP", resultToken, argumentToken1, argumentToken2, argumentToken3);
}

ConvertedShader ShaderConverter::Convert(uint32_t* shader)
{
	//mConvertedShader = {};
	mInstructions.clear();

	uint32_t stringWordSize = 0;
	uint32_t token = 0;
	uint32_t instruction = 0;
	uint32_t extraInfo = 0;

	mBaseToken = mNextToken = mPreviousToken = shader;

	token = GetNextToken().i;
	mPreviousToken--; //Make Previous token one behind the current token.
	mMajorVersion = D3DSHADER_VERSION_MAJOR(token);
	mMinorVersion = D3DSHADER_VERSION_MINOR(token);

	if ((token & 0xFFFF0000) == 0xFFFF0000)
	{
		mIsVertexShader = false;
		BOOST_LOG_TRIVIAL(info) << "PS " << mMajorVersion << "." << mMinorVersion;
	}
	else
	{
		mIsVertexShader = true;
		BOOST_LOG_TRIVIAL(info) << "VS" << mMajorVersion << "." << mMinorVersion;
	}
	//Probably more info in this word but I'll handle that later.


	//Capability
	mCapabilityInstructions.push_back(Pack(2, spv::OpCapability)); //size,Type
	mCapabilityInstructions.push_back(spv::CapabilityShader); //Capability

	//Import
	mGlslExtensionId = GetNextId();
	std::string importStatement = "GLSL.std.450";
	//The spec says 3+variable but there are only 2 before string literal.
	stringWordSize = 2 + (importStatement.length() / 4);
	if (importStatement.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mExtensionInstructions.push_back(Pack(stringWordSize, spv::OpExtInstImport)); //size,Type
	mExtensionInstructions.push_back(mGlslExtensionId); //Result Id
	PutStringInVector(importStatement, mExtensionInstructions);

	//Memory Model
	mMemoryModelInstructions.push_back(Pack(3, spv::OpMemoryModel)); //size,Type
	mMemoryModelInstructions.push_back(spv::AddressingModelLogical); //Addressing Model
	mMemoryModelInstructions.push_back(spv::MemoryModelGLSL450); //Memory Model

	//mSourceInstructions
	mSourceInstructions.push_back(Pack(3, spv::OpSource)); //size,Type
	mSourceInstructions.push_back(spv::SourceLanguageGLSL); //Source Language
	mSourceInstructions.push_back(400); //Version

	std::string sourceExtension1 = "GL_ARB_separate_shader_objects";
	stringWordSize = 2 + (sourceExtension1.length() / 4);
	if (sourceExtension1.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension1, mSourceExtensionInstructions);

	std::string sourceExtension2 = "GL_ARB_shading_language_420pack";
	stringWordSize = 2 + (sourceExtension2.length() / 4);
	if (sourceExtension2.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension2, mSourceExtensionInstructions);

	std::string sourceExtension3 = "GL_GOOGLE_cpp_style_line_directive";
	stringWordSize = 2 + (sourceExtension3.length() / 4);
	if (sourceExtension3.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension3, mSourceExtensionInstructions);

	std::string sourceExtension4 = "GL_GOOGLE_include_directive";
	stringWordSize = 2 + (sourceExtension4.length() / 4);
	if (sourceExtension4.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension4, mSourceExtensionInstructions);

	GenerateConstantBlock();

	//Start of entry point
	mEntryPointTypeId = GetNextId();
	mEntryPointId = GetNextId();

	Push(spv::OpFunction, GetSpirVTypeId(spv::OpTypeVoid), mEntryPointId, spv::FunctionControlMaskNone, mEntryPointTypeId);
	Push(spv::OpLabel, GetNextId());

	GenerateConstantIndices();
	if (mIsVertexShader)
	{
		GeneratePostition();
	}

	Generate255Constants();

	if (mIsVertexShader)
	{
		GeneratePushConstant();
	}

	//Read DXBC instructions
	while (token != D3DPS_END())
	{
		mTokenOffset = mNextToken - shader;
		token = GetNextToken().i;
		instruction = GetOpcode(token);
		extraInfo = ((token & D3DSP_OPCODESPECIFICCONTROL_MASK) >> D3DSP_OPCODESPECIFICCONTROL_SHIFT);

		switch (instruction)
		{
		case D3DSIO_NOP:
			//Nothing
			break;
		case D3DSIO_PHASE:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_PHASE.";
			break;
		case D3DSIO_RET:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_RET.";
			break;
		case D3DSIO_ENDLOOP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_ENDLOOP.";
			break;
		case D3DSIO_BREAK:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BREAK.";
			break;
		case D3DSIO_TEXDEPTH:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXDEPTH.";
			SkipTokens(1);
			break;
		case D3DSIO_TEXKILL:
			Process_TEXKILL();
			break;
		case D3DSIO_BEM:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BEM.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXBEM:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXBEM.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXBEML:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXBEML.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXDP3:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXDP3.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXDP3TEX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXDP3TEX.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXM3x2DEPTH:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x2DEPTH.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXM3x2TEX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x2TEX.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXM3x3:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXM3x3PAD:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3PAD.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXM3x3TEX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3TEX.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXM3x3VSPEC:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3VSPEC.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXREG2AR:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXREG2AR.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXREG2GB:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXREG2GB.";
			SkipTokens(2);
			break;
		case D3DSIO_TEXREG2RGB:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXREG2RGB.";
			SkipTokens(2);
			break;
		case D3DSIO_LABEL:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_LABEL.";
			SkipTokens(2);
			break;
		case D3DSIO_CALL:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CALL.";
			SkipTokens(2);
			break;
		case D3DSIO_LOOP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_LOOP.";
			SkipTokens(2);
			break;
		case D3DSIO_BREAKP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BREAKP.";
			SkipTokens(2);
			break;
		case D3DSIO_DSX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_DSX.";
			SkipTokens(2);
			break;
		case D3DSIO_DSY:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_DSY.";
			SkipTokens(2);
			break;
		case D3DSIO_IFC:
			Process_IFC(extraInfo);
			break;
		case D3DSIO_IF:
			Process_IF();
			break;
		case D3DSIO_ELSE:
			Process_ELSE();
			break;
		case D3DSIO_ENDIF:
			Process_ENDIF();
			break;
		case D3DSIO_REP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_REP.";
			SkipTokens(2);
			break;
		case D3DSIO_ENDREP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_ENDREP.";
			break;
		case D3DSIO_NRM:
			Process_NRM();
			break;
		case D3DSIO_MOVA:
			Process_MOVA();
			break;
		case D3DSIO_MOV:
			Process_MOV();
			break;
		case D3DSIO_RCP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_RCP.";
			SkipTokens(2);
			break;
		case D3DSIO_RSQ:
			Process_RSQ();
			break;
		case D3DSIO_EXP:
			Process_EXP();
			break;
		case D3DSIO_EXPP:
			Process_EXPP();
			break;
		case D3DSIO_LOG:
			Process_LOG();
			break;
		case D3DSIO_LOGP:
			Process_LOGP();
			break;
		case D3DSIO_FRC:
			Process_FRC();
			break;
		case D3DSIO_LIT:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_LIT.";
			SkipTokens(2);
			break;
		case D3DSIO_ABS:
			Process_ABS();
			break;
		case D3DSIO_TEXM3x3SPEC:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3SPEC.";
			SkipTokens(3);
			break;
		case D3DSIO_M4x4:
			Process_M4x4();
			break;
		case D3DSIO_M4x3:
			Process_M4x3();
			break;
		case D3DSIO_M3x4:
			Process_M3x4();
			break;
		case D3DSIO_M3x3:
			Process_M3x3();
			break;
		case D3DSIO_M3x2:
			Process_M3x2();
			break;
		case D3DSIO_CALLNZ:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CALLNZ.";
			SkipTokens(3);
			break;
		case D3DSIO_SETP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SETP.";
			SkipTokens(3);
			break;
		case D3DSIO_BREAKC:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BREAKC.";
			SkipTokens(3);
			break;
		case D3DSIO_ADD:
			Process_ADD();
			break;
		case D3DSIO_SUB:
			Process_SUB();
			break;
		case D3DSIO_MUL:
			Process_MUL();
			break;
		case D3DSIO_DP3:
			Process_DP3();
			break;
		case D3DSIO_DP4:
			Process_DP4();
			break;
		case D3DSIO_MIN:
			Process_MIN();
			break;
		case D3DSIO_MAX:
			Process_MAX();
			break;
		case D3DSIO_DST:
			Process_DST();
			break;
		case D3DSIO_SLT:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SLT.";
			SkipTokens(3);
			break;
		case D3DSIO_SGE:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SGE.";
			SkipTokens(3);
			break;
		case D3DSIO_CRS:
			Process_CRS();
			break;
		case D3DSIO_POW:
			Process_POW();
			break;
		case D3DSIO_DP2ADD:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_DP2ADD.";
			SkipTokens(4);
			break;
		case D3DSIO_LRP:
			Process_LRP();
			break;
		case D3DSIO_SGN:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SGN.";
			SkipTokens(4);
			break;
		case D3DSIO_CND:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CND.";
			SkipTokens(4);
			break;
		case D3DSIO_CMP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CMP.";
			SkipTokens(4);
			break;
		case D3DSIO_SINCOS:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SINCOS.";
			if (mMajorVersion >= 3)
			{
				SkipTokens(3);
			}
			else
			{
				SkipTokens(2);
			}
			break;
		case D3DSIO_MAD:
			Process_MAD();
			break;
		case D3DSIO_TEXLDD:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXLDD.";
			SkipTokens(5);
			break;
		case D3DSIO_TEXCOORD:
			Process_TEXCOORD();
			break;
		case D3DSIO_TEX:
			Process_TEX();
			break;
		case D3DSIO_TEXLDL:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXLDL.";
			SkipTokens(3);
			break;
		case D3DSIO_DCL:
			Process_DCL();
			break;
		case D3DSIO_DEFB:
			Process_DEFB();
			break;
		case D3DSIO_DEFI:
			Process_DEFI();
			break;
		case D3DSIO_DEF:
			Process_DEF();
			break;
		case D3DSIO_COMMENT:
			SkipTokens(((token & 0x0fff0000) >> 16));
			break;
		case D3DSIO_END:
			//Nothing
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction " << instruction << ".";
			break;
		}

	}

	if (mIsVertexShader)
	{
		GenerateYFlip();
	}

	//After inputs & outputs are defined set the function type with the type id defined earlier.
	GetSpirVTypeId(spv::OpTypeFunction, mEntryPointTypeId);

	//End of entry point
	Push(spv::OpReturn);
	Push(spv::OpFunctionEnd);
	uint32_t outputIndex = 0;

	//EntryPoint
	std::string entryPointName = "main";
	std::vector<uint32_t> interfaceIds;
	typedef boost::container::flat_map<uint32_t, uint32_t> maptype2;
	for (const auto& inputRegister : mInputRegisters)
	{
		interfaceIds.push_back(inputRegister);

#ifdef _EXTRA_SHADER_DEBUG_INFO
		auto& inputRegisterType = mIdTypePairs[inputRegister];
		auto& inputRegisterName = mNameIdPairs[inputRegister];

		BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Convert " << inputRegisterName << "(" << inputRegister << ") as " << inputRegisterType << "(Input)";
#endif
	}
	for (const auto& outputRegister : mOutputRegisters)
	{
		interfaceIds.push_back(outputRegister);

#ifdef _EXTRA_SHADER_DEBUG_INFO
		auto& outputRegisterType = mIdTypePairs[outputRegister];
		auto& outputRegisterName = mNameIdPairs[outputRegister];

		BOOST_LOG_TRIVIAL(info) << "ShaderConverter::Convert " << outputRegisterName << "(" << outputRegister << ") as " << outputRegisterType << "(Output)";
#endif
	}

	//The spec says 4+variable but there are only 3 before the string literal.
	stringWordSize = 3 + (entryPointName.length() / 4) + interfaceIds.size();
	if (entryPointName.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mEntryPointInstructions.push_back(Pack(stringWordSize, spv::OpEntryPoint)); //size,Type
	if (mIsVertexShader)
	{
		mEntryPointInstructions.push_back(spv::ExecutionModelVertex); //Execution Model
	}
	else
	{
		mEntryPointInstructions.push_back(spv::ExecutionModelFragment); //Execution Model
	}
	mEntryPointInstructions.push_back(mEntryPointId); //Entry Point (Id)
	PutStringInVector(entryPointName, mEntryPointInstructions); //Name
	mEntryPointInstructions.insert(std::end(mEntryPointInstructions), std::begin(interfaceIds), std::end(interfaceIds)); //Interfaces

	//Write entry point name.
	PushName(mEntryPointId, entryPointName);

	//ExecutionMode
	if (!mIsVertexShader)
	{
		mExecutionModeInstructions.push_back(Pack(3, spv::OpExecutionMode)); //size,Type
		mExecutionModeInstructions.push_back(mEntryPointId); //Entry Point (Id)
		mExecutionModeInstructions.push_back(spv::ExecutionModeOriginUpperLeft); //Execution Mode
	}
	else
	{
		//TODO: figure out what to do for vertex execution mode.
	}


	//Write SPIR-V header
	const GeneratorMagicNumber generatorMagicNumber = { 1,13 };

	mInstructions.push_back(spv::MagicNumber);
	mInstructions.push_back(0x00010000); //spv::Version
	mInstructions.push_back(generatorMagicNumber.Word); //I don't have a generator number ATM.
	mInstructions.push_back(GetNextId()); //Bound
	mInstructions.push_back(0); //Reserved for instruction schema, if needed

	//Dump other opcodes into instruction collection is required order.
	CombineSpirVOpCodes();

	//Pass the word blob to Vulkan to generate a module.
	CreateSpirVModule();

	return mConvertedShader; //Return value optimization don't fail me now.
}
