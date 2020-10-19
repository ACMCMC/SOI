#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    return(EXIT_FAILURE);
  }

  int* p;
  int i;
  int num_reservar = atoi(argv[1]);

  while (1) {
    p = malloc(sizeof(int)*num_reservar);
    printf("Reservados %lu bytes: %p\n", sizeof(int)*num_reservar, p);
    for (i = 0; i < num_reservar; i+=500) {
      p[i] = 0;
    }
    sleep(1);
  }

  return(EXIT_SUCCESS);
}