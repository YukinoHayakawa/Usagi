#include "VulkanGraphicsPipelineCompiler.hpp"

#include <cstdint>

#include <Usagi/Runtime/Graphics/Shader/SpirvBinary.hpp>
#include <Usagi/Utility/TypeCast.hpp>
#include <Usagi/Core/Logging.hpp>

#include "VulkanGpuDevice.hpp"
#include "VulkanEnumTranslation.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanGraphicsPipeline.hpp"

using namespace spirv_cross;
using namespace usagi::vulkan;

vk::UniqueShaderModule usagi::VulkanGraphicsPipelineCompiler::
    createShaderModule(const SpirvBinary *binary) const
{
    auto &bytecodes = binary->bytecodes();
	vk::ShaderModuleCreateInfo create_info;
	create_info.setCodeSize(bytecodes.size() * sizeof(SpirvBinary::Bytecode));
	create_info.setPCode(bytecodes.data());
	return mDevice->device().createShaderModuleUnique(create_info);
}

void usagi::VulkanGraphicsPipelineCompiler::setupShaderStages()
{
    mShaderStageCreateInfos.clear();

	if(mShaders.count(ShaderStage::VERTEX) == 0)
	{
        LOG(error, "Pipeline does not contain vertex stage.");
        return;
	}
    if(mShaders.count(ShaderStage::FRAGMENT) == 0)
    {
        LOG(warn, "Pipeline does not contain fragment stage.");
    }

    for(auto &&shader : mShaders)
    {
        LOG(info, "Adding {} stage", to_string(shader.first));

        vk::PipelineShaderStageCreateInfo info;
        info.setStage(translate(shader.first));
        if(!shader.second.module)
            shader.second.module = createShaderModule(
                shader.second.binary.get());
        info.setModule(shader.second.module.get());
        info.setPName(shader.second.entry_point.c_str());

        mShaderStageCreateInfos.push_back(info);
    }
    mPipelineCreateInfo.setStageCount(
        static_cast<uint32_t>(mShaderStageCreateInfos.size()));
    mPipelineCreateInfo.setPStages(mShaderStageCreateInfos.data());
}

void usagi::VulkanGraphicsPipelineCompiler::setupVertexInput()
{
    mVertexInputStateCreateInfo.setVertexBindingDescriptionCount(
        static_cast<uint32_t>(mVertexInputBindings.size()));
    mVertexInputStateCreateInfo.setPVertexBindingDescriptions(
        mVertexInputBindings.data());
    mVertexInputStateCreateInfo.setVertexAttributeDescriptionCount(
        static_cast<uint32_t>(mVertexAttributeLocationArray.size()));
    mVertexInputStateCreateInfo.setPVertexAttributeDescriptions(
        mVertexAttributeLocationArray.data());
    mPipelineCreateInfo.setPVertexInputState(&mVertexInputStateCreateInfo);
}

void usagi::VulkanGraphicsPipelineCompiler::setupDynamicStates()
{
    // todo optional
    // Viewport and scissor should be set after binding the pipeline
    mDynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor,
        vk::DynamicState::eLineWidth,
	};
	mDynamicStateCreateInfo.dynamicStateCount =
        static_cast<uint32_t>(mDynamicStates.size());
	mDynamicStateCreateInfo.pDynamicStates = mDynamicStates.data();
	mPipelineCreateInfo.pDynamicState = &mDynamicStateCreateInfo;
}

void usagi::VulkanGraphicsPipelineCompiler::setRenderPass(
    const std::shared_ptr<RenderPass> render_pass)
{
    mRenderPass = dynamic_pointer_cast_throw<VulkanRenderPass>(render_pass);
    mPipelineCreateInfo.setRenderPass(mRenderPass->renderPass());
}

struct usagi::VulkanGraphicsPipelineCompiler::Context
{
    // Descriptor Set Layouts
    VulkanGraphicsPipeline::DescriptorSetLayoutBindingMap
        desc_set_layout_bindings;
    VulkanGraphicsPipeline::DescriptorSetLayoutMap desc_set_layouts;
    std::vector<vk::DescriptorSetLayout> desc_set_layout_array;

