//
// Created by wxy325 on 2018/3/6.
//

#include "graph.h"

void add_arrow(struct GraphNode* from, struct GraphNode* to) {
    struct ListNode* n = List_createNode(to);
    List_insertTail(&from->pointTo, n);
}

void remove_arrow(struct GraphNode* from, struct GraphNode* to) {
    struct ListNode* targetNode = List_findNode(from->pointTo, to);
    List_deleteNode(&from->pointTo, targetNode);
}

_Bool has_circular_helper(struct GraphNode* sourceNode, struct GraphNode* currentNode) {
    if (!currentNode) {
        return false;
    }

    struct ListNode* cur = currentNode->pointTo;
    while (cur) {
        struct GraphNode* nextNode = (struct GraphNode*)cur->item;

        if (sourceNode == nextNode) {
            return true;
        }

        if (has_circular_helper(sourceNode, nextNode)) {
            return true;
        }

        cur = cur->next;
    }
    return false;
}

_Bool has_circular(struct GraphNode* from) {
    return has_circular_helper(from, from);
}

_Bool is_arrow_exist(struct GraphNode* from, struct GraphNode* to) {
    struct ListNode* cur = from->pointTo;
    while (cur) {
        if (to == cur->item) {
            return true;
        }
        cur = cur->next;
    }
    return false;
}