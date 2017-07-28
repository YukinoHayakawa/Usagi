#pragma once

#include <Usagi/Engine/Utility/BindingSentry.hpp>
#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

/**
 * \brief A memory region on GPU for transferring data between the host
 * and the graphics card. It is primarily for providing data to shader
 * programs.
 * 
 * The buffer may be used to accomplish the following tasks:
 *	- Streaming data to the shader.
 *		Before rendering of each frame, the user fill the buffer with new
 *		data, where through directly reading from a buffer on the user
 *		side, or mapping the buffer to the user side.
 *	- Linking the buffer structure with the shader input layouts.
 *		- Uniform/Constant Buffers
 *			The mainstream shader languages use different methods to provide
 *			correct data to the shader inputs. For example, in HLSL, the user
 *			directly binding constant buffers to a logical binding point
 *			declared in the shader program similar to 'register(b0)'. However,
 *			in GLSL with uniform blocks, the block index is queries using its
 *			name, and then bound to a uniform buffer binding point, which is
 *			bound to the actual buffer. This extra level of indirection makes
 *			binding a buffer to the uniform block more tricky, but this can be
 *			alleviated by using explicit bindings from OpenGL 4.3.
 *		- Vertex Buffers
 *			For vertex buffers, the situation is even more complicated as the
 *			shader may not use all data in the buffer. As a consequence, the
 *			APIs usually provide a mechanism for binding a particular input
 *			variable in the shader to a location in a buffer, with an offset
 *			and stride defined.
 *			In HLSL, this is accomplished by binding the
 *			so-called 'semantic names' to a buffer slot with the offset info.
 *			The collection of this information is encapsulated in a structure
 *			called GDInputLayout. When setting up the pipeline, the user bind
 *			the correct buffer to the slot accordingly to the layout information
 *			defined before.
 *			In OpenGL, prior to 4.3 when ARB_vertex_attrib_binding was merged into
 *			the core, the typical way of supplying the vertex data is using
 *			the vertex array object which directly binds to buffer objects.
 *			The extension then released this coupling by allowing binding
 *			shader inputs with binding slots.
 *		- Index buffer
 *			Index buffers typically contain only an array of integer values,
 *			and not visible to shaders, thus pose no extra need of specifying
 *			the format.
 */
class GDBuffer : public Noncopyable
{
protected:
	class MemoryMappingSentry : public BindingSentry
	{
	public:
		void *const storage;

		MemoryMappingSentry(void *storage)
			: BindingSentry{ []() {} }
			, storage{ storage }
		{
		}

		MemoryMappingSentry(std::function<void()> unmap_func, void *storage)
			: BindingSentry{ std::move(unmap_func) }
			, storage{ storage }
		{
		}
	};

public:
	virtual ~GDBuffer() = default;

	/**
	 * \brief Reallocate the buffer storage with the same format and size, then copy
	 * from the given data to it. An exception will be thrown if length parameter is
	 * greater than the buffer length.
	 * \param data 
	 * \param length 
	 */
	virtual void streamFromHostBuffer(const void *data, size_t length) = 0;
	/**
	 * \brief Map the buffer to client memory as write-only. It is usually
	 * for streaming new data at the beginning of each frame. Sequential writes
	 * may result in better performance as drivers usually optimize for that.
	 * \param orphan_old 
	 * \return A sentry object, containing a pointer to the mapped memory region called
	 * <storage> which may be write to.
	 */
	virtual MemoryMappingSentry mapWrite(bool orphan_old) = 0;
};

}
