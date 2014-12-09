/*
 * toyProxy - A reverse proxy for server
 *
 * filename: server.c
 * descript: Server related data structures and functions
 */


#include <sys/time.h>

#include "server.h"
#include "network.h"
#include "stdio.h"
#include "string.h"


// server thread worker
void server_thread(void *_server_conf)
{
    server_conf_t *server_conf = (server_conf_t *)_server_conf;
    int clientsockfd = server_conf->clientsockfd;
    int backendsockfd;
    int recved;
    char buffer[NET_BUFFER];
    float response = RESPONSE;
    int err = 0;
    struct timeval timer1, timer2;

    // printf("[DEBUG] A new thread is assigned!\n");

    // printf("[DEBUG] Connecting to %s : %d!\n", server_conf->backend_host,
    //         server_conf->backend_port);

    if ((backendsockfd = sock_connect(server_conf->backend_host,
                                      server_conf->backend_port)) < 0) {
        printf("Error connecting to server\n");
        return;
    }

    // recv request from client
    if ((recved = recv_buffer(clientsockfd, buffer)) < 0) {
        err |= 1;
        return;
    }

    puts(buffer);

    // if end of recving
    if (recved == 0) {
        return;
    }

    // request from backend host
    if (send_buffer(backendsockfd, buffer, recved) < 0) {
        err |= 1;
        return;
    }

    gettimeofday(&timer1, NULL);
    int flag = 1;

    while (1) {
        flag = 0;

        // recv response from backend host
        if ((recved = recv_buffer(backendsockfd, buffer)) < 0) {
            err |= 1;
            break;
        }

        // only record the first time
        if (flag) {
            gettimeofday(&timer2, NULL);
        }

        // respond to client
        if (send_buffer(clientsockfd, buffer, recved) < 0) {
            err |= 1;
            break;
        }

        // if end of the receving
        if (recved < NET_BUFFER) {
            break;
        }
    }

    // coarsely estimate server response time
    response += (timer2.tv_sec - timer1.tv_sec) * 1000 
        + (timer2.tv_usec - timer2.tv_usec) / 1000;

    if (!err) {
        pthread_mutex_lock(&(server_conf->lock));
        server_conf->response = response;
        pthread_mutex_unlock(&(server_conf->lock));
    }

    // printf("[DEBUG] Closing connectiion\n");

    close(backendsockfd);
    close(clientsockfd);
}
