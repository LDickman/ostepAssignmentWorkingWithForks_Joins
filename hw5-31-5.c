#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
//#include "common_threads.h"
#include <semaphore.h>
#include <assert.h>

//
// Your code goes in the structure and functions below
//

typedef struct __rwlock_t {
    sem_t writelock;
    sem_t readlock;
    sem_t lock;
    int readers;
} rwlock_t;


void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    sem_init(&rw->readlock, 0, 1);
    sem_init(&rw->lock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
    //sem_init(&rw->writelock, 1, sleep(1));
    //sem_init(&rw->readlock, 1, sleep(1));
    //sem_init(&rw->lock, 1, sleep(1));
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    // sem_post(&rw->lock);  // needed if using sleep in the sem_init
    //sem_post(&rw->readlock); // needed if using sleep in the sem_init
    sem_wait(&rw->readlock);
    
    //critcal sections
    sleep(1);
    rw->readers++;
    if (rw->readers == 1) {
        //printf("writer acquires readlock\n");  // used print statemnets to know where issues are occuring
        sem_wait(&rw->writelock); // first writer acquires writelock
        sem_wait(&rw->lock); // ensures that each writer get a turn
    }
    sleep(1);
    //critcal sections

    sem_post(&rw->readlock);
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->readlock);
    
    //critcal sections
    sleep(1);
    rw->readers--;
    if (rw->readers == 0) {
        //printf("writer release readlock\n");  // used print statemnets to know where issues are occuring
        sem_post(&rw->writelock); // then first writer releases writelock
        sem_post(&rw->lock); // ensures that each writer get a turn
    }
    sleep(1);
    //critcal sections 
    
    sem_post(&rw->readlock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sleep(1);
    sem_wait(&rw->lock);
    sem_wait(&rw->writelock);
    sleep(1); 
}

void rwlock_release_writelock(rwlock_t *rw) {
    sleep(1);
    sem_post(&rw->writelock);
    sem_post(&rw->lock);
    sleep(1);
}

//
// Don't change the code below (just use it!)
// 

int loops;
int value = 0;

rwlock_t lock;

void *reader(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
	rwlock_acquire_readlock(&lock);
	printf("read %d\n", value);
	rwlock_release_readlock(&lock);
    }
    return NULL;
}

void *writer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
	rwlock_acquire_writelock(&lock);
	value++;
	printf("write %d\n", value);
	rwlock_release_writelock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 4);
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t pr[num_readers], pw[num_writers];

    rwlock_init(&lock);

    printf("begin\n");

    int i;
    for (i = 0; i < num_readers; i++)
	pthread_create(&pr[i], NULL, reader, NULL);
    for (i = 0; i < num_writers; i++)
	pthread_create(&pw[i], NULL, writer, NULL);

    for (i = 0; i < num_readers; i++)
	pthread_join(pr[i], NULL);
    for (i = 0; i < num_writers; i++)
	pthread_join(pw[i], NULL);

    printf("end: value %d\n", value);

    return 0;
}