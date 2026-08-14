#pragma once
#include <cstddef>
#include "../../../sdk/vfunc/vfunc.hpp"

class i_mem_alloc
{
public:
	void* alloc(std::size_t nSize)
	{
		return vmt::call_virtual<void*>(this, 1, nSize);
	}

	std::size_t free(const void* pMemory)
	{
		return vmt::call_virtual<std::size_t>(this, 21, pMemory);
	}

};