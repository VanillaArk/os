#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "settings.h"

int main() {
	int fd_server, fd_client;
	struct message client_msg;
	struct message server_msg;

	std::string pid_str = std::to_string(getpid());
    std::string fifo_name(CLIENT_FIFO_TEMPLATE);
    std::string client_pipe = fifo_name + pid_str;
    printf("Client pipe: %s\n", client_pipe.c_str());

    mkfifo(client_pipe.c_str(), PERMS);

    while (true) {
    	printf("Enter message: ");
    	scanf("%s", client_msg.text);

    	fd_server = open(SERVER_FIFO, O_WRONLY);
    	client_msg.client_pid = getpid();
    	write(fd_server, &client_msg, sizeof(client_msg));

    	fd_client = open(client_pipe.c_str(), O_RDONLY);
        read(fd_client, &server_msg, sizeof(struct message));
        close(fd_client);

        printf("Server said: %s\n\n", server_msg.text);
    }
    close(fd_server);
	unlink(client_pipe.c_str());
    return 0;
}
