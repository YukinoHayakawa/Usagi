#pragma once

namespace yuki
{

/**
 * \brief Specifies the type of data from the perspective of
 * shader programs.
 */
enum class ShaderDataType
{
	BOOL,
    INTEGER, UNSIGNED_INTEGER,
    FLOAT,
	VECTOR2, VECTOR3, VECTOR4,
	MATRIX3, MATRIX4,
    ENUM_END
};

}
