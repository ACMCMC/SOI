#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int var_global; // Variable global para testear usando el fork()

// Se puede usar ps -v -g (PID DEL PADRE) para ver información de los procesos

int main()
{
    int var_local; // Una variable local para ver cómo parece seguir siendo la misma con el fork()
    int *var_dinamica; // Una variable dinámica reservada con malloc()
    FILE *arch1; // Vamos a abrir dos archivos, el primero antes del fork()
    FILE *arch2; // Este archivo lo abrirán el padre y el hijo por separado
    var_global = 1;
    var_local = 2;
    var_dinamica = (int *)malloc(sizeof(int)); // Reservamos memoria para la variable dinámica
    *(var_dinamica) = 3;

    pid_t proc_id; // Aquí guardaremos el resultado del fork(). Será el ID del proceso hijo en el padre, y 0 en el hijo

    printf("Antes del fork():\n\tvar_global (%p)=%d\n\tvar_local (%p)=%d\n\tvar_dinamica (%p)=%d\n", (void *)&var_global, var_global, (void *)&var_local, var_local, (void *)var_dinamica, *var_dinamica); // Imprimimos los valores y posiciones en memoria virtual de las variables antes del fork()

    arch1 = fopen("salida.txt", "w+");
    if (arch1 == NULL)
    {
        perror("Error ejecutando fopen(). Abortando.");
        exit(EXIT_FAILURE);
    }

    setenv("variable_prueba", "valor antes del fork()", 0); // Escribimos una nueva variable de entorno (sin sobreescribir): "variable_prueba" -> "valor"
    printf("Acabamos de crear una nueva variable de entorno\n");

    proc_id = fork();
    if (((int)proc_id) == -1)
    {
        perror("Error al ejecutar fork(). Abortando.");
        fclose(arch1);
        exit(EXIT_FAILURE);
    }

    printf("ID de este proceso: %d\nID del padre: %d\n", (int)getpid(), (int)getppid()); // Imprimimos información sobre este proceso

    fprintf(arch1, "%s\n", ((int)proc_id) == 0 ? "hijo" : "padre"); // Escribimos en un fichero, esto será una carrera crítica

    if (proc_id == 0)
    { // Este es el proceso hijo
    printf("HIJO\n");
        sleep(10); // Metemos 10 segundos de espera adicionales para que el hijo se quede huérfamno durante 10 segundos (el padre acabará antes casi seguro)
        var_global += 10; // Sumamos un valor distinto a todas sus variables para ver cómo se guardan en posiciones de memoria física diferentes aunque en memoria virtual parezcan las mismas posiciones
        var_local += 10;
        *var_dinamica += 10;
        fclose(arch1); // Cerramos el archivo salida.txt (con file descriptor 3)
        arch2 = fopen("salida2.txt", "w+"); // Abrimos otro archivo salida2.txt, recibirá el file descriptor 3 MIENTRAS en el padre ese mismo file descriptor sigue apuntando a salida.txt
        if (arch2 == NULL)
        {
            perror("Error ejecutando fopen(). Abortando.");
            fclose(arch1);
            exit(EXIT_FAILURE);
        }

        setenv("variable_prueba", "esto es del hijo\n", 1); // Cambiamos el valor de la variable de entorno
        printf("Acabamos de cambiar el valor de la variable de entorno en el hijo\n");
    }
    else
    { // Este proceso es el padre
    printf("PADRE\n");
        var_global += 5; // Sumamos un valor distinto a todas sus variables para ver cómo se guardan en posiciones de memoria física diferentes aunque en memoria virtual parezcan las mismas posiciones
        var_local += 5;
        *var_dinamica += 5;
        arch2 = fopen("salida2.txt", "w+"); // En el caso del padre, como no hemos cerrado salida.txt, al abrir este archivo, arch2 tendrá file descriptor 4
        if (arch2 == NULL)
        {
            perror("Error ejecutando fopen(). Abortando.");
            fclose(arch1);
            exit(EXIT_FAILURE);
        }
        setenv("variable_prueba", "esto es del padre\n", 1); // Cambiamos el valor de la variable de entorno
        printf("Acabamos de cambiar el valor de la variable de entorno en el padre\n");
    }
    printf("Después del fork():\n\tvar_global (%p)=%d\n\tvar_local (%p)=%d\n\tvar_dinamica (%p)=%d\n", (void *)&var_global, var_global, (void *)&var_local, var_local, (void *)var_dinamica, *var_dinamica); // Imprimimos los valores y posiciones en memoria virtual de las variables después del fork()

    printf("Descriptor del archivo 1: %d\nDescriptor del archivo 2: %d\n", fileno(arch1), fileno(arch2)); // Imprimimos los descriptores de archivo. En el hijo, el archivo 2 tendrá descriptor 3 porque ya cerramos el archivo 1, que abrió el padre. En el padre, serán 3 y 4, porque ambos estarán abiertos en este punto. Los descriptores locales de archivo, que mapean a una tabla global en el kernel, también se copian en el fork(), pero no se comparten después.

    sleep(20); // Metemos un sleep() para mantener ambos procesos en ejecución durante un tiempo

    fprintf(arch2, "%s\n", ((int)proc_id) == 0 ? "hijo" : "padre"); // Escribimos en salida2.txt. Ya no será una carrera crítica porque hay mucha diferencia de tiempo entre cuándo se ejecuta estar orden en el padre y en el hijo. Solo veremos escrito "hijo" porque no se comparte el puntero en el fichero, entre padre e hijo, ya que los hemos abierto con open() distintos.

    printf("Valor de la variable de entorno: %s\n", getenv("variable_prueba")); // Imprimimos la variable de entorno que establecimos arriba

    printf("UID: %d; UID efectivo: %d, GID: %d\n", (unsigned int) getuid(), (unsigned int) geteuid(), (unsigned int) getgid());
    
    printf("El %s sale.\n", ((int)proc_id) == 0 ? "hijo" : "padre");
    fclose(arch1); // Cerramos los archivos
    fclose(arch2);
    unsetenv("variable_prueba"); // Eliminamos la variable de entorno, primero en el padre y después en el hijo
    exit(EXIT_SUCCESS);
}
