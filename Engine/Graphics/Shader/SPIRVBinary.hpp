#pragma once

#include <vector>
#include <filesystem>

#include "ShaderStage.hpp"

namespace usagi
{
class SPIRVBinary
{
	// SPIR-V binary is 32-bit
    using Bytecode = std::uint32_t;
    std::vector<Bytecode> mBytecodes;

public:
    explicit SPIRVBinary(std::vector<Bytecode> bytecodes);

    const std::vector<Bytecode> & bytecodes() const
    {
        return mBytecodes;
    }

    void dump(std::ostream &output);

    static std::shared_ptr<SPIRVBinary> fromFile(
        const std::filesystem::path &binary_path);
    static std::shared_ptr<SPIRVBinary> fromStream(
        std::istream &glsl_source_stream);
    static std::shared_ptr<SPIRVBinary> fromGlslSourceString(
        const std::string &glsl_source_code,
        ShaderStage stage);
    static std::shared_ptr<SPIRVBinary> fromGlslSourceFile(
        const std::filesystem::path &glsl_source_path,
        ShaderStage stage);
    static std::shared_ptr<SPIRVBinary> fromGlslSourceStream(
        std::istream &glsl_source_stream,
        ShaderStage stage);
};
}
