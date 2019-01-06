#include "list_sol.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

static _Bool doSinglePassOnSort(struct ListNode **headRef, ListItemComparison comparison);

static void swapElements(struct ListNode **previous, struct ListNode *nodeA, struct ListNode *b);


/*
 * Allocate memory for a node of type struct ListNode and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct ListNode *List_createNode(void *item) {
    struct ListNode *pNode = malloc(sizeof(struct ListNode));
    if (pNode != NULL) {
        pNode->item = item;
    }
    return pNode;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead(struct ListNode **headRef, struct ListNode *node) {
    node->next = *headRef;
    *headRef = node;
}

/*
 * Insert node after the tail of the list.
 */
void List_insertTail(struct ListNode **headRef, struct ListNode *node) {
    node->next = NULL;

    // Handle empty list
    if (*headRef == NULL) {
        *headRef = node;
    } else {
        // Find the tail and insert node
        struct ListNode *current = *headRef;
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
int List_countNodes(struct ListNode *head) {
    int count = 0;
    struct ListNode *current = head;
    while (current != NULL) {
        current = current->next;
        count++;
    }
    return count;
}


int void_pointer_comparison(void *l, void *r) {
    return l == r ? 0 : 1;
}

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct ListNode *List_findNode(struct ListNode *head, void *item) {
    return List_findNode_with_comparison(head, item, void_pointer_comparison);
}

struct ListNode *List_findNode_with_comparison(struct ListNode *head, void *item, ListItemComparison comparison) {
    struct ListNode *current = head;
    while (current != NULL) {
        if (comparison(current->item, item) == 0) {
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
void List_deleteNode(struct ListNode **headRef, struct ListNode *node) {
    assert(headRef != NULL);
    assert(*headRef != NULL);

    // Is it the first element?
    if (*headRef == node) {
        *headRef = node->next;
    } else {
        // Find the previous node:
        struct ListNode *previous = *headRef;
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

void List_sort(struct ListNode **headRef, ListItemComparison comparison) {
    while (doSinglePassOnSort(headRef, comparison)) {
        // Do nothing: work done in loop condition.
    }
}

static _Bool doSinglePassOnSort(struct ListNode **headRef, ListItemComparison comparison) {
    _Bool didSwap = false;
    while (*headRef != NULL) {
        struct ListNode *nodeA = *headRef;
        // If we don't have 2 remaining elements, nothing to swap.
        if (nodeA->next == NULL) {
            break;
        }
        struct ListNode *nodeB = nodeA->next;

        // Swap needed?
        if (comparison(nodeA->item, nodeB->item) > 0) {
            swapElements(headRef, nodeA, nodeB);
            didSwap = true;
        }

        // Advance to next elements
        headRef = &((*headRef)->next);
    }
    return didSwap;
}

static void swapElements(struct ListNode **previous,
                         struct ListNode *nodeA,
                         struct ListNode *nodeB) {
    *previous = nodeB;
    nodeA->next = nodeB->next;
    nodeB->next = nodeA;
}

//
//void dumpList(struct ListNode *head) {
//    struct ListNode *current = head;
//    while (current != NULL) {
//        printf("%p (%d), ", current->item.memory, current->item.size);
//        current = current->next;
//    }
//    printf("\n");
//}
