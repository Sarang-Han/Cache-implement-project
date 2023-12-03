/*
 * main.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 15, 2023
 *
 */

#include <stdio.h>
#include "cache_impl.h"

// Variables to track cache and memory access metrics
int num_cache_hits = 0;     // Number of cache hits
int num_cache_misses = 0;   // Number of cache misses
int num_bytes = 0;          // Number of accessed bytes
int num_access_cycles = 0;  // Number of clock cycles
int global_timestamp = 0;   // Number of data access trials
cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC]; // Cache array

// Function to retrieve data from cache or memory based on the address and data type
int retrieve_data(void *addr, char data_type) {
    int value_returned = -1; // Accessed data

    int result = check_cache_data_hit(addr, data_type); // Check if data exists in cache

    if (result == -1) {
        // Cache miss: Retrieve data from memory
        value_returned = access_memory(addr, data_type);
    } else {
        // Cache hit: Return the data from the cache
        cache_entry_t *cache_entry = &cache_array[result / DEFAULT_CACHE_ASSOC][result % DEFAULT_CACHE_ASSOC];

        // Update based on the data type
        switch (data_type) {
            case 'b':
                value_returned = cache_entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE];
                break;
            case 'h':
                value_returned = (cache_entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 1] << 8) |
                                 cache_entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE];
                break;
            case 'w':
                value_returned = (cache_entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 3] << 24) |
                                 (cache_entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 2] << 16) |
                                 (cache_entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 1] << 8) |
                                  cache_entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE];
                break;
            default:
                break;
        }
    }

    // Increase the number of accessed bytes
    num_bytes += (data_type == 'b') ? 1 : (data_type == 'h') ? 2 : 4;
    return value_returned;
}

// Main function
int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; // Byte address from "access_input.txt"
    char access_type; // 'b'(byte), 'h'(halfword), or 'w'(word) from "access_input.txt"
    
    // Initialize memory and cache content
    init_memory_content();
    init_cache_content();
    
    // File handling
    ifp = fopen("access_input.txt", "r");
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("access_output.txt", "w");
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
    /* Read each line and retrieve data based on the (address, type) pair */
    fprintf(ofp, "[Access Data] \n");
    while (fscanf(ifp, "%lu %c", &access_addr, &access_type) != EOF) {
        int accessed_data = retrieve_data((void *)access_addr, access_type); // Search for data

        fprintf(ofp, "%lu %c %#x\n", access_addr, access_type, accessed_data); // Print the result
    }

    // Calculate hit ratio and bandwidth
    float hit_ratio = (float)num_cache_hits / (num_cache_hits + num_cache_misses);
    float bandwidth = (float)num_bytes / num_access_cycles;
    
    fprintf(ofp, "----------------------------------------------\n");
    // Print cache performance based on association size
    switch (DEFAULT_CACHE_ASSOC) {
        case 1:
            fprintf(ofp, "[Direct mapped cache performance]\n");
            break;
        case 2:
            fprintf(ofp, "[2-way set associative cache performance]\n");
            break;
        case 4:
            fprintf(ofp, "[Fully associative cache performance]\n");
            break;
        default:
            fprintf(ofp, "[Unknown cache performance]\n");
            break;
    }

    // Output hit ratio and bandwidth
    fprintf(ofp, "Hit ratio = %.2f (%d/%d)\n", hit_ratio, num_cache_hits, num_cache_hits + num_cache_misses);
    fprintf(ofp, "Bandwidth = %.2f (%d/%d)\n", bandwidth, num_bytes, num_access_cycles);

    fclose(ifp);
    fclose(ofp);

    // Print final cache entries
    print_cache_entries();
    return 0;
}
