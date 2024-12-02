#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "cstr.h"
#include "msgqueue.h"

MsgQueue* q;

void* producer() {
    printf("producer thread created.\n");
    for (int i = 0; i < 100; ++i) {
        usleep(400000);
        CString* item = cstrSprintf("ABC%d", i);
        printf("produced [%s].\n", item->data);
        msgQueuePush(q, item);
    }
    return NULL;
}

void* consumer() {
    printf("consumer thread created.\n");
    usleep(1300000);
    for (int i = 0; i < 100; ++i) {
        usleep(300000);
        CString* item = msgQueuePoll(q);
        printf("comsumed [%s].\n", item->data);
        cstrDelete(item);
    }
    return NULL;
}


int main()
{
    q = msgQueueInit();

    pthread_t pt, ct;
    pthread_create(&pt, NULL, producer, NULL);
    pthread_create(&ct, NULL, consumer, NULL);

    pthread_join(pt, NULL);
    pthread_join(ct, NULL);

    return 0;
}

