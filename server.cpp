/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/mman.h>
#include <sys/shm.h>
#include "imple.hpp"

// extern stack * stack1;
enum
{
    SEGMENT_SIZE = 0x6400
};
#define PORT "3490" // the port users will be connecting to
#define MAXDATASIZE 1024
#define BACKLOG 10 // how many pending connections queue will hold
#define BUFFER_LEN 1024
using namespace std;

char buf[MAXDATASIZE];
static int count = 0;
// stack *stack1;



// struct flock lock = {F_UNLCK, SEEK_SET, 0, BUFFER_LEN, 0};

void sigchld_handler(int s)
{

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// void *client_handle(void *p)
// {
//     int sfd = *((int *)p);

//     while (1)
//     {
//         recv(sfd, buf, MAXDATASIZE - 1, 0);
//         pthread_mutex_lock(&lock);

//         if (strcmp(buf, "\n") == 0)
//         {
//             printf("enter a valid option!");
//             pthread_mutex_unlock(&lock);
//         }

//         else if (strcmp(buf, "POP\n") == 0)
//         {
//             std::string s;
//             s = stack.pop();

//             if (s.empty())
//             {
//                 std::cout << "stack is empty" << endl;
//             }

//             memset(buf, 0, MAXDATASIZE - 1);
//             pthread_mutex_unlock(&lock);
//         }

//         else if (strcmp(buf, "TOP\n") == 0)
//         {

//             std::string s;
//             s = stack.peek();

//             if (s.empty())
//             {
//                 std::cout << "stack is empty" << endl;
//             }

//             const char *c = s.c_str();
//             // send(sfd,c,MAXDATASsIZE-1 , 0);
//             printf("OUTPUT -->  '%s'\n", c);
//             memset(buf, 0, MAXDATASIZE - 1);

//             pthread_mutex_unlock(&lock);
//         }

//         else if (strcmp(buf, "TOP\n") != 0 && strcmp(buf, "POP\n") != 0)
//         {

//             stack.push(buf);
//             memset(buf, 0, MAXDATASIZE - 1);

//             pthread_mutex_unlock(&lock);
//         }
//         pthread_mutex_unlock(&lock);
//     }

// }

int main(void)
{
    //    int ft;
    //   stack2->create_file();
    // ft = shm_open("lock.txt", O_RDWR|O_CREAT, 0666);

    // ftruncate(ft, sizeof(int)*1000);
    // stack2= (stack*)mmap(0, sizeof(stack), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, ft, 0);

    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    int fd = open("lock.txt", O_RDWR);

    struct flock flock;

    pnode head; 

    int *capacity;


    head = (pnode)mmap(NULL,sizeof(pnode) * 10000,PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS,-1,0);


    capacity = (int *)mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
    *capacity = 0; 



    memset(&flock, 0, sizeof(flock));
    //  stack * stack1=(stack *)my_malloc(sizeof(stack));
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    // int * count=(int *)mmap(NULL, sizeof(int), PROT_READ ,
    //                          MAP_SHARED | MAP_ANONYMOUS, ft, 0);

    while (1)
    { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        // if (pthread_mutex_init(&lock, NULL) != 0)
        // {
        //     printf("\n mutex init has failed\n");
        //     return 1;
        // }
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        // stack1 = (stack *)mmap(NULL, sizeof(stack), PROT_READ | PROT_WRITE,
        //                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        int status;
        int segment_id;

        if (!fork())
        {

            //   segment_id = shmget (IPC_PRIVATE, SEGMENT_SIZE,
            //                         IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
            //     stack *stack2 = (stack *) shmat(segment_id, 0, 0);

            int pid_t = getpid();
            // lock.l_type = F_WRLCK;
            // lock.l_pid = pid_t;

            while (1)
            {

                recv(new_fd, buf, MAXDATASIZE - 1, 0);
                // pthread_mutex_lock(&lock);

                // (fcntl(new_fd, F_SETLK, &lock));
                (flock).l_type = F_WRLCK;
                fcntl(fd, F_SETLKW, &flock);

                if (strcmp(buf, "\n") == 0)
                {

                    printf("enter a valid option!");
                    // pthread_mutex_unlock(&lock);

                    // if (!fcntl(new_fd,F_ULOCK, &lock))
                    //         return 0;
                }

                else if (strcmp(buf, "POP\n") == 0)
                {

                    // std::string s;
                    // s = stack2->pop();

                    // if (s.empty())
                    // {
                    //     std::cout << "stack is empty" << endl;
                    // }

                    int length = (*capacity) - 1;
                    strcpy(head[length].data, "\0");

                    *capacity = length;

                    memset(buf, 0, MAXDATASIZE - 1);

                    // if (!fcntl( new_fd,F_UNLCK, &lock))
                    //         return 0;
                }

                else if (strcmp(buf, "TOP\n") == 0)
                {

                    // std::string s;
                    // s = stack2->peek();

                    // if (s.empty())
                    // {
                    //     std::cout << "stack is empty" << endl;
                    // }

                    // const char *c = s.c_str();
                    // send(sfd,c,MAXDATASsIZE-1 , 0);
                    // printf("OUTPUT -->  '%s'\n", c);

                    int length = (*capacity) - 1;
                    char *pos = (head[length]).data;

                    if (*capacity == 0)
                    {
                        printf("stack is empty!");
                    }


                    memset(buf, 0, MAXDATASIZE - 1);

                    // if (!fcntl( new_fd,F_UNLCK, &lock))
                    //         return 0;
                    printf("OUTPUT -->  '%s'\n", pos);
                }

                else if (strcmp(buf, "TOP\n") != 0 && strcmp(buf, "POP\n") != 0)
                {
                    int length = *capacity;
                    // copy the content into a stack pnode
                    strcpy(head[length].data, buf);
                    // increase the size of the stack by 1
                    *capacity = length + 1;

                    memset(buf, 0, MAXDATASIZE - 1);
                }

                (flock).l_type = F_UNLCK;
                fcntl(fd, F_SETLKW, &flock);
            }
        }
        close(new_fd);
        // pthread_t thread;
        // pthread_create(&thread, NULL, client_handle, (void *)&new_fd);
    }

    return 0;
}