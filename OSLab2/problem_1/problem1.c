#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PERMS 0644
#define SHM_KEY 0x1234

int N = 2000;
struct Info {
  int ratings[2000];
  int cnt[2000];
};

void process(char* fileName) {
  FILE* file = fopen(fileName, "r");
  int userID, movieID, rating, timeStamp;
  int shmid = shmget(SHM_KEY, sizeof(struct Info), PERMS | IPC_CREAT);

  struct Info* shm = (struct Info *) shmat(shmid, 0, 0);
  if (shm == (struct Info*) -1) {
    perror("shmat");
    exit(1);
  }

  while (fscanf(file, "%d %d %d %d", &userID, &movieID, &rating, &timeStamp) == 4) {
    shm->ratings[movieID] += rating;
    shm->cnt[movieID] += 1;
  }

  fclose(file);
}

void goForking(char *fileName) {
  int child = fork();
  if (child < 0) {
    perror("fork");
    exit(1);
  } else if (child > 0) {
    process(fileName);
    exit(0);
  }
}

int main(int argc, char* argv[]) {
  struct Info info;
  int shmid = shmget(SHM_KEY, sizeof(struct Info), PERMS | IPC_CREAT);
  if (shmid < 0) {
    perror("shmget");
    exit(1);
  } else {
    printf("Shared memory Id: %d\n", shmid);
  }

  
  //set buffer values to zero
  struct Info* shm = (struct Info *) shmat(shmid, 0, 0);
  if (shm == (struct Info *) -1) {
    perror("shmat");
    exit(1);
  }

  memset(shm->ratings, 0, N * sizeof(shm->ratings[0]));
  memset(shm->cnt, 0, N * sizeof(shm->cnt[0]));
  

  goForking("movie-100k_1.txt");
  goForking("movie-100k_2.txt");

  FILE *file = fopen("movie-ans.txt", "w");
  for (int id = 0; id < N; id++) {
    if (shm->cnt[id] == 0) {
      continue;
    }
    fprintf(file, "%d %f\n", id, (float)shm->ratings[id] / shm->cnt[id]);
  }
  fclose(file);
  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    return 1;
  }
  return 0;
}