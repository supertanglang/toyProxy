/*
 * toyProxy - A reverse proxy for server
 *
 * filename: threadpool.c
 * descript: A simple threadpool implementation, with ideas and inputs
 *           from https://github.com/mbrossard/threadpool
 */


#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "threadpool.h"


// Data structure for task assigned to single thread
typedef struct thread_task
{
    // task function for thread
    void (*task_func)(void *);

    // argument for the task function
    void *argument;
} thread_task_t;


// Data structure for thread pool
struct threadpool
{
    // lock for updating data structure
    pthread_mutex_t lock;

    // lock for pthread conditional waiting
    pthread_cond_t notify;

    // thread number
    int thread_count;

    // started thread count
    int working_count;

    // thread ID
    pthread_t *threads;

    // queue of task functions assigned to pool
    thread_task_t *task_queue;

    // queue size
    int queue_size;

    // task count in the queue
    int task_count;

    // queue head index in array
    int queue_head;

    // queue tail index in array
    int queue_tail;

    // signal to shutdown all threads
    int shutdown;
};


// free threadpool memory 
int threadpool_free(threadpool_t *pool);


// worker thread function
static void *threadpool_function(void *threadpool);


// free threadpool memory
// param: pool - the thread pool
int threadpool_free(threadpool_t *pool) 
{
    if (pool->task_queue) {
        free(pool->task_queue);
        free(pool->threads);

        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }

    if (pool) {
        free(pool);
    }
    return 0;
}


// worker thread function 
// param: argument - the thread pool 
static void *threadpool_function(void *threadpool) 
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    thread_task_t task;
     
    // keep looping, accepting new tasks
    while (1) {
        // acquire the lock
        pthread_mutex_lock(&(pool->lock));
          
        // wait until signaled for new task
        while ((pool->task_count == 0) && (!pool->shutdown)) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        // end the loop if shutdown is set
        if (pool->shutdown) {
            break;
        }

        // grab the task
        task.task_func = pool->task_queue[pool->queue_head].task_func;
        task.argument = pool->task_queue[pool->queue_head].argument;
        pool->queue_head++;
        pool->queue_head = (pool->queue_head == pool->queue_size) ?
            0 : pool->queue_head;
        pool->working_count++;
        pool->task_count--;
          
        // unlock
        pthread_mutex_unlock(&(pool->lock));

        // go to work
        (*task.task_func)(task.argument);
        pool->working_count--;
    }
     
    // shutdown the thread
    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
}


// create thread pool with thread number and queue size
// param: thread_num - the total thread number in the pool
// param: queue_size - the total queue size for all the task function
// return: the threadpool on success, NULL on error
threadpool_t *threadpool_create(int thread_num, int queue_size)
{
    threadpool_t *pool;

    // malloc memory for threadpool
    if ((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL){
        goto err;
    }
     
    // malloc memory for all pthread IDs
    if ((pool->threads 
         = (pthread_t *)malloc(sizeof(pthread_t) * thread_num)) == NULL){
        goto err;
    }

    // malloc memory for task queue
    if ((pool->task_queue 
         = (thread_task_t *)malloc(sizeof(thread_task_t) * queue_size)) 
        == NULL){
        goto err;
    }

    // init data structures and variables
    pool->queue_size = queue_size;
    pool->thread_count = 0;
    pool->working_count = 0;
    pool->queue_head = 0;
    pool->queue_tail = 0;
    pool->shutdown = 0;
     
    // init pthread conditional var and mutex
    if ((pthread_mutex_init(&(pool->lock), NULL) != 0)){
        goto err;
    }
     
    // init pthread conditional variables
    if ((pthread_cond_init(&(pool->notify), NULL) != 0)){
        goto err;
    }

    // create and run worker threads
    int i;
    for (i = 0; i < thread_num; ++i) {
        if (pthread_create(&(pool->threads[i]), NULL,
                           threadpool_function, (void *)pool) != 0) {
            threadpool_destroy(pool);
            return NULL;
        }
        pool->thread_count++;
    }
     
    return pool;

    // handle errors in creating threadpool
err:

    threadpool_free(pool);

    return NULL;
}


// assign new task to the pool
// param: threadpool - the threadpool to assign task to
// param: task - the task function
// param: argument - the argument
// return: the threadpool on success, NULL on error
int threadpool_assign(threadpool_t *pool, void (*task_func)(void *),
                      void *argument)
{
    if (pool == NULL || task_func == NULL) {
        return -1;
    }
     
    // acquire lock
    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        goto err;
    }
     
    // find next available slot in queue
    int next = pool->queue_tail + 1;
    next = (next == pool->queue_size) ? 0 : next;

    // find if full
    if (pool->task_count == pool->queue_size) {
        goto err;
    }

    // find if shutdown
    if (pool->shutdown) {
        goto err;
    }

    // add task to the queue
    pool->task_queue[pool->queue_tail].task_func = task_func;
    pool->task_queue[pool->queue_tail].argument = argument;
    pool->queue_tail = next;
    pool->task_count++;

    // signal the condition variable
    if (pthread_cond_signal(&(pool->notify)) != 0) {
        goto err;
    }

err:
    // unlock the mutex
    if (pthread_mutex_unlock(&(pool->lock)) != 0) {
        return -1;
    }

    return 0;
}


// destroy the thread pool
// param: threadpool - the threadpool to destroy
// return 0 on success, -1 on error
int threadpool_destroy(threadpool_t *pool)
{
    if (pool == NULL) {
        return -1;
    }
     
    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        return -1;
    }
     
    if (pool->shutdown) {
        return -1;
    }

    pool->shutdown = 1;
     
    if (pthread_cond_broadcast(&(pool->notify)) != 0) {
        return -1;
    }
     
    if (pthread_mutex_unlock(&(pool->lock)) != 0) {
        return -1;
    }
     
    // join all the threads
    int i;
    for (i = 0; i < pool->thread_count; ++i) {
        if (pthread_join(pool->threads[i], NULL) != 0) {
            return -1;
        }
    }
     
    threadpool_free(pool);
    return 0;
}