    // Push Constants
    std::vector<vk::PushConstantRange> push_constants;
    VulkanGraphicsPipeline::PushConstantFieldMap push_constant_field_map;
    // Allocate enough space for push constant buffer
    // all shader stages have the same binding of the constant buffer
    // (see the same memory). they must pad the buffer correctly to get
    // the desired data.
    // todo if multiple push constant buffer are supported in the future
    // a separate max size should be calculated for each buffer
    std::size_t max_push_constant_size = 0;
};

struct usagi::VulkanGraphicsPipelineCompiler::ReflectionHelper
{
    Context &ctx;
    VulkanGraphicsPipelineCompiler *p = nullptr;
    const ShaderMap::value_type &shader;
    const Compiler & compiler;
    ShaderResources resources;
    std::size_t push_constant_offset = 0;
    std::size_t push_constant_size = 0;
    VulkanGraphicsPipeline::PushConstantFieldMap::value_type::second_type &
        push_constant_fields;

    ReflectionHelper(
        Context &ctx,
        VulkanGraphicsPipelineCompiler *pipeline_compiler,
        const ShaderMap::value_type &shader)
        : ctx { ctx }
        , p { pipeline_compiler }
        , shader { shader }
        , compiler { shader.second.binary->reflectionCompiler() }
        , resources { compiler.get_shader_resources() }
        , push_constant_fields { ctx.push_constant_field_map[shader.first] }
    {
    }

    void reflectPushConstantField(
        const Resource &resource, const SPIRType &type, unsigned i)
    {
        const auto &member_name =
            compiler.get_member_name(type.self, i);
        const auto member_offset =
            compiler.type_struct_member_offset(type, i);
        const auto member_size =
            compiler.get_declared_struct_member_size(type, i);

        // if first member is named padding, it is considered as offset hint
        // and is ignored.
        if(i == 0 && member_name == "padding")
        {
            push_constant_offset = member_size;
            return;
        }

        LOG(info, "{}: offset={}, size={}",
            member_name, member_offset, member_size);

        VulkanPushConstantField field;
        field.size = static_cast<uint32_t>(member_size);
        field.offset = static_cast<uint32_t>(member_offset);

        // add to reflection record
        // todo: if multiple constant buffers are allowed in the
        // future, member name may not uniquely identify the fields.
        // use struct_name.field_name instead?
        push_constant_fields[member_name] = field;
    }

    std::size_t reflectPushConstantBuffer(
        const Resource &resource)
    {
        const auto &type = compiler.get_type(resource.base_type_id);
        const auto size = compiler.get_declared_struct_size(type);

        const auto member_count = type.member_types.size();
        for(unsigned i = 0; i < member_count; i++)
            reflectPushConstantField(resource, type, i);

        return size;
    }

    void reflectPushConstantRanges()
    {
        for(const auto &resource : resources.push_constant_buffers)
            push_constant_size += reflectPushConstantBuffer(resource);

        if(push_constant_size == 0) return;

        vk::PushConstantRange range;
        // todo allow multiple stages in one shader source
        range.setOffset(static_cast<uint32_t>(push_constant_offset));
        range.setSize(static_cast<uint32_t>(
            push_constant_size - push_constant_offset));
        range.setStageFlags(translate(shader.first));
        ctx.push_constants.push_back(range);

        ctx.max_push_constant_size = std::max(
            ctx.max_push_constant_size,
            push_constant_size
        );
    }

    void reflectVertexInputAttributes()
    {
        LOG(info, "Vertex input attribtues:");

        for(auto &&resource : resources.stage_inputs)
        {
            const auto location = compiler.get_decoration(
                resource.id, spv::DecorationLocation);

            LOG(info, "{}: location={}", resource.name, location);

            // normalize to location-based indexing
            const auto it = p->mVertexAttributeNameMap.find(resource.name);
            if(it != p->mVertexAttributeNameMap.end())
            {
                it->second.location = location;
                p->mVertexAttributeLocationArray.push_back(it->second);
                p->mVertexAttributeNameMap.erase(it);
            }
        }
    }

