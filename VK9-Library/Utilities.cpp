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

#include "Utilities.h"

#include "winres.h"
#include <stdio.h>
#include <stdlib.h>

HMODULE GetModule(HMODULE module)
{
	static HMODULE dllModule = 0;

	if (module != 0)
	{
		dllModule = module;
	}

	return dllModule;
}

VkShaderModule LoadShaderFromFile(VkDevice device, const char *filename)
{
	VkShaderModuleCreateInfo moduleCreateInfo = {};
	VkShaderModule module = VK_NULL_HANDLE;
	VkResult result = VK_SUCCESS;
	FILE *fp = fopen(filename, "rb");
	if (fp != nullptr)
	{
		fseek(fp, 0L, SEEK_END);
		int32_t dataSize = ftell(fp);	
		fseek(fp, 0L, SEEK_SET);
		void* data = (uint32_t*)malloc(dataSize);
		if (data != nullptr && fread(data, dataSize, 1, fp))
		{
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.pNext = NULL;
			moduleCreateInfo.codeSize = dataSize;
			moduleCreateInfo.pCode = (uint32_t*)data; //Why is this uint32_t* if the size is in bytes?
			moduleCreateInfo.flags = 0;

			result = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &module);
			if (result != VK_SUCCESS)
			{
				BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromFile vkCreateShaderModule failed with return code of " << result;
			}
			else
			{
				BOOST_LOG_TRIVIAL(info) << "LoadShaderFromFile vkCreateShaderModule succeeded.";
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromFile unable to read file.";
		}
		free(data);
		fclose(fp);
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromFile unable to open file.";
	}	

	return module;
}

VkShaderModule LoadShaderFromResource(VkDevice device, WORD resource)
{
	VkShaderModuleCreateInfo moduleCreateInfo = {};
	VkShaderModule module = VK_NULL_HANDLE;
	VkResult result = VK_SUCCESS;
	HMODULE dllModule = NULL;

	//dllModule = GetModule();
	dllModule = GetModuleHandle(L"D3d9.dll");

	if (dllModule == NULL)
	{
		BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource dllModule is null.";
	}
	else
	{
		HRSRC hRes = FindResource(dllModule, MAKEINTRESOURCE(resource), L"Shader");
		if (NULL != hRes)
		{
			HGLOBAL hData = LoadResource(dllModule, hRes);
			if (NULL != hData)
			{
				int32_t dataSize = SizeofResource(dllModule, hRes);
				uint32_t* data = (uint32_t*)LockResource(hData);

				moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				moduleCreateInfo.pNext = NULL;
				moduleCreateInfo.codeSize = dataSize;
				moduleCreateInfo.pCode = data; //Why is this uint32_t* if the size is in bytes?
				moduleCreateInfo.flags = 0;

				result = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &module);
				if (result != VK_SUCCESS)
				{
					BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource vkCreateShaderModule failed with return code of " << result;
				}
				else
				{
					BOOST_LOG_TRIVIAL(info) << "LoadShaderFromResource vkCreateShaderModule succeeded.";
				}
			}
			else
			{
				BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource resource data is null.";
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource resource not found.";
		}
	}

	return module;
}

void CopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height, uint32_t srcMip, uint32_t dstMip)
{
	VkResult result = VK_SUCCESS;

	VkImageSubresourceLayers subResource1 = {};
	subResource1.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subResource1.baseArrayLayer = 0;
	subResource1.mipLevel = srcMip;
	subResource1.layerCount = 1;

	VkImageSubresourceLayers subResource2 = {};
	subResource2.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subResource2.baseArrayLayer = 0;
	subResource2.mipLevel = dstMip;
	subResource2.layerCount = 1;

	VkImageCopy region = {};
	region.srcSubresource = subResource1;
	region.dstSubresource = subResource2;
	region.srcOffset = { 0, 0, 0 };
	region.dstOffset = { 0, 0, 0 };
	region.extent.width = width;
	region.extent.height = height;
	region.extent.depth = 1;

	vkCmdCopyImage(
		commandBuffer,
		srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &region
	);
}

void SetImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, uint32_t levelCount, uint32_t mipIndex)
{
	VkResult result = VK_SUCCESS;
	VkPipelineStageFlags sourceStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destinationStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	if (aspectMask == 0)
	{
		aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = mipIndex;
	imageMemoryBarrier.subresourceRange.levelCount = levelCount;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		break;
	case VK_IMAGE_LAYOUT_GENERAL:
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; //Added based on validation layer complaints.
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		break;
	default:
		break;
	}

	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		break;
	case VK_IMAGE_LAYOUT_GENERAL:
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.dstAccessMask |= VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		break;
	default:
		break;
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}