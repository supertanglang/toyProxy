/*
 * cs257 Assignment 4
 * Implement query flooding protocol
 *
 * filename: network.h
 * descript: Define network interface for send, recv
 */


#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define NET_BUFFER 512


// bind socket to portno
// param sockfd - the listening socket fd
// param portno - the portno
// return 0 on success, -1 on any error
int sock_bind_listen(int sockfd, int portno, int backlog);

// accept new connections
// param sockfd - the sever socket to accept
// return client fd on success, -1 on any error
int sock_accept(int sockfd);

// nonblocking accept new connections
// param sockfd - the sever socket to accept
// return client fd on success, -1 on any error
int sock_nonblocking_accept(int sockfd);

// connect to dest server
// param dest_addr - the dest addr
// param portno - the dest addr portno
// return server fd on success, -1 on any error
int sock_connect(char *dest_addr, int portno);

// send http to socket
// param sockfd - the sockfd to send to 
// param buffer - the buffer to send
// param size - the buffer size to send
// return 0 on success, -1 on any error
int send_buffer(int sockfd, char *buffer, int size);

// send http to socket
// param sockfd - the sockfd to send to 
// param buffer - the buffer to send
// return the bytes recved on success, -1 on any error
int recv_buffer(int sockfd, char *buffer);


#endif
