#include "DataFormat.hpp"

namespace usagi
{
std::string_view to_string(const DataFormat format)
{
    using namespace std::literals;

    switch(format)
    {
        case DataFormat::FLOAT32: return "FLOAT32"sv;
        case DataFormat::FLOAT64: return "FLOAT64"sv;
        case DataFormat::INT8: return "INT8"sv;
        case DataFormat::INT16: return "INT16"sv;
        case DataFormat::INT24: return "INT24"sv;
        case DataFormat::INT32: return "INT32"sv;
        case DataFormat::INT64: return "INT64"sv;
        case DataFormat::UINT8: return "UINT8"sv;
        default: return { };
    }
}
}
