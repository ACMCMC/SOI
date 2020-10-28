#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    FILE* fp = fopen("file", "wb");
    int a = 2047;
    pid_t pid = fork();
    if (pid == 0) { // This is the child
        fclose(fp);
        fclose(fopen("file", "ab+"));
    } else if (pid > 0) {
        fwrite(&a, sizeof(int), 1, fp);
        fclose(fp);
    }
}
