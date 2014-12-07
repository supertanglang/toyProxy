/*
 * toyProxy - A reverse proxy for server
 *
 * filename: server.c
 * descript: Server related data structures and functions
 */


#include <sys/time.h>

#include "server.h"
#include "network.h"


// server thread worker
void server_thread(void *_server_conf)
{
    server_conf_t *server_conf = (server_conf_t *)_server_conf;
    int clientsockfd = server_conf->clientsockfd;
    int backendsockfd = sock_connect(server_conf->backend_host,
                                       server_conf->backend_port);
    int recved;
    char buffer[NET_BUFFER];
    float response;
    int err = 0;

    struct timeval timer1, timer2;

    while (1) {
        // recv request from client
        if ((recved = recv_buffer(clientsockfd, buffer)) < 0) {
            err |= 1;
            break;
        }

        // if end of recving
        if (recved == 0) {
            break;
        }

        // request from backend host
        if (send_buffer(backendsockfd, buffer, recved) < 0) {
            err |= 1;
            break;
        }
    }

    gettimeofday(&timer1, NULL);

    while (1) {
        // recv response from backend host
        if ((recved = recv_buffer(backendsockfd, buffer)) < 0) {
            err |= 1;
            break;
        }

        // if end of the receving
        if (recved == 0) {
            break;
        }

        // respond to client
        if (send_buffer(clientsockfd, buffer, recved) < 0) {
            err |= 1;
            break;
        }
    }    

    gettimeofday(&timer2, NULL);

    // coarsely estimate server response time
    response += (timer2.tv_sec - timer1.tv_sec) * 1000 
        + (timer2.tv_usec - timer2.tv_usec) / 1000;

    if (!err) {
        server_conf->response = response;
    }
    else {
        server_conf->response = -1;
    }
}
