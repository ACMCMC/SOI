#include <stdlib.h>
#include <stdio.h>

void a(int num);

int main() {
    int x = 5;
    void (*func)(int);

    func = a;
    (*func)(x);
}

void a(int num) {
    printf("%d\n", num);
}
