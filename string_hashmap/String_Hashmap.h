#ifndef SHM_H
#define SHM_H
#pragma once

/*
	Usage
	#define DYNAMIC_ARRAY_IMPLEMENTATION
#define SHM_IMPLEMENTATION
#include "String_Hashmap.h"

*/

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdbool.h>
#include "dynamic_array2.h"

typedef struct
{
	dynamic_array* item_data;
	dynamic_array* items;
	dynamic_array* hash_table;
} SHMap;

#ifdef __cplusplus
extern "C" {
#endif
	/**
	* Init the String hashmap
	\param T The type of item that will be stored
	\param RESERVE_SIZE The amount of item that can be stored initially
	\return SHMap
	*/
#define SHMAP_INIT(T, RESERVE_SIZE) _SHMap_Init(sizeof(T), RESERVE_SIZE)
	extern void* SHMap_Find(SHMap* const shmap, const char* p_str);
	extern bool SHMap_Has(SHMap* const shmap, const char* p_str);
	extern void* SHMap_Insert(SHMap* const shmap, const char* p_name, const void* p_data);
	extern void SHMap_Erase(SHMap* const shmap, const char* p_name);
	extern void* SHMap_AtIndex(SHMap* const shmap, size_t p_index);
	extern size_t SHMap_Size(SHMap* const shmap);
	extern void SHMap_Clear(SHMap* const shmap);
	extern void SHMap_Destruct(SHMap* shmap);

#ifdef __cplusplus
}
#endif

/*
This is for preventing greying out of the implementation section.
*/
#if defined(Q_CREATOR_RUN) || defined(__INTELLISENSE__) || defined(__CDT_PARSER__)
#define SHM_IMPLEMENTATION
#endif

#if defined(SHM_IMPLEMENTATION)
#ifndef SHM_C
#define SHM_C

#define SHM_HASHTABLE_BLOCK_ALLOCATION_SIZE 256


#include <assert.h>

typedef struct
{
	unsigned long long data_array_index;
	const char* name;

	struct _Hm_s_item* hash_next;
} _SHMap_item;

void _SHMap_Assert(SHMap* const shmap)
{
	assert(shmap->hash_table);
	assert(shmap->items);
}

SHMap _SHMap_Init(size_t p_allocSize, size_t p_initReserveSize)
{
	assert(p_allocSize > 0 && "Invalid Alloc Size");

	SHMap map;

	map.items = dA_INIT(_SHMap_item, p_initReserveSize);
	map.item_data = dA_INIT2(p_allocSize, p_initReserveSize);

	size_t hash_table_size = max(p_initReserveSize, SHM_HASHTABLE_BLOCK_ALLOCATION_SIZE);
	map.hash_table = dA_INIT(sizeof(_SHMap_item*), hash_table_size);

	return map;
}

const char* const _shmp_empty_string = "";

char* _SHMap_safeCopy(const char* p_source)
{
	char* out;

	if (!p_source)
	{
		return _shmp_empty_string;
	}
	out = malloc(strlen(p_source) + 1);
	if (out)
	{
		strcpy(out, p_source);
		return out;
	}

	return _shmp_empty_string;
}
#define _HASH_BITS_LONG_LONG ((sizeof (size_t)) * 8)
#define _HASH_ROTATE_LEFT(val, n)   (((val) << (n)) | ((val) >> (_HASH_BITS_LONG_LONG - (n))))
uint32_t _SHMap_HashStr(const char* p_str)
{
	//src: http://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm
	size_t hash = 0;

	while (*p_str)
	{
		hash = _HASH_ROTATE_LEFT(hash, 9) + (unsigned char)*p_str++;
	}
	const int c2 = 0x27d4eb2d;
	hash = (hash ^ 61) ^ (hash >> 16);
	hash = hash + (hash << 3);
	hash = hash ^ (hash >> 4);
	hash = hash * c2;
	hash = hash ^ (hash >> 15);

	return hash;
}

_SHMap_item* _SHMap_findItem(SHMap* const shmap, const char* p_str)
{
	_SHMap_Assert(shmap);

	uint32_t hash = _SHMap_HashStr(p_str);
	hash &= shmap->hash_table->capacity - 1;

	_SHMap_item* item;

	_SHMap_item** hash_array = shmap->hash_table->data;
	
	size_t str_len = strlen(p_str);
	
	for (item = hash_array[hash]; item; item = item->hash_next)
	{
		//found
		if (*item->name == *p_str && !_strnicmp(p_str, item->name, str_len))
		{
			return item;
		}
	}

	return NULL;
}

