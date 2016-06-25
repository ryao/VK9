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
		size_t dataSize = ftell(fp);	
		fseek(fp, 0L, SEEK_SET);
		void* data = (uint32_t*)malloc(dataSize);
		if (fread(data, dataSize, 1, fp))
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
				size_t dataSize = SizeofResource(dllModule, hRes);
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