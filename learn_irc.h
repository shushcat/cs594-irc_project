#ifndef LEARN_IRC_H
#define LEARN_IRC_H

#define DEFAULT_SERV_IP "127.0.0.1"
#define DEFAULT_SERV_PORT 6660
#define CMD_LEN 7
#define NAME_LEN 75
#define MAX_ADR_LEN 50
#define MAXLINE 1024
#define MSG_LEN 512

typedef struct cmd_s {
	char pre[MSG_LEN];
    char cmd[MSG_LEN];
	char msg[MSG_LEN];
    char name[NAME_LEN];
    int sock;
} cmd_t;

#define INIT_CMD {NULL, NULL, NULL, {'\0'}, 0};

#define LOG if(is_verbose>2) fprintf(stderr, "%s %s %d\n", \
		__FILE__, __func__, __LINE__);

// IRC commands:
#define CMD_GET "get"
#define CMD_PUT "put"
#define CMD_DIR "dir"

#define CMD_QUIT "/quit"

#define CLIENT_OPTIONS "i:p:hv"

#define SERVER_OPTIONS "p:uvh"

#define CMD_DIR_POPEN "ls -lFABhS --si --group-directories-first"

#define SERVER_CMD_COUNT "count"
#define SERVER_CMD_VPLUS "v+"
#define SERVER_CMD_VMINUS "v-"
#define SERVER_CMD_HELP "help"
#define SERVER_CMD_EXIT "exit"

#define CLIENT_CMD_EXIT "exit"

#endif
