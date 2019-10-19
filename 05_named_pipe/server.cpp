#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "settings.h"

int main() {
	int fd_server, fd_client, fd_useless;
	struct message client_msg;
	struct message server_msg;
	
	printf("Server started...\n");

	mkfifo(SERVER_FIFO, PERMS);
	fd_server = open(SERVER_FIFO, O_RDONLY);
	fd_useless = open(SERVER_FIFO, O_WRONLY);

	while (true) {
		read(fd_server, &client_msg, sizeof(client_msg));

        std::string pid_str = std::to_string(client_msg.client_pid);
        std::string fifo_name(CLIENT_FIFO_TEMPLATE);
        std::string client_pipe = fifo_name + pid_str;

		printf("GOT THIS: %s; ANSWER TO: %s\n", client_msg.text, client_pipe.c_str());

        server_msg.client_pid = client_msg.client_pid;
        for (int i = 0; i < BUFFER_SIZE; i++) {
        	server_msg.text[i] = client_msg.text[i];
        }

        fd_client = open(client_pipe.c_str(), O_WRONLY);
        write(fd_client, &server_msg, sizeof(struct message));
        close(fd_client);
	}
    close(fd_server);
	unlink(SERVER_FIFO);
    return 0;
}
