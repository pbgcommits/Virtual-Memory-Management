#ifndef OFFSET_BITS
#include "addresses.h"
#endif

#define PT_QUEUE_H 1

typedef struct pt_entry_queue_t pt_entry_queue_t;

/**
 * Initialise the queue. The queue must be destroyed later using
 * destroy_queue().
 */
void init_queue(pt_entry_queue_t **queue);

/**
 * Add an entry to the queue.
 */
void push(pt_entry_queue_t *queue, pt_entry_t *entry);

/**
 * Remove an entry from the queue.
 */
pt_entry_t *pop(pt_entry_queue_t *queue);

/**
 * Free all memory currently used by the queue.
 */
void destroy_queue(pt_entry_queue_t **queue);