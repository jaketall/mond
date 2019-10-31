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
#include "mond.h"

#define LINE_SIZE 1000

Monitor m; // global

void parse_args(Monitor * monitor, int argc, char *argv[]) {
    monitor -> sys_mon = 0;
    if (argc > 4) {
        monitor -> sys_mon = 1;
    }
    monitor -> exec = argv[2];
    monitor -> interval = atoi(argv[3]);
    monitor -> filename = argv[4];
}

int execute(Monitor * monitor) {
    int pid;

    pid = fork();
    if (pid < 0) {
        perror("Fork Failure");
    } else if (pid == 0) {
        printf(monitor->exec);
        if(execl(monitor->exec, ((char *)NULL)) < 0) {
            perror("exec fail");
        }
    } else {
        return pid;
    }
    return 0;
}

void write_system_stats(){
    char *token;
    char buf[LINE_SIZE];
    char output[LINE_SIZE];
    const char s[2] = " ";

    output[0] = '\0';

    FILE *fp;
    fp=fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("failed to open proc stat");
        return;
    }
    fgets(buf, LINE_SIZE, fp);
    
    token = strtok(buf, s);
    token = strtok(NULL, s);
    // TODO timestamp
    printf("System [PROCESS] cpuusermode %s", token);
    printf("\n");
}

void write_job_stats() {

}

// signal handler for timer
void wake_up() {
    if (DEBUG)
        printf("monitor wakeup --- \n");

    if (m.sys_mon) {
        write_system_stats();
    }
    write_job_stats();
}

int main(int argc, char *argv[]) {
    int pid, status;
    struct itimerval it_val;

    parse_args(&m, argc, argv);

    // for catching timer sig
    if (signal(SIGALRM, (void (*)(int)) wake_up) == SIG_ERR) {
        perror("Unable to catch SIGALRM");
        exit(1);
    }

    it_val.it_value.tv_sec = m.interval/1000000;
    it_val.it_value.tv_usec = m.interval;
    it_val.it_interval = it_val.it_value;
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("error calling setitimer()");
        exit(1);
    }

    // printf(m.filename);
    pid = execute(&m);
    waitpid(pid, &status, 0);
    return 0;
}


