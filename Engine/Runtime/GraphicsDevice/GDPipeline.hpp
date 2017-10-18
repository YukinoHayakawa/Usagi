#pragma once

#include <memory>
#include <vector>

#include "NativeDataType.hpp"
#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

class VertexBuffer;
class ConstantBuffer;
class IndexBuffer;
class VertexShader;
class FragmentShader;
class GDTexture;
class GDSampler;

/**
 * \brief Captures all states required to set up the functional
 * stages of the graphics pipeline.
 * 
 * This implementation assumes the following prerequisites:
 * - An OpenGL 4.5+ context is used.
 * - The inputs in shaders use explicit location assignment in a
 *   continuous manner starting from 0.
 * - Uniforms are wrapped within interface blocks with std140 layout.
 * 
 * todo: optimization: no-op if last assembled pipeline is the same.
 */
class GDPipeline : public Noncopyable
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
		 * todo: is this field redundant to vector index? - we may omit some index to apply default values to those inputs
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

	virtual void vpSetVertexInputFormat(const std::vector<VertexAttributeSource> &sources) = 0;
	virtual void vpUseIndexBuffer(const std::shared_ptr<IndexBuffer> &buffer) = 0;
	virtual void vpBindVertexBuffer(size_t slot, const std::shared_ptr<VertexBuffer> &buffer) = 0;

	/*
	 * Vertex Shading
	 * 
	 * Process vertex data in the scope of vertex shader.
	 */

	virtual void vsBindConstantBuffer(size_t slot, const std::shared_ptr<ConstantBuffer> &buffer) = 0;
	virtual void vsUseVertexShader(const std::shared_ptr<VertexShader> &shader) = 0;
    virtual void vsBindTexture(size_t slot, const std::shared_ptr<GDTexture> &texture) = 0;
    virtual void vsBindSampler(size_t slot, const std::shared_ptr<GDSampler> &sampler) = 0;

    struct SamplerUsage
    {
        size_t tex_slot;
        size_t sampler_slot;
    };
    /**
     * \brief Provide the possible combinations of textures and samplers. This information
     * may be ignored by the some implementation, but required for others.
     * \param usages 
     */
    virtual void vsSamplerUsageHint(const std::vector<SamplerUsage> &usages) = 0;

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
	virtual void raSetVertexOrder(VertexOrder order) = 0;
	/**
	 * \brief NONE by default.
	 * \param type 
	 * \return 
	 */
	virtual void raSetFaceCulling(FaceCullingType type) = 0;

	/*
	 * Fragment Shading
	 * 
	 * Use interpolated data from the vertex shader to output samples to the screen.
	 */

	virtual void fsBindConstantBuffer(size_t slot, const std::shared_ptr<ConstantBuffer> &buffer) = 0;
	virtual void fsUseFragmentShader(const std::shared_ptr<FragmentShader> &shader) = 0;

    virtual void fsBindTexture(size_t slot, const std::shared_ptr<GDTexture> &texture) = 0;
    virtual void fsBindSampler(size_t slot, const std::shared_ptr<GDSampler> &sampler) = 0;
    virtual void fsSamplerUsageHint(const std::vector<SamplerUsage> &usages) = 0;

	/*
	 * Fragment Testing
	 * 
	 * Filter out the fragments fail the tests.
	 */

	/**
	 * \brief Scissor test. Disabled by default.
	 * Set scissor rectangle area through GraphicsDevice.
	 * \param enable 
	 * \return 
	 */
	virtual void ftEnableScissorTest(bool enable) = 0;
	/**
	 * \brief Depth test. Enabled by default.
	 * \param enable 
	 * \return 
	 */
	virtual void ftEnableDepthTest(bool enable) = 0;

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
        ADD, SUBTRACT, REVERSE_SUBTRACT, MIN, MAX,
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
