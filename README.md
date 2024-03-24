- [ ] List needed functionality
- [ ] Remove port scan---use those assigned for IRC

# Plan

- [x] RFC added to repository
- [x] Setup client & server skeletons
- [x] Skim RFC
- [x] Basic diagram
- [x] Server process
	- [x] Setup port in IRC range, 6660--7000
- [x] Client command process.
- [ ] Client can connect to a server
- [ ] Associate commands with below functionality
- [ ] Client can create a room
- [ ] Client can list all rooms
- [ ] Client can join a room
- [ ] Client can leave a room
- [ ] Client can list members of a room
- [ ] Multiple clients can connect to a server
- [ ] Client can send messages to a room
- [ ] Client can join multiple (selected) rooms
- [ ] Client can send distinct messages to multiple (selected) rooms
- [ ] Client can disconnect from a server
- [ ] Server can disconnect from clients
- [ ] Server can gracefully handle client crashes
- [ ] Client can gracefully handle server crashes
- [ ] Programming style
- [ ] Private or Ephemeral Messaging
- [ ] Secure messaging
- [ ] File transfer
	- DCC transfers?
- [ ] Cloud connected server

## Questions

How would these three work?

- [ ] Private or Ephemeral Messaging
- [ ] Secure messaging
- [ ] Cloud connected server

# TODO Final checks

- [ ] Crawl through those `valgrind` gates
	- Detached threads are reported as possibly leaky.

# RFC

client
	real name :client host
	username :client host
	?operator
	len(nick) <= 9

channels
	len(name) <= 200
	name: ~{ \s Ctrl-G ,}
	name[0] :: '&' | '#'
		'#': local join
		'&': ____

- messages:
	- [x] len(msg) <= 512
	- ?timeout
<prefix> command parameter1 <parameter2, ... , parameter15>
	prefix:
		`:prefix`
		~prefix: origin::sender
		(send(self) :client): ~prefix
		~prefix(server)|prefix(other link): ignore
	command:
		IRC command | 3 digit ASCII number

(message: server): numeric reply :: sender, 3 digits, target

