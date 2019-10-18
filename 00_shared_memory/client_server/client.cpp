#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include "keys.h"

void client_loop(int shmid, int semid, char *buf) {
    while(1) {
        if (semctl(semid, EMPTY_SHM, GETVAL, 0) <= 0) {
            printf("%d\n", semctl(semid, EMPTY_SHM, GETVAL, 0));
            printf("%d\n", semctl(semid, FULL_SHM, GETVAL, 0));
            printf("%d\n", errno);
            continue;
        }
        semctl(semid, EMPTY_SHM, SETVAL, 0);

        fscanf(stdin, "%s", buf);

        semctl(semid, FULL_SHM, SETVAL, 1);
        if(!strcmp(buf, QUIT_MSG)) {
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    char *buf;
    int semid;
    int shmid;

    // 1: path
    // 2: id
    // returns a key based on params for semget and shmget
    key_t sem_key = ftok(SEM_PATH, SEM_KEY);
    key_t shm_key = ftok(SHM_PATH, SHM_KEY);

    // 1: key for semaphore (like ID) 
    // 2: numbers of semaphores
    // 3: semaphore flags. Only permissions, would search segment with #1 key
    if ((semid = semget(sem_key, SEM_NUMBER, PERMS)) < 0) {
        return 1;
    }

    // 1: key for shared memory (like ID) 
    // 2: buffer size
    // 3: semaphore flags. Only permissions, would search segment with #1 key
    if ((shmid = shmget(shm_key, BUF_SIZE, PERMS)) < 0) {
        return 2;
    }
    
    // 1: shared memory ID
    // 2: a character pointer, normally NULL
    // 3: acces flag. May be SHM_RDONLY
    if ((buf = (char*)shmat(shmid, 0, 0)) == NULL) {
        return 3;
    }

    // CLIENT MAIN LOOP
    client_loop(shmid, semid, buf);
    /*while(1) {
        if (semctl(semid, EMPTY_SHM, GETVAL, 0) <= 0) {
            printf("%d\n", semctl(semid, 0, GETVAL, 0));
            printf("%d\n", semctl(semid, 1, GETVAL, 0));
            printf("%d\n", errno);
            continue;
        }

        semctl(semid, EMPTY_SHM, SETVAL, 0);

        fscanf(stdin, "%s", buf);

        semctl(semid, FULL_SHM, SETVAL, 1);

        if(!strcmp(buf, "q")) {
            break;
        }
    }*/
    shmdt(buf);
    return 0;
}
