// TODO Audit these.
#include <arpa/inet.h>
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

int get_socket(char*, int);
void get_file(char*);
void put_file(char*);
void list_dir(cmd_t*);
void* thread_get(void*);
void* thread_put(void*);
void print_help(char*);
void build_command(char*, cmd_t*);

static unsigned short is_verbose = 0;
static unsigned sleep_flag = 0;
static char ip_addr[MAX_ADR_LEN] = DEFAULT_SERV_IP;
static short ip_port = DEFAULT_SERV_PORT;

int main(int argc, char* argv[]) {
	cmd_t cmd;
	int ret = 0;
	pthread_t* threads = NULL;
	pthread_attr_t attr;
	char message_input[MSG_LEN] = {'\0'};

	// Setup the attributes structure to allow starting detached threads.
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	memset(&cmd, 0, sizeof(cmd_t));

	while((ret = getopt(argc, argv, CLIENT_OPTIONS)) != -1) {
		switch(ret) {
		case 'i':
			if(strlen(optarg) > MAX_ADR_LEN -1) {
				fprintf(stderr, "%s isn't an IP address. \n", optarg);
				exit(EXIT_FAILURE);
			}
			strcpy(ip_addr, optarg);
			ip_addr[strlen(optarg)] = '\0';
			break;
		case 'p':
			ip_port = (short) atoi(optarg);
			if(ip_port == 0) {
				fprintf(stderr, "atoi: \"%s\" not set", optarg);
			}
			break;

		/* case 'c': */
		/* 	strncpy(cmd.cmd, optarg, CMD_LEN - 1); */
		/* 	cmd.cmd[strlen(cmd.cmd)] = '\0'; */
		/* 	if((strncmp(cmd.cmd, CMD_PUT, strlen(CMD_PUT)) != 0) && */
		/* 			(strncmp(cmd.cmd, CMD_GET, strlen(CMD_GET)) != 0) && */
		/* 			(strncmp(cmd.cmd, CMD_DIR, strlen(CMD_DIR)) != 0)) { */
		/* 		fprintf(stderr, "%s is an invalid command.\n", cmd.cmd); */
		/* 		exit(EXIT_FAILURE); */
		/* 	} */
		/* 	break; */

		case 'v':
			is_verbose++;
			break;
		/* case 'u': */
		/* 	sleep_flag += 1000; */
		/* 	break; */

		case 'h':
			print_help(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		default:
			fprintf(stderr, "Parsing failure for <%s>.\n", argv[0]);
			break;
		}
	}

	ret = 0;
	for( ; ; ) {
		/* pthread_t tid; // This gets passed to `pthread_create()` calls. */
		if(is_verbose) fprintf(stderr, "Command to server: <%s> %d\n", cmd.cmd, __LINE__);
		fputs("> ", stdout);
		if(fgets(message_input, sizeof(message_input), stdin) == NULL) {
			fprintf(stderr, "Couldn't read input.");
			break;
		}
		message_input[strlen(message_input) - 1] = '\0';
		build_command(&(message_input[0]), &cmd);
		LOG;
		if(strlen(message_input) == 0) {
			continue;
		/* } else if(strcmp(cmd.cmd, CMD_GET) == 0) { */
			// Process the files left on the command line, creating a thread for
			// each file to connect to the server
			/* for(i = optind; i < argc; i++) { */
				/* pthread_create(&threads[i], &attr, thread_get, argv[i]); */
			/* } */
		/* } else if(strcmp(cmd.cmd, CMD_PUT) == 0) { */
			/* for(i = optind; i < argc; i++) { */
				/* pthread_create(&threads[i], &attr, thread_put, argv[i]); */
			/* } */
		} else if(strcmp(cmd.cmd, CMD_DIR) == 0) {
			LOG;
			list_dir(&cmd);
		} else if(strcmp(cmd.cmd, CMD_QUIT) == 0) {
			fprintf(stdout, "Quitting....\n");
			break;
		} else {
			fprintf(stderr, "ERROR: unknown command >%s< %d\n", cmd.cmd, __LINE__);
			exit(EXIT_FAILURE);
		}
		memset(message_input, 0, sizeof(message_input));
	}

	pthread_attr_destroy(&attr);
    pthread_exit(NULL);
	free(threads);
}

// Setup and return an open (that is, connected) socket.
int get_socket(char* addr, int port) {
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port); // Host to network short
	inet_pton(AF_INET, addr, &servaddr.sin_addr.s_addr); // Convert *p*resentation to
														 // *n*etwork, storing in the
														 // `servaddr` struct.
	if(connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}
    return(sockfd);
}

void* thread_get(void* info) {
    char* file_name = (char* ) info;
    get_file(file_name);
    pthread_exit(NULL);
}

