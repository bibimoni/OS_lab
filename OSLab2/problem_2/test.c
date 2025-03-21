#include <stdio.h>
#include <stdlib.h>

FILE *file_r;
FILE *file;
void calc(int64_t N, int nthread) {
  float curr;
  int ismul = 1;
  float mul = 0, single = 0;
  int cmul = 0, csingle = 0;
  for (int _ = 0; _ < 10; _++) {
    if (fscanf(file_r, "%f", &curr) == 1) {
      if (ismul) {
        mul += curr;
        cmul += 1;
      } else {
        single += curr;
        csingle += 1;
      }
      ismul ^= 1;
    }
  }
  printf("N: %d, nthread: %d\n", N, nthread);
  printf("single: %f, multi: %f\n", single / csingle, mul / cmul);
  // sprintf(file, "N = %d, thread = %d, multi = %f, single = %f", N, nthread, mul / cmul, single / csingle);
  // fclose(file_r);
}

int main(void) {
  file_r = fopen("results", "r");
  file = fopen("benmark", "a");
  // for (int64_t N = 1000; N <= 1000000000; N *= 1000) {
  //   for (int nthread = 1; nthread <= 4; nthread++) {
  //     for (int _ = 0; _ < 5; _++) {
  //       char cmd[1000];
  //       sprintf(cmd, "make sum_multi-thread && ./sum_multi-thread %d %ld && make sum_serial && ./sum_serial %ld", nthread, N, N);
  //       // printf(cmd);
  //       system(cmd);
  //     }
  //   }
  // }
  for (int64_t N = 1000; N <= 1000000000; N *= 1000) {
    for (int nthread = 1; nthread <= 4; nthread++) {
      calc(N, nthread);
    }
  }
  return 0;
}