#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>

typedef struct ThreadPool ThreadPool_t;

ThreadPool_t* threadpool_create(int num_threads, int max_queue_size);
bool threadpool_add(ThreadPool_t* pool, void (*function)(void *p), void *argument);
void threadpool_destroy(ThreadPool_t* pool, bool finish_pending);

#endif // THREADPOOL_H