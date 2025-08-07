#include "addresses.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Given a 22-bit logical address (as a 32-bit integer),
 * get its page number and offset.
 */
parsed_address_t parse_logical_address(uint32_t logical_address) {
    parsed_address_t a;
    a.address = logical_address;
    /* Offset is last 12 bits of l_addr */
    a.offset = logical_address & ((1 << OFFSET_BITS) - 1);
    /* Page num is the 10 bits before the 12-bit offset */
    a.page_num = (logical_address >> OFFSET_BITS) & ((1 << PAGE_BITS) - 1);
    printf("logical-address=%u,page-number=%d,offset=%d\n", logical_address,
           a.page_num, a.offset);
    return a;
}

/**
 * Given a page's frame number and offset, get its
 * address in physical memory.
 */
uint32_t get_physical_address(uint16_t frame_num, uint16_t offset) {
    return (frame_num << OFFSET_BITS) + offset;
}