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

#define LINE_SIZE 1000

Monitor m; // global
int pid;

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
    const char s[10] = " \t\r\n\v\f";

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

    token = strtok(NULL, s);
    token = strtok(NULL, s);
    printf(" cpusystemmode %s", token);

    token = strtok(NULL, s);
    printf(" idletaskrunning %s", token);

    token = strtok(NULL, s);
    printf(" iowaittime %s", token);

    token = strtok(NULL, s);
    printf(" irqservicetime %s", token);

    token = strtok(NULL, s);
    printf(" softirqservicetime %s", token);

    do {
        fgets(buf, LINE_SIZE, fp);
        token = strtok(buf, s);
    } while (strcmp(token, "intr"));

    token = strtok(NULL, s);
    printf(" intr %s", token);

    do {
        fgets(buf, LINE_SIZE, fp);
        token = strtok(buf, s);
    } while (strcmp(token, "ctxt"));

    token = strtok(NULL, s);
    printf(" ctxt %s", token);

    fgets(buf, LINE_SIZE, fp); //btime

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" forks %s", token);

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" runnable %s", token);

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" blocked %s", token);

    printf(" [MEMORY]"); // ----

    fclose(fp);

    fp=fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("failed to open proc mem");
        return;
    }

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" memtotal %s", token);


    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" memfree %s", token);


    fgets(buf, LINE_SIZE, fp); //avail
    fgets(buf, LINE_SIZE, fp); //buf
    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" cached %s", token);

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" swapcached %s", token);

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" active %s", token);

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    printf(" inactive %s", token);

    printf(" [LOADAVG]"); //-------

    fclose(fp);

    fp=fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        perror("failed to open proc ldavg");
        return;
    }

    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);

    printf(" 1min %s", token);

    token = strtok(NULL, s);
    printf(" 5min %s", token);

    token = strtok(NULL, s);
    printf(" 15min %s", token);

    printf(" [DISKSTATS(sda)]"); //-------

    fclose(fp);

    fp=fopen("/proc/diskstats", "r");
    if (fp == NULL) {
        perror("failed to open proc ldavg");
        return;
    }

    fgets(buf, LINE_SIZE, fp); // fd0
    fgets(buf, LINE_SIZE, fp);
    token = strtok(buf, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);

    token = strtok(NULL, s);
    printf(" totalnoreads %s", token);

    token = strtok(NULL, s); // merged
    token = strtok(NULL, s);
    printf(" totalsectorsread %s", token);

    token = strtok(NULL, s);
    printf(" nomsread %s", token);

    token = strtok(NULL, s);
    printf(" totalnowrites %s", token);

    token = strtok(NULL, s); // merged
    token = strtok(NULL, s);
    printf(" nosectorswritten %s", token);

    token = strtok(NULL, s);
    printf(" nomswritten %s", token);

    fclose(fp);

    printf("\n");
}

void print_time() {
    time_t rawtime;
    struct tm * timeinfo;
    int i;
    char c;
    char * timeinfos;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    timeinfos = asctime (timeinfo);

    i = 0;
    while ((c = timeinfos[i]) != '\n') {
        i ++;
    }
    timeinfos[i] = '\0';
    printf ( "[%s]", asctime (timeinfo) );

}

void write_job_stats() {
    char *token;
    char filename[50];
    char pid_s[30];
    char buf[LINE_SIZE];
    const char s[10] = " \t\r\n\v\f";
    FILE *fp;

    filename[0] = '\0';
    strcat(filename, "/proc/");
    sprintf(pid_s, "%d", pid);
    strcat(filename, pid_s);
    strcat(filename, "/stat");

    if (DEBUG)
        printf("openfile %s\n", filename);
    fp=fopen(filename, "r");
    if (fp == NULL) {
        perror("failed to open proc file ");
        return;
    }

    fgets(buf, LINE_SIZE, fp); //pid
    fgets(buf, LINE_SIZE, fp);

    token = strtok(buf, s);
    token = strtok(NULL, s);
    print_time();
    printf("Process(%s) [STAT] ", pid_s);

    printf("executable (%s) ", token);

    token = strtok(NULL, s);
    printf("stat %s ", token);

    token = strtok(NULL, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);
    printf("minorfaults %s ", token);

    token = strtok(NULL, s);
    token = strtok(NULL, s);
    printf("majorfaults %s ", token);

    token = strtok(NULL, s);
    token = strtok(NULL, s);
    printf("usermodetime %s ", token);

    token = strtok(NULL, s);
    printf("kernelmodetime %s ", token);

    token = strtok(NULL, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);
    printf("priority %s ", token);

    token = strtok(NULL, s);
    printf("nice %s ", token);

    token = strtok(NULL, s);
    printf("nothreads %s ", token);

    token = strtok(NULL, s);
    token = strtok(NULL, s);
    token = strtok(NULL, s);
    printf("vsize %s ", token);

    token = strtok(NULL, s);
    printf("rss %s ", token);

    fclose(fp);

    strcat(filename, "m");
    if (DEBUG)
        printf("openfile %s\n", filename);
    fp=fopen(filename, "r");
    if (fp == NULL) {
        perror("failed to open proc file ");
        return;
    }

    fgets(buf, LINE_SIZE, fp);

    token = strtok(buf, s);
    printf("[STATM] ");
    printf("program %s ", token);

    token = strtok(NULL, s);
    printf("residentset %s ", token);

    token = strtok(NULL, s);
    printf("share %s ", token);

    token = strtok(NULL, s);
    printf("text %s ", token);

    token = strtok(NULL, s);

    token = strtok(NULL, s);
    printf("data %s", token);

    printf("\n");

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

    pid = execute(&m);
    waitpid(pid, &status, 0);
    return 0;
}


