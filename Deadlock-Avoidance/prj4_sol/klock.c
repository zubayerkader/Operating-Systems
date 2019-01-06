#include "klock.h"
#include <stdio.h>
#include "list_sol.h"
#include "graph.h"
#include <stdlib.h>

static unsigned s_current_lock_id = 1;


static struct ListNode* all_lock_nodes = NULL; // ListNode<GraphNode>
static struct ListNode* all_thread_nodes = NULL;  // ListNode<GraphNode>

pthread_mutex_t graph_mutex = PTHREAD_MUTEX_INITIALIZER; // This mutex is used to avoid race condition when manipulating the resource allocation graph

// Lock Nodes
struct ListNode* insert_lock_list_node(int lock_id) {
    struct GraphNode* n = (struct GraphNode*)malloc(sizeof(struct GraphNode));
    n->id.lockId = lock_id;
    n->type = LockNode;
    n->pointTo = NULL;
    struct ListNode* listNode = List_createNode(n);
    List_insertTail(&all_lock_nodes, listNode);
    return listNode;
};

struct ListNode* find_lock_list_node(int lock_id) {
    struct ListNode* currentNode = all_lock_nodes;
    while (currentNode) {
        struct GraphNode* n = (struct GraphNode*)currentNode->item;
        if (n->id.lockId == lock_id) {
            break;
        }
        currentNode = currentNode->next;
    }
    return currentNode;
}

struct ListNode* find_or_insert_lock_list_node(int lockId) {
    struct ListNode* n = find_lock_list_node(lockId);
    if (!n) {
        n = insert_lock_list_node(lockId);
    }
    return n;
}

// Thread Nodes
struct ListNode* insert_thread_list_node(pthread_t threadId) {
    struct GraphNode* n = (struct GraphNode*)malloc(sizeof(struct GraphNode));
    n->id.threadId = threadId;
    n->type = ThreadNode;
    n->pointTo = NULL;
    struct ListNode* listNode = List_createNode(n);
    List_insertTail(&all_thread_nodes, listNode);
    return listNode;
}

struct ListNode* find_thread_list_node(pthread_t threadId) {
    struct ListNode* currentNode = all_thread_nodes;
    while (currentNode) {
        struct GraphNode* n = (struct GraphNode*)currentNode->item;
        if (n->id.threadId == threadId) {
            break;
        }
        currentNode = currentNode->next;
    }
    return currentNode;
}
struct ListNode* find_or_insert_thread_list_node(pthread_t threadId) {
    struct ListNode* n = find_thread_list_node(threadId);
    if (!n) {
        n = insert_thread_list_node(threadId);
    }
    return n;
}


void init_lock(SmartLock* lock) {
    pthread_mutex_init(&(lock->mutex), NULL);
    lock->lock_id = s_current_lock_id++;
//    insert_lock_list_node(lock->lock_id);
}



int lock(SmartLock* lock) {
    pthread_mutex_lock(&graph_mutex);

    struct ListNode* lockListNode = find_or_insert_lock_list_node(lock->lock_id);
    struct GraphNode* lockGraphNode = (struct GraphNode*)lockListNode->item;
    pthread_t threadId = pthread_self();
    struct ListNode* threadListNode = find_or_insert_thread_list_node(threadId);
    struct GraphNode* threadGraphNode = (struct GraphNode*)threadListNode->item;

    if (!is_arrow_exist(threadGraphNode, lockGraphNode)) {
        add_arrow(threadGraphNode, lockGraphNode);
//        printf("Add arrow: thread:%lu -> lock:%d\n", threadGraphNode->id.threadId, lockGraphNode->id.lockId);
    }
    _Bool hasCircular = has_circular(threadGraphNode);
    if (hasCircular) {
        remove_arrow(threadGraphNode, lockGraphNode);
//        printf("Remove arrow: thread:%lu -> lock:%d\n", threadGraphNode->id.threadId, lockGraphNode->id.lockId);
        pthread_mutex_unlock(&graph_mutex);
        return 0;
    } else {
//        printf("%lu locking\n", threadId);
        pthread_mutex_unlock(&graph_mutex);

        pthread_mutex_lock(&(lock->mutex));

        pthread_mutex_lock(&graph_mutex);
        remove_arrow(threadGraphNode, lockGraphNode);
//        printf("Remove arrow: thread:%lu -> lock:%d\n", threadGraphNode->id.threadId, lockGraphNode->id.lockId);
        add_arrow(lockGraphNode, threadGraphNode);
//        printf("Add arrow: lock:%d -> thread:%lu\n", lockGraphNode->id.lockId, threadGraphNode->id.threadId);

        hasCircular = has_circular(threadGraphNode);
        if (hasCircular) {
            remove_arrow(lockGraphNode, threadGraphNode);
            pthread_mutex_unlock(&(lock->mutex));
            pthread_mutex_unlock(&graph_mutex);
            return 0;
        }

        pthread_mutex_unlock(&graph_mutex);
        return 1;
    }
}

void unlock(SmartLock* lock) {
    pthread_mutex_lock(&graph_mutex);
    struct ListNode* lockListNode = find_or_insert_lock_list_node(lock->lock_id);
    struct GraphNode* lockGraphNode = (struct GraphNode*)lockListNode->item;
    pthread_t threadId = pthread_self();
    struct ListNode* threadListNode = find_or_insert_thread_list_node(threadId);
    struct GraphNode* threadGraphNode = (struct GraphNode*)threadListNode->item;
    remove_arrow(lockGraphNode, threadGraphNode);
//    printf("Add arrow: lock:%d -> thread:%lu\n", lockGraphNode->id.lockId, threadGraphNode->id.threadId);

    pthread_mutex_unlock(&graph_mutex);
    pthread_mutex_unlock(&(lock->mutex));
}

void cleanup() {
    while (all_thread_nodes) {
        free(all_thread_nodes->item);
        List_deleteNode(&all_thread_nodes, all_thread_nodes);
    }

    while (all_lock_nodes) {
        free(all_lock_nodes->item);
        List_deleteNode(&all_lock_nodes, all_lock_nodes);
    }
}