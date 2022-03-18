#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

// side 107 - part 2
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

// side 107 - part 3
#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {// 108/2
    printf("Configuring local address.\n"); // 108/2
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));   // zeros out the hints
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // indicates wr're using TCP
    hints.ai_flags = AI_PASSIVE;        // Tells getaddrinfo that we want to bind it to the wilcard address

    struct addrinfo *bind_address;      // holds the return info from getaddrinfo();
    // using getaddrinfo() makes it easy to convert program from ipv4 to ipv6
    getaddrinfo(0, "8080", &hints, &bind_address); // generates an address that's suitable for bind()

    printf("Creating socket...\n"); // 109/1
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);

    if(!ISVALIDSOCKET(socket_listen)){  // 109/2
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Binding socket to local address...\n");
    // bind() returns 0 on success and non-zero on failure
    if(bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)){ 
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");   // 111/1
    if(listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n",
                GETSOCKETERRNO());
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen,
            (struct sockaddr*) &client_address, &client_len);
    if(!ISVALIDSOCKET(socket_client)){
        fprintf(stderr, "accept() failed. (%d)\n", 
                GETSOCKETERRNO());
        return 1;
    }

    printf("Client is connected... ");  // 112/1
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address,
            client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    printf("Reading request...\n");
    char request[1024];
    // receive the request
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes.\n", bytes_received);

    // print the browser's request to the console
    printf("%.*s", bytes_received, request);    // 114/1

    // Now that the web browser has sent its request, we can send our response back
    printf("Sending response...\n");    // 114/2
    const char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";
        // send() returns the number of bytes sent. You should generally check the number
        // of bytes sent was as expectend
        int bytes_sent = send(socket_client, response, strlen(response), 0);
        printf("Sent    %d  of  %d  bytes.\n", bytes_sent, (int)strlen(response));

    // get the time and send it (115/1)
    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("Sent    %d  of  %d  bytes.\n", bytes_sent, (int)strlen(time_msg));

    // close the client connection, to indicate to the browser that
    // we've sent all of our data: (115/2)
    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);

    // At this point, we could call accpet() on socket_listen to accept additional
    // connection. That is exactly what a real server would do. 115/3
    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

    printf("Finished.\n");
    
    return 0;
}


