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
// data in cache with size (# of sets X # of associations),
// 4X1 for direct, 2X2 for 2-way, 1X4 for fully
int memory_array[DEFAULT_MEMORY_SIZE_WORD];
// data in memory with size of 128 words

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i=0, j=1, gap = 1;
    
    for (index=0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
        if (++i >= 16) i = 0; // cycle
        if (++j >= 16)j = 0; // cycle
        
        if (i == 0 && j == i+gap)   //difference of i and j==gap
            j = i + (++gap);        // increases 1 gap and new j for each cycle
            
        printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
    int i, j;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        for (j=0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0; // invalid
            pEntry->tag = -1; // no tag
            pEntry->timestamp = 0; // no access trial
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
    int i, j, k;
    
    printf("ENTRY >>\n");
    // for each set
    for (i=0; i<CACHE_SET_SIZE; i++) { 
        printf("[Set %d] ", i);
        // for each entry in a set
        for (j=0; j <DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("Valid: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            // for each block in a entry
            for (k=0; k<DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
                printf("(%d)%#x ", k, pEntry->data[k]);
            }
            printf("\t");
        }
        printf("\n");
    }
}

int check_cache_data_hit(void *addr, char type) {
    /* 주어진 주소와 타입을 기반으로 캐시에 데이터가 있는지 확인하고, 없으면 메모리에서 가져와야 해요. */
    num_access_cycles += CACHE_ACCESS_CYCLE; // 캐시 액세스 사이클 추가

    int set_index = ((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE;
    int tag = ((int)addr / DEFAULT_CACHE_SIZE_BYTE);

    for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
        cache_entry_t *entry = &cache_array[set_index][i];
        if (entry->valid && entry->tag == tag) {
            num_cache_hits++; // 캐시 히트
            entry->timestamp = global_timestamp++; // 타임스탬프 업데이트
            return 1; // 데이터를 찾았으므로 리턴
        }
    }
    // 데이터가 캐시에 없음 (캐시 미스)
    num_cache_misses++;
    return -1;
}

int find_entry_index_in_set(int cache_index) {
    /* 캐시 세트 내에서 새 데이터를 저장할 인덱스를 찾아야 해요. */
    int set_index = cache_index % CACHE_SET_SIZE;
    int empty_entry_index = -1;
    int lru_index = 0;
    int lru_timestamp = cache_array[set_index][0].timestamp;

    for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
        cache_entry_t *entry = &cache_array[set_index][i];
        if (!entry->valid) {
            empty_entry_index = i; // 비어있는 엔트리가 있으면 해당 인덱스 저장
            break;
        }
        // LRU(Least Recently Used) 찾기
        if (entry->timestamp < lru_timestamp) {
            lru_index = i;
            lru_timestamp = entry->timestamp;
        }
    }

    if (empty_entry_index != -1) {
        return empty_entry_index; // 비어있는 엔트리가 있으면 반환
    } else {
        return lru_index; // 아니면 LRU 인덱스 반환
    }
}

int access_memory(void *addr, char type) {
    /* 메모리에서 데이터를 읽어와서 캐시에 저장해야 해요. */
    int set_index = ((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE;
    int entry_index = find_entry_index_in_set(set_index);

    num_access_cycles += MEMORY_ACCESS_CYCLE; // 메모리 액세스 사이클 추가

    cache_entry_t *entry = &cache_array[set_index][entry_index];
    int tag = ((int)addr / DEFAULT_CACHE_SIZE_BYTE);
    entry->valid = 1;
    entry->tag = tag;
    entry->timestamp = global_timestamp++;
    
    int memory_index = ((int)addr / WORD_SIZE_BYTE);
    int offset = ((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE);
    
    // 메모리에서 데이터를 읽어와 캐시에 복사
    for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {
        entry->data[i] = (char)((memory_array[memory_index] >> (i * 8)) & 0xFF);
    }
    
    switch (type) {
        case 'b': // 바이트
            return entry->data[offset];
        case 'h': // 하프워드
            return ((entry->data[offset + 1] << 8) | entry->data[offset]);
        case 'w': // 워드
            return ((entry->data[offset + 3] << 24) | (entry->data[offset + 2] << 16) |
                    (entry->data[offset + 1] << 8) | entry->data[offset]);
        default:
            return -1; // 알 수 없는 타입
    }
}
