#include "apue.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#define BUFF_SIZE 300
#define QUIT_MSG "quit"
#define NEED_TO_QUIT if (strncmp(buffer, QUIT_MSG, 4) == 0) break;

pthread_rwlock_t rwlock        = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t file_rwlock   = PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t  wasWriteMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  wasReadMutex  = PTHREAD_MUTEX_INITIALIZER;

char *buffer = new char[BUFF_SIZE];
int   file   = -1;

void *reader(void *arg) {
    while (true) {
        pthread_mutex_lock(&wasWriteMutex);
        pthread_rwlock_wrlock(&rwlock);
        read(0, buffer, BUFF_SIZE);

        if (strncmp(buffer, QUIT_MSG, 4) == 0) {
            break;
        }

        pthread_rwlock_unlock(&rwlock);
        pthread_mutex_unlock(&wasReadMutex);
    }
    pthread_rwlock_unlock(&rwlock);
    pthread_mutex_unlock(&wasReadMutex);
    return ((void *) 1);
}

void *writer(void *arg) {
    char *str = new char;
    while (true) {
        pthread_mutex_lock(&wasReadMutex);
        pthread_rwlock_rdlock(&rwlock);
        strcpy(str, buffer);
        pthread_mutex_unlock(&wasWriteMutex);
        pthread_rwlock_unlock(&rwlock);

        if (strncmp(str, QUIT_MSG, 4) == 0) {
            break;
        }

        pthread_rwlock_wrlock(&file_rwlock);
        write(file, str, strlen(str));
        pthread_rwlock_unlock(&file_rwlock);
    }
    pthread_mutex_unlock(&wasReadMutex);
    pthread_rwlock_unlock(&rwlock);
    return ((void *) 1);
}

int main(void) {
    const int READERS_NUMBER = 1;
    const int WRITERS_NUMBER = 5;
    pthread_t readerThreads[READERS_NUMBER];
    pthread_t writerThreads[WRITERS_NUMBER];
    void *thread_return;

    file = creat("./result.txt", FILE_MODE);
    pthread_mutex_lock(&wasReadMutex);

    for (int i = 0; i < READERS_NUMBER; i++) {
        pthread_create(&(readerThreads[i]), NULL, reader, NULL);
    }
    for (int i = 0; i < WRITERS_NUMBER; i++) {
        pthread_create(&(writerThreads[i]), NULL, writer, NULL);
    }

    for (int i = 0; i < READERS_NUMBER; i++) {
        pthread_create(readerThreads[i], &thread_return);
    }
    for (int i = 0; i < WRITERS_NUMBER; i++) {
        pthread_create(writerThreads[i], &thread_return);
    }

    pthread_rwlock_destroy(&rwlock);
    pthread_rwlock_destroy(&file_rwlock);
    pthread_mutex_destroy(&wasReadMutex);
    pthread_mutex_destroy(&wasWriteMutex);
    return 0;
}
