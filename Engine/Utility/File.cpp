#include <fstream>
#include <sstream>

#include "File.hpp"

std::string yuki::readFileContent(const std::string &filename)
{
    std::ifstream src(filename);
    src.exceptions(std::ios::failbit);
    std::ostringstream content;
    std::string line;
    while(std::getline(src, line))
    {
        content << line << '\n';
    }
    return content.str();
}
