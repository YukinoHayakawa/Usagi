#pragma once

#include <string_view>

namespace usagi
{
/**
* \brief Common data format enum for audio/graphics APIs.
*/
enum class DataFormat
{
    FLOAT32,
    FLOAT64,

    INT8,
    INT16,
    INT24,
    INT32,
    INT64,

    UINT8,
};

std::string_view to_string(DataFormat format);
}
