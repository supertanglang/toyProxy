/*
 * toyProxy - A reverse proxy for server
 *
 * filename: main.c
 * descript: The main process
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "threadpool.h"
#include "balancer.h"
#include "network.h"
#include "server.h"
#include "config.h"


// The main function
int
main(int argc, char **argv)
{
    // Read in argc
    int i;
    int num_servers;
    int portno;
    char *backend_hosts[MAX_SERVERS];
    int backend_port[MAX_SERVERS];
    int weights[MAX_SERVERS];

    // if not enough number of servers or too many servers
    num_servers = (argc - 2) / 3;
    if (argc < 4 || num_servers > MAX_SERVERS || (argc - 2) % 3) {
        fprintf(stderr, "Usage: %s PROXY_PORTNO [BACKEND_ADDR PORTNO WEIGHT]...\n"
                "MAX server num: %d\n", argv[0], MAX_SERVERS);
        exit(1);
    }

    // Read in configs
    portno = atoi(argv[1]);
    for (i = 0; i < num_servers; ++i) {
        int host_index = i * 3 + 2;

        backend_hosts[i] = argv[host_index];
        backend_port[i] = atoi(argv[host_index + 1]);
        weights[i] = atoi(argv[host_index + 2]);

        printf("[DEBUG] server %s:%d weight %d\n",
               backend_hosts[i],
               backend_port[i],
               weights[i]);
    }

    // Start server procedure
    // setup network, start server
    int serverfd;
    server_conf_t server_conf;
    balancer_t balancer;
    unsigned backend_rotate;
    threadpool_t *pool;

    // Init balancer
    balancer_init(&balancer, num_servers, weights);

    // Init thread pool
    pool = threadpool_create(THREAD_NUM, THREADPOOL_SIZE);

    // creating server thread pool
    printf("[INFO] Created server pool with %d threads and %d queue\n",
           THREAD_NUM, THREADPOOL_SIZE);

    // Init server config
    pthread_mutex_init(&(server_conf.lock), NULL);
    server_conf.total_conn = 0;
    for (i = 0; i < num_servers; ++i) {
        server_conf.connections[i] = 0;
    }

    // starting server procedure
    // Starting binding and listening
    printf("[INFO] Starting server procedure\n");
    if ((serverfd = socket(AF_INET,
                           SOCK_STREAM,
                           0)) < 0) {
        perror("Error creating socket\n");
        exit(1);
    }

    if (sock_bind_listen(serverfd, portno, MAX_CONNECT) < 0) {
        perror("Error binding and listening to socket\n");
        exit(1);
    }

    // Main loop
    while (1) {
        // accept new connection
        int clientfd;

        // setup server
        balancer_balance(&balancer, &server_conf);

        server_conf.index = balancer.index;
        server_conf.backend_host = backend_hosts[balancer.index];
        server_conf.backend_port = backend_port[balancer.index];

        printf("balancer index %d out of %d servers\n", balancer.index, balancer.server_num);

        // setup connection number
        pthread_mutex_lock(&(server_conf.lock));
        server_conf.connections[balancer.index]++;
        server_conf.total_conn++;
        pthread_mutex_unlock(&(server_conf.lock));

        // accept and new thread
        if ((clientfd = sock_accept(serverfd)) > 0) {
            printf("[INFO] Accepting new connection\n");
           
            // hand new request to threads
            server_conf.clientsockfd = clientfd;

            threadpool_assign(pool, server_thread, (void *)&server_conf);
        }
    }

    // Gracefully shutting down
    threadpool_destroy(pool);

    // DEBUG
    printf("[INFO] closing procedure\n");

    pthread_mutex_destroy(&(server_conf.lock));

    return 0;
}
