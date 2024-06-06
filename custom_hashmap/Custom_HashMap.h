#ifndef CHM_H
#define CHM_H
#pragma once


/*
Usage
#define DYNAMIC_ARRAY_IMPLEMENTATION
#define CHM_IMPLEMENTATION
#include "Custom_Hashmap.h"
*/

#include <stdint.h>
#include <stdbool.h>
#include "dynamic_array2.h"

typedef uint32_t(*CHMap_HashFun)(const void* _key);

typedef struct
{
	CHMap_HashFun hash_function;
	dynamic_array* item_data;
	dynamic_array* items;
	dynamic_array* hash_table;
	size_t key_bit_size;
} CHMap;

#ifdef __cplusplus
extern "C" {
#endif
/**
* Init the custom hashmap
\param HASH_FUN The function that will be used to hash the key. If NULL, the default buffer hash function will be used
\param T The type of key that will be used
\param U The type of item that will be stored
\param RESERVE_SIZE The amount of item that can be stored initially
\return SHMap
*/
#define CHMAP_INIT(HASH_FUN, T, U, RESERVE_SIZE) _CHMap_Init(HASH_FUN, sizeof(T), sizeof(U), RESERVE_SIZE)
	extern void* CHMap_Find(CHMap* const chmap, const void* p_key);
	extern bool CHMap_Has(CHMap* const chmap, const void* p_key);
	extern void* CHMap_Insert(CHMap* const chmap, const void* p_key, const void* p_data);
	extern void CHMap_Erase(CHMap* const chmap, const void* p_key);
	extern void CHMap_Reserve(CHMap* const chmap, size_t p_amount);
	extern void* CHMap_AtIndex(CHMap* const chmap, size_t p_index);
	extern size_t CHMap_Size(CHMap* const chmap);
	extern void CHMap_Clear(CHMap* const chmap);
	extern void CHMap_Destruct(CHMap* chmap);

#ifdef __cplusplus
}
#endif

/*
This is for preventing greying out of the implementation section.
*/
#if defined(Q_CREATOR_RUN) || defined(__INTELLISENSE__) || defined(__CDT_PARSER__)
#define CHM_IMPLEMENTATION
#endif

#if defined(CHM_IMPLEMENTATION)
#ifndef CHM_C
#define CHM_C

#define CHM_HASHTABLE_BLOCK_ALLOCATION_SIZE 256
typedef struct
{
	unsigned long long data_array_index;
	void* key;

	struct _CHMap_item* hash_next;
} _CHMap_item;

#include <assert.h>

