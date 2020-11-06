#include <stdlib.h>
#include <stdio.h>

// Compilar con gcc ej_1.c

// Este proceso se queda esperando. La idea es matarlo con SIGKILL (kill -s SIGKILL [PID]), para ver cómo se mata un proceso con una señal. El proceso no puede manejar esta señal.

int main() {
    int x;
    scanf("%d", &x);
    exit(EXIT_SUCCESS);
}