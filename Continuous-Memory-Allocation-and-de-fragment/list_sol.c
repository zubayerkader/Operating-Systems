#include "list_sol.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static _Bool doSinglePassOnSort(Node **headRef);
static void swapElements(Node **previous, Node *nodeA, Node *nodeB);


/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
Node* List_createNode (int size, char type, void* pointer)
{
    Node* head = malloc(sizeof(Node));

    head->size = size;
    head->type = type;
    head->ptr = pointer;
    head->next = NULL;

    return head;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead (Node **headRef, Node *node)
{
	node->next = *headRef;
	*headRef = node;
}

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (Node **headRef, Node *node)
{
	node->next = NULL;

	// Handle empty list
	if (*headRef == NULL) {
		*headRef = node;
	}
	else {
		// Find the tail and insert node
		Node *current = *headRef;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = node;
	}
}

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (Node *head)
{
	int count = 0;
	Node *current = head;
	while (current != NULL) {
		current = current->next;
		count++;
	}
	return count;
}

/*
 * Return the first node holding the value item, return NULL if none found
 */
Node* List_findNode(Node *head, void* mem_ptr)
{
	Node *current = head;
	while (current != NULL) {
		if (current->ptr == mem_ptr) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set (by for example
 * calling List_findNode()) to a valid node in the list. If the list contains
 * only node, the head of the list should be set to NULL.
 */
void List_deleteNode (Node **headRef, Node *node)
{
	assert(headRef != NULL);
	assert(*headRef != NULL);

	// Is it the first element?
	if (*headRef == node) {
		*headRef = node->next;
	}
	else {
		// Find the previous node:
		Node *previous = *headRef;
		while (previous->next != node) {
			previous = previous->next;
			assert(previous != NULL);
		}

		// Unlink node:
		assert(previous->next == node);
		previous->next = node->next;
	}

	// Free memory:
	free(node);
}


/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort (Node **headRef)
{
	while (doSinglePassOnSort(headRef)) {
		// Do nothing: work done in loop condition.
	}
}
static _Bool doSinglePassOnSort(Node **headRef)
{
	_Bool didSwap = false;
	while (*headRef != NULL) {
		Node *nodeA = *headRef;
		// If we don't have 2 remaining elements, nothing to swap.
		if (nodeA->next == NULL) {
			break;
		}
		Node *nodeB = nodeA->next;

		// Swap needed?
		if (nodeA->ptr > nodeB->ptr){
			swapElements(headRef, nodeA, nodeB);
			didSwap = true;
		}

		// Advance to next elements
		headRef = &((*headRef)->next);
	}
	return didSwap;
}
static void swapElements(Node **previous, Node *nodeA, Node *nodeB)
{
	*previous = nodeB;
	nodeA->next = nodeB->next;
	nodeB->next = nodeA;
}

void List_print(Node *head)
{
	Node* traverser = head;
	while(traverser != NULL)
	{
		printf("type: %c, size: %d, ptr: %p  ->  ", traverser->type, traverser->size, traverser->ptr);
		traverser = traverser->next;
	}
	printf("NULL\n");
}