    void addResource(std::vector<Resource>::value_type &resource,
        const vk::DescriptorType resource_type) const
    {
        // todo: ensure different stages uses different set indices
        const auto set = compiler.get_decoration(
            resource.id, spv::DecorationDescriptorSet);
        const auto binding = compiler.get_decoration(
            resource.id, spv::DecorationBinding);
        const auto &type = compiler.get_type(resource.type_id);

        vk::DescriptorSetLayoutBinding layout_binding;
        layout_binding.setStageFlags(translate(shader.first));
        layout_binding.setBinding(binding);
        layout_binding.setDescriptorCount(type.vecsize);
        layout_binding.setDescriptorType(resource_type);

        ctx.desc_set_layout_bindings[set].push_back(layout_binding);
    }

    void ignoreResource(std::vector<Resource>::value_type &resource,
        const vk::DescriptorType resource_type) const
    {
        const auto set = compiler.get_decoration(
            resource.id, spv::DecorationDescriptorSet);
        const auto binding = compiler.get_decoration(
            resource.id, spv::DecorationBinding);

        LOG(warn, "{type} {name} (set={},binding={}) is ignored.",
            to_string(resource_type), resource.name, set, binding);
    }

    void reflectDescriptorSets()
    {
        LOG(info, "Descriptor set layouts:");

        // todo: deal with others resource types

        for(auto &&resource : resources.storage_buffers)
            ignoreResource(resource, vk::DescriptorType::eStorageBuffer);

        // sampler2D is not supported in HLSL so not included here.
        // don't use them in shaders.
        for(auto &&resource : resources.sampled_images)
            ignoreResource(resource, vk::DescriptorType::eSampledImage);

        for(auto &&resource : resources.separate_images)
            addResource(resource, vk::DescriptorType::eSampledImage);

        for(auto &&resource : resources.separate_samplers)
            addResource(resource, vk::DescriptorType::eSampler);

        for(auto &&resource : resources.uniform_buffers)
            addResource(resource, vk::DescriptorType::eUniformBuffer);

        // note that only one subpass is used to maintain compatibility
        // for shader cross-compiling
        for(auto &&resource : resources.subpass_inputs)
            addResource(resource, vk::DescriptorType::eInputAttachment);
    }

    // todo auto get render targets?
    //void reflectRenderTargets()
    //{
    //    for(const auto &resource : resources.stage_outputs)
    //    {
    //        const auto attachment_index = compiler.get_decoration(
    //            resource.id, spv::DecorationInputAttachmentIndex);
    //    }
    //}
};

std::shared_ptr<usagi::GraphicsPipeline>
    usagi::VulkanGraphicsPipelineCompiler::compile()
{
	LOG(info, "Compiling graphics pipeline...");

	setupShaderStages();
	setupDynamicStates();

	LOG(info, "Generating pipeline layout...");

    Context ctx;

	for(auto &&shader : mShaders)
	{
        LOG(info, "Reflecting {} shader", to_string(shader.first));

        ReflectionHelper helper { ctx, this, shader };
        if(shader.first == ShaderStage::VERTEX)
            helper.reflectVertexInputAttributes();
        // todo
		//if(shader.first == ShaderStage::FRAGMENT)
        //    helper.reflectRenderTargets();
        helper.reflectPushConstantRanges();
        helper.reflectDescriptorSets();
	}

    vk::UniquePipelineLayout compatible_pipeline_layout;
	{
		vk::PipelineLayoutCreateInfo info;

        for(auto &&layout : ctx.desc_set_layout_bindings)
        {
            vk::DescriptorSetLayoutCreateInfo layout_info;
            layout_info.setBindingCount(
                static_cast<uint32_t>(layout.second.size()));
            layout_info.setPBindings(layout.second.data());
            auto l = mDevice->device().createDescriptorSetLayoutUnique(
				layout_info);
            ctx.desc_set_layout_array.push_back(l.get());
            ctx.desc_set_layouts[layout.first] = std::move(l);
        }
        info.setSetLayoutCount(
            static_cast<uint32_t>(ctx.desc_set_layout_array.size()));
        info.setPSetLayouts(ctx.desc_set_layout_array.data());

        info.setPushConstantRangeCount(
            static_cast<uint32_t>(ctx.push_constants.size()));
        info.setPPushConstantRanges(ctx.push_constants.data());

		compatible_pipeline_layout =
			mDevice->device().createPipelineLayoutUnique(info);
		mPipelineCreateInfo.setLayout(compatible_pipeline_layout.get());
	}

    setupVertexInput();

	auto pipeline = mDevice->device().createGraphicsPipelineUnique(
		{ }, mPipelineCreateInfo);
    auto wrapped_pipeline = std::make_shared<VulkanGraphicsPipeline>(
        std::move(pipeline),
        std::move(compatible_pipeline_layout),
        mRenderPass,
        std::move(ctx.desc_set_layout_bindings),
        std::move(ctx.desc_set_layouts),
        std::move(ctx.push_constant_field_map)
    );

    if(!mParentPipeline)
    {
        mParentPipeline = wrapped_pipeline;
        // if more pipelines are created using this compiler, they will be
        // the children of the first one
        mPipelineCreateInfo.setFlags(vk::PipelineCreateFlagBits::eDerivative);
        mPipelineCreateInfo.setBasePipelineHandle(mParentPipeline->pipeline());
        mPipelineCreateInfo.setBasePipelineIndex(-1);
    }

    return std::move(wrapped_pipeline);
}

