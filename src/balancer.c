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
void balancer_init(balancer_t *balancer, int server_num, int *weights)
{
    int i;

    balancer->server_num = server_num;
    balancer->index = 0;

    for (i = 0; i < server_num; ++i) {
        balancer->weights[i] = weights[i];
    }

}


// balance the load, find the next index
void balancer_balance(balancer_t *balancer, server_conf_t *server_conf)
{
    int i;
    int index = balancer->index;
    int weight_sum = 0;
    float min_weight;
    float weight;

    (balancer->index)++;
    balancer->index %= balancer->server_num;

    if (server_conf->total_conn == 0) {
        return;
    }

    // find the least weight
    min_weight = (float)server_conf->connections[index] / (float)balancer->weights[index];
    for (i = 0; i < balancer->server_num; ++i) {

        printf("server %d has %d connections\n", i, server_conf->connections[i]);
        weight = (float)server_conf->connections[i] / (float)balancer->weights[i];
        if (weight < min_weight) {
            min_weight = weight;
            index = i;
        }
    }
    balancer->index = index;
}

