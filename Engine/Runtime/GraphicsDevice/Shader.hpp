#pragma once

#include <string>
#include <functional>

#include <Usagi/Engine/Utility/BindingSentry.hpp>

namespace yuki
{

class VertexBuffer;

class Shader
{
public:
    virtual ~Shader() = default;

    virtual Shader & setVertexShaderSource(const std::string &src) = 0;
    virtual Shader & setFragmentShaderSource(const std::string &src) = 0;
    virtual Shader & compile() = 0;

    /**
     * \brief Bind input variables to corresponding entries in the given vertex buffer.
     * Must be called before each call to bind() to ensure correct layout is used.
     * However, implementation may deploy caching to improve performance.
     * 
     * todo: support multiple vertex buffers
     * \param vbo 
     * \return 
     */
    virtual Shader & linkInputs(VertexBuffer &vbo) = 0;

    virtual BindingSentry bind() = 0;
};

}
