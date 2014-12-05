/*
 * toyProxy - A reverse proxy for server
 *
 * filename: server.h
 * descript: Server related data structures and functions
 */


#ifndef __SERVER_H__
#define __SERVER_H__


// default value config
const int MAX_SERVERS = 8;

// server thread configuration
typedef struct server_conf
{
    // client socket file
    int clientsockfd;

    // backend server hostname
    char *backend_host;

    // the portno of backend server
    int backend_port;

    // delay of backend server response
    float response;

} server_conf_t;


// server thread worker
void server_thread(server_conf_t *server_conf);


#endif
