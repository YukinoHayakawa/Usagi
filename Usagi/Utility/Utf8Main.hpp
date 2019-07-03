#pragma once

#include <vector>
#include <string>

// implement this function to get uniformed main function with UTF-8 inputs.
int usagi_main(const std::vector<std::string> &args);

namespace usagi
{
std::vector<std::string> argsFromMain(int argc, wchar_t *argv[]);
std::vector<std::string> argsFromMain(int argc, char *argv[]);
}

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
{
    const auto args = usagi::argsFromMain(argc, argv);
    return usagi_main(args);
}
#else
int main(int argc, char *argv[])
{
    const auto args = usagi::argsFromMain(argc, argv);
    return usagi_main(args);
}
#endif
