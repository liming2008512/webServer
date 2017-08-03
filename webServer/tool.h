
#ifndef TOOL_H_
#define TOOL_H_

#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cassert>
#include <string.h>

int setnonblocking( int fd );

void addfd( int epollfd, int fd, bool one_shot );

void removefd( int epollfd, int fd );

void modfd( int epollfd, int fd, int ev );

void addsig( int sig, void( handler )(int));

void show_error( int connfd, const char* info );

#endif /* TOOL_H_ */
