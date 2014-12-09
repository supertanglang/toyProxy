/*
 * toyProxy - A reverse proxy for server
 *
 * filename: server.h
 * descript: Server related data structures and functions
 */


#ifndef __SERVER_H__
#define __SERVER_H__


#include <pthread.h>


// default value config
#define MAX_SERVERS 8
#define RESPONSE 15000 


// server thread configuration
typedef struct server_conf
{
    // server mutex
    pthread_mutex_t lock;

    // client socket file
    int clientsockfd;

    // backend server index
    int backend_index;

    // backend server socketfd
    int backend_fd[MAX_SERVERS];

    // backend server names
    char *backend_host;

    // the portno of backend server
    int backend_port;

    // delay of backend server response
    float response;

} server_conf_t;


// server thread worker
void server_thread(void *_server_conf);


#endif
