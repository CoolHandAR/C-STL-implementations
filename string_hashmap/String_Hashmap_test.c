#define DYNAMIC_ARRAY_IMPLEMENTATION
#define SHM_IMPLEMENTATION
#include "String_Hashmap.h"

#include "thirdparty/utest.h"

UTEST_MAIN();


UTEST(STRING_HASHMAP, INSERTING_AND_ERASING)
{
	SHMap map = SHMAP_INIT(int, 10);

	int value = 2;
	int* inserted_int = SHMap_Insert(&map, "Tuesday", &value);

	ASSERT_TRUE(inserted_int);

	int* find_int = SHMap_Find(&map, "Tuesday");

	ASSERT_EQ(*find_int, 2);

	value = 5;
	int* second_inserted_int = SHMap_Insert(&map, "Friday", &value);

	ASSERT_TRUE(*second_inserted_int);

	int* second_found_int = SHMap_Find(&map, "Friday");

	ASSERT_EQ(*second_found_int, 5);
	
	SHMap_Erase(&map, "Tuesday");

	int* should_be_null = SHMap_Find(&map, "Tuesday");

	ASSERT_FALSE(should_be_null);

	SHMap_Destruct(&map);
}

UTEST(STRING_HASHMAP, INSERTING_AND_ERASING)