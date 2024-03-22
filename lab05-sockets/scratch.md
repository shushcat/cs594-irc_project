Implement two programs, `rockem_server` and `rockem_client`, which can run on different systems, and which behave something like `wget` and the unattested `wput`.

To get started, run `rockem_server`, then start `rockem_client` from a different shell or system.

And as always: plan the work, work the plan.

- [x] Verify reference implementations
	- [x] `-c dir`
	- [x] `-c put`
	- [x] `-c get`
- [x] set `#define LISTENQ 100`

# DONE Implement `rockem_client`

- [x] Read over and tidy
- [x] Comment and stub to compile
	- Use logging to higher verbosity levels to quelch unused variable errors
	- Initialize some variables to `0`---undo later?
- [x] Parse command line arguments with `getopt()`
	- [x] one command per client; the command that appears last on the command line is the one used.
	- [x] `-i address` IPv4 address of the server
	- [x] `-p #` port to connect server
	- [x] `-u` add 1000 microsecond delay to client's sleep after each read or write (`put` or `get`); can occur multiple times
	- [x] `-v` set verbose flag; can occur multiple times
	- [x] `-h` display help & exit
	- [x] `-c` command to send to the server
- [x] Setup `get_socket()`
- [x] `dir` command
	- with function call
	- create socket
	- set socket to IP address
	- connect with `connect()`
- [x] `get` command
	- for each thread
		- [x] create a socket
		- [x] Send the command to the server
	- [x] `get` a single file
	- [x] save in client's current directory
	- [x] multiple files can be listed
	- [x] thread per transfer
	- [x] open&close socket per thread
	- [x] exit when all transferred
	- [x] permissions: `S_IRUSR | S_IWUSR`
- [x] `put` command
	- [x] save in server's current directory
	- [x] create thread for each file
	- [x] each thread:
	- [x] socket per transfer
		- [x] send `put`: server "with a single file"
	- [x] multiple files can be sent
- mode command line info in `rockem_hdr.h`

# TODO Implement `rockem_server`

- [x] Read over and tidy
- [x] Comment and stub to compile
- [x] Parse options with `getopt`
	- [x] `-p #` listen port
	- [x] `-u` each occurrance adds a 1000 microsecond delay to server sleeps after reads and writes
		- see `usleep()`
	- [x] `-v` verbose; can occur multiple times
	- [x] `-h` help message & exit
- [x] Create a new `listenfd` socket
- [x] Bind the `listenfd` socket to the server address
- [x] Set socket to passive with `listen()`
- [x] Console command thread:
	- [x] Read command in console thread?
		- Garbled with client commands?
	- [x] `exit` kill connections "harshly"
	- [x] `count` display
		- [x] total number of connections since startup
		- [x] number of current connections
		- [x] verbosity level
	- [x] `v+` increment verbosity
	- [x] `v-` decrement verbosity
	- [x] `help` print help
	- [ ] Fix prompt marker redisplay
	- [x] Fix host IP display
- [x] Loop while `accept()`ing connections from clients
- [x] Accept connection from client
- [x] Get command from client
- [x] `process_connection()`s
- [x] Pass each socket from an `accept()`ed connection to its own thread.
- [x] `dir` command
	- return directory listing
		- ("`CMD_DIR_POPEN` macro from the `rockem_hdr.h`")
	- `popen()` to do it; see `rockem_hdr.h` for `popen()` options
	- server closes socket when directory sent; client exists when detects socket closed
- [x] `get` fetch listed files from server
	- [x] save in client's current directory
	- [x] multiple files can be listed
	- [x] thread per transfer
	- [x] open&close socket per thread "easier to know when an individual file transfer is complete"
	- [x] exit when all transferred
	- [x] permissions: `S_IRUSR | S_IWUSR`
- [x] `put` "send each of the file names listed on the command line to the server"
	- [x] multiple files can be listed
	- [x] thread per file transfer
	- [x] open&close new socket per transfer
	- [x] exit client when all files transferred
	- [x] permissions: `S_IRUSR | S_IWUSR`
- [ ] Setup a nice exit handler?
	- [ ] `atexit` for frees and such

# TODO Final checks

- [x] Scan for an open port at the beginning
- [x] Tidy everything
- [x] Crawl through those `valgrind` gates
	- Detached threads are reported as possibly leaky.
