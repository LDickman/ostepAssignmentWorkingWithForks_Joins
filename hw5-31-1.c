#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
//#include "common_threads.h"
#include <semaphore.h>

sem_t s; 

void *child(void *arg) {
    printf("child\n");
    // use semaphore here
    sem_post(&s); // signal here: child is done
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");
    // init semaphore here
    sem_init(&s, 0, sleep(1)); /// sleep(1) is replace of X
    pthread_create(&p, NULL, child, NULL);
    // use semaphore here
    sem_wait(&s); // wait here for child
    printf("parent: end\n");
    return 0;
}