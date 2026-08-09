#pragma once
#include <cstdint>
#include "../../../sdk/vfunc/vfunc.hpp"
#include "c_buffer_string_growable.hpp"

class i_resource_system
{
public:
	[[nodiscard]] void* BlockingLoadResourceByName(const char* name, uint64_t extension)
	{
		c_buffer_string_growable<256> buff;
		buff.fixup_resource_name(name, extension);

		return vmt::call_virtual<void*>(this, 41, &buff, "");
	}
};