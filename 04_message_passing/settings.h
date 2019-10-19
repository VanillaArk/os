#define MSGQ_PATH "msq_path"
#define MSGQ_KEY  80
#define PERMS     0666

#define MSG_SIZE 256
#define MSG_TYPE 0
#define MSG_QUIT "quit"

struct my_msgbuf {
   long mtype;
   char mtext[MSG_SIZE];
};
