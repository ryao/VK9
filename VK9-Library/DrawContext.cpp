#include "DrawContext.h"

DrawContext::~DrawContext()
{
	if (mRealDevice != nullptr)
	{
		//BOOST_LOG_TRIVIAL(warning) << "DrawContext::~DrawContext";
		auto& device = mRealDevice->mDevice;
		device.destroyPipeline(Pipeline, nullptr);
		device.destroyPipelineLayout(PipelineLayout, nullptr);
		device.destroyDescriptorSetLayout(DescriptorSetLayout, nullptr);
	}
}