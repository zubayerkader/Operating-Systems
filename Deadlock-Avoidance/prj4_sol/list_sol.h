// Linked list module.

#ifndef LIST_H_
#define LIST_H_

struct ListNode {
    void* item;
    struct ListNode *next;
};

typedef int ListItemComparison(void* left, void* right);

/*
 * Allocate memory for a node of type struct ListNode and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct ListNode* List_createNode(void* item_ptr);

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct ListNode **headRef, struct ListNode *node);

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct ListNode **headRef, struct ListNode *node);

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct ListNode *head);

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct ListNode* List_findNode(struct ListNode *head, void* item);

struct ListNode* List_findNode_with_comparison(struct ListNode *head, void* item, ListItemComparison comparison);

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set (by for example
 * calling List_findNode()) to a valid node in the list. If the list contains
 * only node, the head of the list should be set to NULL.
 */
void List_deleteNode (struct ListNode **headRef, struct ListNode *node);

/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort (struct ListNode **headRef, ListItemComparison comparison);

//void dumpList(struct ListNode *head);

#endif
