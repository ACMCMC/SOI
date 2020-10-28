#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    FILE* fp = fopen("salida_calculo.bin", "rb");
    double num;
    int i = 0;
    while (!feof(fp)) {
        if (fread(&num, sizeof(double), 1, fp) == 1) {
            printf("DOUBLE %d: %lf\n", i++, num);
        }
    }
}
