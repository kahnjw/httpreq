/* Original code from https://gist.github.com/nolim1t/126991 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 1024


int socket_connect(char *host, in_port_t port){
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock, conn;

    if((hp = gethostbyname(host)) == NULL) {
        herror("gethostbyname");
        exit(1);
    }
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

    if(sock == -1) {
        perror("setsockopt");
        exit(1);
    }

    conn = connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if(conn == -1) {
        perror("connect");
        exit(1);
    }

    return sock;
}

int main(int argc, char *argv[]){
    int fd;
    char buffer[BUFFER_SIZE];

    if(argc < 5) {
        fprintf(stderr, "Usage: %s <method> <hostname> <port> <url>\n", argv[0]);
        exit(1);
    }

    fd = socket_connect(argv[2], atoi(argv[3]));
    bzero(buffer, BUFFER_SIZE);
    sprintf(buffer, "%s %s\r\n", argv[1], argv[4]);

    write(fd, buffer, strlen(buffer));
    bzero(buffer, BUFFER_SIZE);

    while(read(fd, buffer, BUFFER_SIZE - 1) != 0) {
        fprintf(stdout, "%s\n", buffer);
        bzero(buffer, BUFFER_SIZE);
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);

    return 0;
}
