#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H
#pragma once

#include <stdlib.h>

/*
   Usage define on atleast one .c file
   #define FORWARD_LIST_IMPLEMENTATION
   #include "forward_list.h"
*/

typedef struct FL_Node
{
    void* value; // The value stored;
    struct FL_Node* next; // Node after this node;
} FL_Node;

typedef struct FL_Head
{
    size_t alloc_size; //The allocation byte size;
    size_t node_size; // The amount of nodes currently stored in the list;
    FL_Node* next; // First node in the list;
} FL_Head;


#ifdef __cplusplus
extern "C" {
#endif
/**
* Init the forward list
\param T The type of item that will be stored

\return Pointer to the FL_Forward_Head
*/
#define FL_INIT(T) _FL_Init(sizeof(T))
    extern FL_Node* FL_emplaceBack(FL_Head* p_head);
    extern FL_Node* FL_emplaceFront(FL_Head* p_head);
    extern FL_Node* FL_insertAfterNode(FL_Head* p_head, FL_Node* p_node);
    extern void FL_eraseAfterNode(FL_Head* p_head, FL_Node* p_node);
    extern FL_Node* FL_insertAfterIndex(FL_Head* p_head, size_t p_index, size_t p_amount);
    extern void FL_eraseAfterIndex(FL_Head* p_head, size_t p_index, size_t p_amount);
    extern void FL_popFront(FL_Head* p_head);
    extern void FL_popLast(FL_Head* p_head);
    extern void FL_remove(FL_Head* p_head, FL_Node* p_targetNode);
    extern void FL_removeAtIndex(FL_Head* p_head, size_t p_index);
    extern FL_Node* FL_at(FL_Head* p_head, size_t p_index);
    extern void FL_clear(FL_Head* p_head);
    extern void FL_Destruct(FL_Head* p_head);
#ifdef __cplusplus
}
#endif


/*
This is for preventing greying out of the implementation section.
*/
#if defined(Q_CREATOR_RUN) || defined(__INTELLISENSE__) || defined(__CDT_PARSER__)
#define FORWARD_LIST_IMPLEMENTATION
#endif

#if defined(FORWARD_LIST_IMPLEMENTATION)
#ifndef FORWARD_LIST_C
#define FORWARD_LIST_C

#include <assert.h>

/*
Internal! DO NOT USE!
*/
FL_Head* _FL_Init(size_t p_allocSize)
{
    assert(p_allocSize > 0 && "Invalid item. Can't Allocate");

    FL_Head* head_ptr = malloc(sizeof(FL_Head));

    //we failed to alloc
    if (head_ptr == NULL)
        return NULL;

    //SUCCESS
    head_ptr->alloc_size = p_allocSize;
    head_ptr->node_size = 0;
    head_ptr->next = NULL;

    return head_ptr;
}
/*
Internal! DO NOT USE!
*/
FL_Head* _FL_AllocSingleNode(FL_Head* p_head)
{
    assert(p_head->alloc_size > 0 && "Alloc size not set");

    FL_Node* new_node = malloc(sizeof(FL_Node));

    //Failed to Alloc
    if (new_node == NULL)
        return NULL;

    new_node->value = calloc(1, p_head->alloc_size);

    //Failed to alloc value
    if (new_node->value == NULL)
    {
        free(new_node);
        return NULL;
    }

    new_node->next = NULL;

    return new_node;
}

/**
 * Place a node in the back of the list
\param p_head Pointer to the List head

\return The emplaced node
*/
FL_Node* FL_emplaceBack(FL_Head* p_head)
{
    //Add to head if its the first element
    if (p_head->next == NULL)
    {
        FL_Node* node = _FL_AllocSingleNode(p_head);

        if (node == NULL)
            return NULL;

        p_head->next = node;
        p_head->node_size++;

        return p_head->next;
    }

    FL_Node* candidate = p_head->next;

    //find the last node without a active next node
    while (candidate->next != NULL)
    {
        candidate = candidate->next;
    }

    FL_Node* new_node = _FL_AllocSingleNode(p_head);

    if (new_node == NULL)
        return NULL;

    candidate->next = new_node;

    p_head->node_size++;

    return new_node;
}

