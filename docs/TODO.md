# TODO Lit

IRC documentation
https://modern.ircdocs.horse/
https://beej.us/guide/bgnet/

## Server and I/O

- [ ] Accept multiple simultaneous clients without hanging.
- [ ] Use TCP/IP (IPv4 or IPv6) for client-server communication.
- [ ] Make every I/O operation non-blocking.
- [ ] Use one `poll()` (or equivalent) event loop for listening, reading, and writing.
- [ ] Handle partial reads/writes, disconnects, and malformed input safely.

## Client registration and messaging

- [ ] Authenticate clients with the server password.
- [ ] Implement nickname registration (`NICK`).
- [ ] Implement username registration (`USER`).
- [ ] Ensure the selected reference client can connect without errors.
- [ ] Support joining channels (`JOIN`).
- [ ] Support private messages between clients (`PRIVMSG`).
- [ ] Broadcast channel messages to every client currently in that channel.

## Users and channels

- [ ] Distinguish channel operators from regular users.
- [ ] Track channel membership, invitations, topic, key, and user limit.
- [ ] Enforce channel permissions and access restrictions.

## Channel operator commands

- [ ] `KICK`: eject a client from a channel.
- [ ] `INVITE`: invite a client to a channel.
- [ ] `TOPIC`: view or change a channel topic.
- [ ] `MODE`: manage the following channel modes:
  - [ ] `i`: invite-only channel.
  - [ ] `t`: restrict topic changes to channel operators.
  - [ ] `k`: set or remove the channel key (password).
  - [ ] `o`: grant or revoke channel-operator privileges.
  - [ ] `l`: set or remove the user limit.

## Testing

- [ ] Test registration, private messages, and channel broadcasts with the reference client.
- [ ] Test each operator command and mode, including permission failures.
- [ ] Test concurrent clients, disconnects, and partial network input.
- [ ] Build with the required flags and verify no warnings or errors.


## Allowed functions:
socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept,
htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa,
inet_ntop, send, recv, signal, sigaction,
sigemptyset, sigfillset, sigaddset, sigdelset,
sigismember, lseek, fstat, fcntl, poll (or
equivalent)

## Requirement

The server must be capable of handling multiple clients simultaneously without
hanging.
•Forking is prohibited. All I/O operations must be non-blocking.
•Only 1 poll() (or equivalent) can be used for handling all these operations (read,
write, but also listen, and so forth)
Several IRC clients exist. You have to choose one of them as a reference. Your
reference client will be used during the evaluation process.
•Your reference client must be able to connect to your server without encountering
any error.
•Communication between client and server has to be done via TCP/IP (v4 or v6).
•Using your reference client with your server must be similar to using it with any
official IRC server. However, you only have to implement the following features:
◦You must be able to authenticate, set a nickname, a username, join a channel,
send and receive private messages using your reference client.
◦All the messages sent from one client to a channel have to be forwarded to
every other client that joined the channel.
◦You must have operators and regular users.
◦Then, you have to implement the commands that are specific to channel
operators:
∗ KICK - Eject a client from the channel
∗ INVITE - Invite a client to a channel
∗ TOPIC - Change or view the channel topic
∗ MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
o: Give/take channel operator privilege
· l: Set/remove the user limit to channel
•Of course, you are expected to write a clean code.