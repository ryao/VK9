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

HMODULE GetModule(HMODULE module)
{
	static HMODULE dllModule = 0;

	if (module != 0)
	{
		dllModule = module;
	}

	return dllModule;
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
				DWORD dataSize = SizeofResource(dllModule, hRes);
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
					return VK_NULL_HANDLE;
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