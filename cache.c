/*
 * cache.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 15, 2023
 *
 */


#include <stdio.h>
#include <string.h>
#include "cache_impl.h" // Including header file for cache implementation

// External declarations for variables used in other files
extern int num_cache_hits;
extern int num_cache_misses;
extern int num_bytes;
extern int num_access_cycles;
extern int global_timestamp;

// Arrays to store cache and memory data
cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];

/* Initialize memory content with predefined sample data */
void init_memory_content() {
    // Predefined sample data for memory initialization
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i = 0, j = 1, gap = 1;

    // Initializing memory_array using predefined sample data
    for (index = 0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);

        if (++i >= 16) i = 0; // Cycle for sample_upward array
        if (++j >= 16) j = 0; // Cycle for sample_downward array

        // Printing memory content for debugging
        printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}

/* Initialize cache content by setting default values */
void init_cache_content() {
    int i, j;

    // Loop to initialize cache_array with default values
    for (i = 0; i < CACHE_SET_SIZE; i++) {
        for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0;       // Marking entry as invalid
            pEntry->tag = -1;        // No tag assigned initially
            pEntry->timestamp = 0;   // No access trial initially
        }
    }
}

/* Utility function to print all cache entries (for debugging) */
void print_cache_entries() {
    int i, j, k;

    printf("ENTRY >>\n");
    // Loop through each set in cache_array
    for (i = 0; i < CACHE_SET_SIZE; i++) {
        printf("[Set %d] ", i);
        // Loop through each entry in a set
        for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("Valid: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            // Loop through each block in an entry
            for (k = 0; k < DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
                printf("(%d)%#x ", k, pEntry->data[k]);
            }
            printf("\t");
        }
        printf("\n");
    }
}

// Function to check if data exists in cache
int check_cache_data_hit(void *addr, char type) {
    num_access_cycles += CACHE_ACCESS_CYCLE; // Adding cache access cycles

    int block_addr = ((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE); // Calculating block address
    int cache_index = block_addr % CACHE_SET_SIZE;
    int tag = block_addr / CACHE_SET_SIZE;
    int byte_offset = ((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE; // Calculating byte offset

    printf("Cache>> block_addr = %d, byte_offset = %d, cache_index = %d, tag = %d\n", block_addr, byte_offset, cache_index, tag);

    for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
        cache_entry_t *entry = &cache_array[cache_index][i];
        if (entry->valid && entry->tag == tag) {
            num_cache_hits++; // Cache hit
            entry->timestamp = global_timestamp++; // Update timestamp

            printf("cache hit!\n");
            return cache_index * DEFAULT_CACHE_ASSOC + i; // Return index of data
        }
    }
    // Data not found in cache (cache miss)
    num_cache_misses++;
    printf("cache miss!\n");
    return -1;
}

// Function to find an entry index within a cache set
int find_entry_index_in_set(int cache_index) {
    int set_index = cache_index % CACHE_SET_SIZE;
    int empty_entry_index = -1;
    int lru_index = 0;
    int lru_timestamp = cache_array[set_index][0].timestamp;

    // For Direct-mapped cache
    if (DEFAULT_CACHE_ASSOC == 1) {
        cache_entry_t *entry = &cache_array[set_index][0];
        if (!entry->valid) {
            return 0; // Return first index if cache is empty
        }
        return 0; // Return first index if cache is full
    }

    if (empty_entry_index != -1) {
        return empty_entry_index; // Return index of empty entry if found
    } else {
        return lru_index; // Return Least Recently Used (LRU) index
    }
}

// Function to access memory and update cache
int access_memory(void *addr, char type) {
    num_access_cycles += MEMORY_ACCESS_CYCLE; // Adding memory access cycles

    int memory_block = ((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE); // Calculating memory block
    int word_index = ((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE; // Calculating word index

    int cache_set_index = memory_block % CACHE_SET_SIZE; // Calculating cache set index
    int cache_entry_index = find_entry_index_in_set(cache_set_index); // Finding index to store in cache

    int tag = memory_block / CACHE_SET_SIZE; // Calculating tag

    cache_entry_t *entry = &cache_array[cache_set_index][cache_entry_index];
    entry->valid = 1;
    entry->tag = tag;
    entry->timestamp = global_timestamp++;

    // Reading data from memory to copy to cache
    int memory_index = memory_block * (DEFAULT_CACHE_BLOCK_SIZE_BYTE / WORD_SIZE_BYTE);
    for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {
        entry->data[i] = (char)((memory_array[memory_index + i / WORD_SIZE_BYTE] >> ((i % WORD_SIZE_BYTE) * 8)) & 0xFF);
    }

    printf("access_memory: data in cache after copy:\n"); // Debugging: printing copied data in cache
    for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {
        printf("(%d)%#x ", i, entry->data[i]);
    }
    printf("\n");

    // Finding and returning data from cache based on type
    switch (type) {
        case 'b': // Byte
            return entry->data[word_index];
        case 'h': // Half-word
            return ((entry->data[word_index + 1] << 8) | (entry->data[word_index] & 0xFF));
        case 'w': // Word
            return ((entry->data[word_index + 3] << 24) | (entry->data[word_index + 2] << 16) |
                    (entry->data[word_index + 1] << 8) | (entry->data[word_index] & 0xFF));
        default:
            return -1; // Unknown type
    }
}

