/*
 * toyProxy - A reverse proxy for server
 *
 * filename: balancer.c
 * descript: The balancing algorithm
 */


#include "balancer.h"


// init the balancer
// param: balancer - the balancer struct
// return 0 on success, -1 on error
void balancer_init(int server_num, balancer_t *balancer)
{
    int i;

    balancer->server_num = server_num;
    balancer->index = 0;
    
    for (i = 0; i < server_num; ++i) {
        balancer->response[i] = MIN_RESPONSE;
    }
}


// balance the load, find the next index
void balancer_balance(balancer_t *balancer)
{
    (balancer->index)++;
    balancer->index %= balancer->server_num;
}

