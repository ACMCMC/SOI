#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    pid_t pid = fork();
    fprintf(stderr, "Error on %s: %s\n", pid==0 ? "child" : "parent", strerror(errno));
}