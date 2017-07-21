#include <vector>
#include <cassert>

#include <Usagi/Engine/Runtime/GraphicsDevice/VertexBuffer.hpp>
#include "OpenGLCommon.hpp"

#include "OpenGLShader.hpp"

void yuki::OpenGLShader::_compileShader(GLenum shaderType, const std::string &source)
{
    GLuint &target = shaderType == GL_VERTEX_SHADER ? mVertShader : mFragShader;

    target = glCreateShader(shaderType);
    
    const char *src = source.c_str();
    glShaderSource(target, 1, &src, nullptr);
    printf("Compiling shader\n");
    glCompileShader(target);

    GLint result;
    int info_log_length;

    glGetShaderiv(target, GL_COMPILE_STATUS, &result);
    glGetShaderiv(target, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0)
    {
        std::vector<char> info_log(info_log_length + 1);
        glGetShaderInfoLog(target, info_log_length, nullptr, &info_log[0]);
        printf("%s\n", &info_log[0]);
    }

    // put at the end so we can see the compiling error log
    YUKI_OPENGL_CHECK();
}

yuki::OpenGLShader::OpenGLShader()
{
    mProgramId = glCreateProgram();
}

void yuki::OpenGLShader::_deleteVAO()
{
    if(mVAO) glDeleteVertexArrays(0, &mVAO);
    mVAO = 0;
}

void yuki::OpenGLShader::_deleteShaders()
{
    if(mVertShader)
    {
        glDeleteShader(mVertShader);
        mVertShader = 0;
    }
    if(mFragShader)
    {
        glDeleteShader(mFragShader);
        mFragShader = 0;
    }
}

yuki::OpenGLShader::~OpenGLShader()
{
    if(mProgramId) glDeleteProgram(mProgramId);
    _deleteShaders();
    _deleteVAO();
}

yuki::OpenGLShader & yuki::OpenGLShader::setVertexShaderSource(const std::string &src)
{
    _compileShader(GL_VERTEX_SHADER, src);
    return *this;
}

yuki::OpenGLShader & yuki::OpenGLShader::setFragmentShaderSource(const std::string &src)
{
    _compileShader(GL_FRAGMENT_SHADER, src);
    return *this;
}

yuki::OpenGLShader & yuki::OpenGLShader::compile()
{
    printf("Linking shaders\n");

    assert(mVertShader);
    assert(mFragShader);

    glAttachShader(mProgramId, mVertShader);
    glAttachShader(mProgramId, mFragShader);
    glLinkProgram(mProgramId);
    glDetachShader(mProgramId, mVertShader);
    glDetachShader(mProgramId, mFragShader);
    _deleteShaders();

    GLint result;
    int info_log_length;

    // Check the program
    glGetProgramiv(mProgramId, GL_LINK_STATUS, &result);
    glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0)
    {
        std::vector<char> ProgramErrorMessage(info_log_length + 1);
        glGetProgramInfoLog(mProgramId, info_log_length, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    YUKI_OPENGL_CHECK();
    return *this;
}

yuki::OpenGLShader & yuki::OpenGLShader::linkInputs(yuki::VertexBuffer &vbo)
{
    size_t sign = vbo.getLayoutSignature();
    if(sign == mLastLayoutSignature) return *this;

    _deleteVAO();
    glGenVertexArrays(1, &mVAO);

    glBindVertexArray(mVAO);
    BindingSentry vaobind { std::bind(glBindVertexArray, 0) };
    auto vbobind = vbo.bind();
    YUKI_OPENGL_CHECK();

    // https://stackoverflow.com/questions/440144/in-opengl-is-there-a-way-to-get-a-list-of-all-uniforms-attribs-used-by-a-shade
    GLint count = 0;
    glGetProgramiv(mProgramId, GL_ACTIVE_ATTRIBUTES, &count);
    printf("Active Attributes: %d\n", count);

    GLint var_size; // this is the real size... like sizeof(int), not component number
    GLenum var_type; // this is the complete input type including element type and amounts. e.g. GL_FLOAT_VEC4
    GLint var_index;

    const GLsizei bufSize = 64;
    GLchar var_name[bufSize];
    GLsizei length;

    GLenum layout_type;
    auto binder = vbo.bind();

    for(GLint i = 0; i < count; ++i)
    {
        glGetActiveAttrib(mProgramId, static_cast<GLuint>(i), bufSize, &length, &var_size, &var_type, var_name);
        printf("Attribute #%d Type: %u Name: %s\n", i, var_type, var_name);
        var_index = glGetAttribLocation(mProgramId, var_name);

        if(auto input = vbo.findLayoutEntry(var_name))
        {
            layout_type = getOpenGLElementType(input->element_type);
            // assert(layout_type == var_type);
            // assert(input->num_elements == var_size);

            glEnableVertexAttribArray(var_index);
            glVertexAttribPointer(
                var_index,
                input->num_elements,
                layout_type,
                input->normalized,
                vbo.getElementSize(),
                reinterpret_cast<const void *>(input->offset)
            );
            YUKI_OPENGL_CHECK();
        }
        else
        {
            throw std::runtime_error("input variable not found in vertex buffer");
        }
    }
    mLastLayoutSignature = sign;

    return *this;
}

yuki::BindingSentry yuki::OpenGLShader::bind()
{
    glUseProgram(mProgramId);
    glBindVertexArray(mVAO);
    YUKI_OPENGL_CHECK();

    return { []()
    {
        glBindVertexArray(0);
        glUseProgram(0);
    } };
}
