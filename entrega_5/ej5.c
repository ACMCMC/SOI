#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Linkado estático: gcc ej5.o -o ej5.out -static -lm
// Linkado dinámico: gcc ej5.o -o ej5.out -lm

int main() {
    cos(2);
    getchar();
    exit(EXIT_SUCCESS);
}