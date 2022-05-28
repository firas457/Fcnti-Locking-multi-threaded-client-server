/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define BUFFER_LEN 1024

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char line[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    while (1)
    {

        if (!fgets(line, BUFFER_LEN, stdin))
        {
            exit(1);
        }
       

        char *pos;
        pos = strstr(line, "PUSH");

        if (pos)
        {
            int i = 0;
            int j = 0;
            char arr[4];
            arr[0] = 'P';
            arr[1] = 'U';
            arr[2] = 'S';
            arr[3] = 'H';
            while (line[i] == arr[j])
            {
                i++;
                j++;
            }
            char temp[1024];
            int k = 0;
            for (j; j < BUFFER_LEN; j++)
            {
                temp[k] = line[j];
                k++;
            }
            send(sockfd, temp, MAXDATASIZE - 1, 0);
            memset(temp, 0, MAXDATASIZE - 1);
            memset(line, 0, MAXDATASIZE - 1);
        }

        // char string[256];
        // scanf("%s", string);

        //   if(send(sockfd, line ,MAXDATASIZE-1 , 0) == -1){
        //     printf("hi i'm here\n");}
        else
        {
            if (strcmp(buf, "TOP") != 0 && strcmp(buf, "POP\n") != 0)
            {
                printf("\n");
            }
            send(sockfd, line, MAXDATASIZE - 1, 0);
            memset(line, 0, MAXDATASIZE - 1);
        }

     


        // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        //     perror("recv");
        //     exit(1);
        // }

        // printf("client: received '%s'\n",buf);
    }

    return 0;
}