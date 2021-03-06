#include <boost/pool/simple_segregated_storage.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <vector>
#include <list>
#include <cstddef>
#include <iostream>

struct int_pool {};
typedef boost::singleton_pool<int_pool, sizeof(int)> singleton_int_pool;

int main()
{
	boost::simple_segregated_storage<std::size_t> storage;
	std::vector<char> v(1024);
	storage.add_block(&v.front(), v.size(), 256);
    
	int* i = static_cast<int*>(storage.malloc());
	*i = 1;

	int* j = static_cast<int*>(storage.malloc_n(1, 512));
	j[10] = 2;

	storage.free(i);
	storage.free_n(j, 1, 512);

	boost::object_pool<int> pool;

	int* a = pool.malloc();
	*a = 1;
	
	int* b = pool.construct(2);

	pool.destroy(a);
	pool.destroy(b);

	boost::object_pool<int> pool1{ 32, 0 };
	pool1.construct();
	std::cout << pool.get_next_size() << std::endl;
	pool.set_next_size(8);

	int* x = static_cast<int*>(singleton_int_pool::malloc());
	*x = 1;

	int* y = static_cast<int*>(singleton_int_pool::ordered_malloc(10));
	y[9] = 2;

	singleton_int_pool::release_memory();
	singleton_int_pool::purge_memory();

	std::vector<int, boost::pool_allocator<int>> v1;
	for (int i = 0; i < 1000; i++)
	{
		v1.push_back(i);
	}

	v1.clear();
	boost::singleton_pool<boost::pool_allocator_tag, sizeof(int)>::purge_memory();

	typedef boost::fast_pool_allocator<int,
		boost::default_user_allocator_new_delete,
		boost::details::pool::default_mutex,
		64, 128> allocator;

	std::list<int, allocator> l;
	for (int i = 0; i < 1000; i++)
	{
		l.push_back(i);
	}

	l.clear();
	boost::singleton_pool<boost::fast_pool_allocator_tag, sizeof(int)>::purge_memory();
}