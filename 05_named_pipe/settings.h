#define SERVER_FIFO          "/tmp/server_fifo_pipe"
#define CLIENT_FIFO_TEMPLATE "/tmp/client_fifo"
#define PERMS                0666

#define BUFFER_SIZE    1024

struct message {
   long client_pid;
   char text[BUFFER_SIZE];
};
