#include <Usagi/Engine/Extension/Vulkan/VulkanGraphicsDevice.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Runtime/Graphics/SwapChain.hpp>
#include <Usagi/Engine/Runtime/Graphics/SPIRVShader.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsPipeline.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsCommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsCommandList.hpp>

using namespace yuki;

int main(int argc, char *argv[])
{
    std::shared_ptr<Win32Window> window = std::make_shared<Win32Window>("Usagi Vulkan Test", 1280, 720);
    std::shared_ptr<VulkanGraphicsDevice> graphics_device = std::make_shared<VulkanGraphicsDevice>();
    
    window->showWindow(true);
    auto swap_chain = graphics_device->createSwapChain(window);
    auto pipeline = graphics_device->createGraphicsPipeline();
    auto command_pool = graphics_device->createGraphicsCommandPool();

    GraphicsPipelineCreateInfo graphics_pipeline_create_info;
    graphics_pipeline_create_info.vertex_shader = SPIRVShader::readFromFile(R"(D:\Development\IntroductionToVulkan\Project\Tutorial03\Data03\vert.spv)");
    graphics_pipeline_create_info.fragment_shader = SPIRVShader::readFromFile(R"(D:\Development\IntroductionToVulkan\Project\Tutorial03\Data03\frag.spv)");
    graphics_pipeline_create_info.input_assembly.topology = InputAssembleState::PrimitiveTopology::TRIANGLE_LIST;
    graphics_pipeline_create_info.rasterization.face_culling_mode = RasterizationState::FaceCullingMode::BACK;
    graphics_pipeline_create_info.rasterization.front_face = RasterizationState::FrontFace::COUNTER_CLOCKWISE;
    graphics_pipeline_create_info.rasterization.polygon_mode = RasterizationState::PolygonMode::FILL;
    graphics_pipeline_create_info.depth_stencil.enable_depth_test = false;
    graphics_pipeline_create_info.color_blend.enable = false;
    graphics_pipeline_create_info.attachment_usages.push_back({ swap_chain->getNativeImageFormat(), GraphicsImageLayout::UNDEFINED, GraphicsImageLayout::PRESENT_SRC });
    pipeline->init(graphics_pipeline_create_info);

    while(true)
    {
        window->processEvents();

        swap_chain->acquireNextImage();

        auto command_buffer = command_pool->createCommandList();

        GraphicsPipelineAssembly pipeline_assembly;
        pipeline_assembly.pipeline = pipeline.get();
        pipeline_assembly.attachments.push_back({ swap_chain->getCurrentImage() });

        command_buffer->begin(pipeline_assembly);
        command_buffer->setScissor(0, 0, 1280, 720);
        command_buffer->setViewport(0, 0, 1280, 720);
        command_buffer->draw(3, 1, 0, 0);
        command_buffer->end();

        graphics_device->submitGraphicsCommandList(command_buffer.get(), { swap_chain->accessImageAvailableSemaphore() }, { swap_chain->accessRenderingFinishedSemaphore() });

        swap_chain->present();
        graphics_device->waitIdle();

        Sleep(10);
    }

    return 0;
}
