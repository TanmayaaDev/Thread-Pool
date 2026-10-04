#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "threadpool.h"

// Mutex to protect stdout from garbled multi-threaded print output
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

// Simulated work function
void sample_task(void *arg) {
    int task_id = *(int *)arg;
    free(arg); // Free the dynamically allocated argument

    pthread_mutex_lock(&print_lock);
    printf("Thread [PID: %lu] started executing Task %d\n", pthread_self(), task_id);
    pthread_mutex_unlock(&print_lock);

    // Simulate varying workloads
    usleep(100000); // 100ms

    pthread_mutex_lock(&print_lock);
    printf("Thread [PID: %lu] finished executing Task %d\n", pthread_self(), task_id);
    pthread_mutex_unlock(&print_lock);
}

int main(void) {
    printf("Initializing thread pool with 4 threads and queue capacity of 20...\n");
    
    ThreadPool_t *pool = threadpool_create(4, 20);
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool!\n");
        return 1;
    }

    // Submit 10 tasks to the pool
    for (int i = 1; i <= 10; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        
        if (!threadpool_add(pool, sample_task, task_id)) {
            fprintf(stderr, "Failed to add Task %d to queue\n", i);
            free(task_id);
        }
    }

    // Allow time for all threads to finish processing tasks
    sleep(2);

    printf("Destroying thread pool and cleaning up resources...\n");
    threadpool_destroy(pool, 0);
    pthread_mutex_destroy(&print_lock);

    printf("Thread pool test suite completed successfully!\n");
    return 0;
}