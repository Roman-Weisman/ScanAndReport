//
//  utils.h
//  ScanAndReport
//
//  Created by Roman Weisman on 10/3/19.
//  Copyright © 2019 Roman Weisman. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <time.h>

enum fileRandomChars {SPACE, UPPER_CASE, LOWER_CASE};

char getRandomChar() {
    char randomChar;
    int r = rand()%3;
    
    switch (r) {
        case SPACE:
            randomChar = 32; // space
            break;
        case UPPER_CASE:
            randomChar = 65; // A
            break;
        case LOWER_CASE:
            randomChar = 97; // a
            break;
        default:
            randomChar = '!';
            break;
    }
    
    if(randomChar != 32 && randomChar != '!')
        randomChar += (rand() % 26);
    
    return randomChar;
}

#endif /* utils_h */
