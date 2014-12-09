/*
 * toyProxy - A reverse proxy for server
 *
 * filename: server.c
 * descript: Server related data structures and functions
 */


#include "server.h"
#include "network.h"
#include "stdio.h"
#include "string.h"


// server thread worker
void server_thread(void *_server_conf)
{
    server_conf_t *server_conf = (server_conf_t *)_server_conf;
    int index = server_conf->index;
    int clientsockfd = server_conf->clientsockfd;
    int backendsockfd;
    int recved;
    char buffer[NET_BUFFER];
    float response = RESPONSE;
    int err = 0;
    struct timeval timer1, timer2;

    if ((backendsockfd = sock_connect(server_conf->backend_host,
                                      server_conf->backend_port)) < 0) {
        printf("Error connecting to server\n");
        goto exit;
    }

    // recv request from client
    if ((recved = recv_buffer(clientsockfd, buffer)) < 0) {
        err |= 1;
        goto exit;
    }

    // if end of recving
    if (recved == 0) {
        goto exit;
    }

    // request from backend host
    if (send_buffer(backendsockfd, buffer, recved) < 0) {
        err |= 1;
        goto exit;
    }


    while (1) {
        // recv response from backend host
        if ((recved = recv_buffer(backendsockfd, buffer)) < 0) {
            err |= 1;
            break;
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

    // printf("[DEBUG] Closing connectiion\n");

    // closing connections
    close(backendsockfd);
    close(clientsockfd);

exit:
    // Update server connection number
    pthread_mutex_lock(&(server_conf->lock));

    server_conf->connections[index]--;
    server_conf->total_conn--;

    pthread_mutex_unlock(&(server_conf->lock));
}
