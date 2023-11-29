/*
 * cache_impl.h
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 15, 2023
 *
 */

/* DO NOT CHANGE THE FOLLOWING DEFINITIONS EXCEPT 'DEFAULT_CACHE_ASSOC */

#ifndef _CACHE_IMPL_H_
#define _CACHE_IMPL_H_

#define WORD_SIZE_BYTE                   4
#define DEFAULT_CACHE_SIZE_BYTE          32
#define DEFAULT_CACHE_BLOCK_SIZE_BYTE    8
#define DEFAULT_CACHE_ASSOC              2   /* This can be changed to 1(for direct mapped cache) or 4(for fully assoc cache) */
#define DEFAULT_MEMORY_SIZE_WORD         128    
#define CACHE_ACCESS_CYCLE               1
#define MEMORY_ACCESS_CYCLE              100
#define CACHE_SET_SIZE                   ((DEFAULT_CACHE_SIZE_BYTE)/(DEFAULT_CACHE_BLOCK_SIZE_BYTE*DEFAULT_CACHE_ASSOC)) // set size=[direct]32/(8*1)=4, [2-way]32/(8*2)=2, [full] 32/(8*4)=1

/* Function Prototypes */
void init_memory_content();
void init_cache_content();
void print_cache_entries();
int check_cache_data_hit(void* addr, char type);
int access_memory(void *addr, char type);


/* Cache Entry Structure */
typedef struct cache_entry {
    int valid;  // present = 1, not present = 0
    int tag;    // tag of the stored data
    int timestamp; // most recent access time
    char data[DEFAULT_CACHE_BLOCK_SIZE_BYTE]; // data from memory[address]
} cache_entry_t; // define type as cache_entry_t


#endif
