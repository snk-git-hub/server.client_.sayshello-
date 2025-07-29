# Socket Programming Functions Reference
<img width="642" height="310" alt="image" src="https://github.com/user-attachments/assets/419fe052-0c0a-49fd-a9cb-98f19c10c399" />

## Core Socket Functions

### socket()
**Purpose:** Create a socket endpoint for communication
```c
int socket(int domain, int type, int protocol);
```
- **domain:** `AF_INET` (IPv4), `AF_INET6` (IPv6), `AF_UNIX` (local)
- **type:** `SOCK_STREAM` (TCP), `SOCK_DGRAM` (UDP)
- **protocol:** Usually 0 (auto-select)
- **Returns:** Socket file descriptor or -1 on error

**Example:**
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) {
    perror("socket failed");
    exit(1);
}
```

### bind()
**Purpose:** Bind socket to a specific address/port (SERVER SIDE)
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
- **sockfd:** Socket file descriptor
- **addr:** Address structure (cast from sockaddr_in)
- **addrlen:** Size of address structure
- **Returns:** 0 on success, -1 on error

**Example:**
```c
struct sockaddr_in addr = {0};
addr.sin_family = AF_INET;
addr.sin_port = htons(8080);
addr.sin_addr.s_addr = INADDR_ANY;

if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    close(sockfd);
    exit(1);
}
```

### listen()
**Purpose:** Mark socket as passive, ready to accept connections (SERVER SIDE)
```c
int listen(int sockfd, int backlog);
```
- **sockfd:** Socket file descriptor
- **backlog:** Maximum pending connections (use SOMAXCONN)
- **Returns:** 0 on success, -1 on error

**Example:**
```c
if (listen(sockfd, SOMAXCONN) < 0) {
    perror("listen failed");
    close(sockfd);
    exit(1);
}
```

### accept()
**Purpose:** Accept incoming connection (SERVER SIDE)
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- **sockfd:** Listening socket
- **addr:** Client address info (can be NULL)
- **addrlen:** Size of address structure
- **Returns:** New socket for communication or -1 on error

**Example:**
```c
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
if (client_fd < 0) {
    perror("accept failed");
    continue; // Try again
}
```

### connect()
**Purpose:** Connect to remote server (CLIENT SIDE)
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
- **sockfd:** Socket file descriptor
- **addr:** Server address structure
- **addrlen:** Size of address structure
- **Returns:** 0 on success, -1 on error

**Example:**
```c
struct sockaddr_in server_addr = {0};
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect failed");
    close(sockfd);
    exit(1);
}
```

### send() / recv()
**Purpose:** Send/receive data on connected socket
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```
- **sockfd:** Connected socket
- **buf:** Data buffer
- **len:** Buffer size
- **flags:** Usually 0
- **Returns:** Bytes sent/received, 0 for EOF, -1 for error

**Example:**
```c
char buffer[1024];
ssize_t bytes_sent = send(sockfd, "Hello", 5, 0);
if (bytes_sent < 0) {
    perror("send failed");
}

ssize_t bytes_recv = recv(sockfd, buffer, sizeof(buffer)-1, 0);
if (bytes_recv < 0) {
    perror("recv failed");
} else if (bytes_recv == 0) {
    printf("Connection closed by peer\n");
} else {
    buffer[bytes_recv] = '\0';
    printf("Received: %s\n", buffer);
}
```

### read() / write()
**Purpose:** Alternative to recv/send (works on any file descriptor)
```c
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

**Example:**
```c
char buffer[1024];
ssize_t n = read(sockfd, buffer, sizeof(buffer));
if (n < 0) {
    perror("read failed");
} else if (n == 0) {
    printf("EOF\n");
}

ssize_t written = write(sockfd, "Hello", 5);
if (written < 0) {
    perror("write failed");
}
```

### close()
**Purpose:** Close socket
```c
int close(int fd);
```

**Example:**
```c
if (close(sockfd) < 0) {
    perror("close failed");
}
```

## Socket Options

### setsockopt()
**Purpose:** Set socket options
```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

