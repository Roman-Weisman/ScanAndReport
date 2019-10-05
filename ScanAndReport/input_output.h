//
//  input_output.h
//  ScanAndReport
//
//  Created by Roman Weisman on 10/3/19.
//  Copyright © 2019 Roman Weisman. All rights reserved.
//

#ifndef input_output_h
#define input_output_h

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "utils.h"
#include "file.h"

const int NUMBER_OF_THREADS = 5;
const char *HOME = "/Users";
const char *SCAN_AND_REPORT = "/ScanAndReport";
FILE *files[NUMBER_OF_THREADS];
const char FILE_NAME[13] = "file_number_";
char *fullPath;
pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;

file *start = NULL;
file *currentFile = NULL;
file *nextFileToWriteTo = NULL;
file *nextFileToReadFrom = NULL;
char *secret;
int secretFile;
int secretLine;
int secretStartingChar;
int linkedListCounter = 0;
bool thisIsTheSecretFile = false;
bool secretFound = false;
const int NUMBER_OF_LINES = 1000;
const int NUMBER_OF_CHARS_IN_LINE = 150; // if you change this value, change the fscanf as well at @readFromFile

struct stat st ={0};

// creates folder under home directory. This folder will hold the files
void createFolder() {
    const char *HOME_DIR = getenv("HOME");
    
    if(HOME_DIR)
        fullPath = strcat((char *)HOME_DIR, SCAN_AND_REPORT);
    else
        fullPath = (char *)HOME;
    
    if(stat(fullPath, &st) == - 1)
        mkdir(fullPath, 0700);
}

void init() {
    srand((unsigned int)time(NULL));
    secret = "The submarine will surface at ten PM"; // the secret word
    secretFile = rand()%NUMBER_OF_THREADS; // the file in which the secret will be implanted
    secretLine = rand()%NUMBER_OF_LINES; // the line where the secret will be implanted
    secretStartingChar = rand()%(NUMBER_OF_CHARS_IN_LINE - strlen(secret)); // the starting char for the secret
    printf("Secret to be implanted to the %d file in the linked list, line: %d, index: %d", secretFile+1, secretLine+1, secretStartingChar+1);
    
    createFolder();
}

void printSeparator() {
    printf("\n\n---------------------------\n\n");
}

void printGeneratedLinkedList() {
    printSeparator();
    
    printf("Generated Linked List: ");
    file *i = start;
    file *next = NULL;
    
    for(; i != NULL; i = next) {
        printf("%lu",i->fileNumber+1);
        if(i->next != NULL)
            printf("->");
        next = i->next;
    }
    
    printSeparator();
}

int getNumberOfDigits(int a) {
    int tmp = 0;
    
    while(a != 0) {
        a = a/10;
        tmp++;
    }
    return tmp;
}

// creates linked list of all the files
void createLink(char *fileName, FILE *theFile, long fileNumber) {
    file *next = createFileStruct(fileName, theFile, fileNumber);
    
    if(start == NULL) {
        start = next;
        currentFile = next;
    } else {
        currentFile->next = next;
        currentFile = next;
    }
}

// creates a files and adds it to a linked list
void* createFile(void *param) {
    pthread_mutex_lock(&thread_lock);
    long i  = (long)param;
    
    // calculates the length of the full path in case: path OR file name OR number of threads will change in the future
    char b[strlen(fullPath) + strlen(FILE_NAME) + getNumberOfDigits(NUMBER_OF_THREADS)+2];
    
    sprintf(b, "%s/%s%ld", fullPath, FILE_NAME, i+1); // append int to the end of char array
    files[i] = fopen(b, "w");
    createLink(b, files[i], i);
    pthread_mutex_unlock(&thread_lock);
    
    return NULL;
}

// writes random chars to file via @getRandomChar()
void* writeToFile(void *param) {
    pthread_mutex_lock(&thread_lock);
    long i = (long)param;
    
    if(nextFileToWriteTo == NULL)
        nextFileToWriteTo = start;
    
    if(linkedListCounter++ == secretFile)
        thisIsTheSecretFile = true;
    
    if(thisIsTheSecretFile) {
        thisIsTheSecretFile= false;
        for(int line = 0; line < NUMBER_OF_LINES ; line ++) {
            for(int startingChar = 0; startingChar < NUMBER_OF_CHARS_IN_LINE-1; startingChar++) {
                if(line == secretLine && startingChar == secretStartingChar) {
                    printf("Thread: %lu - implanted secret at file: %lu", i+1, nextFileToWriteTo->fileNumber+1);
                    printSeparator();
                    fprintf(nextFileToWriteTo->file, "%s", secret);
                    startingChar += strlen(secret)-1;
                } else {
                    fprintf(nextFileToWriteTo->file, "%c", getRandomChar());
                }
            }
            fprintf(nextFileToWriteTo->file, "\n");
        }
    } else {
        for(int line = 0; line < NUMBER_OF_LINES ; line ++) {
            for(int startingChar = 0; startingChar < NUMBER_OF_CHARS_IN_LINE; startingChar++) {
                fprintf(nextFileToWriteTo->file, "%c", getRandomChar());
            }
            fprintf(nextFileToWriteTo->file, "\n");
        }
    }
    
    if(nextFileToWriteTo->next != NULL)
        nextFileToWriteTo = nextFileToWriteTo->next;
    
    pthread_mutex_unlock(&thread_lock);
    
    return NULL;
}

void* readFromFile(void *param) {
    pthread_mutex_lock(&thread_lock);
    
    if(!secretFound) { // if secret found, avoid searching in remaining files
        if(nextFileToReadFrom == NULL) {
            nextFileToReadFrom = start;
        }
        
        long i = (long)param;
        int lineNumber = 0;
        char line[NUMBER_OF_CHARS_IN_LINE];
        files[i] = fopen(nextFileToReadFrom->name, "r");
        
        while(fscanf(files[i], "%149[^\n]\n", line) == 1) {
            lineNumber++;
            if(strstr(line, secret) !=NULL) {
                secretFound = true;
                printf("Thread %lu found the secret at file: %lu, line: %d", i+1, nextFileToReadFrom->fileNumber+1, lineNumber);
                printSeparator();
            }
        }
        
        nextFileToReadFrom = nextFileToReadFrom->next;
    }
    pthread_mutex_unlock(&thread_lock);
    
    return NULL;
}

// closes all open files
void closeFiles() {
    for(int i = 0; i < NUMBER_OF_THREADS; i++) {
        fclose(files[i]);
    }
}

// frees strdup and all file structs created by malloc
void release() {
    file *i = start;
    file *next = NULL;
    
    for(; i != NULL ; i = next) {
        next = i->next;
        free(i->name);
        free(i);
    }
}

#endif /* input_output_h */
