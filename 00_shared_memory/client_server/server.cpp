#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include "keys.h"

void server_loop(int shmid, int semid, char *buf) {
    while(1) {
        if (semctl(semid, FULL_SHM, GETVAL, 0) <= 0){
            continue;
        }
        semctl(semid, FULL_SHM, SETVAL, 0);
        fprintf(stdout, "%s\n", buf);

        if(!strcmp(buf, QUIT_MSG)) {
            break;
        }
        semctl(semid, EMPTY_SHM, SETVAL, 1);
    }
    printf("cycle ended\n");
}

int main(int argc, char *argv[]) {
    int semid;
    int shmid;
    char *buf;

    // 1: path
    // 2: id
    // returns a key based on params for semget and shmget
    key_t sem_key = ftok(SEM_PATH, SEM_KEY);
    key_t shm_key = ftok(SHM_PATH, SHM_KEY);

    // 1: key for semaphore (like ID) 
    // 2: numbers of semaphores
    // 3: semaphore flags
    // IPC_CREAT - create a new segment. w/o it if would search segment with #1 key
    // IPC_EXCL - with IPC_CREAT would fall with EEXIST if segment exists for this key
    if ((semid = semget(sem_key, SEM_NUMBER, PERMS | IPC_CREAT | IPC_EXCL)) < 0) {
        printf("error opening semaphore");
        return 1;
    }

    // 1: key for shared memory (like ID) 
    // 2: buffer size
    // 3: semaphore flags
    // IPC_CREAT - create a new segment. w/o it if would search segment with #1 key
    // IPC_EXCL - with IPC_CREAT would fall with EEXIST if segment exists for this key
    if ((shmid = shmget(shm_key, BUF_SIZE, PERMS | IPC_CREAT | IPC_EXCL)) < 0) {
        printf("error creating shared memory");
        return 2;
    }

    // 1: shared memory ID
    // 2: a character pointer, normally NULL
    // 3: acces flag. May be SHM_RDONLY
    if ((buf = (char *) shmat(shmid, 0, 0)) == NULL) {
        printf("error attaching to shared memory");
        return 3;
    }
    printf("semaphores id: %d\n", semid);
    printf("memory id: %d\n", shmid);

    // SETVAL - set semaphore memory
    // GETVAL - get semaphore memory as result
    semctl(semid, EMPTY_SHM, SETVAL, 1);
    semctl(semid, FULL_SHM, SETVAL, 0);

    // SERVER MAIN LOOP
    server_loop(shmid, semid, buf); 
    /*while(1) {
        if (semctl(semid, FULL_SHM, GETVAL, 0) <= 0){
            continue;
        }
        semctl(semid, FULL_SHM, SETVAL, 0);

        fprintf(stdout, "%s\n", buf);

        if(!strcmp(buf, "q")) {
            break;
        }
        semctl(semid, EMPTY_SHM, SETVAL, 1);
    }
    printf("cycle ended\n");*/

    // IPC_RMID - segment to destroy
    if (semctl(semid, 0, IPC_RMID, (struct semid_ds *)0) < 0) {
        return 4;
    }

    // detach itself from shared memory
    shmdt(buf);
    if (shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) < 0) {
        return 5;
    }
    return 0;
}
