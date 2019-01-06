//
// Created by wxy325 on 2018/3/6.
//

#ifndef DEADLOCK_PREVENTION_GRAPH_H
#define DEADLOCK_PREVENTION_GRAPH_H
#include "list_sol.h"
#include <pthread.h>
#include <stdbool.h>

enum GraphNodeType {ThreadNode, LockNode};
struct GraphNode {
    enum GraphNodeType type;
    union {
        int lockId;
        pthread_t threadId;
    } id;
    struct ListNode* pointTo;// ListNode<GraphNode*>
};

void add_arrow(struct GraphNode* from, struct GraphNode* to);
void remove_arrow(struct GraphNode* from, struct GraphNode* to);
_Bool has_circular(struct GraphNode* from);
_Bool is_arrow_exist(struct GraphNode* from, struct GraphNode* to);

#endif //DEADLOCK_PREVENTION_GRAPH_H
