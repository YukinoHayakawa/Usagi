#pragma once

#include <string>
#include <memory>
#include <vector>

namespace yuki
{

class SPIRVShader
{
    typedef uint32_t ByteCodeWord;
    std::vector<char> mByteCode;

public:
    // size in bytes
    size_t getByteCodeSize() const;
    const ByteCodeWord * getByteCode() const;

    static std::shared_ptr<SPIRVShader> readFromFile(const std::string &file_path);
    //static std::shared_ptr<SPIRVShader> compileFromFile(const std::string &file_path);
    //static std::shared_ptr<SPIRVShader> compileFromString(const std::string &source_code);
};

}