usagi::VulkanGraphicsPipelineCompiler::VulkanGraphicsPipelineCompiler(
    VulkanGpuDevice *device)
    : mDevice { device }
{
    // IA
    mPipelineCreateInfo.setPInputAssemblyState(&mInputAssemblyStateCreateInfo);
    setInputAssemblyState({ });

    // Viewport (configured as dynamic state in command lists)
    // todo: multiple viewports
    mViewportStateCreateInfo.setViewportCount(1);
    mViewportStateCreateInfo.setScissorCount(1);
    mPipelineCreateInfo.setPViewportState(&mViewportStateCreateInfo);

    // Rasterization
    mPipelineCreateInfo.setPRasterizationState(&mRasterizationStateCreateInfo);
    setRasterizationState({ });

    // Multi-sampling
    // todo: anti-aliasing
    mMultisampleStateCreateInfo.setRasterizationSamples(
        vk::SampleCountFlagBits::e1);
    mMultisampleStateCreateInfo.setMinSampleShading(1.f);
    mPipelineCreateInfo.setPMultisampleState(&mMultisampleStateCreateInfo);

    // Depth/stencil
    mPipelineCreateInfo.setPDepthStencilState(&mDepthStencilStateCreateInfo);
    setDepthStencilState({ });

    // Blending
    // todo: multiple attachments
    mColorBlendStateCreateInfo.setAttachmentCount(1);
    mColorBlendStateCreateInfo.setPAttachments(&mColorBlendAttachmentState);
    mPipelineCreateInfo.setPColorBlendState(&mColorBlendStateCreateInfo);
    setColorBlendState({ });

    // Derivatives
    mPipelineCreateInfo.setFlags(
        vk::PipelineCreateFlagBits::eAllowDerivatives);
}

void usagi::VulkanGraphicsPipelineCompiler::setShader(
    const ShaderStage stage,
    std::shared_ptr<SpirvBinary> shader)
{
    ShaderInfo info;
    info.binary = std::move(shader);
    mShaders[stage] = std::move(info);
}

void usagi::VulkanGraphicsPipelineCompiler::setVertexBufferBinding(
    const std::uint32_t binding_index,
    const std::uint32_t stride,
    const VertexInputRate input_rate)
{
    vk::VertexInputBindingDescription vulkan_binding;
    vulkan_binding.setBinding(binding_index);
    vulkan_binding.setStride(stride);
    vulkan_binding.setInputRate(translate(input_rate));

    const auto iter = std::find_if(
        mVertexInputBindings.begin(), mVertexInputBindings.end(),
        [=](auto &&item) { return item.binding == binding_index; }
    );
    if(iter != mVertexInputBindings.end())
        *iter = vulkan_binding; // update old info
    else
        mVertexInputBindings.push_back(vulkan_binding);
}