void* _SHMap_getItemData(SHMap* const shmap, _SHMap_item* item)
{
	return dA_at(shmap->item_data, item->data_array_index);
}

void* SHMap_Find(SHMap* const shmap, const char* p_str)
{
	_SHMap_item* item = _SHMap_findItem(shmap, p_str);

	if (!item)
		return NULL;

	return _SHMap_getItemData(shmap, item);
}
bool SHMap_Has(SHMap* const shmap, const char* p_str)
{
	return _SHMap_findItem(shmap, p_str) != NULL;
}

void* SHMap_Insert(SHMap* const shmap, const char* p_name, const void* p_data)
{
	//check if the item already exists with the same key
	_SHMap_item* find_item = _SHMap_findItem(shmap, p_name);
	if (find_item)
	{
		return _SHMap_getItemData(shmap, find_item);
	}

	_SHMap_item* item = NULL;
	
	item = dA_emplaceBack(shmap->items);

	item->name = _SHMap_safeCopy(p_name);
	void* data_ptr = dA_emplaceBack(shmap->item_data);
	memcpy(data_ptr, p_data, shmap->item_data->alloc_size);
	item->data_array_index = shmap->item_data->elements_size - 1;

	if (shmap->hash_table->capacity + 1 <= shmap->items->elements_size)
	{
		dA_reserve(shmap->hash_table, SHM_HASHTABLE_BLOCK_ALLOCATION_SIZE);
	}

	uint32_t hash = _SHMap_HashStr(p_name);
	hash &= shmap->hash_table->capacity - 1;
	
	_SHMap_item** hash_array = shmap->hash_table->data;

	item->hash_next = hash_array[hash];
	hash_array[hash] = item;

	return data_ptr;
}

void SHMap_Erase(SHMap* const shmap, const char* p_name)
{
	_SHMap_Assert(shmap);

	_SHMap_item** hash_array = shmap->hash_table->data;

	uint32_t hash = _SHMap_HashStr(p_name);
	hash &= shmap->hash_table->capacity - 1;
	
	//remove from the hash table
	if (!_strcmpi(hash_array[hash]->name, p_name))
	{
		hash_array[hash] = hash_array[hash]->hash_next;
	}
	else
	{
		_SHMap_item* item = NULL;
		for (item = hash_array[hash]; item; item = item->hash_next)
		{
			if (item->hash_next)
			{
				_SHMap_item* hash_next = item->hash_next;
				if (*p_name == *hash_next->name && !_strcmpi(hash_next->name, p_name))
				{
					item->hash_next = hash_next->hash_next;
					break;
				}
			}
		}
	}
	
	_SHMap_item* item_array = shmap->items->data;
	//remove from the item array
	for (size_t i = 0; i < shmap->items->elements_size; i++)
	{
		_SHMap_item* item = &item_array[i];

		//found it
		if (*item->name == *p_name && !_strcmpi(item->name, p_name))
		{
			//free the data
			if (item->name && item->name != _shmp_empty_string)
			{
				free(item->name);
			}
			dA_erase(shmap->item_data, item->data_array_index, 1);
			dA_erase(shmap->items, i, 1);
			break;
		}
	}
}

void* SHMap_AtIndex(SHMap* const shmap, size_t p_index)
{
	return dA_at(shmap->item_data, p_index);
}

size_t SHMap_Size(SHMap* const shmap)
{
	_SHMap_Assert(shmap);

	return shmap->item_data->elements_size;
}

void SHMap_Clear(SHMap* const shmap)
{
	dA_clear(shmap->items);
	dA_clear(shmap->item_data);

	memset(shmap->hash_table->data, 0, sizeof(_SHMap_item*) * shmap->hash_table->elements_size);
}

void SHMap_Destruct(SHMap* shmap)
{
	dA_Destruct(shmap->hash_table);

	_SHMap_item* item_array = shmap->items->data;
	//free all the items and data
	for (size_t i = 0; i < shmap->items->elements_size; i++)
	{
		_SHMap_item* item = &item_array[i];

		if (item->name && item->name != _shmp_empty_string)
		{
			free(item->name);
		}
	}
	dA_Destruct(shmap->items);
	dA_Destruct(shmap->item_data);
}

#endif
#endif
#endif 