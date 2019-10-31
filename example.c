//
// Created by Jake Tallman on 10/30/19.
//


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include "mond.h"

int main () {
    int i;
    int val;
    for (i = 0; i < 20; i ++) {
        val = 7;
        val ++;
        val = val * 9;
        sleep(1);
    }
    return 0;
}