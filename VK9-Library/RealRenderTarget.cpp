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

#include "RealRenderTarget.h"

RealRenderTarget::RealRenderTarget(vk::Device device, RealSurface* colorSurface, RealSurface* depthSurface)
	: mDevice(device),
	mColorSurface(colorSurface),
	mDepthSurface(depthSurface)
{
	vk::Result result;

	vk::AttachmentReference colorReference;
	colorReference.attachment = 0;
	colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::AttachmentReference depthReference;
	depthReference.attachment = 1;
	depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthReference;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	mRenderAttachments[0].format = mColorSurface->mRealFormat;
	mRenderAttachments[0].samples = vk::SampleCountFlagBits::e1;
	mRenderAttachments[0].loadOp = vk::AttachmentLoadOp::eLoad;
	mRenderAttachments[0].storeOp = vk::AttachmentStoreOp::eStore;
	mRenderAttachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	mRenderAttachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	mRenderAttachments[0].initialLayout = vk::ImageLayout::ePresentSrcKHR;
	mRenderAttachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

	mRenderAttachments[1].format = mDepthSurface->mRealFormat;
	mRenderAttachments[1].samples = vk::SampleCountFlagBits::e1;
	mRenderAttachments[1].loadOp = vk::AttachmentLoadOp::eClear;
	mRenderAttachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
	mRenderAttachments[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	mRenderAttachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	mRenderAttachments[1].initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	mRenderAttachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::RenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.attachmentCount = 2; //revisit
	renderPassCreateInfo.pAttachments = mRenderAttachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 0;
	renderPassCreateInfo.pDependencies = nullptr;

	result = mDevice.createRenderPass(&renderPassCreateInfo, nullptr, &mStoreRenderPass);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealRenderTarget::RealRenderTarget vkCreateRenderPass failed with return code of " << GetResultString((VkResult)result);
		return;
	}
	mRenderAttachments[0].loadOp = vk::AttachmentLoadOp::eClear;

	result = mDevice.createRenderPass(&renderPassCreateInfo, nullptr, &mClearRenderPass);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealRenderTarget::RealRenderTarget vkCreateRenderPass failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageView attachments[2];
	attachments[0] = mColorSurface->mStagingImageView;
	attachments[1] = mDepthSurface->mStagingImageView;

	vk::FramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.renderPass = mStoreRenderPass;
	framebufferCreateInfo.attachmentCount = 2; //revisit
	framebufferCreateInfo.pAttachments = attachments;
	framebufferCreateInfo.width = mColorSurface->mExtent.width; //revisit
	framebufferCreateInfo.height = mColorSurface->mExtent.height; //revisit
	framebufferCreateInfo.layers = 1;

	result = mDevice.createFramebuffer(&framebufferCreateInfo, nullptr, &mFramebuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealRenderTarget::RealRenderTarget vkCreateFramebuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

}

RealRenderTarget::~RealRenderTarget()
{

}