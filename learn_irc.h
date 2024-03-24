#ifndef LEARN_IRC_H
#define LEARN_IRC_H

#define DEFAULT_SERV_IP "127.0.0.1"
#define DEFAULT_SERV_PORT 6660
#define CMD_LEN 7
#define NAME_LEN 75
#define MAX_ADR_LEN 50
#define MAXLINE 1024
#define MAXMSG 512

typedef struct cmd_s {
    char cmd[CMD_LEN];
    char name[NAME_LEN];
    int sock;
} cmd_t;

// TODO Combine with `cmd_t`?
typedef struct msg_s {
	char pref[MAXMSG];
	char cmd[MAXMSG];
	char args[MAXMSG];
} msg_t;

// IRC commands:
#define CMD_GET "get"
#define CMD_PUT "put"
#define CMD_DIR "dir"

#define CLIENT_OPTIONS "i:p:h"

#define SERVER_OPTIONS "p:uvh"

#define CMD_DIR_POPEN "ls -lFABhS --si --group-directories-first"

#define SERVER_CMD_COUNT "count"
#define SERVER_CMD_VPLUS "v+"
#define SERVER_CMD_VMINUS "v-"
#define SERVER_CMD_HELP "help"
#define SERVER_CMD_EXIT "exit"

#endif
