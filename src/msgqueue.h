#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include "cstr.h"
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>

#ifndef MAX_MSGQUEUE_SIZE
#define MAX_MSGQUEUE_SIZE 20
#endif

typedef struct MsgQueue {
    pthread_rwlock_t lock;
    sem_t empty, full;
    size_t head, tail;
    CString* data[MAX_MSGQUEUE_SIZE + 1];
} MsgQueue;

MsgQueue* msgQueueInit();

void msgQueueDelete(MsgQueue* queue);

bool msgQueueEmpty(MsgQueue* queue);

bool msgQueueFull(MsgQueue* queue);

size_t msgQueueSize(MsgQueue* queue);

void msgQueuePush(MsgQueue* queue, CString* item);

CString* msgQueuePoll(MsgQueue* queue);


#endif
