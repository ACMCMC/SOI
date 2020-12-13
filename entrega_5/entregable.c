#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>

int var_global; // Variable global que comparten los 3 hilos (el principal y dos que crea)

void *hilo(void *param)
{
    int var_local_2 = var_global * ((int)(intptr_t)param);

    printf("\nDireccion de la variable global desde el hilo (es la misma): %p\n", &var_global);
    printf("Direccion del parametro recibido: %p\n", &param);
    printf("Direccion de la variable local del hilo: %p\n", &var_local_2);

    scanf("%*d"); // Esperamos al usuario

    printf("Salgo\n");

    return NULL;
}

int main()
{
    int var_local = 50;
    int var_local2 = 100;
    pthread_t p1, p2;

    printf("Direccion de la variable global desde el padre: %p\n", &var_global);
    printf("Direcciones de las variables locales: %p, %p\n", &var_local, &var_local2);

    scanf("%*d"); // Esperamos al usuario

    if (pthread_create(&p1, NULL, hilo, (void *)(intptr_t)var_local))
    {
        perror("Error creando el hilo");
        exit(EXIT_FAILURE);
    }
    scanf("%*d"); // Esperamos al usuario
    if (pthread_create(&p2, NULL, hilo, (void *)(intptr_t)var_local2))
    {
        perror("Error creando el hilo");
        exit(EXIT_FAILURE);
    }
    scanf("%*d"); // Esperamos al usuario

    if (pthread_join(p1, NULL))
    {
        perror("Error ejecutando pthread_join");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(p2, NULL))
    {
        perror("Error ejecutando pthread_join");
        exit(EXIT_FAILURE);
    }
    scanf("%*d"); // Esperamos al usuario

    exit(EXIT_SUCCESS);
}