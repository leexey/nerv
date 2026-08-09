#pragma once
#include <stdint.h>
#include <cstddef>
#include <cstring>
#include <cstdint>

template <std::size_t MAX_SIZE, bool allow_heap_allocation>
class c_buffer_string_growable;

class c_buffer_string
{
protected:
	c_buffer_string() = default;

public:
	enum allocation_option_t
	{
		unk1 = -1,
		unk2 = 0,
		unk3 = (1 << 1),
		unk4 = (1 << 8),
		unk5 = (1 << 9),
		allow_heap_allocation_flag = (1 << 31)
	};

	enum allocation_flags_t
	{
		length_mask = (1 << 30) - 1,
		flags_mask = ~length_mask,

		stack_allocation_marker = (1 << 30),
		heap_allocation_marker = (1 << 31)
	};

	template <std::size_t MAX_SIZE = 8, bool allow_heap_allocation = true, typename T = c_buffer_string_growable<MAX_SIZE, allow_heap_allocation>>
	T* to_growable();

	void fixup_resource_name(const char* resource, uint64_t extension);
	void move_from(c_buffer_string& src);
	void insert(int index, const char* str, int length, bool b1);
};

template <std::size_t MAX_SIZE, bool allow_heap_allocation = true>
class c_buffer_string_growable : public c_buffer_string
{
	friend class c_buffer_string;

public:
	c_buffer_string_growable();
	c_buffer_string_growable(const c_buffer_string_growable& other);
	~c_buffer_string_growable();

	c_buffer_string_growable& operator=(const c_buffer_string_growable& src);

	[[nodiscard]] int get_allocated_number() const;
	[[nodiscard]] int get_total_number() const;
	[[nodiscard]] bool is_stack_allocated() const;
	[[nodiscard]] bool is_heap_allocated() const;

	bool is_input_string_unsafe(const char* data) const;

	[[nodiscard]] const char* get() const;
	void clear();

private:
	int total_count_;
	int allocated_;

	union
	{
		char* heap_string;
		char stack_string[MAX_SIZE];
	} memory_;
};

struct ResourceBinding_t;
class CBufferString
{
	std::uint32_t m_nUnknown1 = 0;
	std::uint32_t m_nUnknown2 = 0xC00000C8;
	union
	{
		std::uintptr_t m_uStrPtr;
		std::uint8_t data[0xC8];
	};
	std::uint64_t m_nUnknown3 = 0;
	std::uint64_t m_nUnknown4 = 0;
public:

	CBufferString(const char* szString, const uint64_t uExt);

	~CBufferString();
};

