
## Structs

```cpp
struct addrinfo {
	int              ai_flags;
	int              ai_family;
	int              ai_socktype;
	int              ai_protocol;
	socklen_t        ai_addrlen;
	struct sockaddr *ai_addr;
	char            *ai_canonname;
	struct addrinfo *ai_next;
};
```
Used by getaddrinfo().

```cpp
struct sockaddr_in {
	sa_family_t     sin_family;     AF_INET
	in_port_t       sin_port;       Port number
	struct in_addr  sin_addr;       IPv4 address
};
```
IPv4 socket address structure.

## Functions

### getaddrinfo()

Prepares address information.

Man page: [getaddrinfo](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)

### socket()

Creates a socket and returns a socket fd.

Man page: [socket](https://man7.org/linux/man-pages/man2/socket.2.html)

### freeaddrinfo()

Frees the address list from getaddrinfo().

Man page: [freeaddrinfo](https://man7.org/linux/man-pages/man3/freeaddrinfo.3p.html)

### bind()

Assigns a local address and port to a socket.

Man page: [bind](https://man7.org/linux/man-pages/man2/bind.2.html)

### listen()

Marks a bound socket as a listening socket.

Man page: [listen](https://man7.org/linux/man-pages/man2/listen.2.html)

### accept()

Accepts a pending connection from a listening socket and fills client's address.

Man page: [accept](https://man7.org/linux/man-pages/man2/accept.2.html)
