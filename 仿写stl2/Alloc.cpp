#include "Alloc.h"
/*内存配置器
使用两级内存配置；
*/
namespace queueSTL {

	char* alloc::start_free = 0;//内存池起始位置
	char* alloc::end_free = 0;//内存池结束位置
	size_t alloc::heap_size = 0;
	alloc::obj* alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};//freelist初始化

	void* alloc::allocate(size_t bytes) {//大于128bytes就直接调用malloc分配
		if (bytes >(size_t)EMaxBytes::MAXBYTES) {
			return malloc(bytes);
		}
		size_t index = FREELIST_INDEX(bytes);
		obj *list = free_list[index];
		if (list) {//在16个list中查找有空间的位置，调整list返回list空位
			free_list[index] = list->next;
			return list;
		}
		else {//此list没有足够的空间，需要从内存池里面取空间
			return refill(ROUND_UP(bytes));
		}
	}

	void alloc::deallocate(void *ptr, size_t bytes) {
		if (bytes > EMaxBytes::MAXBYTES) {//大于128直接free释放
			free(ptr);
		}
		else {//寻找对应的list,回收块区
			size_t index = FREELIST_INDEX(bytes);
			obj *node = static_cast<obj *>(ptr);
			node->next = free_list[index];
			free_list[index] = node;
		}
	}
	void *alloc::reallocate(void *ptr, size_t old_sz, size_t new_sz) {
		deallocate(ptr, old_sz);
		ptr = allocate(new_sz);
		return ptr;
	}

	//返回一个大小为n的对象，并且有时候会为适当的free list增加节点
	//假设bytes已经上调为8的倍数
	void *alloc::refill(size_t bytes) {
		size_t nobjs = ENObjs::NOBJS;
		//从内存池里取
		char *chunk = chunk_alloc(bytes, nobjs);
		obj **my_free_list = 0;
		obj *result = 0;
		obj *current_obj = 0, *next_obj = 0;

		if (nobjs == 1) {//取出的空间只够一个对象使用
			return chunk;
		}
		else {
			my_free_list = free_list + FREELIST_INDEX(bytes);
			result = (obj *)(chunk);//申请到这一块返回到客户端使用
			*my_free_list = next_obj = (obj *)(chunk + bytes);
			//将取出的多余的空间加入到相应的free list里面去
			for (int i = 1;; ++i) {//第0个在上面返回了 从1开始
				current_obj = next_obj;
				next_obj = (obj *)((char *)next_obj + bytes);
				if (nobjs - 1 == i) {
					current_obj->next = 0;
					break;
				}
				else {
					current_obj->next = next_obj;
				}
			}
			return result;
		}
	}

	//假设bytes已经上调为8的倍数
	char *alloc::chunk_alloc(size_t bytes, size_t& nobjs) {//从内存池中分配空间给freelist
		char *result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) {//内存池剩余空间完全满足需要
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes) {//内存池剩余空间不能完全满足需要，但足够供应一个或以上的区块
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {//内存池剩余空间连一个区块的大小都无法提供
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {//内存池还有零头，先试着分配给freelist
				obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj *)start_free)->next = *my_free_list;
				*my_free_list = (obj *)start_free;
			}
			start_free = (char *)malloc(bytes_to_get);//malloc补充内存池
			if (!start_free) {//malloc失败 heap空间不足
				obj **my_free_list = 0, *p = 0;
				for (int i = 0; i <= EMaxBytes::MAXBYTES; i += EAlign::ALIGN) {//试着寻找还有没用的freelist区块，且区块足够大
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = 0;
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}
	}
}