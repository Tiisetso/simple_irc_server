# TODO Lit

IRC documentation
https://modern.ircdocs.horse/

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