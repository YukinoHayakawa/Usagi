#include <fstream>

#include <Usagi/Engine/Runtime/Exception.hpp>

#include "SPIRVShader.hpp"

size_t yuki::SPIRVShader::getByteCodeSize() const
{
    return mByteCode.size();
}

const yuki::SPIRVShader::ByteCodeWord * yuki::SPIRVShader::getByteCode() const
{
    return reinterpret_cast<const ByteCodeWord *>(mByteCode.data());
}

std::shared_ptr<yuki::SPIRVShader> yuki::SPIRVShader::readFromFile(const std::string &file_path)
{
    std::ifstream bytecode_file(file_path, std::ios::binary | std::ios::in);

    if(!bytecode_file)
        throw CannotOpenFileException() << boost::errinfo_file_name(file_path);

    auto shader = std::make_shared<SPIRVShader>();
    shader->mByteCode = std::vector<char>(std::istreambuf_iterator<char>(bytecode_file), std::istreambuf_iterator<char>());
    
    if(shader->mByteCode.size() % sizeof(ByteCodeWord) != 0)
        throw CorruptedDataFileException() << boost::errinfo_file_name(file_path);

    return shader;
}
