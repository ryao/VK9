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
	D3DFILLMODE FillMode = D3DFILL_FORCE_DWORD;
	D3DCULL CullMode = D3DCULL_FORCE_DWORD;

	//D3d9 State - Lights
	D3DSHADEMODE ShadeMode = D3DSHADE_FORCE_DWORD;
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
	VkPushConstantRange mPushConstantRanges[1] = {};
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

	VkShaderModule mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;
	VkShaderModule mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = VK_NULL_HANDLE;

	const VkSpecializationMapEntry mSpecializationMapEntries[148] =		
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
		{ 147 , 147 * sizeof(int) , sizeof(int) }

	};

	VkSpecializationInfo mSpecializationInfo = 
	{
		148,                                           // mapEntryCount
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

	Eigen::Matrix4f mModel;
	Eigen::Matrix4f mView;
	Eigen::Matrix4f mProjection;
	Eigen::Matrix4f mTotalTransformation;

	float mEpsilon = std::numeric_limits<float>::epsilon();

	void BeginDraw(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext, D3DPRIMITIVETYPE type);
	void CreatePipe(std::shared_ptr<DrawContext> context);
	void CreateDescriptorSet(std::shared_ptr<DrawContext> context, std::shared_ptr<ResourceContext> resourceContext);
	void CreateSampler(std::shared_ptr<SamplerRequest> request);

	void UpdateUniformBuffer(std::shared_ptr<DrawContext> context);
	void FlushDrawBufffer();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& deviceMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

private:

};

#endif // BUFFERMANAGER_H
