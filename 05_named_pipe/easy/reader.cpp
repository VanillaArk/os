#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "settings.h"

int main() {
    int pipe_descr;
    char buffer[BUFFER_SIZE];
    printf("%d\n", getpid());

    /* open, read, and display the message from the FIFO */
    pipe_descr = open(FIFO_PIPE_NAME, O_RDONLY);
    
    read(pipe_descr, buffer, BUFFER_SIZE);
    printf("Received: %s\n", buffer);
    
    close(pipe_descr);

    return 0;
}
