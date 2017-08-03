
#ifndef TOOL_H_
#define TOOL_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int setnonblocking( int fd );

void addfd( int epoll_fd, int fd );

bool write_nbytes( int sockfd, const char* buffer, int len );

bool read_once( int sockfd, char* buffer, int len );

void start_conn( int epoll_fd, int num, const char* ip, int port );

void close_conn( int epoll_fd, int sockfd );

#endif /* TOOL_H_ */
