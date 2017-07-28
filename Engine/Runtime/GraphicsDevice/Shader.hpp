#pragma once

#include <string>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{

class Shader : public Noncopyable
{
public:
    virtual ~Shader() = default;

    virtual void useSourceString(std::string source_code) = 0;
    virtual void readFromSourceFile(const std::string &file_path) = 0;

    virtual void compile() = 0;
};

class VertexShader: public virtual Shader
{
public:
};

class FragmentShader : public virtual Shader
{
public:
};

}
