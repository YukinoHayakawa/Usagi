#include "SpirvBinary.hpp"

#include <fstream>
#include <cstring>

#include <glslang/glslang/Public/ShaderLang.h>
#include <glslang/StandAlone/ResourceLimits.h>
#include <glslang/glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Utility/RAIIHelper.hpp>
#include <Usagi/Utility/File.hpp>
#include <Usagi/Utility/Hash.hpp>
#include <Usagi/Utility/Stream.hpp>

// See https://www.khronos.org/registry/spir-v/papers/WhitePaper.html for
// SPIR-V format.
usagi::SpirvBinary::SpirvBinary(std::vector<std::uint32_t> bytecodes)
    : mBytecodes { std::move(bytecodes) }
    , mReflectionCompiler { mBytecodes }
{
    // check header magic code
    if(mBytecodes.empty() || mBytecodes.front() != 0x07230203)
        throw std::runtime_error(
            "Not valid SPIR-V binary: header magic code does not match.");
}

namespace fs = std::filesystem;

void usagi::SpirvBinary::dumpBytecodeBitstream(std::ostream &output)
{
    output.write(reinterpret_cast<const char *>(mBytecodes.data()),
        mBytecodes.size() * sizeof(Bytecode));
}

std::shared_ptr<usagi::SpirvBinary> usagi::SpirvBinary::fromFile(
    const fs::path &binary_path)
{
    std::ifstream file(binary_path, std::ios::binary);
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    const auto size = file_size(binary_path);

    if(size % sizeof(Bytecode) != 0)
        throw std::runtime_error(
            "Not valid SPIR-V binary: file size is not a multiple of 4.");

    std::vector<Bytecode> content;
    content.resize(size / sizeof(Bytecode));
    file.read(reinterpret_cast<char*>(content.data()), size);

    return std::make_shared<SpirvBinary>(std::move(content));
}

std::shared_ptr<usagi::SpirvBinary> usagi::SpirvBinary::fromStream(
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

    return std::make_shared<SpirvBinary>(std::move(content));
}

namespace
{
EShLanguage translate(const usagi::ShaderStage stage)
{
    switch(stage)
    {
        case usagi::ShaderStage::VERTEX: return EShLangVertex;
        case usagi::ShaderStage::FRAGMENT: return EShLangFragment;
        default: throw std::runtime_error("Invalid shader stage.");
    }
}
}

// refer to glslangValidator source code
std::shared_ptr<usagi::SpirvBinary> usagi::SpirvBinary::fromGlslSourceString(
    const std::string &glsl_source_code,
    const ShaderStage stage,
    const std::optional<std::filesystem::path> & cache_folder)
{
    LOG(info, "Compiling {} shader...", to_string(stage));

    std::filesystem::path cache_file;
    if(cache_folder)
    {
        cache_file = cache_folder.value();
        create_directories(cache_file);
        cache_file /= sha256(glsl_source_code);
        cache_file += ".spv";

        // try loading from cache
        try
        {
            auto cache = fromFile(cache_file);
            LOG(info, "Shader loaded from cache {}", cache_file);
            return std::move(cache);
        }
        catch(const std::exception &e)
        {
            LOG(warn, "Could not load the shader from cache: {}, {}",
                e.what(), strerror(errno));
        }
        catch(...)
        {
            LOG(error,
                "Unknown error occurred while loading the shader from cache.");
        }
    }

    using namespace glslang;
    using namespace spv;

    // default Vulkan version
    const auto client_input_semantics_version = 100;
    const auto vulkan_client_version = EShTargetVulkan_1_0;
    // doesn't influence anything yet, but maps to OpenGL 4.50
    const auto opengl_client_version = EShTargetOpenGL_450;
    const auto target_version = EShTargetSpv_1_0;
    const auto default_version = 110; // defaults to desktop version
    const auto glslang_stage = translate(stage);
    const auto messages = EShMsgDefault;

    RAIIHelper glslang_process {
        []() { InitializeProcess(); },
        []() { FinalizeProcess(); }
    };
    // shader must be released after program because the program may reference
    // the shaders.
    TShader shader(glslang_stage);
    TProgram program;
    TBuiltInResource resources = DefaultTBuiltInResource;

    const char *strings[] = { glsl_source_code.data() };
    const int sizes[] = { static_cast<int>(glsl_source_code.size()) };
    shader.setStringsWithLengths(strings, sizes, 1);

    shader.setEnvInput(EShSourceGlsl, glslang_stage, EShClientVulkan,
        client_input_semantics_version);
    shader.setEnvClient(EShClientVulkan, vulkan_client_version);
    shader.setEnvTarget(EShTargetSpv, target_version);

    TShader::ForbidIncluder includer;
    const auto compilation_suceeded =
        shader.parse(&resources, default_version, false, messages, includer);

    if(shader.getInfoLog()[0])
        LOG(info, "Compiler output:\n{}", shader.getInfoLog());
    if(shader.getInfoDebugLog()[0])
        LOG(info, "Compiler debug output:\n{}", shader.getInfoDebugLog());

    program.addShader(&shader);

    const auto link_succeeded = program.link(messages) && program.mapIO();

    if(program.getInfoLog()[0])
        LOG(info, "Linker output:\n{}", program.getInfoLog());
	if(program.getInfoDebugLog()[0])
        LOG(info, "Linker debug output:\n{}", program.getInfoDebugLog());

    if(!compilation_suceeded || !link_succeeded)
        throw std::runtime_error("Shader compilation failed.");

    program.buildReflection();
	LOG(info, "Reflection database:");
	program.dumpReflection();

    // Generate SPIR-V code
    std::vector<Bytecode> spirv;
    std::string warnings_errors;
    SpvBuildLogger logger;
    SpvOptions spv_options;
    spv_options.generateDebugInfo = true;
    spv_options.disableOptimizer = false;
    spv_options.optimizeSize = false;

    GlslangToSpv(*program.getIntermediate(glslang_stage), spirv, &logger,
        &spv_options);
	// LOG(info, "Disassembly:");
    // Disassemble(std::cout, spirv);

    // save bytecode to cache
    if(cache_folder)
    {
        LOG(info, "Saving snader cache to {}", cache_file);
        dumpBinary(cache_file, spirv.data(), spirv.size() * sizeof(Bytecode));
    }

    return std::make_shared<SpirvBinary>(std::move(spirv));
}

std::shared_ptr<usagi::SpirvBinary> usagi::SpirvBinary::fromGlslSourceFile(
    const fs::path &glsl_source_path,
    const ShaderStage stage,
    const std::optional<std::filesystem::path> & cache_folder)
{
    return fromGlslSourceString(
        readFileAsString(glsl_source_path), stage, cache_folder);
}

std::shared_ptr<usagi::SpirvBinary> usagi::SpirvBinary::fromGlslSourceStream(
    std::istream &glsl_source_stream,
    const ShaderStage stage,
    const std::optional<std::filesystem::path> & cache_folder)
{
    return fromGlslSourceString(
        readStreamAsString(glsl_source_stream), stage, cache_folder);
}
