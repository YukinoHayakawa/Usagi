#pragma once

#include <memory>
#include <initializer_list>

#include "NativeDataType.hpp"

namespace yuki
{

class VertexBuffer;
class ConstantBuffer;
class IndexBuffer;
class VertexShader;
class FragmentShader;

/**
 * \brief Captures all states required to set up the functional
 * stages of the graphics pipeline.
 * todo: optimization: no-op if last assembled pipeline is the same.
 * todo: blending
 */
class GDPipeline
{
public:
	virtual ~GDPipeline() = default;

	/*
	 * Vertex Preparation
	 * 
	 * Prepare the data for building the vertex list. This is
	 * a configurable stage not a programmable stage.
	 */

	struct VertexAttributeSource
	{
		/**
		 * \brief The sequential number of the input attribute.
		 * When using GLSL, the user should explicitly assign
		 * the locations of each attribute, starting with 0 and
		 * without any skipping in counting. Notice that vertex
		 * inputs cannot be defined within any interface blocks.
		 * For HLSL, the semantic names are queried by their indices
		 * using shader reflection.
		 * https://www.khronos.org/opengl/wiki/Vertex_Shader#Inputs
		 */
		int					index;
		/**
		 * \brief The binding slot of vertex buffer supplying the
		 * data of this attribute.
		 */
		int					buffer_index;
		/**
		 * \brief The position of first byte of the data in the buffer.
		 */
		size_t				offset;
		NativeDataType		type;
		/**
		 * \brief The number of elements of this attribute. A non
		 * -array attribute should have this field always being 1.
		 */
		size_t				count;
		/**
		 * \brief Should the pipeline normalize the data when uploading
		 * the data to the device. Signed data will be normalized to
		 * [-1, 1] and unsigned data to [0, 1].
		 */
		bool				normalize;
	};

	virtual void vpSetVertexInputFormat(std::initializer_list<VertexAttributeSource> sources) = 0;
	virtual void vpUseIndexBuffer(std::shared_ptr<IndexBuffer> buffer) = 0;
	virtual void vpBindVertexBuffer(size_t slot, std::shared_ptr<VertexBuffer> buffer) = 0;

	/*
	 * Vertex Shading
	 * 
	 * Process vertex data in the scope of vertex shader.
	 */

	virtual void vsBindConstantBuffer(size_t slot, std::shared_ptr<ConstantBuffer> buffer) = 0;
	virtual void vsUseVertexShader(std::shared_ptr<VertexShader> shader) = 0;

	/*
	 * Primitive Assembly & Rasterization
	 * 
	 * Construct primitives (like triangles) from the processed vertices. Then convert
	 * them into fragments.
	 */

	enum class FaceCullingType
	{
		NONE, FRONT, BACK
	};
		
	enum class VertexOrder
	{
		COUNTER_CLOCKWISE, CLOCKWISE
	};
	/**
	 * \brief COUNTER_CLOCKWISE by default.
	 * \param order 
	 * \return 
	 */
	virtual void rsSetVertexOrder(VertexOrder order) = 0;
	/**
	 * \brief NONE by default.
	 * \param type 
	 * \return 
	 */
	virtual void rsSetFaceCulling(FaceCullingType type) = 0;

	/*
	 * Fragment Shading
	 * 
	 * Use interpolated data from the vertex shader to output samples to the screen.
	 */

	virtual void fsBindConstantBuffer(size_t slot, std::shared_ptr<ConstantBuffer> buffer) = 0;
	virtual void fsUseFragmentShader(std::shared_ptr<FragmentShader> shader) = 0;

	/*
	 * Fragment Screening
	 * 
	 * Filter out the fragments fail the tests.
	 */

	/**
	 * \brief Scissor test. Disabled by default.
	 * Set scissor rectangle area through GraphicsDevice.
	 * \param enable 
	 * \return 
	 */
	virtual void fsEnableScissor(bool enable) = 0;
	/**
	 * \brief Depth test. Enabled by default.
	 * \param enable 
	 * \return 
	 */
	virtual void fsEnableDepthTest(bool enable) = 0;

    /*
     * Color Blending
     */

    /**
     * \brief Color blending. Disabled by default.
     * \param enable 
     */
    virtual void bldEnableBlend(bool enable) = 0;

    enum class BlendingFactor
    {
        ZERO, ONE,
        SOURCE_ALPHA, INV_SOURCE_ALPHA,
    };

    enum class BlendingOperation
    {
        ADD
    };

    void bldSetOp(BlendingOperation op)
    {
        bldSetColorOp(op);
        bldSetAlphaOp(op);
    }

    void bldSetSrcFactor(BlendingFactor factor)
    {
        bldSetColorSrcFactor(factor);
        bldSetAlphaSrcFactor(factor);
    }

    void bldSetDestFactor(BlendingFactor factor)
    {
        bldSetColorDestFactor(factor);
        bldSetAlphaDestFactor(factor);
    }

    virtual void bldSetColorOp(BlendingOperation op) = 0;
    virtual void bldSetAlphaOp(BlendingOperation op) = 0;
    virtual void bldSetColorSrcFactor(BlendingFactor factor) = 0;
    virtual void bldSetAlphaSrcFactor(BlendingFactor factor) = 0;
    virtual void bldSetColorDestFactor(BlendingFactor factor) = 0;
    virtual void bldSetAlphaDestFactor(BlendingFactor factor) = 0;

	/**
	 * \brief Apply the states to the device context creating this pipeline.
	 */
	virtual void assemble() = 0;
};

}
