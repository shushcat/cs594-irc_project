#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "learn_irc.h"

#define LISTENQ 100

void process_connection(int sockfd, void* buf, int n);
void* thread_get(void* p);
void* thread_put(void* p);
void* thread_dir(void* p);
void* server_commands(void*);
void current_connections_inc(void);
void current_connections_dec(void);
unsigned int current_connections_get(void);
void server_help(void);

static short is_verbose = 0;
static unsigned sleep_flag = 0;
static unsigned long tcount = 0;
static unsigned int current_connections = 0;
static pthread_mutex_t connections_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    int listenfd = 0;
    int sockfd = 0;
    int n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr;
    struct sockaddr_in servaddr;
    short ip_port = DEFAULT_SERV_PORT;
    int opt;
    pthread_t cmd_thread;

    while((opt = getopt(argc, argv, SERVER_OPTIONS)) != -1) {
        switch(opt) {
        case 'p':
			ip_port = (short) atoi(optarg);
			if(ip_port == 0) {
				fprintf(stderr, "atoi: \"%s\" just isn't a very good port number.\n", optarg);
			}
            break;
        case 'u':
			sleep_flag += 1000;
            break;
        case 'v':
            is_verbose++;
            break;
        case 'h':
            fprintf(stderr, "%s ...\n\tOptions: %s\n", argv[0], SERVER_OPTIONS);
            fprintf(stderr, "\t-p #\t\tport on which the server will listen (default %hd)\n",
					DEFAULT_SERV_PORT);
            fprintf(stderr, "\t-u\t\tnumber of thousands of microseconds the server will sleep between "
                    "read/write calls (default %d)\n", sleep_flag);
            fprintf(stderr, "\t-v\t\tenable verbose output. "
					"Can occur more than once to increase output\n");
            fprintf(stderr, "\t-h\t\tshow this rather lame help message\n");
            exit(EXIT_SUCCESS);
            break;
        default:
            fprintf(stderr, "*** Oops, something strange happened <%s> ***\n", argv[0]);
            break;
        }
    }

    // Create a socket from the AF_INET family---that is, a stream socket.
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Host to "network long".
	servaddr.sin_port = htons(ip_port); // Host to "network short".

	// If the passed port doesn't work, try to connect to one in the usual IRC
	// range, 6660--7000.
	while(bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
		if(errno == EADDRINUSE) {
			if(is_verbose) {
				fprintf(stderr, "Port %d is already in use.\n", ip_port);
			}
			ip_port++;
			servaddr.sin_port = htons(ip_port);
			usleep(1000);
		} else if((errno != EADDRINUSE) || (ip_port > 7000)) {
			perror("bind");
			exit(EXIT_FAILURE);
		}
	}

    // Commence to listening.
	listen(listenfd, LISTENQ);

    {
        char hostname[256];

		// Get the hostname tersely, but perhaps horribly.
		char ipbuf[MAXLINE] = {'\0'};
		char* fp = NULL;
		fp = (char*) popen("hostname -I", "r");
		fgets(ipbuf, sizeof(ipbuf), (FILE*) fp);
		pclose((FILE*) fp);
		fp = strchr(ipbuf, ' ');
		ipbuf[strlen(ipbuf) - strlen(fp)] = '\0';

        memset(hostname, 0, sizeof(hostname));
		if(gethostname(hostname, sizeof(hostname)) == -1) perror("hostname");
        
        fprintf(stdout, "Hostname: %s\n", hostname);
        fprintf(stdout, "IP:       %s\n", ipbuf);
        fprintf(stdout, "Port:     %d\n", ip_port);
    }

    // Create the input handler thread.
	if(pthread_create(&cmd_thread, NULL, server_commands, NULL) != 0) {
		perror("pthread_create");
	}

    // Client length.
    clilen = sizeof(cliaddr);
	if(is_verbose > 2) fprintf(stderr, "clilen: %d\n", clilen);

    for( ; ; ) {
		// Accept connections on the listening file descriptor; close as `cmd->sock`.
		if((sockfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen)) == -1) {
			perror("accept");
			continue;
		}
        memset(buf, 0, sizeof(buf));
        if((n = read(sockfd, buf, sizeof(buf))) == 0) {
            fprintf(stdout, "EOF found on client connection socket, "
                    "closing connection.\n");
			close(sockfd);
			break;
        } else {
            if(is_verbose) {
				fprintf(stdout, "Connection from client: <%s>\n", buf);
            }
			process_connection(sockfd, buf, n);
        }
    }
    printf("Closing listen socket\n");
    close(listenfd);
    pthread_exit(NULL);
    return(EXIT_SUCCESS);
}

