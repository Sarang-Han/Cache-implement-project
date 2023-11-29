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

/* hit ratio = (num_cache_hits / (num_cache_hits + num_cache_misses)) */
int num_cache_hits = 0;     // # of hits
int num_cache_misses = 0;   // # of misses

/* bandwidth = (num_bytes / num_acess_cycles) */
int num_bytes = 0;          // # of accessed bytes
int num_access_cycles = 0;  // # of clock cycles

int global_timestamp = 0;   // # of data access trials

int retrieve_data(void *addr, char data_type) {
    int value_returned = -1; /* accessed data */
    cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];

    int result = check_cache_data_hit(addr, data_type); // 캐시에 데이터 있는지 확인

    if (result == -1) {
        // 캐시 미스일 경우 메모리에서 데이터 가져오기
        value_returned = access_memory(addr, data_type);
    } else {
        // 캐시 히트일 경우 해당 데이터 반환
        switch (data_type) {
            case 'b':
                value_returned = cache_array[((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE][result].data[((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE)];
                break;
            case 'h':
                value_returned = ((cache_array[((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE][result].data[((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE) + 1] << 8) |
                                  cache_array[((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE][result].data[((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE)]);
                break;
            case 'w':
                value_returned = ((cache_array[((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE][result].data[((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE) + 3] << 24) |
                                  (cache_array[((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE][result].data[((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE) + 2] << 16) |
                                  (cache_array[((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE][result].data[((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE) + 1] << 8) |
                                  cache_array[((int)addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE) % CACHE_SET_SIZE][result].data[((int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE)]);
                break;
            default:
                break;
        }
    }

    num_bytes++; // 액세스한 바이트 수 증가
    return value_returned;
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    int total_accesses = num_cache_hits + num_cache_misses;
    float hit_ratio = (total_accesses > 0) ? ((float)num_cache_hits / total_accesses) : 0;
    float bandwidth = (float)num_bytes / num_access_cycles;

    init_memory_content();
    init_cache_content();
    
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
    
    /* read each line and get the data in given (address, type)
    by invoking retrieve_data() */

    /* print hit ratio and bandwidth for each cache mechanism
    as regards to cache association size */
    while (fscanf(ifp, "%lu %c", &access_addr, &access_type) != EOF) {
        int accessed_data = retrieve_data((void *)access_addr, access_type); // 데이터 검색

        fprintf(ofp, "%lu %c %#x\n", access_addr, access_type, accessed_data); // 결과 출력
    }

    fprintf(ofp, "----------------------------------------------\n");
    fprintf(ofp, "Hit ratio = %.2f (%d/%d)\n", hit_ratio, num_cache_hits, total_accesses);
    fprintf(ofp, "Bandwidth = %.2f (%d/%d)\n", bandwidth, num_bytes, num_access_cycles);


    fclose(ifp);
    fclose(ofp);
    
    /* print the final cache entries by invoking print_cache_entries() */
    print_cache_entries();
    return 0;
}
