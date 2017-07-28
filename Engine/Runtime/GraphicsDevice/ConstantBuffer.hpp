#pragma once

#include "GDBuffer.hpp"

namespace yuki
{

/**
 * \brief 
 */
class ConstantBuffer : public GDBuffer
{
public:
	struct EntryFormat
	{
		const char *name;

	};
	virtual ConstantBuffer & updateVariable(const char *name, const void *data) = 0;

    virtual void bind(int bind_point) = 0;
    
protected:
	/**
	 * \brief Calculate the aligned address of element.
	 * \param cursor_pos Next available position of bytes.
	 * \param alignment The alignment requirement of the type.
	 * \return 
	 */
	static size_t _calcElementOffset(size_t cursor_pos, size_t alignment);
};

}
