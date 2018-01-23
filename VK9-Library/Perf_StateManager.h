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

#include "Utilities.h"

#ifdef _DEBUG
#include "renderdoc_app.h"
#endif // _DEBUG

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData);

struct RealInstance
{
	vk::Instance mInstance;
	vk::PhysicalDevice* mPhysicalDevices = nullptr;
	uint32_t mPhysicalDeviceCount = 0;

	boost::container::small_vector<RealDevice, 1> mDevices;

#ifdef _DEBUG
	RENDERDOC_API_1_1_1* mRenderDocApi = nullptr;
	vk::DebugReportCallbackEXT mCallback;
#endif // _DEBUG

	RealInstance();
	~RealInstance();
};

struct RealDevice
{
	//Feature and property information
	vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
	vk::PhysicalDeviceFeatures mPhysicalDeviceFeatures;
	vk::PhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
	vk::QueueFamilyProperties* mQueueFamilyProperties;
	uint32_t mQueueFamilyPropertyCount;

	//Stuff that does things.
	vk::Device mDevice;
	vk::DescriptorPool mDescriptorPool;

	RealDevice();
	~RealDevice();
};

struct RealWindow
{
	RealInstance& mRealInstance;
	RealDevice& mRealDevice;

	//Command, queue, and render pass stuff
	vk::CommandPool mCommandPool;
	vk::Queue mQueue;
	vk::CommandBuffer* mSwapchainBuffers;
	uint32_t mCurrentSwapchainBuffer = 0;
	vk::AttachmentDescription mRenderAttachments[2];
	vk::RenderPass mStoreRenderPass;
	vk::RenderPass mClearRenderPass;
	vk::Framebuffer* mFramebuffers;
	vk::CommandBufferInheritanceInfo mCommandBufferInheritanceInfo;
	vk::CommandBufferBeginInfo mCommandBufferBeginInfo;
	vk::RenderPassBeginInfo mRenderPassBeginInfo;
	vk::ImageMemoryBarrier mImageMemoryBarrier;
	vk::Extent2D mSwapchainExtent;
	vk::ClearValue mClearValues[2];
	vk::ColorSpaceKHR mColorSpace;
	vk::ClearColorValue mClearColorValue;
	vk::SubmitInfo mSubmitInfo;
	vk::PushConstantRange mPushConstants[1];
	vk::PipelineStageFlags mPipeStageFlags;

	bool mIsSceneStarted = false;

	//Surface stuff
	vk::SurfaceKHR mSurface;
	vk::SurfaceCapabilitiesKHR mSurfaceCapabilities;
	uint32_t mSurfaceFormatCount;
	vk::SurfaceFormatKHR* mSurfaceFormats;

	//Presentation Mode stuff
	uint32_t mPresentationModeCount;
	vk::PresentModeKHR* mPresentationModes;
	vk::SemaphoreCreateInfo mPresentCompleteSemaphoreCreateInfo;
	vk::Semaphore mPresentCompleteSemaphore;
	vk::PresentInfoKHR mPresentInfo;
	vk::ImageMemoryBarrier mPrePresentBarrier;

	//Swapchain & depth stuff
	vk::SwapchainKHR mSwapchain;
	vk::PresentModeKHR mSwapchainPresentMode;
	uint32_t mSwapchainImageCount;
	vk::Image* mSwapchainImages;
	vk::ImageView* mSwapchainViews;
	vk::Image mDepthImage;
	vk::DeviceMemory mDepthDeviceMemory;
	vk::ImageView mDepthView;

	//Misc Handles
	vk::Fence mNullFence;

	//Misc State
	DeviceState mDeviceState = {};

	RealWindow(RealInstance& realInstance,RealDevice& realDevice);
	~RealWindow();

	void SetImageLayout(vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, uint32_t levelCount = 1, uint32_t mipIndex = 0, uint32_t layerCount = 1);
};

struct StateManager
{
	boost::container::small_vector< std::shared_ptr<RealInstance> ,1> mInstances;
	std::atomic_size_t mInstanceKey = 0;

	boost::container::small_vector< std::shared_ptr<RealWindow>, 1> mWindows;
	std::atomic_size_t mWindowsKey = 0;

	StateManager();
	~StateManager();

	void DestroyWindow(size_t id);
	void CreateWindow1(size_t id, void* argument1, void* argument2);

	void DestroyInstance(size_t id);
	void CreateInstance();
};

#endif // STATEMANAGER_H