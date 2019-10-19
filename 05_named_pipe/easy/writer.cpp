#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "settings.h"

int main() {
    int pipe_descr;
    char buffer[BUFFER_SIZE];

    /* create the FIFO (named pipe) */
    mkfifo(FIFO_PIPE_NAME, 0666);
    printf("%d\n", getpid());

    std::string pid_str = std::to_string(getpid());
    std::string fifo_name(FIFO_PIPE_NAME);
    std::string result = fifo_name + pid_str;
    // char const *pchar = pid_str.c_str();
    printf("%s\n", result.c_str());

    /* write "Hi" to the FIFO */
    pipe_descr = open(FIFO_PIPE_NAME, O_WRONLY);

    printf("Enter message: ");
    scanf("%s", buffer);
    write(pipe_descr, buffer, sizeof(buffer));

    close(pipe_descr);

    /* remove the FIFO */
    unlink(FIFO_PIPE_NAME);
    return 0;
}
