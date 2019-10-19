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

void client_loop(int msq_id) {
	struct my_msgbuf msq_buf;
	int length;

	msq_buf.mtype = 1;
	while (true) {
		fscanf(stdin, "%s", msq_buf.mtext);
		length = strlen(msq_buf.mtext);

		// Send message to queue
        // 1: queue ID
        // 2: message
        // 3: message length
        // 4: flag that used if queue is full
        if (msgsnd(msq_id, &msq_buf, length + 1, 0) == -1) {
            perror("msgsnd");
        }
	}

	strcpy(msq_buf.mtext, MSG_QUIT);
    length = strlen(msq_buf.mtext);

    // Send message to queue
    // 1: queue ID
    // 2: message
    // 3: message length
    // 4: flag that used if queue is full
    if (msgsnd(msq_id, &msq_buf, length + 1, 0) == -1) {
        perror("msgsnd");
    }
}

int main(void) {
	struct my_msgbuf msq_buf;
	key_t msq_key;
    int msq_id;
    int length;

    msq_key = ftok(MSGQ_PATH, MSGQ_KEY);

    // Create message queue
    // 1: key for queue
    // 2: options for queue
    if ((msq_id = msgget(msq_key, PERMS | IPC_CREAT)) == -1) {
       perror("msgget");
       exit(1);
    }
    printf("Mesage Queue: ready to send messages.\n");
    printf("Enter lines of text, Ctrl+C to quit:\n");

    client_loop(msq_id);
   
    // Destroy message queue
    // 1: queue ID
    // 2: mark segment as "deleted"
    // 3: operation params OR for return info about operation result
    if (msgctl(msq_id, IPC_RMID, NULL) == -1) {
       perror("msgctl");
       exit(1);
    }
    printf("Message Queue: done sending messages.\n");
    return 0;
}
