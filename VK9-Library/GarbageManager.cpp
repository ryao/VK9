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
#include "GarbageManager.h"


GarbageManager::GarbageManager()
{

}

GarbageManager::~GarbageManager()
{
	this->DestroyHandles();
}

void GarbageManager::DestroyHandles()
{
	//Images
	for (size_t i = 0; i < mImages.size(); i++)
	{
		if (mImages[i] != VK_NULL_HANDLE)
		{
			vkDestroyImage(mDevice, mImages[i], NULL);
		}
	}
	mImages.clear();

	//Memories
	for (size_t i = 0; i < mMemories.size(); i++)
	{
		if (mMemories[i] != VK_NULL_HANDLE)
		{
			vkFreeMemory(mDevice, mMemories[i], NULL);
		}
	}
	mMemories.clear();

	//Samplers
	for (size_t i = 0; i < mSamplers.size(); i++)
	{
		if (mSamplers[i] != VK_NULL_HANDLE)
		{
			vkDestroySampler(mDevice, mSamplers[i], NULL);
		}
	}
	mSamplers.clear();

	//DescriptorSets
	for (size_t i = 0; i < mDescriptorSets.size(); i++)
	{
		if (mDescriptorSets[i] != VK_NULL_HANDLE)
		{
			vkFreeDescriptorSets(mDevice, mDescriptorPool, 1, &mDescriptorSets[i]);
		}
	}
	mDescriptorSets.clear();

}
