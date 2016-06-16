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
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include <unordered_map>

#include "CTypes.h"

class CDevice9;

class BufferManager
{
public:
	BufferManager();
	BufferManager(CDevice9* device);
	~BufferManager();

	VkResult mResult;

	CDevice9* mDevice;
	bool mIsDirty;

	std::unordered_map<UINT, StreamSource> mStreamSources;

	VkDynamicState mDynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineColorBlendAttachmentState mPipelineColorBlendAttachmentState[1];

	VkPipelineVertexInputStateCreateInfo mPipelineVertexInputStateCreateInfo;
	VkPipelineInputAssemblyStateCreateInfo mPipelineInputAssemblyStateCreateInfo;
	VkPipelineRasterizationStateCreateInfo mPipelineRasterizationStateCreateInfo;
	VkPipelineColorBlendStateCreateInfo mPipelineColorBlendStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo mPipelineDepthStencilStateCreateInfo;
	VkPipelineViewportStateCreateInfo mPipelineViewportStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo mPipelineMultisampleStateCreateInfo;
	VkPipelineDynamicStateCreateInfo mPipelineDynamicStateCreateInfo;
	VkGraphicsPipelineCreateInfo mGraphicsPipelineCreateInfo;
	VkPipelineCacheCreateInfo mPipelineCacheCreateInfo;

	VkDescriptorSetLayoutBinding mDescriptorSetLayoutBinding;
	VkDescriptorSetLayoutCreateInfo mDescriptorSetLayoutCreateInfo;
	VkPipelineLayoutCreateInfo mPipelineLayoutCreateInfo;

	VkVertexInputBindingDescription mVertexInputBindingDescription[1];
	VkVertexInputAttributeDescription mVertexInputAttributeDescription[2];

	VkDescriptorSetLayout mDescriptorSetLayout;
	VkPipelineLayout mPipelineLayout;
	VkPipeline mPipeline;
	VkPipelineCache mPipelineCache;

	D3DPRIMITIVETYPE mLastType;

	VkPipeline GetPipeline(D3DPRIMITIVETYPE type);

private:

};

#endif // BUFFERMANAGER_H
