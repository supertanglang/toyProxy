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


    // if not enough number of servers or too many servers
    num_servers = (argc - 2) / 2;
    if (argc < 4 || num_servers > MAX_SERVERS || argc % 2) {
        fprintf(stderr, "Usage: %s PROXY_PORTNO [BACKEND_ADDR PORTNO]...\n"
                "MAX server num: %d\n", argv[0], MAX_SERVERS);
        exit(1);
    }

    // Read in configs
    portno = atoi(argv[1]);
    for (i = 0; i < num_servers ; ++i) {
        backend_hosts[i] = argv[i * 2 + 2];
        backend_port[i] = atoi(argv[i * 2 + 3]);
    }

    // Start server procedure
    // setup network, start server
    int serverfd;
    server_conf_t server_conf;
    balancer_t balancer;
    unsigned backend_rotate;

    // Init balancer
    balancer_init(num_servers, &balancer);

    // Init thread pool
    threadpool_t *pool = threadpool_create(THREAD_NUM, THREADPOOL_SIZE);

    // creating server thread pool
    printf("[INFO] Created server pool with %d threads and %d queue\n",
           THREAD_NUM, THREADPOOL_SIZE);

    // starting server procedure
    printf("[INFO] Starting server procedure\n");

    if (serverfd = socket(AF_INET,
                          SOCK_STREAM,
                          0) < 0) {
        perror("Error creating socket\n");
        exit(1);
    }
    if (sock_bind_listen(serverfd, portno, MAX_CONNECT) < 0) {
        perror("Error binding and listening to socket\n");
        exit(1);
    }
    printf("[INFO] Server procedure started\n");

    // Main loop
    while (1) {
        // accept new connection
        int clientfd;
        int server_index;
        float response;

        server_index = balancer_balance(&balancer);

        // accept and new thread
        if ((clientfd = sock_accept(serverfd)) > 0) {
            // hand new request to threads
            server_conf.clientsockfd = clientfd;

            threadpool_assign(pool, server_thread, (void *)&server_conf);
        }
        // get server response  
        balancer.response[server_index] = server_conf.response;
    }

    // Gracefully shutting down
    threadpool_destroy(pool);

    // DEBUG
    printf("[INFO] Starting server procedure\n");

    return 0;
}
