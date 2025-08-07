#include "addresses.h"
// #include "memory_loader.h"
#include "pt_queue.h"
#include "tlb.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_to_memory(pt_entry_t **page_table, parsed_address_t logical_address,
                    int *num_frames_used, pt_entry_queue_t **queue, tlb_t *tlb);

int main(int argc, char **argv) {
    /* Read in command line args. Format: translate -f <filename> -t <task> */
    char *usage_instructions = "Usage: ./translate -f <filename> -t <task>\n";
    if (argc != 5) {
        printf("%s", usage_instructions);
    }
    char *task = "";
    char *filename = "";
    for (int i = 1; i < argc; i += 2) {
        if (strcmp("-f", argv[i]) == 0) {
            filename = argv[i + 1];
        } else if (strcmp("-t", argv[i]) == 0) {
            task = argv[i + 1];
        } else {
            printf("%s", usage_instructions);
            exit(1);
        }
    }
    int task4 = strcmp(task, "task4") == 0;
    int task3 = (strcmp(task, "task3") == 0) || task4;
    int task2 = (strcmp(task, "task2") == 0) || task3;
    /* The page table tracks pages of memory and their physical frame */
    pt_entry_t **page_table = NULL;
    /* The queue tracks the order of addresses which are added */
    pt_entry_queue_t *queue = NULL;
    tlb_t *tlb = NULL;
    int num_frames_used = 0;
    if (task2) {
        init_queue(&queue);
        page_table = malloc(sizeof(pt_entry_t *) * NUM_LOGICAL_PAGES);
        // Initialise page table
        for (int i = 0; i < NUM_LOGICAL_PAGES; i++) {
            page_table[i] = malloc(sizeof(pt_entry_t));
            page_table[i]->present = 0;
            page_table[i]->frame_num = 0;
            page_table[i]->page_num = 0;
        }
    }
    if (task4) {
        init_tlb(&tlb, DEFAULT_TLB_MAX_ENTRIES);
    }
    uint32_t l_addr;
    FILE *file = fopen(filename, "r");
    while (fscanf(file, "%u", &l_addr) == 1) {
        parsed_address_t address = parse_logical_address(l_addr);
        int hit = 0;
        if (task4) {
            hit = check_tlb(tlb, address);
        }
        if (task2 && !hit) {
            load_to_memory(page_table, address, &num_frames_used, &queue, tlb);
        }
    }
    fclose(file);
    if (task2) {
        for (int i = 0; i < NUM_LOGICAL_PAGES; i++) {
            free(page_table[i]);
        }
        free(page_table);
        destroy_queue(&queue);
    }
    if (task4) {
        destroy_tlb(&tlb);
    }
}

/**
 * Given a list of logical addresses, add each address's page to the page table,
 * and convert the logical address to a physical address. Additionally, if a
 * valid TLB address is provided, update the TLB with the new memory address.
 */
void load_to_memory(pt_entry_t **page_table, parsed_address_t logical_address,
                    int *num_frames_used, pt_entry_queue_t **queue,
                    tlb_t *tlb) {

    int page_fault = 0;
    pt_entry_t *entry = page_table[logical_address.page_num];
    entry->page_num = logical_address.page_num;
    pt_entry_t *evictee = NULL;
    if (!(entry->present)) {
        push(*queue, entry);
        page_fault = 1;
        int new_frame = *num_frames_used;
        (*num_frames_used)++;
        // If we've used more than the max number of frames, we need to evict a
        // page from physical memory
        if (*num_frames_used > NUM_FRAMES) {
            (*num_frames_used)--;
            evictee = pop(*queue);
            evictee->present = 0;
            new_frame = evictee->frame_num;
            printf("evicted-page=%d,freed-frame=%d\n", evictee->page_num,
                   evictee->frame_num);
        }
        // Place the new entry in the page table
        entry->present = 1;
        entry->frame_num = new_frame;
        page_table[logical_address.page_num] = entry;
    }
    // Update TLB
    if (tlb != NULL) {
        if (evictee != NULL) {
            flush_tlb(tlb, evictee->page_num);
        }
        add_page_to_tlb(tlb, *entry);
    }
    uint32_t physical_address =
        get_physical_address(entry->frame_num, logical_address.offset);
    printf("page-number=%d,page-fault=%d,frame-number=%d,physical-address=%u\n",
           logical_address.page_num, page_fault, entry->frame_num,
           physical_address);
}
