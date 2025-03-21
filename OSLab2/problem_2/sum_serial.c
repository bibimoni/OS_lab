#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Error! You must provide exactly 2 arguments");
    exit(1);
  }
  clock_t t;
  t = clock();
  int64_t n = atoi(argv[1]);  
  int64_t sum = 0;
  for (int i = 1; i <= n; i++) {
    sum += i;
  }  
  t = clock() - t;

  FILE *file = fopen("results", "a");
  if (file == NULL) {
    printf("WTF\n");
  }
  // fprintf(file, "Serial: n = %d, time = %f\n", n, ((double) t) / CLOCKS_PER_SEC);
  fprintf(file, "%f\n", ((double) t) / CLOCKS_PER_SEC);
  fclose(file);
  // printf("Sum from %d to %ld is: %ld\n", 1, n, sum);
  // printf("Elapsed time: %f\n", ((double) t) / CLOCKS_PER_SEC);
}