#pragma once

namespace yuki
{

/**
 * \brief Specifies the type of data from the perspective of
 * shader programs.
 */
enum class ShaderDataType
{
    BYTE, UNSIGNED_BYTE,
    SHORT, UNSIGNED_SHORT,
    INTEGER, UNSIGNED_INTEGER,
	BOOL,
    FLOAT,
	VECTOR2, VECTOR3, VECTOR4,
	MATRIX3, MATRIX4,
};

}
