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

    /* Check data by invoking check_cache_data_hit() */

   /* In case of the cache miss event, retrieve data from the main memory
      by invoking access_memory() */

   /* If there is no data neither in cache nor memory, return -1,
      else return data */
      
    return value_returned;    
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
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

    fclose(ifp);
    fclose(ofp);
    
    /* print the final cache entries by invoking print_cache_entries() */
    print_cache_entries();
    return 0;
}
