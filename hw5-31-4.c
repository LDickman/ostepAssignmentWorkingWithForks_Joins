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
    // add semaphores and other information here
    sem_t writelock;
    sem_t readlock;
    int readers;
} rwlock_t;


void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    sem_init(&rw->readlock, 0, 1);
    //sem_init(&rw->readlock, 1, sleep(1));
    sem_init(&rw->writelock, 0, 1);
    //sem_init(&rw->writelock, 1, sleep(1));

}

void rwlock_acquire_readlock(rwlock_t *rw) {
    //sem_post(&rw->readlock); //needed if using sleep
    sem_wait(&rw->readlock);
    sleep(1);
    rw->readers++;
    if (rw->readers == 1) {
        sem_wait(&rw->writelock); // first reader acquires writelock
    }
    sleep(1);
    sem_post(&rw->readlock);
}

void rwlock_release_readlock(rwlock_t *rw) {
    //sem_post(&rw->readlock); //needed if using sleep
    sem_wait(&rw->readlock);
    sleep(1);
    rw->readers--;
    if (rw->readers == 0) {
        sem_post(&rw->writelock); // last reader releases writelock
    }
    sleep(1);
    sem_post(&rw->readlock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sleep(1); /// comminting these out result in write printing first before read prints
    sem_wait(&rw->writelock);
    sleep(1); /// comminting these out result in write printing first before read prints
}

void rwlock_release_writelock(rwlock_t *rw) {
    sleep(1); /// comminting these out result in write printing first before read prints
    sem_post(&rw->writelock);
    sleep(1);  /// comminting these out result in write printing first before read prints
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
