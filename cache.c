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
#include "cache_impl.h"

extern int num_cache_hits;
extern int num_cache_misses;

extern int num_bytes;
extern int num_access_cycles;

extern int global_timestamp;

/* new variables used in this file */
cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
// Data in the cache with size (# of sets X # of associations),
// 4X1 for direct, 2X2 for 2-way, 1X4 for fully
int memory_array[DEFAULT_MEMORY_SIZE_WORD];
// Data in memory with a size of 128 words

// Function to calculate the cache index based on address
int calculate_cache_index(void *addr) {
    return ((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE;
}

// Function to calculate the cache tag
int calculate_cache_tag(void *addr) {
    return ((int)addr / (CACHE_SET_SIZE * DEFAULT_CACHE_BLOCK_SIZE_BYTE));
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
    // Sample data for memory initialization
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i = 0, j = 1, gap = 1;
    
    // Initializing memory content
    for (index = 0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
        if (++i >= 16) i = 0; // Cycle through sample data
        if (++j >= 16) j = 0; // Cycle through sample data
        
        if (i == 0 && j == i + gap)   // Difference of i and j equals gap
            j = i + (++gap);         // Increase gap by 1 and set new j for each cycle
        
        printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
    int i, j;
    
    // Initializing cache content
    for (i = 0; i < CACHE_SET_SIZE; i++) {
        for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0; // Entry is invalid
            pEntry->tag = -1; // No tag assigned
            pEntry->timestamp = 0; // No access trial
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
    int i, j, k;
    
    printf("ENTRY >>\n");
    // For each set
    for (i = 0; i < CACHE_SET_SIZE; i++) { 
        printf("[Set %d] ", i);
        // For each entry in a set
        for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("Valid: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            // For each block in an entry
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

    int cache_index = calculate_cache_index(addr);
    int tag = calculate_cache_tag(addr);

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

// Function to find the entry index in the set for data storage
int find_entry_index_in_set(int cache_index) {
    /* You need to find the index to store new data within the cache set. */
    int set_index = cache_index % CACHE_SET_SIZE;
    int empty_entry_index = -1;
    int lru_index = 0;
    int lru_timestamp = cache_array[set_index][0].timestamp;

    // For direct-mapped cache
    if (DEFAULT_CACHE_ASSOC == 1) {
        cache_entry_t *entry = &cache_array[set_index][0];
        if (!entry->valid) {
            return 0; // Return the first index if cache is empty
        }
        return 0; // Return the first index if cache is full
    }

    // For 2-way, 4-way cache
    for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
        cache_entry_t *entry = &cache_array[set_index][i];
        if (!entry->valid) {
            empty_entry_index = i; // Save the index if an entry is empty
            break;
        }
        // Find Least Recently Used (LRU)
        if (entry->timestamp < lru_timestamp) {
            lru_index = i;
            lru_timestamp = entry->timestamp;
        }
    }

    if (empty_entry_index != -1) {
        return empty_entry_index; // Return the index if an entry is empty
    } else {
        return lru_index; // Return the LRU index
    }
}

// Function to access memory and update cache
int access_memory(void *addr, char type) {
    num_access_cycles += MEMORY_ACCESS_CYCLE; // Adding memory access cycles

    int cache_index = calculate_cache_index(addr);
    int cache_entry_index = find_entry_index_in_set(cache_index);

    int tag = calculate_cache_tag(addr);

    cache_entry_t *entry = &cache_array[cache_index][cache_entry_index];
    entry->valid = 1;
    entry->tag = tag;
    entry->timestamp = global_timestamp++;

    // Reading data from memory to copy to cache
    int memory_index = ((int)addr / WORD_SIZE_BYTE);
    for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {
        entry->data[i] = (char)((memory_array[memory_index] >> (i * 8)) & 0xFF);
    }

    // Returning data from cache based on type
    switch (type) {
        case 'b': // Byte
            return entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE];
        case 'h': // Half-word
            return ((entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 1] << 8) |
                    entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE]);
        case 'w': // Word
            return ((entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 3] << 24) |
                    (entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 2] << 16) |
                    (entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE + 1] << 8) |
                    entry->data[((int)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE]);
        default:
            return -1; // Unknown type
    }
}

