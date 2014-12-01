/*
 * toyProxy - A reverse proxy for server
 *
 * filename: main.c
 * descript: The main process
 */


#include "network.h"
#include "server.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>


// The main function 
int 
main(int argc, char **argv)
{
     // Read in argc
     int i;
     int num_servers;
     int portno;
     char *backend_servers[MAX_SERVERS];
     int backend_portno[MAX_SERVERS];

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
          backend_servers[i] = argv[i * 2 + 2];
          backend_portno[i] = atoi(argv[i * 2 + 3]);
     }
     
     // Init thread pool


     
     // Start server procedure





     // Gracefully shutting down


     return 0;
}
