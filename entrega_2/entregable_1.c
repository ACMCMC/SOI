#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int var_global;

int main()
{
    int var_local;
    int *var_dinamica;
    var_global = 1;
    var_local = 2;
    var_dinamica = (int *)malloc(sizeof(int));
    *(var_dinamica) = 3;
    pid_t proc_id;
    printf("Antes del fork():\n\tvar_global (%p)=%d\n\tvar_local (%p)=%d\n\tvar_dinamica (%p)=%d\n", (void *)&var_global, var_global, (void *)&var_local, var_local, (void *)var_dinamica, *var_dinamica);

    FILE *arch = fopen("salida.txt", "w+");
    if (arch == NULL)
    {
        perror("Error ejecutando fopen(). Abortando.");
        exit(EXIT_FAILURE);
    }

    proc_id = fork();
    if (((int)proc_id) == -1)
    {
        perror("Error al ejecutar fork(). Abortando.");
        fclose(arch);
        exit(EXIT_FAILURE);
    }

    printf("ID de este proceso: %d\nID del padre: %d\n", (int) getpid(), (int) getppid());
    if (proc_id == 0)
    { // Este es el proceso hijo
        var_global += 10;
        var_local += 10;
        *var_dinamica += 10;
    }
    else
    { // Este proceso es el padre
        var_global += 5;
        var_local += 5;
        *var_dinamica += 5;
        sleep(1); // Metemos un sleep() para que el hijo no se quede sin padre antes de tiempo
    }
    printf("Después del fork():\n\tvar_global (%p)=%d\n\tvar_local (%p)=%d\n\tvar_dinamica (%p)=%d\n", (void *)&var_global, var_global, (void *)&var_local, var_local, (void *)var_dinamica, *var_dinamica);
    fclose(arch);
    exit(EXIT_SUCCESS);
}
