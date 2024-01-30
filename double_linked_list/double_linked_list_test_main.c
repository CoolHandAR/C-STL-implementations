#include "double_linked_list.h"

#include "thirdparty/utest.h"

UTEST_MAIN();

UTEST(DLL, EMPLACING_AND_POPPING)
{
	DLL_Head* list = DLL_INIT(int);

	DLL_Node* node1 = DLL_emplaceBack(list);
	ASSERT_EQ(list->node_size, 1);
	
	DLL_Node* node2 = DLL_emplaceFront(list);
	ASSERT_EQ(list->node_size, 2);

	DLL_Node* node3 = DLL_emplaceBack(list);

	DLL_popFront(list);
	ASSERT_EQ(list->node_size, 2);

	DLL_popLast(list);
	ASSERT_EQ(list->node_size, 1);
	

	DLL_Destruct(list);
}

UTEST(DLL, INSERTING_AND_ERASING)
{
	DLL_Head* list = DLL_INIT(int);

	DLL_Node* node1 = DLL_emplaceBack(list);
	DLL_Node* node2 = DLL_emplaceBack(list);
	*(int*)node2->value = 12;

	DLL_Node* node3 = DLL_insertAfterNode(list, node2);

	ASSERT_EQ(list->node_size, 3);
	ASSERT_EQ(*(int*)node3->prev->value, *(int*)node2->value);

	DLL_Node* node4 = DLL_insertBeforeNode(list, node2);

	ASSERT_EQ(list->node_size, 4);
	ASSERT_EQ(*(int*)node4->next->value, *(int*)node2->value);

	DLL_Node* itr_node = list->next;
	for (int i = 0; i < list->node_size; i++)
	{
		*(int*)itr_node->value = i;

		if(itr_node->next != NULL)
			itr_node = itr_node->next;
	}

	DLL_Node* itr_last_node = DLL_getLast(list);
	
	ASSERT_EQ(itr_node, itr_last_node);
	
	DLL_Node* node_set_start = DLL_insertAfterIndex(list, 2, 100);
	ASSERT_EQ(list->node_size, 104);

	DLL_eraseAfterIndex(list, 2, 100);
	ASSERT_EQ(list->node_size, 4);

	DLL_Destruct(list);
}