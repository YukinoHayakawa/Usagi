#include "File.hpp"

std::string yuki::readFileContent(const std::string &filename)
{
    std::ifstream src(filename);
    src.exception(std::ios::failbit);
    std::ostringstream content;
    std::string line;
    while(getline(source, line))
    {
        content << line << '\n';
    }
    return content.str();
}
