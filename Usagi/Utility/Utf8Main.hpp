#pragma once

#include <vector>
#include <string>

// implement this function to get uniformed main function with UTF-8 inputs.
int usagi_main(const std::vector<std::string> &args);

namespace usagi
{
std::vector<std::string> argsFromMain(int argc, wchar_t *argv[]);
std::vector<std::string> argsFromMain(int argc, char *argv[]);

int wrappedMain(const std::vector<std::string> &args);
}

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    return usagi::wrappedMain(usagi::argsFromMain(argc, argv));
}
