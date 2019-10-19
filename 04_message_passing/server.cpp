#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "settings.h"

/*struct my_msgbuf {
   long mtype;
   char mtext[MSG_SIZE];
};*/

void server_loop(int msq_id) {
    struct my_msgbuf msq_buf;
    int areQuit;

    while (true) {
        // Receive message from queue
        // 1: queue ID
        // 2: structure of message
        // 3: size of message
        // 4: message type (see mtype)
        // 5: special flag
        if (msgrcv(msq_id, &msq_buf, sizeof(msq_buf.mtext), MSG_TYPE, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("RECEIVED: \"%s\"\n", msq_buf.mtext);
        // areQuit = strcmp(msq_buf.mtext, MSG_QUIT);
        if (strcmp(msq_buf.mtext, MSG_QUIT) == 0) {
            break;
        }
    }
}

int main(void) {
    key_t msq_key;
    int msq_id;

    msq_key = ftok(MSGQ_PATH, MSGQ_KEY);

    // Connect to message queue
    // 1: key for queue
    // 2: options for queue
    if ((msq_id = msgget(msq_key, PERMS)) == -1) {
        perror("msgget");
        exit(1);
    }
    printf("Message Queue: ready to receive messages.\n");

    server_loop(msq_id);

    printf("Message Queue: done receiving messages.\n");
    return 0;
}
