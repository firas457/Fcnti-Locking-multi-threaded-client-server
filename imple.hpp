#include "iostream"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "stdio.h"
using namespace std;

int fd_lock = open("lock.txt", O_RDWR);
struct flock flock;


typedef struct stack_node
{
    char data[1024];
    struct stack_node *next;

} node, *pnode;

typedef struct next_addr
{

    struct next_addr *next;
    size_t capacity;

} next_addr;

static next_addr dirent = {0, 0};

static const size_t next_to = 16;

void *my_malloc(size_t capacity)
{

    (flock).l_type = F_WRLCK;
    fcntl(fd_lock, F_SETLKW, &flock);

    capacity = (capacity + sizeof(next_addr) + (next_to - 1)) & ~(next_to - 1);

    next_addr *mem_size = dirent.next;
    next_addr **head = &(dirent.next);

    while (mem_size != 0)
    {

        if (mem_size->capacity >= capacity)
        {
            *head = mem_size->next;
            return ((char *)mem_size) + sizeof(next_addr);
        }

        head = &(mem_size->next);
        mem_size = mem_size->next;
    }

    mem_size = (next_addr *)mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mem_size->capacity = capacity;

    (flock).l_type = F_UNLCK;
    fcntl(fd_lock, F_SETLKW, &flock);

    return ((char *)mem_size) + sizeof(next_addr);
}

void my_free(void *mem_addr)
{
    if (mem_addr == NULL)
    {
        return;
    }

    (flock).l_type = F_WRLCK;
    fcntl(fd_lock, F_SETLKW, &flock);

    next_addr *mem_size = (next_addr *)(((char *)mem_addr) - sizeof(next_addr));

    mem_size->next = dirent.next;
    dirent.next = mem_size;

    (flock).l_type = F_UNLCK;
    fcntl(fd_lock, F_SETLKW, &flock);
}

void push(pnode *head, char data[1024])
{

    if (*head == NULL)
    {
        *head = (pnode)my_malloc(sizeof(node));

        memcpy((*head)->data, data, strlen(data));

        (*head)->next = NULL;
    }

    else
    {

        pnode new_node = (pnode)my_malloc(sizeof(node));

        memcpy(new_node->data, data, strlen(data));

        new_node->next = NULL;

        pnode *temp_node = head;

        while ((*temp_node)->next)
        {

            temp_node = &((*temp_node)->next);
        }

        (*temp_node)->next = new_node;
    }
}

void pop(pnode *head)
{

    if (*head == NULL)
    {
        return;
    }

    pnode *temp_node = head;

    while ((*temp_node)->next)
    {
        temp_node = &((*temp_node)->next);
    }

    pnode rem = (*temp_node);

    (*temp_node) = NULL;

    my_free(rem);
}

char *peek(pnode *head)
{
    if (*head == NULL)
    {
        return NULL;
    }

    pnode *temp_node = head;

    while ((*temp_node)->next)
    {

        temp_node = &((*temp_node)->next);
    }

    return (*temp_node)->data;
}
