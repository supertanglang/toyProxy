/*
 * toyProxy - A reverse proxy for server
 *
 * filename: threadpool.h
 * descript: A simple threadpool implementation, with ideas and inputs
 *           from https://github.com/mbrossard/threadpool
 */


#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__


typedef struct threadpool threadpool_t;


// create thread pool with thread number and queue size
// param: thread_num - the total thread number in the pool
// param: queue_size - the total queue size for all the task function
// return: the threadpool on success, NULL on error
threadpool_t *threadpool_create(int thread_num, int queue_size);

// assign new task to the pool
// param: threadpool - the threadpool to assign task to
// param: thread_task - the task function
// param: argument - the argument
// return: the threadpool on success, NULL on error
int threadpool_assign(threadpool_t *pool, void (*task_func)(void *),
                      void *argurment);

// destroy the thread pool
// param: threadpool - the threadpool to destroy
int threadpool_destroy(threadpool_t *threadpool);


#endif
