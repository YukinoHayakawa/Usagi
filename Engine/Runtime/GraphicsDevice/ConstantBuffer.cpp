#include "ConstantBuffer.hpp"

size_t yuki::ConstantBuffer::_calcElementOffset(size_t cursor_pos, size_t alignment)
{
	return cursor_pos + (alignment - cursor_pos % alignment) % alignment;
}