**Common Options:**
```c
// Allow address reuse (important for servers)
int opt = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// Set receive timeout
struct timeval timeout;
timeout.tv_sec = 5;
timeout.tv_usec = 0;
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

// Set send timeout
setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
```

## Address Conversion Functions

### inet_addr()
**Purpose:** Convert IP string to network address
```c
in_addr_t inet_addr(const char *cp);
```

**Example:**
```c
addr.sin_addr.s_addr = inet_addr("192.168.1.1");
if (addr.sin_addr.s_addr == INADDR_NONE) {
    fprintf(stderr, "Invalid IP address\n");
}
```

### inet_ntoa()
**Purpose:** Convert network address to IP string
```c
char *inet_ntoa(struct in_addr in);
```

**Example:**
```c
printf("Client IP: %s\n", inet_ntoa(client_addr.sin_addr));
```

### htons() / ntohs() / htonl() / ntohl()
**Purpose:** Convert between host and network byte order
```c
uint16_t htons(uint16_t hostshort);    // Host to network (short/port)
uint16_t ntohs(uint16_t netshort);     // Network to host (short/port)
uint32_t htonl(uint32_t hostlong);     // Host to network (long/address)
uint32_t ntohl(uint32_t netlong);      // Network to host (long/address)
```

**Example:**
```c
addr.sin_port = htons(8080);           // Convert port to network order
addr.sin_addr.s_addr = htonl(INADDR_ANY); // Convert address to network order

uint16_t port = ntohs(addr.sin_port);  // Convert back to host order
```

## Error Handling Functions

### perror()
**Purpose:** Print system error message
```c
void perror(const char *s);
```

**Example:**
```c
if (socket(...) < 0) {
    perror("socket"); // Prints "socket: <error description>"
    exit(1);
}
```

### strerror()
**Purpose:** Get error string from errno
```c
char *strerror(int errnum);
```

**Example:**
```c
#include <errno.h>
if (connect(...) < 0) {
    fprintf(stderr, "connect failed: %s\n", strerror(errno));
}
```

### errno
**Purpose:** Global variable containing last error code
```c
extern int errno;
```

**Example:**
```c
errno = 0; // Clear previous error
if (read(...) < 0) {
    if (errno == EAGAIN) {
        printf("Operation would block\n");
    } else {
        printf("Error: %d\n", errno);
    }
}
```

## Complete Error Handling Template

```c
// Robust error handling function
void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Safe read function (handles partial reads)
int read_all(int fd, void *buf, size_t count) {
    size_t bytes_read = 0;
    while (bytes_read < count) {
        ssize_t result = read(fd, (char*)buf + bytes_read, count - bytes_read);
        if (result < 0) {
            if (errno == EINTR) continue; // Interrupted, try again
            return -1; // Real error
        }
        if (result == 0) {
            return 0; // EOF
        }
        bytes_read += result;
    }
    return bytes_read;
}

// Safe write function (handles partial writes)
int write_all(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    while (bytes_written < count) {
        ssize_t result = write(fd, (const char*)buf + bytes_written, 
                              count - bytes_written);
        if (result < 0) {
            if (errno == EINTR) continue; // Interrupted, try again
            return -1; // Real error
        }
        bytes_written += result;
    }
    return bytes_written;
}
```

## Common Error Codes

| Error Code | Meaning | When it happens |
|------------|---------|-----------------|
| EADDRINUSE | Address already in use | Port already bound |
| ECONNREFUSED | Connection refused | No server listening |
| ETIMEDOUT | Connection timed out | Server not responding |
| EINTR | Interrupted system call | Signal interrupted call |
| EAGAIN/EWOULDBLOCK | Resource temporarily unavailable | Non-blocking operation would block |
| EPIPE | Broken pipe | Writing to closed socket |
| ECONNRESET | Connection reset by peer | Remote side closed unexpectedly |

## Quick Reference - Typical Flow

**Server:**
```c
socket() → bind() → listen() → accept() → recv()/send() → close()
```

**Client:**
```c
socket() → connect() → send()/recv() → close()
```
