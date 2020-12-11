#include <stdlib.h>
#include <stdio.h>

void f1(int par) {
    int x;
    printf("LOCAL: %p, PARAMETRO: %p\n", &x, &par);
}

void f2(int par) {
    int x;
    printf("LOCAL: %p, PARAMETRO: %p\n", &x, &par);
}

int main() {
    int x;

    f1(x);
    f2(x);

    printf("F1: %p, F2: %p\n", f1, f2);

    exit(EXIT_SUCCESS);
}