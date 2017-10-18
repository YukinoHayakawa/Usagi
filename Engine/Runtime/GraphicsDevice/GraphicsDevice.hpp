#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

/**
 * \brief Provide unified access to graphics device and manage hardware
 * resources. Usually it manages a graphics device context such as OpenGL
 * or DirectX context, and all resources allocated by the class is bound
 * to that context, so this class should have longer lifespan than objects
 * created using it. The allocated resources are internally linked with
 * the context, and actual actions involving the resource, such as binding
 * a shader program, is defined in the resource class. This class provides
 * necessary information for the resource to enable it complete such actions,
 * so the user of the resources don't have to know the actual type of
 * this class and resource classes.
 * 
 * This class is only intended for providing runtime support for displaying
 * rendering results of ray-traced images, and debugging. It is not supposed
 * to render complex scene geometries.
 * 
 * Responsibilities:
 * 
 * Manage GPU resources such as textures and shaders.
 * Issue drawing commands.
 * Maintain a swap chain.
 */
class GraphicsDevice : public Noncopyable
{
public:
    virtual ~GraphicsDevice() = default;
    
    /*
     * Device Context
     * 
     * The rendering context is created from the window system.
     */

    virtual void setContextCurrent() = 0;

    /*
     * Resource Management
     * 
     * The resources are bound to the rendering context the class instance
     * is holding. Make sure the resources are all released before destroying
     * the GraphicsDevice.
     */

    virtual std::shared_ptr<class GDTexture> createTexture() = 0;
    virtual std::shared_ptr<class GDSampler> createSampler() = 0;

    virtual std::shared_ptr<class ConstantBuffer> createConstantBuffer() = 0;
    virtual std::shared_ptr<class VertexBuffer> createVertexBuffer() = 0;

    virtual std::shared_ptr<class VertexShader> createVertexShader() = 0;
    virtual std::shared_ptr<class FragmentShader> createFragmentShader() = 0;

    /*
     * Swap Chain
     */
    
    virtual void swapFrameBuffers() = 0;
    virtual void clearCurrentFrameBuffer() = 0;

    /*
     * Pipeline
     */

    virtual std::shared_ptr<class GDPipeline> createPipeline() = 0;


	/*
	 * Commands
	 */

	// coordinates start from the upper-left corner, only available after
	// enabling scissoring test using RasterizerState.
    virtual void setScissorRect(int x, int y, int width, int height) = 0;

    virtual void drawPoints(size_t first, size_t num_indices) = 0;
    virtual void drawLines(size_t first, size_t num_indices) = 0;
    virtual void drawTriangles(size_t first, size_t num_indices) = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;
};

}
