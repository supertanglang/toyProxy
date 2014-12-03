/*
 * toyProxy - A reverse proxy for server
 *
 * filename: config.c
 * descript: Macros and configurations
 */


#ifndef __CONFIG_H__
#define __CONFIG_H__


#include <stdio.h>


#define DEBUG

const int MAX_SERVERS = 8;
const int MAX_STRLEN = 32;

const int MAX_CONNECT = 1000;
const int THREAD_NUM = 1000;
const int THREADPOOL_SIZE = 2000;




#endif
