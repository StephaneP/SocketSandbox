#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>


#define SA struct sockaddr
#define MAX_LINE 4096
#define MAX_SUB 1024

#define LISTEN_Q 1024

extern int h_errno;

ssize_t process_http(int socketfd, char *host, char *page, char *poststr) {
    char sendline[MAX_LINE + 1], recvline[MAX_LINE +1];
    ssize_t n;
    snprintf(sendline, MAX_SUB,
                             "POST %s HTTP/1.0\r\n"
                             "Host: %s\r\n"
                             "X-Parse-Application-Id: myAppId\r\n"
                             "Content-type: application/json\r\n"
                             "Content-length: %d\r\n\r\n"
                             "%s", page, host, (int)strlen(poststr), poststr);
    write(socketfd, sendline, strlen(sendline));

    while((n = read(socketfd, recvline, MAX_LINE))> 0) {
        recvline[n] = '\0';
        printf("%s", recvline);
    }

    return n;
}

using namespace std;

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;

    char **pptr;
    char  host[] = "requestb.in";
    char  page[] = "/1hhlob31";
    char jsonText[] = "{\"field1\":\"value1\",\"field2\":\"value2\"}";


    char str[50];
    struct hostent *hptr;

    if ((hptr = gethostbyname(host)) == NULL) {
        fprintf(stderr, " gethostbyname error for host: %s: %s", host, hstrerror(h_errno));
        exit(1);
    }

    printf("hostname: %s\n", hptr->h_name);

    if (hptr->h_addrtype == AF_INET && (pptr = hptr->h_addr_list) != NULL) {
        printf("address: %s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
    } else {
        fprintf(stderr, "Error call inet_stop\n");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(80);
    inet_pton(AF_INET, str, &serveraddr.sin_addr);

    connect(sockfd, (SA *) & serveraddr, sizeof(serveraddr));
    process_http(sockfd, host, page, jsonText);
    close(sockfd);
    return 0;
}
