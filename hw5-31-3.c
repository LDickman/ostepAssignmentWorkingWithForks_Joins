#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
//#include "common_threads.h"
#include <semaphore.h>
#include <assert.h>

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.


typedef struct __barrier_t {
    // add semaphores and other information here
    sem_t lock;
    sem_t barr;
    int thread_count;
    int thread_arrived;
} barrier_t;


// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    // initialization code goes here
    //sem_init(&b->lock, 0, 1); // intialing the lock pointer
    sem_init(&b->lock, 1, sleep(1)); ///makes it faster??
    //sem_init(&b->barr, 0, 0); //intialing the barrier pointer
    sem_init(&b->barr, 0, sleep(1)); ///makes it faster??
    b->thread_count = num_threads;
    b->thread_arrived = 0;
}

void barrier(barrier_t *b) {
    // barrier code goes here
    sem_post(&b->lock); // including this does eally make a different expect it's faster, needed if using sleep
    sem_wait(&b->lock);
    
    // critial section
    b->thread_arrived++;    
    sleep(1);
    if (b->thread_arrived == b->thread_count) {
        sem_post(&b->barr);    /// indicates when to start the barrier pointer
    }
    sleep(1);
    // critial section
    
    sem_post(&b->lock);
    sem_wait(&b->barr);
    sem_post(&b->barr);
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}


// run with a single argument indicating the number of 
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);
    
    int i;
    for (i = 0; i < num_threads; i++) {
	t[i].thread_id = i;
	pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (i = 0; i < num_threads; i++) 
	pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}
