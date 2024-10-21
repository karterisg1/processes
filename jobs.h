#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define FIFO1 "./fifo1"
#define FIFO2 "./fifo2"
#define server_file "jobExecutorServer.txt"

extern int LIMIT;
extern int N;

void eerror(char *);
void issuejob(char *, int, int);
void server();
void client(char **);
void exit_op();
void tokenize(char *, char **);
void handler(int);
void sendv(char **);
void send(char *);
void update();