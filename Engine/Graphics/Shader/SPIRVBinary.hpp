#pragma once

#include <vector>
#include <filesystem>
#include <map>

#include <spirv_cross.hpp>

#include "ShaderStage.hpp"

namespace usagi
{
/**
 * \brief SPIR-V bytecode container. Used to derive shaders fed to the driver,
 * and can be disposed after that.
 */
class SpirvBinary
{
	// SPIR-V binary is 32-bit
    using Bytecode = std::uint32_t;
    std::vector<Bytecode> mBytecodes;
    spirv_cross::Compiler mReflectionCompiler;

    struct ConstantFieldInfo
    {
        std::size_t size;
        std::uint32_t offset;
    };
    std::map<std::string, ConstantFieldInfo> mConstantFieldInfoMap;

public:
    explicit SpirvBinary(std::vector<Bytecode> bytecodes);

    const std::vector<Bytecode> & bytecodes() const { return mBytecodes; }
    void dumpBytecodeBitstream(std::ostream &output);

    const spirv_cross::Compiler & getReflectionCompiler() const
    {
        return mReflectionCompiler;
    }


    static std::shared_ptr<SpirvBinary> fromFile(
        const std::filesystem::path &binary_path);
    static std::shared_ptr<SpirvBinary> fromStream(
        std::istream &glsl_source_stream);
    static std::shared_ptr<SpirvBinary> fromGlslSourceString(
        const std::string &glsl_source_code,
        ShaderStage stage);
    static std::shared_ptr<SpirvBinary> fromGlslSourceFile(
        const std::filesystem::path &glsl_source_path,
        ShaderStage stage);
    static std::shared_ptr<SpirvBinary> fromGlslSourceStream(
        std::istream &glsl_source_stream,
        ShaderStage stage);
};
}
