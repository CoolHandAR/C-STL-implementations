#include "thirdparty/utest.h"

#include "Basic_string.h"

UTEST_MAIN();

UTEST(STRING, EMPLACING_AND_POPPING)
{
	String* string = String_INIT("Hello");

	ASSERT_EQ(string->size, 5);
	ASSERT_EQ(string->capacity, 16);
	ASSERT_STREQ("Hello", string->data);

	String_pushBack(string, '!');
	ASSERT_EQ(string->size, 6);
	ASSERT_EQ(string->capacity, 16);
	ASSERT_STREQ("Hello!", string->data);

	String_popBack(string);
	ASSERT_EQ(string->size, 5);
	ASSERT_EQ(string->capacity, 16);
	ASSERT_STREQ("Hello", string->data);

	String_append(string, " World");
	ASSERT_EQ(string->size, 11);
	ASSERT_EQ(string->capacity, 16);
	ASSERT_STREQ("Hello World", string->data);

	String_Destruct(string);
}

UTEST(STRING, INSERTING_AND_ERASING)
{
	String* string = String_INIT("Hello World!");

	String_insert(string, " Cool", 5);
	ASSERT_EQ(string->size, 17);
	ASSERT_EQ(string->capacity, 32);
	ASSERT_STREQ("Hello Cool World!", string->data);

	String_erase(string, 5, 5);
	ASSERT_EQ(string->size, 12);
	ASSERT_EQ(string->capacity, 32);
	ASSERT_STREQ("Hello World!", string->data);

	String_Destruct(string);
}

UTEST(STRING, CHECKING_BOOL_OPERATIONS)
{
	String* string = String_INIT("Hello Hello World World!");

	ASSERT_TRUE(String_contains(string, "ello"));
	ASSERT_FALSE(String_contains(string, "Tree"));

	ASSERT_TRUE(String_startsWith(string, "Hello"));
	ASSERT_FALSE(String_startsWith(string, "ello"));

	ASSERT_TRUE(String_endsWith(string, "orld!"));
	ASSERT_FALSE(String_endsWith(string, "Gorld"));
	
	ASSERT_EQ(String_findFirstOf(string, "ello"), 1);
	ASSERT_EQ(String_findFirstOf(string, "Gello"), STRING_NPOS);

	ASSERT_EQ(String_findLastOf(string, "ello"), 7);
	ASSERT_EQ(String_findLastOf(string, "Gello"), STRING_NPOS);

	String_Destruct(string);
}

UTEST(STRING, SIZING_AND_RESERVING)
{
	String* string = String_INIT("Hello World!");

	String_reserve(string, 45);
	ASSERT_EQ(string->capacity, 64);
	ASSERT_EQ(string->size, 12);
	
	String_resize(string, 5);
	ASSERT_EQ(string->capacity, 64);
	ASSERT_EQ(string->size, 5);
	ASSERT_STREQ("Hello", string->data);

	String_shrinkToFit(string);
	ASSERT_EQ(string->capacity, 16);
	ASSERT_EQ(string->size, 5);

	String_clear(string);
	ASSERT_EQ(string->capacity, 16);
	ASSERT_EQ(string->size, 0);
	ASSERT_STREQ("", string->data);

	String_Destruct(string);
}
UTEST(STRING, SETTING)
{
	String* string = String_INIT("Hello World!");

	String_set(string, "Another completely new string");

	ASSERT_EQ(string->size, 29);
	ASSERT_STREQ("Another completely new string", string->data);
	
	String_Destruct(string);
}