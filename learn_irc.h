#ifndef LEARN_IRC_H
#define LEARN_IRC_H

#define DEFAULT_SERV_IP "127.0.0.1"
#define DEFAULT_SERV_PORT 10001
#define CMD_LEN 7
#define NAME_LEN 75
#define MAX_ADR_LEN 50
#define MAXLINE 1024

typedef struct cmd_s {
    char cmd[CMD_LEN];
    char name[NAME_LEN];  // TODO Change to `msg`
    int sock;
} cmd_t;

// IRC commands:
#define CMD_GET "get"
#define CMD_PUT "put"
#define CMD_DIR "dir"

#define CLIENT_OPTIONS "i:p:c:uvh"

#define SERVER_OPTIONS "p:uvh"

#define CMD_DIR_POPEN "ls -lFABhS --si --group-directories-first"

#define SERVER_CMD_COUNT "count"
#define SERVER_CMD_VPLUS "v+"
#define SERVER_CMD_VMINUS "v-"
#define SERVER_CMD_HELP "help"
#define SERVER_CMD_EXIT "exit"

#endif