// Get one file.
void get_file(char* file_name) {
    cmd_t cmd;
    int sockfd = 0;
    int fd = 0;
    ssize_t bytes_read = 0;
    char buffer[MAXLINE] = {'\0'};
	memset(&cmd, 0, sizeof(cmd_t));
    strncpy(cmd.cmd, CMD_GET, CMD_LEN);
	strncpy(cmd.name, file_name, NAME_LEN);
	if(is_verbose > 2) {
		fprintf(stderr, "%d: %d, %d, %ld, %s\n", __LINE__, sockfd, fd, bytes_read, buffer);
		fprintf(stderr, "%s %s %s\n", cmd.cmd, cmd.name, file_name);
	}
    if(is_verbose) {
		fprintf(stderr, "%d: next file: <%s>\n", __LINE__, file_name);
        fprintf(stderr, "%d: get from server: %s %s\n", __LINE__, cmd.cmd, cmd.name);
	}
	// Setup the socket.
	sockfd = get_socket(ip_addr, ip_port);
    // Open the file to write.
	fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	// Send the command.
	write(sockfd, &cmd, sizeof(cmd));
	// Read, but watch out for errors.  Note that the dread "failure 281"
	// happens if I don't insert enough microsecond delays with `-u`.
	while((bytes_read = read(sockfd, buffer, sizeof(buffer))) != 0) {
		if(bytes_read == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		if(write(fd, buffer, bytes_read) == -1) {
			perror("write");
		}
		usleep(sleep_flag);
	}
	close(fd);
	close(sockfd);
}

void* thread_put(void* info) {
    char* file_name = (char*) info;
	put_file(file_name);
    pthread_exit(NULL);
}

void put_file(char* file_name) {
    cmd_t cmd;
    int sockfd = 0;
    int fd = 0;
	ssize_t bytes_read = 0;
	ssize_t bytes_written = 0;
	ssize_t total_bytes_written = 0;
    char buffer[MAXLINE] = {'\0'};
	memset(&cmd, 0, sizeof(cmd_t));
	strncpy(cmd.cmd, CMD_PUT, CMD_LEN);
	strncpy(cmd.name, file_name, NAME_LEN);
	if(is_verbose > 2) {
		fprintf(stderr, "%d: %d, %d, %ld, %s\n", __LINE__, sockfd, fd, bytes_read, buffer);
		fprintf(stderr, "%s %s %s\n", cmd.cmd, cmd.name, file_name);
	}
    if(is_verbose) {
        fprintf(stderr, "%d: next file: <%s>\n", __LINE__, file_name);
		fprintf(stderr, "%d: put to server: %s %s\n", __LINE__, cmd.cmd, cmd.name);
	}
	sockfd = get_socket(ip_addr, ip_port);
	fd = open(file_name, O_RDONLY);
	if(fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}
    write(sockfd, &cmd, sizeof(cmd));
	while((bytes_read = read(fd, buffer, sizeof(buffer))) != 0) {
		if(bytes_read == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		if((bytes_written = write(sockfd, buffer, bytes_read)) == -1) {
			perror("write");
		} else {
			total_bytes_written += bytes_written;
			if(is_verbose > 2) {
				fprintf(stderr, "total_bytes_written: %ld\n", total_bytes_written);
			}
		}
		usleep(sleep_flag);
	}
	close(fd);
	close(sockfd);
}

void list_dir(cmd_t* cmd_p) {
    int sockfd = 0;
    ssize_t bytes_read = 0;
    char buffer[MAXLINE] = {'\0'};
	if(is_verbose > 2) fprintf(stderr, "%d: %d, %ld, %s\n", __LINE__, sockfd, bytes_read, buffer);
    sockfd = get_socket(ip_addr, ip_port);
    printf("dir from server: %s \n", cmd_p->cmd);
	write(sockfd, cmd_p, sizeof(*cmd_p) - 1);
	while((bytes_read = read(sockfd, buffer, sizeof(buffer))) != 0) {
		if(bytes_read == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		printf("%s", buffer);
		memset(buffer, 0, sizeof(buffer));
	}
	close(sockfd);
}

void print_help(char* prog_name) {
	printf("\n%s -[%s]\n", prog_name, CLIENT_OPTIONS);
	printf("\t-i <address>\tSets the server's IP address.\n");
	printf("\t-p <port>\tSets the server's port.\n");
	printf("\t-h\t\tDisplays this very message.\n\n");
	printf("\t-v\t\tIncrease verbosity for each occurrence.\n\n");
}

void build_command(char* message_input, cmd_t* cmd) {
	char* tok = NULL;
	tok = strtok(message_input, " ");
	LOG;
	if(tok[0] == ':') {
		LOG;
		strcpy(cmd->pre, tok);
		tok = strtok(NULL, " ");
		if(tok) strcpy(cmd->cmd, tok);
	} else {
		LOG;
		strcpy(cmd->cmd, tok);
	}
	LOG;
	tok = strtok(NULL, " ");
	if(tok) strcpy(cmd->msg, tok);
}
