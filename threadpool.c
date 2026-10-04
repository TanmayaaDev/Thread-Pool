#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "threadpool.h"

// Task queue entry used internally by the pool.
typedef struct Task {
    void (*function)(void *p);
    void *argument;
} Task_t;

// Internal structure for the Thread Pool
struct ThreadPool {
    pthread_t *threads;
    Task_t *task_queue;
    int capacity;
    int front;
    int rear;
    int count;
    int num_threads;
    bool stop;
    pthread_mutex_t lock;
    pthread_cond_t notify;
};

// Worker thread routine
static void *worker_thread(void *thread_pool) {
    ThreadPool_t *pool = (ThreadPool_t *)thread_pool;

    while (true) {
        pthread_mutex_lock(&(pool->lock));

        // Wait until there is a task in the queue or the pool is stopped
        while (pool->count == 0 && !pool->stop) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if (pool->stop && pool->count == 0) {
            pthread_mutex_unlock(&(pool->lock));
            break;
        }

        // Pull task from the queue (FIFO)
        Task_t task = pool->task_queue[pool->front];
        pool->front = (pool->front + 1) % pool->capacity;
        pool->count--;

        pthread_mutex_unlock(&(pool->lock));

        // Execute the task function outside of the critical section
        if (task.function != NULL) {
            task.function(task.argument);
        }
    }

    return NULL;
}

ThreadPool_t *threadpool_create(int num_threads, int max_queue_size) {
    if (num_threads <= 0 || max_queue_size <= 0) {
        return NULL;
    }

    ThreadPool_t *pool = (ThreadPool_t *)malloc(sizeof(ThreadPool_t));
    if (!pool) {
        return NULL;
    }

    pool->num_threads = num_threads;
    pool->capacity = max_queue_size;
    pool->front = 0;
    pool->rear = 0;
    pool->count = 0;
    pool->stop = false;

    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    pool->task_queue = (Task_t *)malloc(sizeof(Task_t) * max_queue_size);

    if (!pool->threads || !pool->task_queue) {
        free(pool->threads);
        free(pool->task_queue);
        free(pool);
        return NULL;
    }

    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->notify), NULL);

    // Spawn worker threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&(pool->threads[i]), NULL, worker_thread, (void *)pool);
    }

    return pool;
}

bool threadpool_add(ThreadPool_t *pool, void (*function)(void *p), void *argument) {
    if (!pool || !function) {
        return false;
    }

    pthread_mutex_lock(&(pool->lock));

    if (pool->count == pool->capacity || pool->stop) {
        pthread_mutex_unlock(&(pool->lock));
        return false; // Queue full or pool shutting down
    }

    // Add task to rear of queue
    pool->task_queue[pool->rear].function = function;
    pool->task_queue[pool->rear].argument = argument;
    pool->rear = (pool->rear + 1) % pool->capacity;
    pool->count++;

    // Signal one waiting worker thread
    pthread_cond_signal(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));

    return true;
}

void threadpool_destroy(ThreadPool_t *pool, bool finish_pending) {
    if (!pool) {
        return;
    }

    pthread_mutex_lock(&(pool->lock));
    pool->stop = true;
    pthread_cond_broadcast(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));

    // Join all worker threads
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // Cleanup synchronization primitives and memory
    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));

    free(pool->threads);
    free(pool->task_queue);
    free(pool);
}