//src https://github.com/godotengine/godot/blob/master/core/templates/hashfuncs.h
uint32_t _CHMap_hash_rotl32(uint32_t x, int8_t r)
{
	return (x << r) | (x >> (32 - r));
}
uint32_t _CHMap_hash_fmix32(uint32_t h) {
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

uint32_t _CHMap_hash_buffer(const void* key, int length)
{
	uint32_t seed = 0x7F07C65;

	const uint8_t* data = (const uint8_t*)key;
	const int nblocks = length / 4;

	uint32_t h1 = seed;

	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;

	const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);

	for (int i = -nblocks; i; i++) {
		uint32_t k1 = blocks[i];

		k1 *= c1;
		k1 = _CHMap_hash_rotl32(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = _CHMap_hash_rotl32(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);

	uint32_t k1 = 0;

	switch (length & 3) {
	case 3:
		k1 ^= tail[2] << 16;
		//FALLTHROUGH
	case 2:
		k1 ^= tail[1] << 8;
		//FALLTHROUGH
	case 1:
		k1 ^= tail[0];
		k1 *= c1;
		k1 = _CHMap_hash_rotl32(k1, 15);
		k1 *= c2;
		h1 ^= k1;
	};

	// Finalize with additional bit mixing.
	h1 ^= length;

	return _CHMap_hash_fmix32(h1);
}

uint32_t _CHMap_Hash(CHMap* const chmap, const void* p_key)
{
	uint32_t hash = 0;

	//if a hash function is provided, call it
	if (chmap->hash_function)
	{
		hash = (*chmap->hash_function)((const int*)p_key);
	}
	//otherwise we will do hash function on the buffer
	else
	{
		hash = _CHMap_hash_buffer(p_key, chmap->key_bit_size);
	}

	return hash;
}


CHMap _CHMap_Init(CHMap_HashFun p_hashFun, size_t p_keyBitSize, size_t p_allocSize, size_t p_initReserveSize)
{
	assert(p_allocSize > 0);	
	assert(p_keyBitSize > 0);

	CHMap map;

	map.items = dA_INIT(_CHMap_item, p_initReserveSize);
	map.item_data = dA_INIT2(p_allocSize, p_initReserveSize);

	size_t hash_table_size = max(p_initReserveSize, CHM_HASHTABLE_BLOCK_ALLOCATION_SIZE);
	map.hash_table = dA_INIT(sizeof(_CHMap_item*), hash_table_size);

	map.hash_function = p_hashFun;
	map.key_bit_size = p_keyBitSize;

	return map;
}

void* _CHMap_getItemData(CHMap* const chmap, _CHMap_item* item)
{
	return dA_at(chmap->item_data, item->data_array_index);
}

_CHMap_item* _CHMap_findItem(CHMap* const chmap, const void* p_key)
{
	uint32_t hash = _CHMap_Hash(chmap, p_key);
	hash &= chmap->hash_table->capacity - 1;

	_CHMap_item** hash_array = chmap->hash_table->data;

	_CHMap_item* item;
	for (item = hash_array[hash]; item; item = item->hash_next)
	{
		//found
		if (*(char*)item->key == *(char*)p_key && !memcmp(item->key, p_key, chmap->key_bit_size))
		{
			return item;
		}
	}

	return NULL;
}

void* CHMap_Find(CHMap* const chmap, const void* p_key)
{
	_CHMap_item* find_item = _CHMap_findItem(chmap, p_key);

	if (!find_item)
	{
		return NULL;
	}

	return _CHMap_getItemData(chmap, find_item);
}

bool CHMap_Has(CHMap* const chmap, const void* p_key)
{
	_CHMap_item* find_item = _CHMap_findItem(chmap, p_key);

	return find_item != NULL;
}
void* CHMap_Insert(CHMap* const chmap, const void* p_key, const void* p_data)
{
	_CHMap_item* find_item = _CHMap_findItem(chmap, p_key);

	//if it exists already, return the data
	if (find_item)
	{
		return _CHMap_getItemData(chmap, find_item);
	}
	_CHMap_item* item = NULL;

	item = dA_emplaceBack(chmap->items);

	memcpy(item->key, p_key, chmap->key_bit_size);
	void* data_ptr = dA_emplaceBack(chmap->item_data);
	memcpy(data_ptr, p_data, chmap->item_data->alloc_size);
	item->data_array_index = chmap->item_data->elements_size - 1;

	if (chmap->hash_table->capacity + 1 <= chmap->items->elements_size)
	{
		dA_reserve(chmap->hash_table, CHM_HASHTABLE_BLOCK_ALLOCATION_SIZE);
	}

	uint32_t hash = _CHMap_Hash(chmap, p_key);
	hash &= chmap->hash_table->capacity - 1;

	_CHMap_item** hash_array = chmap->hash_table->data;

	item->hash_next = hash_array[hash];
	hash_array[hash] = item;

	return data_ptr;
}

void CHMap_Erase(CHMap* const chmap, const void* p_key)
{
	_CHMap_item** hash_array = chmap->hash_table->data;

	uint32_t hash = _CHMap_Hash(chmap, p_key);
	hash &= chmap->hash_table->capacity - 1;

	//remove from the hash table
	if (memcmp(hash_array[hash]->key, p_key, chmap->key_bit_size))
	{
		hash_array[hash] = hash_array[hash]->hash_next;
	}
	else
	{
		_CHMap_item* item = NULL;
		for (item = hash_array[hash]; item; item = item->hash_next)
		{
			if (item->hash_next)
			{
				_CHMap_item* hash_next = item->hash_next;
				if (*(char*)hash_next->key == *(char*)p_key && !memcmp(hash_next, p_key, chmap->key_bit_size))
				{
					item->hash_next = hash_next->hash_next;
					break;
				}
			}
		}
	}

	_CHMap_item* item_array = chmap->items->data;
	//remove from the item array
	for (size_t i = 0; i < chmap->items->elements_size; i++)
	{
		_CHMap_item* item = &item_array[i];

		//found it
		if (*(char*)item->key == *(char*)p_key && !memcmp(item->key, p_key, chmap->key_bit_size))
		{
			//free the data
			if (item->key)
			{
				free(item->key);
			}
			dA_erase(chmap->item_data, item->data_array_index, 1);
			dA_erase(chmap->items, i, 1);
			break;
		}
	}
}

void CHMap_Reserve(CHMap* const chmap, size_t p_amount)
{
	dA_reserve(chmap->items, p_amount);
	dA_reserve(chmap->item_data, p_amount);
}

void* CHMap_AtIndex(CHMap* const chmap, size_t p_index)
{
	return dA_at(chmap->item_data, p_index);
}

size_t CHMap_Size(CHMap* const chmap)
{
	return chmap->item_data->elements_size;
}

void CHMap_Clear(CHMap* const chmap)
{
	dA_clear(chmap->items);
	dA_clear(chmap->item_data);

	memset(chmap->hash_table, 0, sizeof(_CHMap_item*) * chmap->hash_table->capacity);
}

void CHMap_Destruct(CHMap* chmap)
{
	dA_Destruct(chmap->hash_table);

	_CHMap_item* item_array = chmap->items->data;
	//free all the items and data
	for (size_t i = 0; i < chmap->items->elements_size; i++)
	{
		_CHMap_item* item = &item_array[i];

		if (item->key)
		{
			free(item->key);
		}
	}
	dA_Destruct(chmap->items);
	dA_Destruct(chmap->item_data);
}

#endif
#endif
#endif