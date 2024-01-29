#include "forward_list.h"

#include "thirdparty/utest.h"


UTEST_MAIN();

UTEST(FORWARD_LIST, EMPLACING_AND_POPPING)
{
    FL_Head* forward_list = FL_INIT(int);

    FL_Node* node1 = FL_emplaceBack(forward_list);

    ASSERT_EQ(forward_list->alloc_size, sizeof(int));
    ASSERT_EQ(forward_list->node_size, 1);
    ASSERT_EQ(*(int*)node1->value, 0);

    FL_Node* node2 = FL_emplaceBack(forward_list);

    *(int*)node2->value = 200;

    FL_Node* node3 = FL_emplaceBack(forward_list);

    FL_Node* find_node = FL_at(forward_list, 1);

    ASSERT_TRUE(find_node != NULL);
    ASSERT_EQ(*(int*)find_node->value, 200);
    ASSERT_EQ(forward_list->node_size, 3);
    
    FL_Node* node4 = FL_emplaceFront(forward_list);
    *(int*)node4->value = 45;

    FL_Node* find_node2 = FL_at(forward_list, 0);
    ASSERT_EQ(*(int*)find_node2->value, *(int*)node4->value);
    ASSERT_EQ(forward_list->node_size, 4);

    FL_popFront(forward_list);
    ASSERT_EQ(forward_list->node_size, 3);

    FL_popLast(forward_list);
    ASSERT_EQ(forward_list->node_size, 2);


    FL_Destruct(forward_list);
}

UTEST(FORWARD_LIST, INSERTION_AND_REMOVING)
{
    FL_Head* forward_list = FL_INIT(int);

    FL_Node* node1 = FL_emplaceBack(forward_list);
    FL_Node* node2 = FL_emplaceFront(forward_list);

    FL_Node* node3 = FL_insertAfterNode(forward_list, node1);
    *(int*)node3->value = 12.0f;

    ASSERT_EQ(forward_list->node_size, 3);

    FL_Node* find_node = FL_at(forward_list, 2);

    ASSERT_EQ(*(int*)find_node->value, *(int*)node3->value);

    FL_eraseAfterNode(forward_list, node2);
    ASSERT_EQ(forward_list->node_size, 2);

    FL_insertAfterIndex(forward_list, 1, 10);

    ASSERT_EQ(forward_list->node_size, 12);

    FL_eraseAfterIndex(forward_list, 0, 10);

    ASSERT_EQ(forward_list->node_size, 2);


    int i = 0;
    FL_Node* itr_node = forward_list->next;
    while(itr_node != NULL)
    {
        *(int*)itr_node->value = i;

        ASSERT_EQ(*(int*)itr_node->value, i);
        itr_node = itr_node->next;
        i++;
    }

    FL_remove(forward_list, node1);
    ASSERT_EQ(forward_list->node_size, 1);
    
    FL_emplaceBack(forward_list);
    ASSERT_EQ(forward_list->node_size, 2);
    
    FL_removeAtIndex(forward_list, 0);

    ASSERT_EQ(forward_list->node_size, 1);

    FL_Destruct(forward_list);
}
