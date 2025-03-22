#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h> 

#define PERMS 0644
#define MSG_KEY 0x1234

int user_id;

struct Msgbuf {
  long mtype;
  char mtext[1000];
};

void* worker_msgrcv(void* data) {
  struct Msgbuf buf;
  int msqid = msgget(MSG_KEY, PERMS | IPC_CREAT);
  
  if (msqid == -1) {
    perror("msgget");
    exit(1);
  }
  printf("message-queue: ready to receive messages.\n");
  while (1) {
    // select the message to receive
    long rcv_type = (user_id == 1) ? 2 : 1;
    if (msgrcv(msqid, &buf, sizeof(buf.mtext), rcv_type, 0) == -1) {
      perror("msgrcv");
      exit(1);
    }

    printf("Received: %s\n", buf.mtext);

    if (strcmp(buf.mtext, "end") == 0) {
      break;
    }
  }
  printf("message-queue: done receiving messages.\n");
}

void* worker_msgsnd(void *data) {
  struct Msgbuf buf;
  int msqid = msgget(MSG_KEY, PERMS | IPC_CREAT);

  if (msqid == -1) {
    perror("msgget");
    exit(1);
  }
  buf.mtype = user_id;
  printf("message-queue: ready to send messages.\n");
  printf("Enter lines of text, ^C to quit:\n");

  while (fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
    int len = strlen(buf.mtext);
    if (buf.mtext[len - 1] == '\n') {
      buf.mtext[len - 1] = '\0';
    }

    // send messages
    if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1) {
      perror("msgsnd");
    }
  }
  strcpy(buf.mtext, "end");
  if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1) {
    perror("msgsnd");
  }
  if (msgctl(msqid, IPC_RMID, NULL) == -1) {
    perror("msgctl");
    exit(1);
  }
  printf("message-queue: done sending messages.\n");
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("You must provide exactly 1 argument: user_id");
    exit(1);
  }
  user_id = atoi(argv[1]);
  pthread_t thread_snd, thread_rcv;

  pthread_create(&thread_snd, NULL, &worker_msgsnd, "snd");
  pthread_create(&thread_rcv, NULL, &worker_msgrcv, "rcv");
  pthread_join(thread_snd, NULL);
  pthread_join(thread_rcv, NULL);
  return 0;
}
