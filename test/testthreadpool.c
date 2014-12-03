

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "threadpool.h"



static int n = 0;


void threadfunc(void *argument) {

    int *i = (int *)argument;

    (*i)++;
    printf ("I'm thread %d\n", *i);

}


int main()
{
     threadpool_t *pool = threadpool_create(100, 150);
     
     if (pool == NULL) {
          perror("Error creating pool\n");
     }
     
     int i = 0;
     int k = 0;
     for (i = 0; i < 100; ++i) {
          if ((k = threadpool_assign(pool, threadfunc, (void *)&n)) != 0){
               printf("error assigning task %d\n", k);
          }
     }

     sleep(1);

     threadpool_destroy(pool);

     return 0;
     
}

