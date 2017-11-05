#include <fstream>

#include <Usagi/Engine/Runtime/Exception.hpp>

#include "SpirvShader.hpp"

namespace yuki::graphics
{

size_t SpirvShader::getByteCodeSize() const
{
    return mByteCode.size();
}

const SpirvShader::ByteCodeWord * SpirvShader::getByteCode() const
{
    return reinterpret_cast<const ByteCodeWord *>(mByteCode.data());
}

std::shared_ptr<SpirvShader> SpirvShader::readFromFile(const std::string &file_path)
{
    std::ifstream bytecode_file(file_path, std::ios::binary | std::ios::in);

    if(!bytecode_file)
        throw CannotOpenFileException() << boost::errinfo_file_name(file_path);

    auto shader = std::make_shared<SpirvShader>();
    shader->mByteCode = std::vector<char>(std::istreambuf_iterator<char>(bytecode_file), std::istreambuf_iterator<char>());

    if(shader->mByteCode.size() % sizeof(ByteCodeWord) != 0)
        throw CorruptedDataFileException() << boost::errinfo_file_name(file_path);

    return shader;
}

}
