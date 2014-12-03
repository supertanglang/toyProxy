/*
 * toyProxy - A reverse proxy for server
 *
 * filename: network.h
 * descript: Define network interface for send, recv
 */


#include "network.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


// find hostname binary ip form
// param: hostname 
// return: ipaddr in bin form
unsigned long 
find_host_addr(char *hostname){
     struct hostent *hp;
     struct in_addr **addr_list;
     unsigned long ip_inaddr;

     if ( (hp = gethostbyname(hostname)) == NULL ){
          return 0;
     }
     addr_list = (struct in_addr**)hp->h_addr_list;
     ip_inaddr = (*addr_list[0]).s_addr;

     return ip_inaddr;
}


// find hostname char readable form
// param: host_in_addr - bin form ip
// return: char pointer to ip
char *
find_host_ip(unsigned long host_in_addr){
     struct in_addr in_addr;
     in_addr.s_addr = host_in_addr;

     return inet_ntoa(in_addr);
}


// create new udp socket
// return socket fd
int 
new_udp_sock(){
     int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

     return sockfd;
}


// bind to port
// param: sockfd, portno
// return: 0 on success -1 on error
int 
udp_bind(int sockfd, int portno){

     struct sockaddr_in serv_addr;
     bzero((char *) &serv_addr, sizeof (serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     serv_addr.sin_port = htons(portno);

     if ( bind(sockfd,
               (struct sockaddr *)&serv_addr,
               sizeof (serv_addr)) < 0 ){
          return -1;
     }
     return 0;
}


// recv from a socket, return bin ip addr
// param: sockfd, buffer, size
// return: ip in bin form
unsigned long 
sock_recvfrom(int sockfd, void *buffer, int totalsize){
     int recved;
     int allrecved = 0;

     struct sockaddr_in remoteaddr;
     socklen_t addrlen = sizeof(remoteaddr);

     while (allrecved < totalsize){
          recved = recvfrom(sockfd,
                            buffer + allrecved,
                            totalsize - allrecved,
                            0,
                            (struct sockaddr *)&remoteaddr,
                            &addrlen);
          allrecved += recved;

          printf("[DEBUG] Package recved, size: %d\n", allrecved);
     }

     unsigned long bin_ip = remoteaddr.sin_addr.s_addr;

     return bin_ip;
}


// send to a socket
// param: inaddr, buffer, size
// return: 0 on success -1 on error
int 
sock_sendto(unsigned long host_inaddr, int portno, void *buffer, int size){
     int sockfd;
     struct in_addr in_addr;
     struct hostent *hp;
     struct sockaddr_in servaddr;
     char *ip;

     // get dest addr, create sockfd
     in_addr.s_addr = host_inaddr;
     ip = inet_ntoa(in_addr);
     if ( (sockfd = socket(AF_INET,
                           SOCK_DGRAM,
                           0)) < 0){
          return -1;
     }

     // get target addr
     memset( (char *)&servaddr, 0, sizeof(struct sockaddr_in) );
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(portno);
     if ( (hp = gethostbyname(ip) ) == NULL ){
          return -1;
     }
     memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

     // send the packet
     int totalsent = 0;
     while (totalsent < size){
          int sent = sendto(sockfd,
                            (char *)buffer + totalsent,
                            size - totalsent,
                            0,
                            (struct sockaddr *)&servaddr,
                            sizeof(servaddr));
          totalsent += sent;
     }

     close(sockfd);

     printf ("[DEBUG] Packet sent. size: %d\n", totalsent);

     return 0;
}
