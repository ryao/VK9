/*VkPipelineShaderStageCreateInfo
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

#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#define UBO_SIZE 64

#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include <boost/container/flat_map.hpp>
#include <boost/container/small_vector.hpp>
#include <Eigen/Dense>
#include <memory>
#include <chrono>

#include "CTypes.h"
#include "CIndexBuffer9.h"

class CDevice9;

struct SamplerRequest
{
	//Vulkan State
	VkSampler Sampler = VK_NULL_HANDLE;

	//D3D9 State
	DWORD SamplerIndex = 0;
	D3DTEXTUREFILTERTYPE MagFilter = D3DTEXF_NONE;
	D3DTEXTUREFILTERTYPE MinFilter = D3DTEXF_NONE;
	D3DTEXTUREADDRESS AddressModeU = D3DTADDRESS_FORCE_DWORD;
	D3DTEXTUREADDRESS AddressModeV = D3DTADDRESS_FORCE_DWORD;
	D3DTEXTUREADDRESS AddressModeW = D3DTADDRESS_FORCE_DWORD;
	DWORD MaxAnisotropy = 0;
	D3DTEXTUREFILTERTYPE MipmapMode = D3DTEXF_NONE;
	float MipLodBias = 0.0f;
	float MaxLod = 1.0f;

	//Resource Handling.
	std::chrono::steady_clock::time_point LastUsed = std::chrono::steady_clock::now();
	CDevice9* mDevice = nullptr;
	SamplerRequest(CDevice9* device) : mDevice(device) {}
	~SamplerRequest();
};

struct ResourceContext
{
	VkDescriptorImageInfo DescriptorImageInfo[16] = {};

	//Vulkan State
	VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;

	//Resource Handling.
	std::chrono::steady_clock::time_point LastUsed = std::chrono::steady_clock::now();
	CDevice9* mDevice = nullptr;
	ResourceContext(CDevice9* device) : mDevice(device) {}
	~ResourceContext();
};

struct DrawContext
{
	//Vulkan State
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkPipeline Pipeline = VK_NULL_HANDLE;
	VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;

	//Misc
	boost::container::flat_map<UINT, UINT> Bindings;

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
	CDevice9* mDevice = nullptr;
	DrawContext(CDevice9* device) : mDevice(device) {}
	~DrawContext();
};

class BufferManager
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	BufferManager();
	explicit BufferManager(CDevice9* device);
	~BufferManager();

	VkResult mResult = VK_SUCCESS;

	CDevice9* mDevice = nullptr;
	bool mIsDirty = true;

	VkDynamicState mDynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE] = {};
	VkPipelineColorBlendAttachmentState mPipelineColorBlendAttachmentState[1] = {};

	VkPipelineVertexInputStateCreateInfo mPipelineVertexInputStateCreateInfo = {};
	VkPipelineInputAssemblyStateCreateInfo mPipelineInputAssemblyStateCreateInfo = {};
	VkPipelineRasterizationStateCreateInfo mPipelineRasterizationStateCreateInfo = {};
	VkPipelineColorBlendStateCreateInfo mPipelineColorBlendStateCreateInfo = {};
	VkPipelineDepthStencilStateCreateInfo mPipelineDepthStencilStateCreateInfo = {};
	VkPipelineViewportStateCreateInfo mPipelineViewportStateCreateInfo = {};
	VkPipelineMultisampleStateCreateInfo mPipelineMultisampleStateCreateInfo = {};
	VkPipelineDynamicStateCreateInfo mPipelineDynamicStateCreateInfo = {};
	VkGraphicsPipelineCreateInfo mGraphicsPipelineCreateInfo = {};
	VkPipelineCacheCreateInfo mPipelineCacheCreateInfo = {};

	VkPipelineShaderStageCreateInfo mPipelineShaderStageCreateInfo[2] = {};

	VkDescriptorSetAllocateInfo mDescriptorSetAllocateInfo = {};
	VkDescriptorSetLayoutBinding mDescriptorSetLayoutBinding[16] = {};
	VkDescriptorSetLayoutCreateInfo mDescriptorSetLayoutCreateInfo = {};
	VkPipelineLayoutCreateInfo mPipelineLayoutCreateInfo = {};
	VkWriteDescriptorSet mWriteDescriptorSet[3] = {};
	VkPushConstantRange mPushConstantRanges[2] = {};
	VkDescriptorBufferInfo mDescriptorBufferInfo[2] = {};

	//Created with max slots. I can pass a count to limit the number. This should prevent me from needing to realloc.
	VkVertexInputBindingDescription mVertexInputBindingDescription[16] = {};
	VkVertexInputAttributeDescription mVertexInputAttributeDescription[32] = {};

	//Command Buffer & Buffer Copy Setup
	VkCommandBufferAllocateInfo mCommandBufferAllocateInfo = {};
	VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	VkCommandBufferBeginInfo mBeginInfo = {};
	VkBufferCopy mCopyRegion = {};
	VkSubmitInfo mSubmitInfo = {};

	//VkDescriptorSetLayout mDescriptorSetLayout;
	//VkPipelineLayout mPipelineLayout;
	VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
	//VkDescriptorSet mDescriptorSet;
	//VkPipeline mPipeline;
	
	VkShaderModule mVertShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_TEX1 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_TEX2 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_TEX2 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE_TEX1 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_DIFFUSE_TEX2 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_NORMAL_TEX1 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL_TEX1 = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_NORMAL_DIFFUSE = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL_DIFFUSE = VK_NULL_HANDLE;

	VkShaderModule mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;

	//Seems like there should be a way to generate these with constexpr.

	const VkSpecializationMapEntry mSlotMapEntries[288] =
	{
		//IntegerConstants
		{ 0 ,                        0, sizeof(uint32_t) * 4 },
		{ 1 , 1 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 2 , 2 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 3 , 3 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 4 , 4 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 5 , 5 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 6 , 6 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 7 , 7 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 8 , 8 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 9 , 9 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 10, 10 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 11, 11 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 12, 12 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 13, 13 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 14, 14 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 15, 15 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },

		//BooleanConstants
		{ 16, 16 * sizeof(BOOL), sizeof(BOOL) },
		{ 17, 17 * sizeof(BOOL), sizeof(BOOL) },
		{ 18, 18 * sizeof(BOOL), sizeof(BOOL) },
		{ 19, 19 * sizeof(BOOL), sizeof(BOOL) },
		{ 20, 20 * sizeof(BOOL), sizeof(BOOL) },
		{ 21, 21 * sizeof(BOOL), sizeof(BOOL) },
		{ 22, 22 * sizeof(BOOL), sizeof(BOOL) },
		{ 23, 23 * sizeof(BOOL), sizeof(BOOL) },
		{ 24, 24 * sizeof(BOOL), sizeof(BOOL) },
		{ 25, 25 * sizeof(BOOL), sizeof(BOOL) },
		{ 26, 26 * sizeof(BOOL), sizeof(BOOL) },
		{ 27, 27 * sizeof(BOOL), sizeof(BOOL) },
		{ 28, 28 * sizeof(BOOL), sizeof(BOOL) },
		{ 29, 29 * sizeof(BOOL), sizeof(BOOL) },
		{ 30, 30 * sizeof(BOOL), sizeof(BOOL) },
		{ 31, 31 * sizeof(BOOL), sizeof(BOOL) },

		//FloatConstants
		{ 32  , 32 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 33  , 33 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 34  , 34 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 35  , 35 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 36  , 36 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 37  , 37 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 38  , 38 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 39  , 39 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 40  , 40 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 41  , 41 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 42  , 42 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 43  , 43 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 44  , 44 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 45  , 45 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 46  , 46 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 47  , 47 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 48  , 48 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 49  , 49 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 50  , 50 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 51  , 51 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 52  , 52 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 53  , 53 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 54  , 54 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 55  , 55 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 56  , 56 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 57  , 57 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 58  , 58 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 59  , 59 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 60  , 60 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 61  , 61 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 62  , 62 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 63  , 63 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 64  , 64 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 65  , 65 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 66  , 66 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 67  , 67 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 68  , 68 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 69  , 69 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 70  , 70 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 71  , 71 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 72  , 72 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 73  , 73 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 74  , 74 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 75  , 75 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 76  , 76 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 77  , 77 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 78  , 78 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 79  , 79 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 80  , 80 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 81  , 81 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 82  , 82 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 83  , 83 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 84  , 84 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 85  , 85 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 86  , 86 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 87  , 87 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 88  , 88 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 89  , 89 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 90  , 90 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 91  , 91 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 92  , 92 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 93  , 93 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 94  , 94 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 95  , 95 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 96  , 96 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 97  , 97 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 98  , 98 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 99  , 99 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 100 , 100 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 101 , 101 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 102 , 102 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 103 , 103 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 104 , 104 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 105 , 105 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 106 , 106 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 107 , 107 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 108 , 108 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 109 , 109 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 110 , 110 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 111 , 111 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 112 , 112 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 113 , 113 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 114 , 114 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 115 , 115 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 116 , 116 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 117 , 117 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 118 , 118 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 119 , 119 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 120 , 120 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 121 , 121 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 122 , 122 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 123 , 123 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 124 , 124 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 125 , 125 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 126 , 126 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 127 , 127 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 128 , 128 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 129 , 129 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 130 , 130 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 131 , 131 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 132 , 132 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 133 , 133 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 134 , 134 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 135 , 135 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 136 , 136 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 137 , 137 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 138 , 138 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 139 , 139 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 140 , 140 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 141 , 141 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 142 , 142 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 143 , 143 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 144 , 144 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 145 , 145 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 146 , 146 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 147 , 147 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 148 , 148 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 149 , 149 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 150 , 150 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 151 , 151 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 152 , 152 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 153 , 153 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 154 , 154 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 155 , 155 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 156 , 156 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 157 , 157 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 158 , 158 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 159 , 159 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 160 , 160 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 161 , 161 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 162 , 162 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 163 , 163 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 164 , 164 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 165 , 165 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 166 , 166 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 167 , 167 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 168 , 168 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 169 , 169 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 170 , 170 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 171 , 171 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 172 , 172 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 173 , 173 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 174 , 174 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 175 , 175 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 176 , 176 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 177 , 177 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 178 , 178 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 179 , 179 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 180 , 180 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 181 , 181 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 182 , 182 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 183 , 183 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 184 , 184 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 185 , 185 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 186 , 186 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 187 , 187 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 188 , 188 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 189 , 189 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 190 , 190 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 191 , 191 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 192 , 192 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 193 , 193 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 194 , 194 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 195 , 195 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 196 , 196 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 197 , 197 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 198 , 198 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 199 , 199 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 200 , 200 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 201 , 201 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 202 , 202 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 203 , 203 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 204 , 204 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 205 , 205 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 206 , 206 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 207 , 207 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 208 , 208 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 209 , 209 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 210 , 210 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 211 , 211 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 212 , 212 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 213 , 213 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 214 , 214 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 215 , 215 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 216 , 216 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 217 , 217 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 218 , 218 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 219 , 219 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 220 , 220 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 221 , 221 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 222 , 222 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 223 , 223 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 224 , 224 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 225 , 225 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 226 , 226 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 227 , 227 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 228 , 228 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 229 , 229 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 230 , 230 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 231 , 231 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 232 , 232 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 233 , 233 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 234 , 234 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 235 , 235 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 236 , 236 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 237 , 237 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 238 , 238 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 239 , 239 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 240 , 240 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 241 , 241 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 242 , 242 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 243 , 243 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 244 , 244 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 245 , 245 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 246 , 246 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 247 , 247 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 248 , 248 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 249 , 249 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 250 , 250 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 251 , 251 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 252 , 252 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 253 , 253 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 254 , 254 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 255 , 255 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 256 , 256 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 257 , 257 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 258 , 258 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 259 , 259 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 260 , 260 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 261 , 261 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 262 , 262 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 263 , 263 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 264 , 264 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 265 , 265 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 266 , 266 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 267 , 267 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 268 , 268 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 269 , 269 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 270 , 270 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 271 , 271 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 272 , 272 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 273 , 273 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 274 , 274 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 275 , 275 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 276 , 276 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 277 , 277 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 278 , 278 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 279 , 279 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 280 , 280 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 281 , 281 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 282 , 282 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 283 , 283 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 284 , 284 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 285 , 285 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 286 , 286 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 },
		{ 287 , 287 * sizeof(uint32_t) * 4, sizeof(uint32_t) * 4 }
	};

	const VkSpecializationMapEntry mSpecializationMapEntries[251] =		
	{ 
		// id,offset,size
		{ 0, 0, sizeof(int)},
		{ 1, 1 * sizeof(int), sizeof(int)},
		{ 2, 2 * sizeof(int), sizeof(int)},
		{ 3, 3 * sizeof(int), sizeof(int)},

		//stage0
		{ 4 , 4 * sizeof(int) , sizeof(int) } ,
		{ 5 , 5 * sizeof(int) , sizeof(int) } ,
		{ 6 , 6 * sizeof(int) , sizeof(int) } ,
		{ 7 , 7 * sizeof(int) , sizeof(int) } ,
		{ 8 , 8 * sizeof(int) , sizeof(int) } ,
		{ 9 , 9 * sizeof(int) , sizeof(int) } ,
		{ 10 , 10 * sizeof(int) , sizeof(int) } ,
		{ 11 , 11 * sizeof(int) , sizeof(int) } ,
		{ 12 , 12 * sizeof(int) , sizeof(int) } ,
		{ 13 , 13 * sizeof(int) , sizeof(int) } ,
		{ 14 , 14 * sizeof(int) , sizeof(int) } ,
		{ 15 , 15 * sizeof(int) , sizeof(int) } ,
		{ 16 , 16 * sizeof(int) , sizeof(int) } ,
		{ 17 , 17 * sizeof(int) , sizeof(int) } ,
		{ 18 , 18 * sizeof(int) , sizeof(int) } ,
		{ 19 , 19 * sizeof(int) , sizeof(int) } ,
		{ 20 , 20 * sizeof(int) , sizeof(int) } ,
		{ 21 , 21 * sizeof(int) , sizeof(int) } ,

		//stage1
		{ 22 , 22 * sizeof(int) , sizeof(int) } ,
		{ 23 , 23 * sizeof(int) , sizeof(int) } ,
		{ 24 , 24 * sizeof(int) , sizeof(int) } ,
		{ 25 , 25 * sizeof(int) , sizeof(int) } ,
		{ 26 , 26 * sizeof(int) , sizeof(int) } ,
		{ 27 , 27 * sizeof(int) , sizeof(int) } ,
		{ 28 , 28 * sizeof(int) , sizeof(int) } ,
		{ 29 , 29 * sizeof(int) , sizeof(int) } ,
		{ 30 , 30 * sizeof(int) , sizeof(int) } ,
		{ 31 , 31 * sizeof(int) , sizeof(int) } ,
		{ 32 , 32 * sizeof(int) , sizeof(int) } ,
		{ 33 , 33 * sizeof(int) , sizeof(int) } ,
		{ 34 , 34 * sizeof(int) , sizeof(int) } ,
		{ 35 , 35 * sizeof(int) , sizeof(int) } ,
		{ 36 , 36 * sizeof(int) , sizeof(int) } ,
		{ 37 , 37 * sizeof(int) , sizeof(int) } ,
		{ 38 , 38 * sizeof(int) , sizeof(int) } ,
		{ 39 , 39 * sizeof(int) , sizeof(int) } ,

		//stage2
		{ 40 , 40 * sizeof(int) , sizeof(int) } ,
		{ 41 , 41 * sizeof(int) , sizeof(int) } ,
		{ 42 , 42 * sizeof(int) , sizeof(int) } ,
		{ 43 , 43 * sizeof(int) , sizeof(int) } ,
		{ 44 , 44 * sizeof(int) , sizeof(int) } ,
		{ 45 , 45 * sizeof(int) , sizeof(int) } ,
		{ 46 , 46 * sizeof(int) , sizeof(int) } ,
		{ 47 , 47 * sizeof(int) , sizeof(int) } ,
		{ 48 , 48 * sizeof(int) , sizeof(int) } ,
		{ 49 , 49 * sizeof(int) , sizeof(int) } ,
		{ 50 , 50 * sizeof(int) , sizeof(int) } ,
		{ 51 , 51 * sizeof(int) , sizeof(int) } ,
		{ 52 , 52 * sizeof(int) , sizeof(int) } ,
		{ 53 , 53 * sizeof(int) , sizeof(int) } ,
		{ 54 , 54 * sizeof(int) , sizeof(int) } ,
		{ 55 , 55 * sizeof(int) , sizeof(int) } ,
		{ 56 , 56 * sizeof(int) , sizeof(int) } ,
		{ 57 , 57 * sizeof(int) , sizeof(int) } ,

		//stage3
		{ 58 , 58 * sizeof(int) , sizeof(int) } ,
		{ 59 , 59 * sizeof(int) , sizeof(int) } ,
		{ 60 , 60 * sizeof(int) , sizeof(int) } ,
		{ 61 , 61 * sizeof(int) , sizeof(int) } ,
		{ 62 , 62 * sizeof(int) , sizeof(int) } ,
		{ 63 , 63 * sizeof(int) , sizeof(int) } ,
		{ 64 , 64 * sizeof(int) , sizeof(int) } ,
		{ 65 , 65 * sizeof(int) , sizeof(int) } ,
		{ 66 , 66 * sizeof(int) , sizeof(int) } ,
		{ 67 , 67 * sizeof(int) , sizeof(int) } ,
		{ 68 , 68 * sizeof(int) , sizeof(int) } ,
		{ 69 , 69 * sizeof(int) , sizeof(int) } ,
		{ 70 , 70 * sizeof(int) , sizeof(int) } ,
		{ 71 , 71 * sizeof(int) , sizeof(int) } ,
		{ 72 , 72 * sizeof(int) , sizeof(int) } ,
		{ 73 , 73 * sizeof(int) , sizeof(int) } ,
		{ 74 , 74 * sizeof(int) , sizeof(int) } ,
		{ 75 , 75 * sizeof(int) , sizeof(int) } ,

		//stage4
		{ 76 , 76 * sizeof(int) , sizeof(int) } ,
		{ 77 , 77 * sizeof(int) , sizeof(int) } ,
		{ 78 , 78 * sizeof(int) , sizeof(int) } ,
		{ 79 , 79 * sizeof(int) , sizeof(int) } ,
		{ 80 , 80 * sizeof(int) , sizeof(int) } ,
		{ 81 , 81 * sizeof(int) , sizeof(int) } ,
		{ 82 , 82 * sizeof(int) , sizeof(int) } ,
		{ 83 , 83 * sizeof(int) , sizeof(int) } ,
		{ 84 , 84 * sizeof(int) , sizeof(int) } ,
		{ 85 , 85 * sizeof(int) , sizeof(int) } ,
		{ 86 , 86 * sizeof(int) , sizeof(int) } ,
		{ 87 , 87 * sizeof(int) , sizeof(int) } ,
		{ 88 , 88 * sizeof(int) , sizeof(int) } ,
		{ 89 , 89 * sizeof(int) , sizeof(int) } ,
		{ 90 , 90 * sizeof(int) , sizeof(int) } ,
		{ 91 , 91 * sizeof(int) , sizeof(int) } ,
		{ 92 , 92 * sizeof(int) , sizeof(int) } ,
		{ 93 , 93 * sizeof(int) , sizeof(int) } ,

		//stage5
		{ 94 , 94 * sizeof(int) , sizeof(int) } ,
		{ 95 , 95 * sizeof(int) , sizeof(int) } ,
		{ 96 , 96 * sizeof(int) , sizeof(int) } ,
		{ 97 , 97 * sizeof(int) , sizeof(int) } ,
		{ 98 , 98 * sizeof(int) , sizeof(int) } ,
		{ 99 , 99 * sizeof(int) , sizeof(int) } ,
		{ 100, 100 * sizeof(int) , sizeof(int) } ,
		{ 101 ,101 * sizeof(int) , sizeof(int) } ,
		{ 102 ,102 * sizeof(int) , sizeof(int) } ,
		{ 103 ,103 * sizeof(int) , sizeof(int) } ,
		{ 104 ,104 * sizeof(int) , sizeof(int) } ,
		{ 105 ,105 * sizeof(int) , sizeof(int) } ,
		{ 106 ,106 * sizeof(int) , sizeof(int) } ,
		{ 107 ,107 * sizeof(int) , sizeof(int) } ,
		{ 108 ,108 * sizeof(int) , sizeof(int) } ,
		{ 109 ,109 * sizeof(int) , sizeof(int) } ,
		{ 110 ,110 * sizeof(int) , sizeof(int) } ,
		{ 111 ,111 * sizeof(int) , sizeof(int) } ,

		//stage6
		{ 112 , 112 * sizeof(int) , sizeof(int) } ,
		{ 113 , 113 * sizeof(int) , sizeof(int) } ,
		{ 114 , 114 * sizeof(int) , sizeof(int) } ,
		{ 115 , 115 * sizeof(int) , sizeof(int) } ,
		{ 116 , 116 * sizeof(int) , sizeof(int) } ,
		{ 117 , 117 * sizeof(int) , sizeof(int) } ,
		{ 118 , 118 * sizeof(int) , sizeof(int) } ,
		{ 119 , 119 * sizeof(int) , sizeof(int) } ,
		{ 120 , 120 * sizeof(int) , sizeof(int) } ,
		{ 121 , 121 * sizeof(int) , sizeof(int) } ,
		{ 122 , 122 * sizeof(int) , sizeof(int) } ,
		{ 123 , 123 * sizeof(int) , sizeof(int) } ,
		{ 124 , 124 * sizeof(int) , sizeof(int) } ,
		{ 125 , 125 * sizeof(int) , sizeof(int) } ,
		{ 126 , 126 * sizeof(int) , sizeof(int) } ,
		{ 127 , 127 * sizeof(int) , sizeof(int) } ,
		{ 128 , 128 * sizeof(int) , sizeof(int) } ,
		{ 129 , 129 * sizeof(int) , sizeof(int) } ,

		//stage7
		{ 130 , 130 * sizeof(int) , sizeof(int) } ,
		{ 131 , 131 * sizeof(int) , sizeof(int) } ,
		{ 132 , 132 * sizeof(int) , sizeof(int) } ,
		{ 133 , 133 * sizeof(int) , sizeof(int) } ,
		{ 134 , 134 * sizeof(int) , sizeof(int) } ,
		{ 135 , 135 * sizeof(int) , sizeof(int) } ,
		{ 136 , 136 * sizeof(int) , sizeof(int) } ,
		{ 137 , 137 * sizeof(int) , sizeof(int) } ,
		{ 138 , 138 * sizeof(int) , sizeof(int) } ,
		{ 139 , 139 * sizeof(int) , sizeof(int) } ,
		{ 140 , 140 * sizeof(int) , sizeof(int) } ,
		{ 141 , 141 * sizeof(int) , sizeof(int) } ,
		{ 142 , 142 * sizeof(int) , sizeof(int) } ,
		{ 143 , 143 * sizeof(int) , sizeof(int) } ,
		{ 144 , 144 * sizeof(int) , sizeof(int) } ,
		{ 145 , 145 * sizeof(int) , sizeof(int) } ,
		{ 146 , 146 * sizeof(int) , sizeof(int) } ,
		{ 147 , 147 * sizeof(int) , sizeof(int) } ,

		//Render State
		{ 148 , 148 * sizeof(int) , sizeof(int) },
		{ 149 , 149 * sizeof(int) , sizeof(int) },
		{ 150 , 150 * sizeof(int) , sizeof(int) },
		{ 151 , 151 * sizeof(int) , sizeof(int) },
		{ 152 , 152 * sizeof(int) , sizeof(int) },
		{ 153 , 153 * sizeof(int) , sizeof(int) },
		{ 154 , 154 * sizeof(int) , sizeof(int) },
		{ 155 , 155 * sizeof(int) , sizeof(int) },
		{ 156 , 156 * sizeof(int) , sizeof(int) },
		{ 157 , 157 * sizeof(int) , sizeof(int) },
		{ 158 , 158 * sizeof(int) , sizeof(int) },
		{ 159 , 159 * sizeof(int) , sizeof(int) },
		{ 160 , 160 * sizeof(int) , sizeof(int) },
		{ 161 , 161 * sizeof(int) , sizeof(int) },
		{ 162 , 162 * sizeof(int) , sizeof(int) },
		{ 163 , 163 * sizeof(int) , sizeof(int) },
		{ 164 , 164 * sizeof(int) , sizeof(int) },
		{ 165 , 165 * sizeof(int) , sizeof(int) },
		{ 166 , 166 * sizeof(int) , sizeof(int) },
		{ 167 , 167 * sizeof(int) , sizeof(int) },
		{ 168 , 168 * sizeof(int) , sizeof(int) },
		{ 169 , 169 * sizeof(int) , sizeof(int) },
		{ 170 , 170 * sizeof(int) , sizeof(int) },
		{ 171 , 171 * sizeof(int) , sizeof(int) },
		{ 172 , 172 * sizeof(int) , sizeof(int) },
		{ 173 , 173 * sizeof(int) , sizeof(int) },
		{ 174 , 174 * sizeof(int) , sizeof(int) },
		{ 175 , 175 * sizeof(int) , sizeof(int) },
		{ 176 , 176 * sizeof(int) , sizeof(int) },
		{ 177 , 177 * sizeof(int) , sizeof(int) },
		{ 178 , 178 * sizeof(int) , sizeof(int) },
		{ 179 , 179 * sizeof(int) , sizeof(int) },
		{ 180 , 180 * sizeof(int) , sizeof(int) },
		{ 181 , 181 * sizeof(int) , sizeof(int) },
		{ 182 , 182 * sizeof(int) , sizeof(int) },
		{ 183 , 183 * sizeof(int) , sizeof(int) },
		{ 184 , 184 * sizeof(int) , sizeof(int) },
		{ 185 , 185 * sizeof(int) , sizeof(int) },
		{ 186 , 186 * sizeof(int) , sizeof(int) },
		{ 187 , 187 * sizeof(int) , sizeof(int) },
		{ 188 , 188 * sizeof(int) , sizeof(int) },
		{ 189 , 189 * sizeof(int) , sizeof(int) },
		{ 190 , 190 * sizeof(int) , sizeof(int) },
		{ 191 , 191 * sizeof(int) , sizeof(int) },
		{ 192 , 192 * sizeof(int) , sizeof(int) },
		{ 193 , 193 * sizeof(int) , sizeof(int) },
		{ 194 , 194 * sizeof(int) , sizeof(int) },
		{ 195 , 195 * sizeof(int) , sizeof(int) },
		{ 196 , 196 * sizeof(int) , sizeof(int) },
		{ 197 , 197 * sizeof(int) , sizeof(int) },
		{ 198 , 198 * sizeof(int) , sizeof(int) },
		{ 199 , 199 * sizeof(int) , sizeof(int) },
		{ 200 , 200 * sizeof(int) , sizeof(int) },
		{ 201 , 201 * sizeof(int) , sizeof(int) },
		{ 202 , 202 * sizeof(int) , sizeof(int) },
		{ 203 , 203 * sizeof(int) , sizeof(int) },
		{ 204 , 204 * sizeof(int) , sizeof(int) },
		{ 205 , 205 * sizeof(int) , sizeof(int) },
		{ 206 , 206 * sizeof(int) , sizeof(int) },
		{ 207 , 207 * sizeof(int) , sizeof(int) },
		{ 208 , 208 * sizeof(int) , sizeof(int) },
		{ 209 , 209 * sizeof(int) , sizeof(int) },
		{ 210 , 210 * sizeof(int) , sizeof(int) },
		{ 211 , 211 * sizeof(int) , sizeof(int) },
		{ 212 , 212 * sizeof(int) , sizeof(int) },
		{ 213 , 213 * sizeof(int) , sizeof(int) },
		{ 214 , 214 * sizeof(int) , sizeof(int) },
		{ 215 , 215 * sizeof(int) , sizeof(int) },
		{ 216 , 216 * sizeof(int) , sizeof(int) },
		{ 217 , 217 * sizeof(int) , sizeof(int) },
		{ 218 , 218 * sizeof(int) , sizeof(int) },
		{ 219 , 219 * sizeof(int) , sizeof(int) },
		{ 220 , 220 * sizeof(int) , sizeof(int) },
		{ 221 , 221 * sizeof(int) , sizeof(int) },
		{ 222 , 222 * sizeof(int) , sizeof(int) },
		{ 223 , 223 * sizeof(int) , sizeof(int) },
		{ 224 , 224 * sizeof(int) , sizeof(int) },
		{ 225 , 225 * sizeof(int) , sizeof(int) },
		{ 226 , 226 * sizeof(int) , sizeof(int) },
		{ 227 , 227 * sizeof(int) , sizeof(int) },
		{ 228 , 228 * sizeof(int) , sizeof(int) },
		{ 229 , 229 * sizeof(int) , sizeof(int) },
		{ 230 , 230 * sizeof(int) , sizeof(int) },
		{ 231 , 231 * sizeof(int) , sizeof(int) },
		{ 232 , 232 * sizeof(int) , sizeof(int) },
		{ 233 , 233 * sizeof(int) , sizeof(int) },
		{ 234 , 234 * sizeof(int) , sizeof(int) },
		{ 235 , 235 * sizeof(int) , sizeof(int) },
		{ 236 , 236 * sizeof(int) , sizeof(int) },
		{ 237 , 237 * sizeof(int) , sizeof(int) },
		{ 238 , 238 * sizeof(int) , sizeof(int) },
		{ 239 , 239 * sizeof(int) , sizeof(int) },
		{ 240 , 240 * sizeof(int) , sizeof(int) },
		{ 241 , 241 * sizeof(int) , sizeof(int) },
		{ 242 , 242 * sizeof(int) , sizeof(int) },
		{ 243 , 243 * sizeof(int) , sizeof(int) },
		{ 244 , 244 * sizeof(int) , sizeof(int) },
		{ 245 , 245 * sizeof(int) , sizeof(int) },
		{ 246 , 246 * sizeof(int) , sizeof(int) },
		{ 247 , 247 * sizeof(int) , sizeof(int) },
		{ 248 , 248 * sizeof(int) , sizeof(int) },
		{ 249 , 249 * sizeof(int) , sizeof(int) },
		{ 250 , 250 * sizeof(int) , sizeof(int) }
	};

	VkSpecializationInfo mVertexSpecializationInfo = 
	{
		251,                                           // mapEntryCount
		mSpecializationMapEntries,                     // pMapEntries
		sizeof(SpecializationConstants),               // dataSize
		nullptr,// pData
	};

	VkSpecializationInfo mPixelSpecializationInfo =
	{
		251,                                           // mapEntryCount
		mSpecializationMapEntries,                     // pMapEntries
		sizeof(SpecializationConstants),               // dataSize
		nullptr,// pData
	};

	VkSampler mSampler = VK_NULL_HANDLE;
	VkImage mImage = VK_NULL_HANDLE;
	VkImageLayout mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkDeviceMemory mDeviceMemory = VK_NULL_HANDLE;
	VkImageView mImageView = VK_NULL_HANDLE;
	int32_t mTextureWidth = 0;
	int32_t mTextureHeight = 0;
	
	int32_t mVertexCount = 0;

	VkBuffer mLightBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mLightBufferMemory = VK_NULL_HANDLE;
	VkBuffer mMaterialBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mMaterialBufferMemory = VK_NULL_HANDLE;


	boost::container::small_vector< std::shared_ptr<SamplerRequest>, 16> mSamplerRequests;
	boost::container::small_vector< std::shared_ptr<DrawContext>, 16> mDrawBuffer;

	boost::container::small_vector< std::shared_ptr<ResourceContext>, 16> mUsedResourceBuffer;
	boost::container::small_vector< std::shared_ptr<ResourceContext>, 16> mUnusedResourceBuffer;

	VkDescriptorSet mLastDescriptorSet = VK_NULL_HANDLE;
	VkPipeline mLastVkPipeline = VK_NULL_HANDLE;

	Transformations mTransformations;

	float mEpsilon = std::numeric_limits<float>::epsilon();

	void BeginDraw(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type);
	void CreatePipe(std::shared_ptr<DrawContext> context);
	void CreateDescriptorSet(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext);
	void CreateSampler(std::shared_ptr<SamplerRequest> request);

	void UpdateBuffer();

	void UpdatePushConstants(std::shared_ptr<DrawContext> context);
	void FlushDrawBufffer();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& deviceMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

private:

};

#endif // BUFFERMANAGER_H
