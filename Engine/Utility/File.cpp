#include "File.hpp"

#include <fstream>

namespace fs = std::filesystem;

std::string usagi::readFileAsString(const fs::path &path)
{
	std::ifstream file(path);
	file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	const auto size = file_size(path);
    return readStreamAsString(file);
}

std::string usagi::readStreamAsString(std::istream &in)
{
	// allows using with forward only streams
	const std::istreambuf_iterator<char> begin(in), end;
	return { begin, end };
}
