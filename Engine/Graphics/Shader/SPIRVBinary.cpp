#include "SPIRVBinary.hpp"

#include <fstream>
#include <cstring>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/SPIRV/disassemble.h>

#include <Usagi/Engine/Utility/RAIIHelper.hpp>
#include <Usagi/Engine/Utility/File.hpp>
#include <iostream>

// See https://www.khronos.org/registry/spir-v/papers/WhitePaper.html for
// SPIR-V format.
yuki::SPIRVBinary::SPIRVBinary(std::vector<std::uint32_t> bytecodes)
    : mBytecodes { std::move(bytecodes) }
{
    // check header magic code
    if(mBytecodes.empty() || mBytecodes.front() != 0x07230203)
        throw std::runtime_error(
            "Not valid SPIR-V binary: header magic code does not match.");
}

namespace fs = std::filesystem;

void yuki::SPIRVBinary::dump(std::ostream &output)
{
    output.write(reinterpret_cast<const char *>(mBytecodes.data()),
        mBytecodes.size() * sizeof(Bytecode));
}

std::shared_ptr<yuki::SPIRVBinary> yuki::SPIRVBinary::fromFile(
    const fs::path &binary_path)
{
    std::ifstream file(binary_path);
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    const auto size = file_size(binary_path);

    if(size % sizeof(Bytecode) != 0)
        throw std::runtime_error(
            "Not valid SPIR-V binary: file size is not a multiple of 4.");

    std::vector<Bytecode> content;
    content.resize(size / sizeof(Bytecode));
    file.read(reinterpret_cast<char*>(content.data()), size);

    return std::make_shared<SPIRVBinary>(std::move(content));
}

std::shared_ptr<yuki::SPIRVBinary> yuki::SPIRVBinary::fromStream(
    std::istream &glsl_source_stream)
{
    const auto old_exceptions = glsl_source_stream.exceptions();
    RAIIHelper exceptions(
        [&]() {
            glsl_source_stream.exceptions(
                std::ifstream::badbit | std::ifstream::failbit);
        }, [&]() {
            glsl_source_stream.exceptions(old_exceptions);
        });

    auto dump = readStreamAsString(glsl_source_stream);
    if(dump.size() % sizeof(Bytecode) != 0)
        throw std::runtime_error(
            "Not valid SPIR-V binary. File size is not a multiple of 4.");

    std::vector<Bytecode> content;
    content.resize(dump.size() / 4);
    std::memcpy(content.data(), dump.data(), dump.size());

    return std::make_shared<SPIRVBinary>(std::move(content));
}

namespace
{
EShLanguage translateShaderStage(yuki::ShaderStage stage)
{
    switch(stage)
    {
        case yuki::ShaderStage::VERTEX: return EShLangVertex;
        case yuki::ShaderStage::FRAGMENT: return EShLangFragment;
        default: throw std::runtime_error("Invalid shader stage.");
    }
}
}

std::shared_ptr<yuki::SPIRVBinary> yuki::SPIRVBinary::fromGlslSourceString(
    const std::string &glsl_source_code,
    const ShaderStage stage)
{
    using namespace glslang;
    using namespace spv;

    // default Vulkan version
    const auto client_input_semantics_version = 100;
    const auto vulkan_client_version = EShTargetVulkan_1_0;
    // doesn't influence anything yet, but maps to OpenGL 4.50
    const auto opengl_client_version = EShTargetOpenGL_450;
    const auto target_version = EShTargetSpv_1_0;
    const auto default_version = 110; // defaults to desktop version
    const auto glslang_stage = translateShaderStage(stage);
    const auto messages = EShMsgDefault;

    RAIIHelper glslang_process {
        []() { InitializeProcess(); },
        []() { FinalizeProcess(); }
    };
    // shader must be released after program, as the program may reference
    // the shaders.
    TShader shader(glslang_stage);
    TProgram program;
    TBuiltInResource resources { };

    const char *strings[] = { glsl_source_code.data() };
    const int sizes[] = { glsl_source_code.size() };
    shader.setStringsWithLengths(strings, sizes, 1);

    shader.setEnvInput(EShSourceGlsl, glslang_stage, EShClientVulkan,
        client_input_semantics_version);
    shader.setEnvClient(EShClientVulkan, vulkan_client_version);
    shader.setEnvTarget(EShTargetSpv, target_version);

    TShader::ForbidIncluder includer;
    if(!shader.parse(&resources, default_version, false, messages, includer))
        throw std::runtime_error("Shader compilation failed.");

    program.addShader(&shader);

    if(!program.link(messages) || !program.mapIO())
        throw std::runtime_error("Shader linking failed.");

    std::cout << program.getInfoLog();
    std::cout << program.getInfoDebugLog();
    program.buildReflection();
    program.dumpReflection();

    // Genearte SPIR-V code
    std::vector<Bytecode> spirv;
    std::string warnings_errors;
    SpvBuildLogger logger;
    SpvOptions spv_options;
    spv_options.generateDebugInfo = true;
    spv_options.disableOptimizer = false;
    spv_options.optimizeSize = false;

    GlslangToSpv(*program.getIntermediate(glslang_stage), spirv, &logger,
        &spv_options);
    Disassemble(std::cout, spirv);

    return std::make_shared<SPIRVBinary>(std::move(spirv));
}

std::shared_ptr<yuki::SPIRVBinary> yuki::SPIRVBinary::fromGlslSourceFile(
    const fs::path &glsl_source_path,
    const ShaderStage stage)
{
    return fromGlslSourceString(readFileAsString(glsl_source_path), stage);
}

std::shared_ptr<yuki::SPIRVBinary> yuki::SPIRVBinary::fromGlslSourceStream(
    std::istream &glsl_source_stream,
    const ShaderStage stage)
{
    return fromGlslSourceString(readStreamAsString(glsl_source_stream), stage);
}
