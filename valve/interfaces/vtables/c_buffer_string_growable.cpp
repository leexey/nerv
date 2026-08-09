#include "c_buffer_string_growable.hpp"
#include <utils/utils.hpp>
#include <valve/modules/modules.hpp>
#include "i_mem_alloc.hpp"

template <std::size_t MAX_SIZE, bool allow_heap_allocation, typename T>
T* c_buffer_string::to_growable()
{
	return reinterpret_cast<T*>(this);
}

void c_buffer_string::fixup_resource_name(const char* resource, uint64_t extension)
{
	static auto fixup_resource_namefn = reinterpret_cast<void(__fastcall*)(void*, const char*, uint64_t)>(g_opcodes->scan(g_modules->m_modules.resourcesystem_dll.get_name(), "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC ? 8B 41"));
	fixup_resource_namefn(this, resource, extension);
}

void c_buffer_string::move_from(c_buffer_string& src)
{

	HMODULE tier0 = GetModuleHandleA("tier0.dll");
	static auto move_from_buffer_stringfn = reinterpret_cast<void (*)(c_buffer_string*, c_buffer_string&)>(GetProcAddress(tier0, "?MoveFrom@CBufferString@@QEAAXAEAV1@@Z"));

	move_from_buffer_stringfn(this, src);
}

void c_buffer_string::insert(int index, const char* str, int length, bool b1)
{
	HMODULE tier0 = GetModuleHandleA("tier0.dll");
	static auto insertfn = (const char* (__fastcall*)(void*, int, const char*, int, bool))GetProcAddress((HMODULE)tier0, "?Insert@CBufferString@@QEAAPEBDHPEBDH_N@Z");
	insertfn(this, index, str, length, b1);
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::c_buffer_string_growable()
	: total_count_(0),
	allocated_(stack_allocation_marker | (MAX_SIZE & length_mask))
{
	memset(memory_.stack_string, 0, sizeof(memory_.stack_string));
	if (allow_heap_allocation)
		allocated_ |= allow_heap_allocation_flag;
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::c_buffer_string_growable(const c_buffer_string_growable& other)
	: total_count_(0),
	allocated_(stack_allocation_marker | (MAX_SIZE & length_mask))
{
	memset(memory_.stack_string, 0, sizeof(memory_.stack_string));
	if (allow_heap_allocation)
		allocated_ |= allow_heap_allocation_flag;
	move_from(const_cast<c_buffer_string_growable&>(other));
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::~c_buffer_string_growable()
{
	if (is_heap_allocated() && memory_.heap_string)
		GameFree((void*)memory_.heap_string);
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>&
c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::operator=(const c_buffer_string_growable& src)
{
	move_from(const_cast<c_buffer_string_growable&>(src));
	return *this;
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
int c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::get_allocated_number() const
{
	return allocated_ & length_mask;
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
int c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::get_total_number() const
{
	return total_count_ & length_mask;
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
bool c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::is_stack_allocated() const
{
	return (allocated_ & stack_allocation_marker) != 0;
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
bool c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::is_heap_allocated() const
{
	return (total_count_ & heap_allocation_marker) != 0;
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
bool c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::is_input_string_unsafe(const char* data) const
{
	return ((void*)data >= this && (void*)data < &this[1]) ||
		(get_allocated_number() != 0 && data >= get() && data < (get() + get_allocated_number()));
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
const char* c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::get() const
{
	if (is_stack_allocated())
		return memory_.stack_string;
	else if (get_allocated_number() != 0)
		return memory_.heap_string;

	return "";
}

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
void c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>::clear()
{
	if (get_allocated_number() != 0)
	{
		if (is_stack_allocated())
			memory_.stack_string[0] = '\0';
		else
			memory_.heap_string[0] = '\0';
	}
	total_count_ &= ~length_mask;
}

template class c_buffer_string_growable<256, true>;

CBufferString::CBufferString(const char* szString, const uint64_t uExt)
{
	static auto CBufferStringfn = reinterpret_cast<bool(__fastcall*)(CBufferString*, const char*)>(g_opcodes->scan(g_modules->m_modules.client_dll.get_name(), ("48 89 5C 24 ? 57 48 83 EC ? 8B 41 ? 48 8D 79")));
	CBufferStringfn(this, szString);

	m_nUnknown4 = uExt;
}

CBufferString::~CBufferString()
{
	HMODULE tier0 = GetModuleHandleA("tier0.dll");
	static auto PurgeCBufferStringfn = reinterpret_cast<void(__fastcall*)(CBufferString*, std::uint32_t)>(g_opcodes->export_fn((unsigned __int64)tier0, "?Purge@CBufferString@@QEAAXH@Z"));


	PurgeCBufferStringfn(this, 0U);
}
