#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
//#include "common_threads.h"
#include <semaphore.h>
#include <assert.h>

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct __ns_mutex_t {
    int waiting_room_1; //number of threads waiting in this room
    int waiting_room_2;
    sem_t mutex;
    sem_t room_1_lock;
    sem_t room_2_lock;

} ns_mutex_t;

// the single mutex lock we are using for this program
 ns_mutex_t lock;

void ns_mutex_init(ns_mutex_t *m) {
    m->waiting_room_1 = 0;
    m->waiting_room_2 = 0;
    sem_init(&m->mutex, 0, 1); 
    sem_init(&m->room_1_lock, 0, 1);
    sem_init(&m->room_2_lock, 0, 0);
}

void ns_mutex_acquire(ns_mutex_t *m) {
    sem_wait(&m->mutex);
    m->waiting_room_1++;
    sem_post(&m->mutex);

    sem_wait(&m->room_1_lock);
    m->waiting_room_2++;
    sem_wait(&m->mutex);
    m->waiting_room_1--;

    sleep(1);
    if (m->waiting_room_1 == 0) {
        sem_post(&m->mutex);
        sem_post(&m->room_2_lock);
    } else {
        sem_post(&m->mutex);
        sem_post(&m->room_1_lock);
    }
    sleep(1);

    sem_wait(&m->room_2_lock);
    m->waiting_room_2--;
}

void ns_mutex_release(ns_mutex_t *m) { 
    sleep(1);
    if (m->waiting_room_2 == 0) {
        sem_post(&m->room_1_lock);
    } else {
        sem_post(&m->room_2_lock);
    }
    sleep(1);
}



 int counter = 0;

void *worker(void *arg) {
    ns_mutex_acquire(&lock);
    int thread_id = (int) pthread_self();
    counter++;
    printf("%d is running...\n", thread_id);
    //counter++;
    ns_mutex_release(&lock);
    //counter++;
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);
    ns_mutex_init(&lock);
    pthread_t p[num_threads];
    printf("parent: begin\n");

    int i;
    for (i = 0; i < num_threads; i++) {
        pthread_create(&p[i], NULL, worker, NULL);
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(p[i], NULL);
    }
    
    printf("counter: %d\n", counter);
    printf("parent: end\n");
    return 0;
}
