/*
 * toyProxy - A reverse proxy for server
 *
 * filename: network.h
 * descript: Define network interface for send, recv
 */


#include "network.h"

#include <string.h>


// bind socket to portno
// param sockfd - the listening socket fd
// param portno - the portno
// return 0 on success, -1 on any error
int sock_bind_listen(int sockfd, int portno, int backlog)
{
    struct sockaddr_in serv_addr;
    bzero((char *)&serv_addr, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof serv_addr) < 0) {
        return -1;
    }

    if (listen(sockfd, backlog) < 0) {
        return -1;
    }
    return 0;
}


// accept new connections
// param sockfd - the sever socket to accept
// return client fd on success, -1 on any error
int sock_accept(int sockfd) 
{
    int client_sockfd;
    struct sockaddr_in client_addr;
    socklen_t client_len;
     
    client_len = sizeof client_addr;
    if ((client_sockfd = accept(sockfd,
                                (struct sockaddr *)&client_sockfd,
                                &client_len)) < 0) {
        return -1;
    }
     
    return client_sockfd;
}


// nonblocking accept new connections
// param sockfd - the sever socket to accept
// return client fd on success, -1 on any error
int sock_nonblocking_accept(int sockfd)
{
    // TODO: make accept non-blocking
     
    return -1;
}


// connect to dest server
// param sockfd - the server socket to connect to
// param dest_addr - the dest addr
// param portno - the dest addr portno
// return 0 on success, -1 on any error
int sock_connect(char *dest_addr, int portno)
{
    int sockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;

    if ((server = gethostbyname(dest_addr)) == NULL) {
        return -1;
    }
     
    bzero((char *)&serv_addr, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
     
    if ((sockfd = socket(AF_INET,
                         SOCK_STREAM,
                         0)) < 0) {
        return -1;
    }

    if (connect(sockfd,
                (struct sockaddr *)&serv_addr,
                sizeof serv_addr) < 0) {
        return -1;
    }

    return sockfd;
}


// send http to socket
// param sockfd - the sockfd to send to
// param buffer - the buffer to send
// param size - the buffer size to send
// return bytes sent on success, -1 on any error
int send_buffer(int sockfd, char *buffer, int size)
{
    int allsent = 0;
    int sent;

    while (allsent < size) {
        sent = send(sockfd, buffer, size, 0);
        if (sent <= 0) {
            return -1;
        }
        allsent += sent;
    }
    return allsent;
}


// send http to socket
// param sockfd - the sockfd to send to
// param buffer - the buffer to send
// return the bytes recved on success, -1 on any error
int recv_buffer(int sockfd, char *buffer)
{
    int allrecved = 0;
    int recved;
    int pointer;
    
    memset(buffer, 0, NET_BUFFER);

    while (1) {
        recved = recv(sockfd, buffer + allrecved, 
                      NET_BUFFER - allrecved, 0);

        allrecved += recved;
        
        if (recved < 0) {
            return -1;
        }
        if (allrecved == NET_BUFFER) {
            break;
        }
        if (recved == 0) {
            break;
        }
        if (strstr(buffer, "\r\n\r\n") ||
            strstr(buffer, "\n\n") ||
            strstr(buffer, "\r\r")) {
            break;
        }
    }
    
    return allrecved;
}


// simple http parser, find the return size of the 
// received buffer
// param: buffer - the buffer of the received info
// return: the total http response size, 0 if not found, -1 if error
/* int find_http_size(char *buffer) { */
/*     char *index; */
/*     char num_buffer[8]; */
/*     char c, next; */

/*     index = strstr(buffer, "Content-Length:"); */

/*     if (index == NULL) { */
/*         return 0; */
/*     } */
    
    


/* } */
