#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

typedef struct heap_block {

    size_t size;
    bool free;
    struct heap_block *next;

} block;

#define SIZE sizeof(block)
pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
void *head = NULL;
block *tail = NULL;

void *find_free_block(size_t s) 
{
    block *b;

    for(b = head; b->next != NULL; b = b->next) {
	if(b && b->size >= s && b->free)
	    return b;
    }

    return NULL;
}

void *split_block(block *b, size_t s) 
{
    block *next = b->next;
    size_t bs = b->size; 
    block *split;
    char *gap;
 
    b->free = false;
    b->size = s;

    ++b;
    gap = (char*) b;
    
    gap = gap + s + 1;

    split = (block*) gap;
    split->free = true;
    split->size = (bs - s);
    split->next = next;

    --b;
    b->next = split;
    return (void *) (b + 1);

}

void *malloc(size_t s) 
{
    if(s <= 0)
	return NULL;
    
    pthread_mutex_lock(&mu);
    if(!head) {
	block *tmp = sbrk(s + SIZE);
	tmp->free = false;
	tmp->size = s;
	tmp->next = NULL;
	head = tmp;
	tail = tmp;
	pthread_mutex_unlock(&mu);
	return (void*) (tmp + 1);
    } 
    else {
	block *tmp = find_free_block(s);
	
	if(!tmp) {
	    tmp = sbrk(s + SIZE);
	    tmp->free = false;
	    tmp->size = s;
	    tmp->next = NULL;
	    tail->next = tmp;
	    tail = tmp;
	    pthread_mutex_unlock(&mu);
	    return (void*) (tmp + 1);
	}
	else {
	    if(tmp->size > s + SIZE + 1) {
		pthread_mutex_unlock(&mu);
		return split_block(tmp, s);
	    }
	    else {
		tmp->free = false;
		tmp->size = s;
		pthread_mutex_unlock(&mu);
		return (void*) (tmp + 1);
	    }
	}

    }
    pthread_mutex_unlock(&mu);
}

void free(void *ptr) 
{

    if(!ptr)
	return;

    block *tmp = ptr;
    tmp = tmp - 1;
    tmp->free = true;

} 
