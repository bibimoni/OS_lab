#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

struct Info {
  int from, to;
};

int64_t sum;
// pthread_mutex_t lock;
void* worker(void *data) {
  struct Info* info = data;
  int64_t local_sum = 0;
  for (int i = info->from; i <= info->to; i++) {
    local_sum += i;
  }
  // pthread_mutex_lock(&lock);
  sum += local_sum;
  // pthread_mutex_unlock(&lock);
  free(info);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Error! You must provide exactly 2 arguments");
    exit(1);
  }
  int n = atoi(argv[2]);
  int nThread = atoi(argv[1]);
  int block = n / nThread;
  pthread_t threads[nThread];

  sum = 0;
  clock_t t;
  t = clock();

  int from = 1, to = block;
  for (int i = 0; i < nThread; i++) {
    struct Info* info = malloc(sizeof(struct Info));
    info->from = from;
    info->to = to;
    pthread_create(&threads[i], NULL, &worker, (void*)info);
    from = to + 1;
    to = to + block > n ? n : to + block;
  }
  for (int i = 0; i < nThread; i++) {
    pthread_join(threads[i], NULL);
  }
  // pthread_mutex_destroy(&lock);
  t = clock() - t;

  FILE *file = fopen("results", "a");
  // fprintf(file, "Multi: n = %d, n_thread = %d, time = %f\n", n, nThread, ((double) t) / CLOCKS_PER_SEC);
  fprintf(file, "%f\n", ((double) t) / CLOCKS_PER_SEC);
  fclose(file);
  // printf("Sum from %d to %d is: %ld\n", 1, n, sum);
  // printf("Elapsed time: %f\n", ((double) t) / CLOCKS_PER_SEC);
}