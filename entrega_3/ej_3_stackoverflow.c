// Compilar con gcc ej_2.c

/* Incluye las librerias necesarias*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static void gestion(int); /*Declaracion de la funcion de gestion de señales recibidas*/
main()
{
    /* Declara las variables necesarias*/
    pid_t padre, hijo;
    int stat_loc;

    if (signal(SIGUSR1, gestion) == SIG_ERR)
        printf("Error al crear gestor 1\n");
    if (signal(SIGUSR2, gestion) == SIG_ERR)
        printf("Error al crear gestor 2\n");
    padre = getpid();
    if ((hijo = fork()) == 0)
    {
        kill(padre, SIGUSR1);
        for (;;);
    }
    else
    {
        /* Completa el código del padre para enviar las señales SIGUSR2 y SIGTERM al hijo,
        y espera a que acabe con wait */
        kill(hijo, SIGUSR2);
        printf("%d\n",sleep(1));
        kill(hijo, SIGTERM);
        wait(&stat_loc);
        if (WIFSIGNALED(stat_loc) && WTERMSIG(stat_loc) == SIGTERM)
        {
            printf("El hijo se ha matado adecuadamente.\n");
            exit(EXIT_SUCCESS);
        }
        else if (WIFSIGNALED(stat_loc))
        {
            fprintf(stderr, "El hijo no ha salido con la señal esperada, sino que ha salido con: %s.\n", strsignal(WTERMSIG(stat_loc)));
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "Se ha obtenido un valor de retorno inesperado del hijo.\n");
            exit(EXIT_FAILURE);
        }
    }
}
static void gestion(int numero_de_senhal)
{ /* Funcion de gestion de señales*/
    switch (numero_de_senhal)
    {
    case SIGUSR1: /*Entra señal SIGUSR1*/
        printf("Señal tipo 1 recibida. Soy %d\n", getpid());
        break; /*Completa para el resto de las señales usadas*/
        return;
    case SIGUSR2: /*Entra señal SIGUSR2*/
        printf("Señal tipo 2 recibida. Soy %d\n", getpid());
        break; /*Completa para el resto de las señales usadas*/
        return;
    }
}