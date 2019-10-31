//
// Created by Jake Tallman on 10/30/19.
//

#ifndef MOND_MOND_H
#define MOND_MOND_H

#define DEBUG 0

typedef struct monitor_s {
    int sys_mon;
    char * exec;
    int interval;
    char * filename;
} Monitor;

#endif //MOND_MOND_H
