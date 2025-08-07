#ifndef OFFSET_BITS
#include "addresses.h"
#endif
#ifndef INT32_MIN
#include <stdint.h>
#endif

#define DEFAULT_TLB_MAX_ENTRIES 32

/**
 * Translation Lookalike Buffer datatype, to cache recently accessed memory
 * addresses.
 */
typedef struct tlb tlb_t;

/**
 * Initialise a TLB to hold max_entries memory addresses. The TLB should later
 * be destroyed using destroy_tlb().
 */
void init_tlb(tlb_t **tlb, uint8_t max_entries);

/**
 * Free memory used by the TLB.
 */
void destroy_tlb(tlb_t **tlb);

/**
 * Check and log whether the TLB currently holds information about a logical
 * address a.
 */
int check_tlb(tlb_t *tlb, parsed_address_t a);

/**
 * Remove the page with page number page_num from the tlb.
 */
void flush_tlb(tlb_t *tlb, uint16_t page_num);

/**
 * Add the page referenced by pt_entry to the tlb.
 * If there is no space in the tlb, this function will make space.
 */
void add_page_to_tlb(tlb_t *tlb, pt_entry_t pt_entry);