void process_connection(int sockfd, void* buf, int n) {
    cmd_t* cmd = (cmd_t*) malloc(sizeof(cmd_t)); // Deallocate in threads.
    int ret = 0;
    pthread_t tid;
    pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    memcpy(cmd, buf, n);
    cmd->sock = sockfd;
    if(is_verbose) {
		fprintf(stderr, "Request from client: <%s> <%s>\n",
				cmd->cmd, cmd->name);
    }


    // Lock, then tally a connection.
	pthread_mutex_lock(&connections_mutex);
	tcount++;
	pthread_mutex_unlock(&connections_mutex);

    if(strcmp(cmd->cmd, CMD_GET) == 0) {
		pthread_create(&tid, &attr, thread_get, cmd); // Handle `-c get`.
        if(ret < 0) {
            fprintf(stderr, "ERROR: %d\n", __LINE__);
        }
    } else if(strcmp(cmd->cmd, CMD_PUT) == 0) {
		pthread_create(&tid, &attr, thread_put, cmd); // Handle `-c put`.
        if(ret < 0) {
            fprintf(stderr, "ERROR: %d\n", __LINE__);
        }
    } else if(strcmp(cmd->cmd, CMD_DIR) == 0) {
		pthread_create(&tid, &attr, thread_dir, cmd); // Handle `-c dir`.
        if(ret < 0) {
            fprintf(stderr, "ERROR: %d\n", __LINE__);
        }
    } else { // Shouldn't happen.
        fprintf(stderr, "ERROR: unknown command >%s< %d\n", cmd->cmd, __LINE__);
		close(sockfd);
    }
}

void* server_commands(void*) {
    char scmd[80] = {'\0'};
    char* ret_val;
	pthread_detach(pthread_self());
    server_help();
    for( ; ; ) {
        fputs(">> ", stdout);
        ret_val = fgets(scmd, sizeof(scmd), stdin);
        if(ret_val == NULL) {
			printf("DANGIT");
			break;
        }
		scmd[strlen(scmd) - 1] = '\0'; // Pleased to stomp.
        if(strlen(scmd) == 0) {
			continue;
        } else if(strcmp(scmd, SERVER_CMD_EXIT) == 0) {
			exit(EXIT_SUCCESS); // TODO Something better than this.
            break;
        } else if(strcmp(scmd, SERVER_CMD_COUNT) == 0) {
            printf("total connections   %lu\n", tcount);
            printf("current connections %u\n", current_connections_get());
            printf("verbose             %d\n", is_verbose);
        } else if(strcmp(scmd, SERVER_CMD_VPLUS) == 0) {
            is_verbose++;
            printf("verbose set to %d\n", is_verbose);
        } else if(strcmp(scmd, SERVER_CMD_VMINUS) == 0) {
            is_verbose--;
            if(is_verbose < 0) {
                is_verbose = 0;
            }
            printf("verbose set to %d\n", is_verbose);
        } else if(strcmp(scmd, SERVER_CMD_HELP) == 0) {
            server_help();
        } else {
            printf("command not recognized >>%s<<\n", scmd);
        }
    }
    exit(EXIT_SUCCESS); // TODO A better way.
}

