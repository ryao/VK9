#include "DrawContext.h"

DrawContext::~DrawContext()
{
	if (mRealWindow != nullptr)
	{
		//BOOST_LOG_TRIVIAL(warning) << "DrawContext::~DrawContext";
		auto& device = mRealWindow->mRealDevice->mDevice;
		device.destroyPipeline(Pipeline, nullptr);
		device.destroyPipelineLayout(PipelineLayout, nullptr);
		device.destroyDescriptorSetLayout(DescriptorSetLayout, nullptr);
	}
}