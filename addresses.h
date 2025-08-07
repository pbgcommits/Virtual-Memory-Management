#define NUM_LOGICAL_PAGES 1024
#define NUM_FRAMES 256
#define OFFSET_BITS 12
#define PAGE_BITS 10

#ifndef INT32_MIN
#include <stdint.h>
#endif

/**
 * Stores the page number and offset of a logical address.
 */
typedef struct logical_address {
    uint32_t address;
    uint16_t page_num;
    uint16_t offset;
} parsed_address_t;

/**
 * Stores whether a page is present in memory, and its frame number if so.
 */
typedef struct pt_entry {
    int8_t present;
    uint16_t page_num;
    uint16_t frame_num;
} pt_entry_t;

/**
 * Given a 22-bit logical address (as a 32-bit integer),
 * get its page number and offset.
 */
parsed_address_t parse_logical_address(uint32_t logical_address);

/**
 * Given a page's frame number and offset, get its
 * address in physical memory.
 */
uint32_t get_physical_address(uint16_t frame_num, uint16_t offset);