#include <chrono>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <boost/container/small_vector.hpp>

#include "Utilities.h"
#include "CTypes.h"

#include "RealDevice.h"
#include "RealInstance.h"

#include "ResourceContext.h"

#ifndef DRAWCONTEXT_H
#define DRAWCONTEXT_H

struct DrawContext
{
	//Vulkan State
	vk::DescriptorSetLayout DescriptorSetLayout;
	vk::Pipeline Pipeline;
	vk::PipelineLayout PipelineLayout;

	//Misc
	//boost::container::flat_map<UINT, UINT> Bindings;
	UINT Bindings[64] = {};

	//D3D9 State - Pipe
	D3DPRIMITIVETYPE PrimitiveType = D3DPT_FORCE_DWORD;
	DWORD FVF = 0;
	CVertexDeclaration9* VertexDeclaration = nullptr;
	CVertexShader9* VertexShader = nullptr;
	CPixelShader9* PixelShader = nullptr;
	int32_t StreamCount = 0;

	//D3d9 State - Lights
	ShaderConstantSlots mVertexShaderConstantSlots = {};
	ShaderConstantSlots mPixelShaderConstantSlots = {};

	//Constant Registers
	SpecializationConstants mSpecializationConstants = {};

	//Resource Handling.
	std::chrono::steady_clock::time_point LastUsed = std::chrono::steady_clock::now();
	RealDevice* mRealDevice = nullptr; //null if not owner.
	DrawContext(RealDevice* realDevice) : mRealDevice(realDevice) {}
	~DrawContext();
};

#endif //DRAWCONTEXT_H
