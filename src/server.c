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
    int backendsockfd = socket_connect(server_conf->backend_host,
                                       server_conf->backend_port);
    int recved;
    char buffer[MAX_BUFFER];
    float response;
    struct timeval timer1, timer2;
        
    // recv request from client
    if ((recved = recv_buffer(clientsockfd, buffer)) < 0) {
        return;
    }
    
    gettimeofday(&timer1, NULL);
    
    // request from backend host
    if (send_buffer(backendsockfd, buffer, recved) != 0) {
        return;
    }
    
    // recv response from backend host
    if ((recved = recv_buffer(backendsockfd, buffer)) < 0) {
        return;
    }

    gettimeofday(&timer2, NULL);

    // respond to client
    if (send_buffer(clientsockfd, buffer, recved) < 0) {
        return;
    }

    // coarsely estimate server response time
    response = (timer2.tv_sec - timer1.tv_sec) * 1.0 
               + (timer2.tv_usec - timer2.tv_usec) * 1000000;
    
    server_conf->response = response;
}
