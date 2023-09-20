#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 1

int counter = 0;
bool locked = false;
pthread_mutex_t lock;

void *count(void *id) {
    locked = true;
    pthread_mutex_lock(&lock);
    sleep(5);
    printf("counter is locked\n");
    pthread_mutex_unlock(&lock);
    printf("counter is unlocked\n");
    pthread_exit(NULL);
}

void createThreads(pthread_t threads[NUM_THREADS]) {
    long t;
    for(t = 0; t < NUM_THREADS; ++t) {
        printf("creating thread %ld\n", t);
        if (pthread_create(&threads[t], NULL, count, (void *)t)) {
            perror("ERROR in pthread_create");
            exit(EXIT_FAILURE);
        }
    }
}

void waitForThreads(pthread_t threads[NUM_THREADS]) {
    long t;
    puts("");
    for (t = 0; t < NUM_THREADS; ++t) {
        if (pthread_join(threads[t], NULL)) {
            pid_t pid = getpid();
            fprintf(stderr, "In process %ld ", (long)pid);
            perror("ERROR in pthread_join");
        }
        else {
            pid_t pid = getpid();
            printf("process %ld joined thread %ld\n", (long)pid, t);
        }
    }
}

void parent_handler() {
    pthread_mutex_unlock(&lock);
    printf("parent handler unlock\n");
}

void child_handler() {
    pthread_mutex_unlock(&lock);
    printf("child handler unlock\n");
}

void prepare_handler() {
    printf("prepare handler called\n");
    while (locked == false) {
        sleep(1);
    }
    pthread_mutex_lock(&lock);
    printf("prepare handler has lock\n");
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    createThreads(threads);

    printf("before fork\n");
    pthread_atfork(prepare_handler, parent_handler, child_handler);
    fork();
    printf("process %ld was forked\n", (long)getpid());

    waitForThreads(threads);

    pthread_mutex_lock(&lock);
    printf("mutex locked successfully by process %ld\n", (long)getpid());
    
    return 0 ;
}