/**
 * Place a node in the front of the list
\param p_head Pointer to the List head

\return The emplaced node
*/
FL_Node* FL_emplaceFront(FL_Head* p_head)
{
    assert(p_head->alloc_size > 0 && "Alloc size not set");

    //Add to head if its the first element
    if (p_head->next == NULL)
    {
        FL_Node* node = _FL_AllocSingleNode(p_head);

        if (node == NULL)
            return NULL;

        p_head->next = node;
        p_head->node_size++;

        return p_head->next;
    }

    FL_Node* new_node = _FL_AllocSingleNode(p_head);

    if (new_node == NULL)
        return NULL;

    //Take the next from the head
    new_node->next = p_head->next;

    p_head->next = new_node;

    p_head->node_size++;

    return p_head->next;
}

/**
 * Place a node after the specified node
\param p_head Pointer to the List head
\param p_node The node to insert after to

\return The inserted node
*/
FL_Node* FL_insertAfterNode(FL_Head* p_head, FL_Node* p_node)
{
    FL_Node* new_node = _FL_AllocSingleNode(p_head);

    if (new_node == NULL)
        return NULL;

    new_node->next = p_node->next;

    p_node->next = new_node;
    p_head->node_size++;

    return new_node;
};

/**
 * Erase a node after the specified node
\param p_head Pointer to the List head
\param p_node The node to erase after
*/
void FL_eraseAfterNode(FL_Head* p_head, FL_Node* p_node)
{
    assert(p_node->next != NULL && "No node after p_node to erase");

    FL_Node* erase_target = p_node->next;

    p_node->next = erase_target->next;

    free(erase_target->value);
    free(erase_target);

    p_head->node_size--;
};

/**
 * Insert nodes after specified index
\param p_head Pointer to the List head
\param p_index The index to insert after
\param p_amount The amount to insert

\return Pointer to the first node inserted
*/
FL_Node* FL_insertAfterIndex(FL_Head* p_head, size_t p_index, size_t p_amount)
{
    assert(p_index < p_head->node_size && "Index out of bounds");
    assert(p_amount > 0 && "Amount must be higher than zero");

    FL_Node* node = p_head->next;
    for (size_t i = 0; i < p_index; i++)
    {
        node = node->next;
    }
    FL_Node* first_inserted_node = NULL;
    FL_Node* last_inserted_node = NULL;
    for (size_t i = 0; i < p_amount; i++)
    {
        FL_Node* inserted_node = FL_insertAfterNode(p_head, node);

        if (i == 0)
            first_inserted_node = inserted_node;

        else
            last_inserted_node = inserted_node;
    }


    return first_inserted_node;
}

/**
 * Erase nodes after specified index
\param p_head Pointer to the List head
\param p_index The index to erase after
\param p_amount The amount to erase
*/
void FL_eraseAfterIndex(FL_Head* p_head, size_t p_index, size_t p_amount)
{
    assert(p_index < p_head->node_size && "Index out of bounds");
    assert(p_amount <= (p_head->node_size - p_index) && "Trying to erase more nodes than possible");

    if (p_amount == 0)
        return;

    FL_Node* node = p_head->next;
    for (size_t i = 0; i < p_index; i++)
    {
        node = node->next;
    }

    for (size_t i = 0; i < p_amount; i++)
    {
        FL_eraseAfterNode(p_head, node);
    }
}

