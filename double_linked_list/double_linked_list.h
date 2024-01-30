#ifndef DOUBLE_LINKED_LIST_H
#define DOUBLE_LINKED_LIST_H

#include <stdlib.h>
#include <assert.h>

typedef struct DLL_Node
{
    void* value; // The value stored;
    struct DLL_Node* prev; // Node before this node
    struct DLL_Node* next; // Node after this node;

} DLL_Node;


typedef struct DLL_Head
{
    size_t node_size; // The amount of nodes currently stored in the list;
    size_t alloc_size; //The allocation byte size;;
    DLL_Node* next; // First node in the list;
} DLL_Head;

/**
 * Init the doubly linked list
\param T The type of item that will be stored

\return Pointer to the DLL_Head
*/
#define DLL_INIT(T) _DLL_Init(sizeof(T))

/*
Internal! DO NOT USE!
*/
static DLL_Head* _DLL_Init(size_t p_allocSize)
{
    assert(p_allocSize > 0 && "Alloc size must be higher than 0");
    
    DLL_Head* head_ptr = malloc(sizeof(DLL_Head));

    //we failed to alloc the dynamic array
    if(head_ptr == NULL)
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
static DLL_Node* _DLL_AllocSingleNode(DLL_Head* p_head)
{
    assert(p_head->alloc_size > 0 && "Alloc size not set");

    DLL_Node* new_node = malloc(sizeof(DLL_Node));
    
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
    new_node->prev = NULL;

    return new_node;
}


/**
 * Place a node in the back of the list
\param p_head Pointer to the List head

\return The emplaced node
*/
DLL_Node* DLL_emplaceBack(DLL_Head* p_head)
{
    //Add to head if its the first element
    if(p_head->next == NULL)
    {
        DLL_Node* new_node = _DLL_AllocSingleNode(p_head);
        
        if (new_node == NULL)
            return NULL;

        p_head->next = new_node;
        p_head->node_size++;

        return new_node;
    }
 
    DLL_Node* candidate = p_head->next;

    //find the last node without a active next node
    while (candidate->next != NULL)
    {
        candidate = candidate->next;
    }
    
    DLL_Node* new_node = _DLL_AllocSingleNode(p_head);

    if (new_node == NULL)
        return NULL;

    new_node->prev = candidate;
    candidate->next = new_node;

    p_head->node_size++;

    return new_node;
}

/**
 * Place a node in the front of the list
\param p_head Pointer to the List head

\return The emplaced node
*/
DLL_Node* DLL_emplaceFront(DLL_Head* p_head)
{
    assert(p_head->alloc_size > 0 && "Alloc size not set");
    
    DLL_Node* new_node = _DLL_AllocSingleNode(p_head);
 
    if (new_node == NULL)
        return NULL;

    new_node->prev = NULL; //NULL == p_head

    new_node->next = p_head->next;
    p_head->next = new_node;
    p_head->node_size++;

    return new_node;
}
/**
 * Place a node after the specified node
\param p_head Pointer to the List head
\param p_node The node to insert after to

\return The inserted node
*/
DLL_Node* DLL_insertAfterNode(DLL_Head* p_head, DLL_Node* p_node)
{
    assert(p_head->alloc_size > 0 && "Alloc size not set");
    assert(p_head->node_size > 0 && "The node does not belong to the list");

    DLL_Node* new_node = _DLL_AllocSingleNode(p_head);
    
    if (new_node == NULL)
        return NULL;

    new_node->prev = p_node;
    new_node->next = p_node->next;
    p_node->next = new_node;

    p_head->node_size++;

    return new_node;
};
/**
 * Place a node before the specified node
\param p_head Pointer to the List head
\param p_node The node to insert before to

\return The inserted node
*/
DLL_Node* DLL_insertBeforeNode(DLL_Head* p_head, DLL_Node* p_node)
{
    assert(p_head->node_size > 0 && "The node does not belong to the list");

    DLL_Node* new_node = _DLL_AllocSingleNode(p_head);

    if (new_node == NULL)
        return NULL;

    new_node->prev = p_node->prev;
    new_node->next = p_node;
    p_node->prev->next = new_node;

    p_head->node_size++;

    return new_node;
};

/**
 * Insert nodes after specified index
\param p_head Pointer to the List head
\param p_index The index to insert after
\param p_amount The amount to insert

\return Pointer to the first node inserted
*/
DLL_Node* DLL_insertAfterIndex(DLL_Head* p_head, size_t p_index, size_t p_amount)
{
    assert(p_index < p_head->node_size && "Index out of bounds");
    assert(p_amount > 0 && "Amount must be higher than zero");

    DLL_Node* node = p_head->next;
    for (size_t i = 0; i < p_index; i++)
    {
        node = node->next;
    }
    DLL_Node* first_inserted_node = NULL;
    DLL_Node* last_inserted_node = NULL;
    for (size_t i = 0; i < p_amount; i++)
    {
        DLL_Node* inserted_node = DLL_insertAfterNode(p_head, node);

        if (i == 0)
            first_inserted_node = inserted_node;

        else
            last_inserted_node = inserted_node;
    }

    return first_inserted_node;
}

/**
 * Erase a node after the specified node
\param p_head Pointer to the List head
\param p_node The node to erase after
*/
void DLL_eraseAfterNode(DLL_Head* p_head, DLL_Node* p_node)
{
    assert(p_node->next != NULL && "No node after p_node to erase");

    DLL_Node* erase_target = p_node->next;

    p_node->next = erase_target->next;
    p_node->next->prev = p_node;

    free(erase_target->value);
    free(erase_target);

    p_head->node_size--;
}
/**
 * Erase a node before the specified node
\param p_head Pointer to the List head
\param p_node The node to erase before
*/
void DLL_eraseBeforeNode(DLL_Head* p_head, DLL_Node* p_node)
{
    assert(p_node->prev != NULL && "No node before p_node to erase");

    DLL_Node* erase_target = p_node->prev;

    p_node->prev = erase_target->prev;
    p_node->prev->next = p_node;

    free(erase_target->value);
    free(erase_target);

    p_head->node_size--;
}
/**
 * Erase nodes after specified index
\param p_head Pointer to the List head
\param p_index The index to erase after
\param p_amount The amount to erase
*/
void DLL_eraseAfterIndex(DLL_Head* p_head, size_t p_index, size_t p_amount)
{
    assert(p_head->alloc_size > 0 && "Alloc size not set");
    assert(p_index < p_head->node_size && "Index out of bounds");
    assert(p_amount <= (p_head->node_size - p_index) && "Trying to erase more nodes than possible");

    if (p_amount == 0)
        return;

    DLL_Node* node = p_head->next;

    for (size_t i = 0; i < p_index; i++)
    {
        node = node->next;
    }

    for (size_t i = 0; i < p_amount; i++)
    {
        DLL_eraseAfterNode(p_head, node);
    }
}
/**
 * Remove the last node from the list
\param p_head Pointer to the List head
*/
void DLL_popLast(DLL_Head* p_head)
{
    assert(p_head->node_size > 0 && "No nodes to pop");

    DLL_Node* last_node = p_head->next;

    //Find the last node
    while (last_node->next != NULL)
    {
        last_node = last_node->next;
    }

    //should not happen
    if(last_node == NULL)
        return;

    //Release from the prev
    if(last_node->prev != NULL)
    {
        last_node->prev->next = NULL;
    }
    //if prev is null then it must be the head
    else
    {
        p_head->next = NULL;
    }

    //free the last node and its value
    free(last_node->value);
    free(last_node);

    p_head->node_size--;
}

/**
 * Remove the first node from the list
\param p_head Pointer to the List head
*/
void DLL_popFront(DLL_Head* p_head)
{
    assert(p_head->node_size > 0 && "No nodes to pop");

    DLL_Node* target_node = p_head->next;

    p_head->next = target_node->next;

    //free the node and its value
    free(target_node->value);
    free(target_node);

    p_head->node_size--;
}

/**
 * Find and remove the specifed node
\param p_head Pointer to the List head
\param p_targetNode The node to remove
*/
void DLL_Remove(DLL_Head* p_head, DLL_Node* p_targetNode)
{
    assert(p_head->alloc_size > 0 && "Alloc size must be higher than 0");

    if (p_targetNode == NULL || p_head->next == NULL)
        return;

    DLL_Node* find_node = p_head->next;

    for (size_t i = 0; i <= p_head->node_size; i++)
    {
        find_node = find_node->next;

        if (find_node == p_targetNode)
        {
            break;
        }
    }

    //we failed to find the node for some reason
    if (find_node == NULL)
        return;

    //if our prev node is not the head swap the next ptr
    if (find_node->prev != NULL)
    {
        find_node->prev->next = find_node->next;
    }
    //do the same if head
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
 * Get node at the specified index
\param p_head Pointer to the List head
\param p_index Index of the node to find
\return Pointer to the node
*/
DLL_Node* DLL_at(DLL_Head* p_head, size_t p_index)
{
    assert(p_head->node_size > 0 && "No nodes emplaced");
    assert(p_index < p_head->node_size && "Index out of bounds");

    DLL_Node* find_node = p_head->next;

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


DLL_Node* DLL_getLast(DLL_Head* p_head)
{
    assert(p_head->node_size > 0 && "No nodes emplaced");

    DLL_Node* find_node = p_head->next;

    while (find_node->next != NULL)
    {
        find_node = find_node->next;
    }

    return find_node;
}


/**
 * Removes all nodes from the list
\param p_head Pointer to the List head
*/
void DLL_clear(DLL_Head* p_head)
{
    //NOT VERY EFFICIENT FUNCTION. NEED TO FIX IN THE FUTURE. We should probaly delete using the prev node

    DLL_Node* last_node = p_head->next;
    //Delete starting from the last node
    while (last_node != NULL)
    {
        //Find the last node
        while (last_node->next != NULL)
        {
            last_node = last_node->next;
        }

        //Release from the prev
        if (last_node->prev != NULL)
        {
            last_node->prev->next = NULL;
        }
        //if prev is null then it must be the head
        else
        {
            p_head->next = NULL;
        }

        //free the last node and its value
        free(last_node->value);
        free(last_node);

        p_head->node_size--;

        //reset for the next loop
        last_node = p_head->next;
    }
}
/**
 * Destroys all nodes from the list and frees the head's memory
\param p_head Pointer to the List head
*/
void DLL_Destruct(DLL_Head* p_head)
{
    DLL_clear(p_head);
    
    free(p_head);

    p_head = NULL;
}

#endif