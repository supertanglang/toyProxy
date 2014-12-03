/*
 * toyProxy - A reverse proxy for server
 *
 * filename: server.h
 * descript: Server related data structures and functions
 */

#include "network.h"
#include "config.h"


// server args configuration
typedef struct server_conf
{
     // number of backend servers
     int num;
     // the portno of backend servers
     int portno;
} server_conf_t;


