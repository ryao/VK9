/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

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
#include <atomic>
#include <memory>
#include <vector>
#include <boost/container/small_vector.hpp>
#include <vulkan/vulkan.hpp>

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

struct RealInstance
{
	//VkInstance

	vk::Instance mInstance;

	RealInstance();
	~RealInstance();
};

struct StateManager
{
	boost::container::small_vector< std::shared_ptr<RealInstance> ,1> mInstances;
	std::atomic_size_t mInstanceKey = 0;

	StateManager();
	~StateManager();

	void CreateInstance();
};

#endif // STATEMANAGER_H