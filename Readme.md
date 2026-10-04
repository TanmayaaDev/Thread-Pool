# ThreadPool-C

A lightweight, zero-dependency C99 concurrent thread pool implementation built using POSIX threads (`pthreads`), mutexes, and condition variables.

---

## Features

- **Fixed Worker Pool:** Spawns a dedicated pool of worker threads upon initialization to process asynchronous tasks concurrently.
- **Thread-Safe FIFO Queue:** Circular task queue protected by mutex locks to prevent race conditions.
- **Busy-Wait Avoidance:** Utilizes condition variables (`pthread_cond_t`) to suspend worker threads when the queue is empty, preventing unnecessary CPU spinning.
- **Graceful Shutdown:** Signals worker threads, flushes pending tasks, and cleanly joins all resources on destruction.
- **Zero External Dependencies:** Built purely on standard C libraries and POSIX threads.

---

## Directory Structure

```text
ThreadPool-C/
├── include/
│   └── threadpool.h
├── src/
│   ├── threadpool.c
│   └── main.c
├── tests/
│   └── test_pool.c
├── Makefile
└── README.md