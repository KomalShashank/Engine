#pragma once

#include <stdlib.h>
#include <xmemory0>



template <typename data_type>
class UntrackedAllocator
{
public:
	typedef data_type value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

public:
	template <typename other_data_type>
	struct rebind
	{
		typedef UntrackedAllocator<other_data_type> other_type;
	};

public:
	inline explicit UntrackedAllocator()
	{

	}

	inline ~UntrackedAllocator()
	{

	}

	inline explicit UntrackedAllocator(UntrackedAllocator const&)
	{

	}

	template <typename other_data_type>
	inline explicit UntrackedAllocator(UntrackedAllocator<other_data_type> const&)
	{

	}

	inline pointer address(reference r)
	{
		return &r;
	}

	inline const_pointer address(const_reference r)
	{
		return &r;
	}

	inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0)
	{
		data_type* p = (data_type*)malloc(cnt * sizeof(data_type));

		return p;
	}

	inline void deallocate(pointer p, size_type cnt)
	{
		cnt;
		free(p);
	}

	inline size_type max_size() const
	{
		return (std::numeric_limits<size_type>::max)() / sizeof(data_type);
	}

	inline void construct(pointer p, const_reference t)
	{
		new(p) data_type(t);
	}

	inline void destroy(pointer p)
	{
		p;
		p->~data_type();
	}

	inline bool operator==(UntrackedAllocator const& a)
	{
		return this == &a;
	}

	inline bool operator!=(UntrackedAllocator const& a)
	{
		return !operator==(a);
	}
};