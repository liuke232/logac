#include "msgqueue.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

static const size_t cap = MAX_MSGQUEUE_SIZE + 1;

MsgQueue* msgQueueInit()
{
    MsgQueue* ret = (MsgQueue*)malloc(sizeof(MsgQueue));
    pthread_rwlock_init(&ret->lock, NULL);
    sem_init(&ret->empty, 0, MAX_MSGQUEUE_SIZE);
    sem_init(&ret->full, 0, 0);
    ret->head = ret->tail = 0;
    return ret;
}

void msgQueueDelete(MsgQueue* queue)
{
    pthread_rwlock_destroy(&queue->lock);
    sem_destroy(&queue->full);
    sem_destroy(&queue->empty);
    free(queue);
}

bool msgQueueEmpty(MsgQueue* queue)
{
    pthread_rwlock_rdlock(&queue->lock);
    bool ret = queue->head == queue->tail;
    pthread_rwlock_unlock(&queue->lock);
    return ret;
}

bool msgQueueFull(MsgQueue* queue)
{
    pthread_rwlock_rdlock(&queue->lock);
    bool ret = queue->head == (queue->tail + 1) % cap;
    pthread_rwlock_unlock(&queue->lock);
    return ret;
}

size_t msgQueueSize(MsgQueue* queue)
{
    pthread_rwlock_rdlock(&queue->lock);
    size_t ret = (cap + queue->tail - queue->tail) % cap;
    pthread_rwlock_unlock(&queue->lock);
    return ret;
}

void msgQueuePush(MsgQueue* queue, CString* item)
{
    sem_wait(&queue->empty);
    pthread_rwlock_wrlock(&queue->lock);
    queue->data[queue->tail] = item;
    queue->tail = (queue->tail + 1) % cap;
    pthread_rwlock_unlock(&queue->lock);
    sem_post(&queue->full);
}

CString* msgQueuePoll(MsgQueue* queue)
{
    sem_wait(&queue->full);
    pthread_rwlock_wrlock(&queue->lock);
    CString* ret = queue->data[queue->head];
    queue->head = (queue->head + 1) % cap;
    pthread_rwlock_unlock(&queue->lock);
    sem_post(&queue->empty);
    return ret;
}

