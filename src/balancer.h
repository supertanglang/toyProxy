/*
 * toyProxy - A reverse proxy for server
 *
 * filename: balancer.h
 * descript: The balancing algorithm
 */


#ifndef __BALANCER_H__
#define __BALANCER_H__


#define SERVERS 8
#define MIN_RESPONSE 0.01


typedef struct balancer
{
    // index
    int index;

    // total num of servers
    int server_num;
    
    // server response time
    float response[SERVERS];
} balancer_t;


// init the balancer
// param: balancer - the balancer struct
// return 0 on success, -1 on error
void balancer_init(int server_num, balancer_t *balancer);

// balance the load, find the next index
void balancer_balance(balancer_t *balancer);


#endif
