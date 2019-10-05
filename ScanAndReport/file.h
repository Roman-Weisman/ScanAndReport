//
//  file.h
//  ScanAndReport
//
//  Created by Roman Weisman on 10/4/19.
//  Copyright © 2019 Roman Weisman. All rights reserved.
//

#ifndef file_h
#define file_h

#include <stdlib.h>

typedef struct file{
    char *name;
    FILE *file;
    long fileNumber;
    struct file *next;
} file;

file* createFileStruct(char *name, FILE *theFile, long fileNumber) {
    file *f = malloc(sizeof(file));
    f->name = strdup(name);
    f->file = theFile;
    f->fileNumber = fileNumber;
    f->next = NULL;
    return f;
}


#endif /* file_h */
