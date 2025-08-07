#include "tlb.h"
#ifndef OFFSET_BITS
#include "addresses.h"
#endif
#include <stdio.h>
#include <stdlib.h>

void add_page_at_index(tlb_t *tlb, pt_entry_t pt_entry, uint8_t i);
void remove_oldest_entry(tlb_t *tlb, uint8_t *removed_index,
                         uint16_t *removed_page_num);

/**
 * Stores relevant information about entries in a TLB.
 */
typedef struct tlb_entry {
    int8_t valid;
    uint16_t page_num;
    uint16_t frame_num;
    long long time_since_access;
} tlb_entry_t;

/**
 * Translation Lookalike Buffer datatype, to cache recently accessed memory
 * addresses.
 */
typedef struct tlb {
    uint8_t max_entries;
    uint8_t num_entries;
    tlb_entry_t **entries;
} tlb_t;

/**
 * Initialise a TLB to hold max_entries memory addresses. The TLB should later
 * be destroyed using destroy_tlb().
 */
void init_tlb(tlb_t **tlb, uint8_t max_entries) {
    *tlb = malloc(sizeof(tlb_t));
    (*tlb)->num_entries = 0;
    (*tlb)->max_entries = max_entries;
    (*tlb)->entries = malloc(sizeof(tlb_entry_t *) * max_entries);
    for (int i = 0; i < max_entries; i++) {
        tlb_entry_t *entry = malloc(sizeof(tlb_entry_t));
        entry->valid = 0;
        entry->page_num = 0;
        entry->frame_num = 0;
        entry->time_since_access = __LONG_LONG_MAX__;
        (*tlb)->entries[i] = entry;
    }
}

/**
 * Free memory used by the TLB.
 */
void destroy_tlb(tlb_t **tlb) {
    for (int i = 0; i < (*tlb)->max_entries; i++) {
        free((*tlb)->entries[i]);
    }
    free((*tlb)->entries);
    free(*tlb);
}

/**
 * Check and log whether the TLB currently holds information about a logical
 * address a. Return 1 if the logical address is in the TLB, or 0 if not.
 */
int check_tlb(tlb_t *tlb, parsed_address_t a) {
    int hit = 0;
    int tlb_index = -1;
    // Check TLB
    for (int i = 0; i < tlb->max_entries; i++) {
        if (tlb->entries[i]->valid && tlb->entries[i]->page_num == a.page_num) {
            hit = 1;
            tlb_index = i;
            tlb->entries[i]->time_since_access = 0;
        } else if (tlb->entries[i]->valid) {
            tlb->entries[i]->time_since_access += 1;
        }
    }
    printf("tlb-hit=%d,page-number=%d,", hit, a.page_num);
    if (!hit) {
        printf("frame=none,physical-address=none\n");
    } else {
        int16_t frame = tlb->entries[tlb_index]->frame_num;
        printf("frame=%d,physical-address=%u\n", frame,
               get_physical_address(frame, a.offset));
    }
    return hit;
}

/**
 * Remove the page with page number page_num from the tlb.
 */
void flush_tlb(tlb_t *tlb, uint16_t page_num) {
    uint8_t initial_tlb_entries = tlb->num_entries;
    for (int i = 0; i < tlb->max_entries; i++) {
        if (tlb->entries[i]->page_num == page_num) {
            tlb->entries[i]->valid = 0;
            tlb->num_entries--;
        }
    }
    if (tlb->num_entries < initial_tlb_entries) {
        printf("tlb-flush=%d,tlb-size=%d\n", page_num, tlb->num_entries);
    }
}

/**
 * Add the page referenced by pt_entry to the tlb.
 * If there is no space in the tlb, this function will make space.
 */
void add_page_to_tlb(tlb_t *tlb, pt_entry_t pt_entry) {
    int removed = 1;
    for (int i = 0; i < tlb->max_entries; i++) {
        if (tlb->entries[i]->valid == 0) {
            removed = 0;
            add_page_at_index(tlb, pt_entry, i);
            break;
        }
    }
    if (!removed) {
        printf("tlb-remove=none,tlb-add=%d\n", pt_entry.page_num);
    } else {
        uint8_t removed_page_index;
        uint16_t removed_page_num;
        remove_oldest_entry(tlb, &removed_page_index, &removed_page_num);
        add_page_at_index(tlb, pt_entry, removed_page_index);
        printf("tlb-remove=%d,tlb-add=%d\n", removed_page_num,
               pt_entry.page_num);
    }
}

void add_page_at_index(tlb_t *tlb, pt_entry_t pt_entry, uint8_t i) {
    tlb->entries[i]->valid = 1;
    tlb->entries[i]->page_num = pt_entry.page_num;
    tlb->entries[i]->frame_num = pt_entry.frame_num;
    tlb->entries[i]->time_since_access = 0;
    tlb->num_entries++;
}

void remove_oldest_entry(tlb_t *tlb, uint8_t *removed_index,
                         uint16_t *removed_page_num) {
    int oldest_index = -1;
    int oldest_age = -1;
    for (int i = 0; i < tlb->max_entries; i++) {
        if (tlb->entries[i]->valid &&
            tlb->entries[i]->time_since_access > oldest_age) {
            oldest_age = tlb->entries[i]->time_since_access;
            oldest_index = i;
        }
    }
    *removed_index = oldest_index;
    *removed_page_num = tlb->entries[oldest_index]->page_num;
    tlb->entries[oldest_index]->valid = 0;
    tlb->num_entries--;
}
