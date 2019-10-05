//
//  main.c
//  ScanAndReport
//
//  Created by Roman Weisman on 10/3/19.
//  Copyright © 2019 Roman Weisman. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "input_output.h"

int main(int argc, const char * argv[]) {
    
    init(); // inits required and creates a folder
    
    // 3 sets of threads for creating, writing and reading the files
    pthread_t create_threads[NUMBER_OF_THREADS];
    pthread_t write_threads[NUMBER_OF_THREADS];
    pthread_t read_threads[NUMBER_OF_THREADS];
    
    void *result;
    
    // creates a linked list of files under the created folder via threads
    // separating create threads with write threads guanratess inconsistency in the order of created file
    // in other words: If number of threads is 5, it's possible that created list will be: 3->2->1->5->4
    // and that's the goal - same as shuffling cards
    for(long t = 0 ; t < NUMBER_OF_THREADS ; t++) {
        if(pthread_create(&create_threads[t], NULL, createFile, (void*)t) == -1) {
            printf("Cant create create thread %ld", t);
            return 1;
        }
    }
    
    // wait for create threads to complete before proceeding to the writing part
    for(int t = 0 ; t < NUMBER_OF_THREADS ; t++) {
        pthread_join(create_threads[t], &result);
    }
    
    printGeneratedLinkedList();
    
    for(long t = 0 ; t < NUMBER_OF_THREADS ; t++) {
        if(pthread_create(&write_threads[t], NULL, writeToFile, (void*)t)== -1) {
            printf("Cant create write thread %ld", t);
            return 1;
        }
    }
    
    // wait for write threads to complete
    for(int t = 0 ; t < NUMBER_OF_THREADS ; t++) {
        pthread_join(write_threads[t], &result);
    }
    
    closeFiles(); // close open files for write, before opening them for read
    
    for(long t = 0 ; t < NUMBER_OF_THREADS ; t++) {
        if(pthread_create(&read_threads[t], NULL, readFromFile, (void*)t)){
            printf("Cant create read %ld", t);
            return 1;
        }
    }

    // wait for read threads to complete
    for(int t = 0 ; t < NUMBER_OF_THREADS ; t++) {
        pthread_join(read_threads[t], &result);
    }
    
    closeFiles();
    release();   // closes the files
    return 0;
}