void server_help(void) {
    printf("available commands are:\n");
    printf("\t%s : show cumulative and current connection counts\n", SERVER_CMD_COUNT);
    printf("\t%s    : increment the is_verbose flag (current %d)\n",
			SERVER_CMD_VPLUS, is_verbose);
    printf("\t%s    : decrement the is_verbose flag (current %d)\n",
			SERVER_CMD_VMINUS, is_verbose);
    printf("\t%s  : exit the server process\n", SERVER_CMD_EXIT);
    printf("\t%s  : show this help\n", SERVER_CMD_HELP);
}

// `get` from server, so I need to *send* data to the client.
void* thread_get(void* p) {
    cmd_t* cmd = (cmd_t*) p;
    int fd = 0;
    ssize_t bytes_read = 0;
    char buffer[MAXLINE];
    current_connections_inc();
    if(is_verbose) fprintf(stderr, "Sending %s to client\n", cmd->name);
	fd = open(cmd->name, O_RDONLY);
    if(fd < 0) {
		perror("open");
		close(fd);
		close(cmd->sock);
		free(cmd);
        pthread_exit((void *) EXIT_FAILURE);
    }
	memset(buffer, 0, sizeof(buffer));
	while((bytes_read = read(fd, buffer, sizeof(buffer))) != 0) {
		if(is_verbose > 2) fprintf(stderr, "bytes_read: %ld\n", bytes_read);
		if(write(cmd->sock, buffer, bytes_read) != bytes_read) {
			perror("write");
		}
		usleep(sleep_flag);
	}
	close(fd);
	close(cmd->sock);
	free(cmd);
    current_connections_dec();
    pthread_exit((void*) EXIT_SUCCESS);
}

void* thread_put(void* cmd_p) {
    cmd_t* cmd = (cmd_t* ) cmd_p;
    int fd = 0;
    ssize_t bytes_read = 0;
    char buffer[MAXLINE];
    current_connections_inc();
    if(is_verbose) fprintf(stderr, "Receiving %s from client\n", cmd->name);
	fd = open(cmd->name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd < 0) {
		perror("open");
		close(fd);
		close(cmd->sock);
		free(cmd);
        pthread_exit((void*) EXIT_FAILURE);
    }
	memset(buffer, 0, sizeof(buffer));
	while((bytes_read = read(cmd->sock, buffer, sizeof(buffer))) != 0) {
		if(is_verbose > 2) fprintf(stderr, "bytes_read: %ld\n", bytes_read);
		if(write(fd, buffer, bytes_read) != bytes_read) {
			perror("write");
		}
		usleep(sleep_flag);
	}
	close(fd);
	close(cmd->sock);
	free(cmd);
    current_connections_dec();
    pthread_exit((void*) EXIT_SUCCESS);
}

void* thread_dir(void* cmd_p) {
    cmd_t* cmd = (cmd_t*) cmd_p;
    FILE* fp = NULL;
    char buffer[MAXLINE];
	if(is_verbose > 2) fprintf(stderr, "%s\n", cmd->name);
    current_connections_inc();
	fp = popen(CMD_DIR_POPEN, "r");
    if(fp == NULL) {
		fprintf(stderr, "%d: Popening ineffective.\n", __LINE__);
		pclose(fp);
		close(cmd->sock);
		free(cmd);
        pthread_exit((void*) EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof(buffer));
	while(fgets(buffer, sizeof(buffer), fp) != NULL) {			// Read from `popen()`'s file stream
		if(write(cmd->sock, buffer, strlen(buffer)) == -1) {	//   and write to this thread's socket.
			perror("write");
		}
		usleep(sleep_flag);
	}
	pclose(fp);
	close(cmd->sock);
	free(cmd);
    current_connections_dec();
    pthread_exit((void*) EXIT_SUCCESS);
}

void current_connections_inc(void) {
	pthread_mutex_lock(&connections_mutex);
	current_connections++;
	pthread_mutex_unlock(&connections_mutex);
}

void current_connections_dec(void) {
	pthread_mutex_lock(&connections_mutex);
	current_connections--;
	pthread_mutex_unlock(&connections_mutex);
}

unsigned int current_connections_get(void) {
    return current_connections;
}