/**
 * Remove the first node from the list
\param p_head Pointer to the List head
*/
void FL_popFront(FL_Head* p_head)
{
    assert(p_head->node_size > 0 && "No nodes to delete");

    FL_Node* pop_target = p_head->next;

    p_head->next = pop_target->next;

    //free the first and its value
    free(pop_target->value);
    free(pop_target);

    p_head->node_size--;
}
/**
 * Remove the last node from the list
\param p_head Pointer to the List head
*/
void FL_popLast(FL_Head* p_head)
{
    assert(p_head->node_size > 0 && "No nodes to delete");

    FL_Node* prev_node = NULL;
    FL_Node* last_node = p_head->next;
    //Find the last node and the prev node before it
    while (last_node->next != NULL)
    {
        prev_node = last_node;
        last_node = last_node->next;
    }

    //free the last node and its value
    free(last_node->value);
    free(last_node);

    //set the previous node's next to null
    if (prev_node != NULL)
        prev_node->next = NULL;
    //otherwise the last node must belong to the head
    else
        p_head->next = NULL;

    p_head->node_size--;
}
/**
 * Find and remove the specifed node
\param p_head Pointer to the List head
\param p_targetNode The node to remove
*/
void FL_remove(FL_Head* p_head, FL_Node* p_targetNode)
{
    assert(p_head->alloc_size > 0 && "Alloc size must be higher than 0");

    if (p_targetNode == NULL || p_head->next == NULL)
        return;

    FL_Node* prev_node = NULL;
    FL_Node* find_node = p_head->next;

    for (size_t i = 0; i <= p_head->node_size; i++)
    {
        prev_node = find_node;
        find_node = find_node->next;

        if (find_node == p_targetNode)
        {
            break;
        }
    }

    //we failed to find the node for some reason
    if (find_node == NULL)
        return;

    //if our prev node is valid add the next to the prev one
    if (prev_node != NULL)
    {
        prev_node->next = find_node->next;
    }
    //otherwise we add to the head
    else
    {
        p_head->next = find_node->next;
    }


    //free the value and the node
    free(find_node->value);
    free(find_node);

    p_head->node_size--;

    p_targetNode = NULL;
};

/**
 * Find and remove node at the specified index
\param p_head Pointer to the List head
\param p_index Index of the node to remove
*/
void FL_removeAtIndex(FL_Head* p_head, size_t p_index)
{
    assert(p_head->alloc_size > 0 && "Alloc size must be higher than 0");
    assert(p_index < p_head->node_size && "Index out of bounds");

    FL_Node* prev_node = NULL;
    FL_Node* find_node = p_head->next;
    for (size_t i = 0; i < p_index; i++)
    {
        prev_node = find_node;
        find_node = find_node->next;
    }

    //we failed to find the node for some reason
    if (find_node == NULL)
        return;

    //check if we have a value after this one
    if (find_node->next != NULL)
    {
        //if our prev node is valid add the next to the prev one
        if (prev_node != NULL)
        {
            prev_node->next = find_node->next;
        }
        //otherwise we add to the head
        else
        {
            p_head->next = find_node->next;
        }
    }

    //free the value and the node
    free(find_node->value);
    free(find_node);

    p_head->node_size--;
}

/**
 * Get node at the specified index
\param p_head Pointer to the List head
\param p_index Index of the node to find
\return Pointer to the node
*/
FL_Node* FL_at(FL_Head* p_head, size_t p_index)
{
    assert(p_head->alloc_size > 0 && "Alloc size must be higher than 0");
    assert(p_index < p_head->node_size && "Index out of bounds");

    FL_Node* find_node = p_head->next;

    for (size_t i = 0; i < p_index; i++)
    {
        if (find_node->next == NULL)
            break;

        find_node = find_node->next;
    }

    //we failed to find the node for some reason
    if (find_node == NULL)
        return NULL;

    return find_node;
}

/**
 * Removes all nodes from the list
\param p_head Pointer to the List head
*/
void FL_clear(FL_Head* p_head)
{
    FL_Node* prev_node = NULL;
    FL_Node* last_node = p_head->next;
    //Delete starting from the last node
    while (last_node != NULL)
    {
        //Find the last node and the prev node before it
        while (last_node->next != NULL)
        {
            prev_node = last_node;
            last_node = last_node->next;
        }

        //free the last node and its value
        free(last_node->value);
        free(last_node);

        //set the previous node's next to null
        if (prev_node != NULL)
            prev_node->next = NULL;
        //otherwise the last node must belong to the head
        else
            p_head->next = NULL;

        p_head->node_size--;

        //reset for the next loop
        prev_node = NULL;
        last_node = p_head->next;
    }
}

/**
 * Destroys all nodes from the list and frees the head's memory
\param p_head Pointer to the List head
*/
void FL_Destruct(FL_Head* p_head)
{
    FL_clear(p_head);

    free(p_head);

    p_head = NULL;
}

#endif //FORWARD_LIST_C
#endif 
#endif // FORWARD_LIST_H