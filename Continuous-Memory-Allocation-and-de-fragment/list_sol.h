// Linked list module.

#ifndef LIST_H_
#define LIST_H_

typedef struct Node {
	char type;
    int size;
    void* ptr;
    struct Node* next;
} Node;

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
Node* List_createNode(int size, char type, void* pointer);

/*
 * Insert node at the head of the list.
 */
void List_insertHead (Node **headRef, Node *node);

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (Node **headRef, Node *node);

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (Node *head);

/*
 * Return the first node holding the value item, return NULL if none found
 */
Node* List_findNode(Node *head, void* mem_ptr);

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set (by for example
 * calling List_findNode()) to a valid node in the list. If the list contains
 * only node, the head of the list should be set to NULL.
 */
void List_deleteNode (Node **headRef, Node *node);

/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort (Node **headRef);

void List_print(Node *head);

#endif
