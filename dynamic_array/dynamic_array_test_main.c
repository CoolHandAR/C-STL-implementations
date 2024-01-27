
#include "thirdparty/utest.h"
#include "dynamic_array.h"

UTEST_MAIN();

UTEST(dA_INSERTION, EMPLACING_ITEMS)
{
    dynamic_array* dynamic_array = dA_INIT(int, 24);

    ASSERT_EQ(dynamic_array->alloc_size, sizeof(int));
    ASSERT_EQ(dynamic_array->capacity, 24);
    ASSERT_EQ(dynamic_array->elements_size, 24);
    
    dA_emplaceBack(dynamic_array);
    ASSERT_EQ(dynamic_array->capacity, 25);
    ASSERT_EQ(dynamic_array->elements_size, 25);    

    dA_emplaceBackMultiple(dynamic_array, 12);
    ASSERT_EQ(dynamic_array->capacity, 37);
    ASSERT_EQ(dynamic_array->elements_size, 37);

    for(int i = 0; i < dynamic_array->elements_size; i++)
    {
        int* array = dynamic_array->data;

        //ALL VALUES SHOULD BE MEMSET TO 0
        ASSERT_EQ(array[i], 0);
    }

    dA_Destruct(dynamic_array);
}

UTEST(dA_INSERTION, EMPLACING_AND_DELETING_ITEMS)
{
    typedef struct Vector3f_UnitTest
    {
        float x, y, z;
    } Vector3f_UnitTest;

    dynamic_array* dynamic_array = dA_INIT(Vector3f_UnitTest, 1000);

    ASSERT_EQ(dynamic_array->alloc_size, sizeof(Vector3f_UnitTest));
    
    dA_emplaceBack(dynamic_array);
    ASSERT_EQ(dynamic_array->elements_size, 1001);
    ASSERT_EQ(dynamic_array->capacity, 1001);
    
    dA_popBack(dynamic_array);
    ASSERT_EQ(dynamic_array->elements_size, 1000);
    ASSERT_EQ(dynamic_array->capacity, 1001);

    dA_emplaceBackMultiple(dynamic_array, 20);
    ASSERT_EQ(dynamic_array->elements_size, 1020);
    ASSERT_EQ(dynamic_array->capacity, 1020);

    dA_popBackMultiple(dynamic_array, 100);
    ASSERT_EQ(dynamic_array->elements_size, 920);
    ASSERT_EQ(dynamic_array->capacity, 1020);
    
    for(int i = 0; i < dynamic_array->elements_size; i++)
    {
        Vector3f_UnitTest* array = dynamic_array->data;

        //ALL VALUES SHOULD BE MEMSET TO 0.0f
        ASSERT_EQ(array[i].x, 0.0f);
        ASSERT_EQ(array[i].y, 0.0f);
        ASSERT_EQ(array[i].z, 0.0f);
    }

    dA_Destruct(dynamic_array);
}

UTEST(dA_INSERTION, TESTING_INSERTION_DELETION)
{
    dynamic_array* dynamic_array = dA_INIT(size_t, 3);

    ASSERT_EQ(dynamic_array->alloc_size, sizeof(size_t));
    
    for(int i = 0; i < dynamic_array->elements_size; i++)
    {
        size_t* array = dynamic_array->data;

        array[i] = i;
    }
    
    dA_insert(dynamic_array, 1, 2);
    ASSERT_EQ(dynamic_array->elements_size, 5);
    ASSERT_EQ(dynamic_array->capacity, 5);

    size_t* array = dynamic_array->data;
    ASSERT_EQ(array[0], 0);
    ASSERT_EQ(array[1], 0);
    ASSERT_EQ(array[2], 0);
    ASSERT_EQ(array[3], 1);
    ASSERT_EQ(array[4], 2);

    dA_erase(dynamic_array, 1, 2);
    ASSERT_EQ(dynamic_array->elements_size, 3);
    ASSERT_EQ(dynamic_array->capacity, 5);

    ASSERT_EQ(array[0], 0);
    ASSERT_EQ(array[1], 1);
    ASSERT_EQ(array[2], 2);

    dA_Destruct(dynamic_array);
}

UTEST(dA_CAPACITY, TESTING_RESIZIGN_AND_CAPACITY)
{
    dynamic_array* dynamic_array = dA_INIT(size_t, 5);

    ASSERT_EQ(dynamic_array->alloc_size, sizeof(size_t));
    
    dA_resize(dynamic_array, 10);
    ASSERT_EQ(dynamic_array->elements_size, 10);
    ASSERT_EQ(dynamic_array->capacity, 10);

    dA_reserve(dynamic_array, 5);
    ASSERT_EQ(dynamic_array->elements_size, 10);
    ASSERT_EQ(dynamic_array->capacity, 15);
    
    dA_emplaceBackMultiple(dynamic_array, 4);
    ASSERT_EQ(dynamic_array->elements_size, 14);
    ASSERT_EQ(dynamic_array->capacity, 15);

    dA_popBackMultiple(dynamic_array, 4);
    ASSERT_EQ(dynamic_array->elements_size, 10);
    ASSERT_EQ(dynamic_array->capacity, 15);

    dA_shrinkToFit(dynamic_array);
    ASSERT_EQ(dynamic_array->elements_size, 10);
    ASSERT_EQ(dynamic_array->capacity, 10);

    dA_Destruct(dynamic_array);
}