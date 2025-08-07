#ifndef OFFSET_BITS
#include "addresses.h"
#endif
#include <stdlib.h>

typedef struct ptq_node_t {
    pt_entry_t *entry;
    struct ptq_node_t *prev;
    struct ptq_node_t *next;
} ptq_node_t;

typedef struct pt_entry_queue_t {
    int num_entries;
    ptq_node_t *head;
    ptq_node_t *tail;
} pt_entry_queue_t;

/**
 * Initialise the queue. The queue must be destroyed later using
 * destroy_queue().
 */
void init_queue(pt_entry_queue_t **queue) {
    *queue = malloc(sizeof(pt_entry_queue_t));
    (*queue)->num_entries = 0;
    (*queue)->head = NULL;
    (*queue)->tail = NULL;
}

/**
 * Add an entry to the queue.
 */
void push(pt_entry_queue_t *queue, pt_entry_t *entry) {
    queue->num_entries++;
    ptq_node_t *new_entry = malloc(sizeof(ptq_node_t));
    new_entry->entry = entry;
    new_entry->next = NULL;
    if (queue->head == NULL) {
        queue->head = new_entry;
        new_entry->prev = NULL;
        new_entry->next = NULL;
    } else {
        new_entry->prev = queue->tail;
        queue->tail->next = new_entry;
    }
    queue->tail = new_entry;
}

/**
 * Remove an entry from the queue.
 */
pt_entry_t *pop(pt_entry_queue_t *queue) {
    queue->num_entries--;
    ptq_node_t *node = queue->head;
    if (node == NULL)
        return NULL;
    queue->head = queue->head->next;
    if (queue->head != NULL) {
        queue->head->prev = NULL;
    }
    pt_entry_t *entry = node->entry;
    free(node);
    return entry;
}

/**
 * Free all memory currently used by the queue.
 */
void destroy_queue(pt_entry_queue_t **queue) {
    while (pop(*queue) != NULL) {
    }
    free(*queue);
}