void usagi::VulkanGraphicsPipelineCompiler::setVertexAttribute(
    std::string attr_name,
    const std::uint32_t binding_index,
    const std::uint32_t offset,
    const GpuBufferFormat source_format)
{
    vk::VertexInputAttributeDescription vulkan_attr;
    // Deferred to compilation when shader reflection is available.
    // vulkan_attr.setLocation();
    vulkan_attr.setBinding(binding_index);
    vulkan_attr.setOffset(offset);
    vulkan_attr.setFormat(translate(source_format));
    mVertexAttributeNameMap.insert(std::make_pair(attr_name, vulkan_attr));
}

void usagi::VulkanGraphicsPipelineCompiler::setVertexAttribute(
    const std::uint32_t attr_location,
    const std::uint32_t binding_index,
    const std::uint32_t offset,
    const GpuBufferFormat source_format)
{
    vk::VertexInputAttributeDescription vulkan_attr;
    vulkan_attr.setLocation(attr_location);
    vulkan_attr.setBinding(binding_index);
    vulkan_attr.setOffset(offset);
    vulkan_attr.setFormat(translate(source_format));
    mVertexAttributeLocationArray.push_back(vulkan_attr);
}

void usagi::VulkanGraphicsPipelineCompiler::iaSetPrimitiveTopology(
    const PrimitiveTopology topology)
{
    mInputAssemblyStateCreateInfo.setTopology(
        translate(topology));
}

void usagi::VulkanGraphicsPipelineCompiler::setInputAssemblyState(
    const InputAssemblyState &state)
{
    iaSetPrimitiveTopology(state.topology);
}

void usagi::VulkanGraphicsPipelineCompiler::rsSetPolygonMode(
    const PolygonMode mode)
{
    mRasterizationStateCreateInfo.setPolygonMode(translate(mode));
}

void usagi::VulkanGraphicsPipelineCompiler::rsSetFaceCullingMode(
    const FaceCullingMode mode)
{
    mRasterizationStateCreateInfo.setCullMode(translate(mode));
}

void usagi::VulkanGraphicsPipelineCompiler::rsSetFrontFace(
    const FrontFace face)
{
    mRasterizationStateCreateInfo.setFrontFace(translate(face));
}

void usagi::VulkanGraphicsPipelineCompiler::setRasterizationState(
    const RasterizationState &state)
{
    mRasterizationStateCreateInfo.setDepthBiasEnable(false);
    rsSetPolygonMode(state.polygon_mode);
    rsSetFaceCullingMode(state.face_culling_mode);
    rsSetFrontFace(state.front_face);
    mRasterizationStateCreateInfo.setLineWidth(1.f);
}

void usagi::VulkanGraphicsPipelineCompiler::omSetDepthEnabled(bool enabled)
{
    mDepthStencilStateCreateInfo.setDepthTestEnable(enabled);
    mDepthStencilStateCreateInfo.setDepthWriteEnable(enabled);
}

void usagi::VulkanGraphicsPipelineCompiler::setDepthStencilState(
    const DepthStencilState &state)
{
    mDepthStencilStateCreateInfo.setDepthTestEnable(state.depth_test_enable);
    mDepthStencilStateCreateInfo.setDepthWriteEnable(state.depth_write_enable);
    mDepthStencilStateCreateInfo.setDepthCompareOp(
        translate(state.depth_compare_op));
}

void usagi::VulkanGraphicsPipelineCompiler::omSetColorBlendEnabled(bool enabled)
{
    mColorBlendAttachmentState.setBlendEnable(enabled);
}

void usagi::VulkanGraphicsPipelineCompiler::setColorBlendState(
    const ColorBlendState &state)
{
    omSetColorBlendEnabled(state.enable);
    mColorBlendAttachmentState.setColorBlendOp(
        translate(state.color_blend_op));
    mColorBlendAttachmentState.setAlphaBlendOp(
        translate(state.alpha_blend_op));
    mColorBlendAttachmentState.setSrcColorBlendFactor(
        translate(state.src_color_factor));
    mColorBlendAttachmentState.setDstColorBlendFactor(
        translate(state.dst_color_factor));
    mColorBlendAttachmentState.setSrcAlphaBlendFactor(
        translate(state.src_alpha_factor));
    mColorBlendAttachmentState.setDstAlphaBlendFactor(
        translate(state.dst_alpha_factor));
    mColorBlendAttachmentState.setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
}
