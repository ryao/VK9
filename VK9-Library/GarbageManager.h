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

#ifndef GARBAGEMANAGER_H
#define GARBAGEMANAGER_H

#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>

/*
D3d9 will hold unto references to resources while they are in use and free them after if there are no other references. 
To mirror this functionality this class will hold handles until after a frame has been drawn and then free them.
*/
class GarbageManager
{
	public:
		GarbageManager();
		~GarbageManager();

		void DestroyHandles();

		//Handles to destroy with.
		VkDevice mDevice = VK_NULL_HANDLE;
		VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;

		//Handles to destroy	
		std::vector<VkImage> mImages;
		std::vector<VkDeviceMemory> mMemories;
		std::vector<VkSampler> mSamplers;
		std::vector<VkDescriptorSet> mDescriptorSets;

};

#endif // GARBAGEMANAGER_H
