#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void generar_estadisticas(int tam, int num_procesos, char *nombre_arch_lect, char *nombre_arch_escr, FILE *arch_estadisticas)
{
    int num_diferencias = 0;
    FILE *arch_lect, *arch_escr;
    char lectura, escritura;

    arch_lect = fopen(nombre_arch_lect, "r");
    if (arch_lect == NULL)
    { //Generamos un archivo vacío
        perror("Error abriendo el archivo de lectura en generar_estadisticas");
        exit(EXIT_FAILURE);
    }
    arch_escr = fopen(nombre_arch_escr, "r");
    if (arch_escr == NULL)
    { //Generamos un archivo vacío
        perror("Error abriendo el archivo de escritura en generar_estadisticas");
        exit(EXIT_FAILURE);
    }

    while (!feof(arch_lect) && !feof(arch_escr))
    {
        fscanf(arch_lect, "%c", &lectura);
        fscanf(arch_escr, "%c", &escritura);
        if (lectura != escritura)
        {
            num_diferencias++;
        }
    }

    fprintf(arch_estadisticas, "%d;%d;%d\n", tam, num_procesos, num_diferencias);

    if (fclose(arch_lect))
    {
        perror("Error cerrando el archivo de lectura en generar_estadisticas");
        exit(EXIT_FAILURE);
    }
    if (fclose(arch_escr))
    {
        perror("Error cerrando el archivo de escritura en generar_estadisticas");
        exit(EXIT_FAILURE);
    }
}

void generar_archivos(int tam, char *nombre_arch_lect, char *nombre_arch_escr)
{
    int i;
    FILE *fp;

    fp = fopen(nombre_arch_escr, "w");
    if (fp == NULL)
    { //Generamos un archivo vacío
        perror("Error abriendo el archivo de escritura");
        exit(EXIT_FAILURE);
    }
    if (fclose(fp))
    {
        perror("Error cerrando el archivo de escritura");
        exit(EXIT_FAILURE);
    }
    fp = fopen(nombre_arch_lect, "w");
    if (fp == NULL)
    {
        perror("Error abriendo el archivo de lectura");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < tam; i++)
    {
        fprintf(fp, "%c", (i % ('z' + 1 - 'a')) + 'a');
    }

    if (fclose(fp))
    {
        perror("Error cerrando el archivo de lectura");
        exit(EXIT_FAILURE);
    }
}

void run_test(int tam, int num_inicial_proc, int incremento_proc, int num_final_proc, FILE *arch_estadisticas)
{
    pid_t pid;
    int stat_loc, num_actual_proc;
    char num_actual_proc_string[13], tam_string[13], nombre_arch_escr[20], nombre_arch_lect[20];
    sprintf(tam_string, "%d", tam);
    printf("Haciendo test con tamano de archivo %d, num inicial de procesos %d, incremento %d, num final %d\n", tam, num_inicial_proc, incremento_proc, num_final_proc);

    sprintf(tam_string, "%d", tam);
    strcpy(nombre_arch_escr, "arch_escr");
    strncat(nombre_arch_escr, tam_string, 12);
    strcpy(nombre_arch_lect, "arch_lect");
    strncat(nombre_arch_lect, tam_string, 12);
    generar_archivos(tam, nombre_arch_lect, nombre_arch_escr);

    for (num_actual_proc = num_inicial_proc; num_actual_proc <= num_final_proc; num_actual_proc += incremento_proc)
    {
        sprintf(num_actual_proc_string, "%d", num_actual_proc);
        pid = fork();
        if (pid < 0)
        {
            perror("Error ejecutando fork(). Abortando.");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            if (execl("carrerascriticas.out", "carrerascriticas.out", nombre_arch_lect, nombre_arch_escr, num_actual_proc_string, (char *)NULL) < 0)
            {
                perror("Error ejecutando execlp");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (wait(&stat_loc) == -1)
            {
                perror("Error esperando por el proceso");
                exit(EXIT_FAILURE);
            }
            else if (!WIFEXITED(stat_loc))
            {
                fprintf(stderr, "Error en el proceso, no ha salido normalmente");
                exit(EXIT_FAILURE);
            }
            else if (WEXITSTATUS(stat_loc) != EXIT_SUCCESS)
            {
                fprintf(stderr, "Error en el proceso, no ha devuelto EXIT_SUCCESS");
                exit(EXIT_FAILURE);
            }
            else
            {
                generar_estadisticas(tam, num_actual_proc, nombre_arch_lect, nombre_arch_escr, arch_estadisticas);
            }
        }
    }
}

int main(int argc, char **argv)
{
    int i, tam_inicial, tam_final, incremento_tam, stat_loc, num_inicial_proc, incremento_proc, num_final_proc;
    pid_t pid = -1;

    FILE *arch_estadisticas;

    if (argc != 7)
    {
        fprintf(stderr, "Deben especificarse: tamaño inicial, incremento de tamaño, tamaño final, numero inicial de procesos, incremento del numero de procesos, numero final de procesos\n");
        exit(EXIT_SUCCESS);
    }

    printf("PID del padre: %d\n", getpid());

    tam_inicial = atoi(argv[1]);
    incremento_tam = atoi(argv[2]);
    tam_final = atoi(argv[3]);
    num_inicial_proc = atoi(argv[4]);
    incremento_proc = atoi(argv[5]);
    num_final_proc = atoi(argv[6]);

    arch_estadisticas = fopen("estadisticas.txt", "w");
    if (arch_estadisticas == NULL)
    {
        perror("Error abriendo el archivo de salida de estadísticas");
        return (EXIT_FAILURE);
    }

    for (i = tam_inicial; (i <= tam_final) && (pid != 0); i += incremento_tam)
    {
        if (i > 0)
        {
            pid = fork();
            if (pid < 0)
            {
                perror("Error ejecutando fork(). Abortando.");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                run_test(i, num_inicial_proc, incremento_proc, num_final_proc, arch_estadisticas);
            }
            else
            {
                if ((pid = waitpid(pid, &stat_loc, 0)) == -1)
                {
                    perror("Error esperando por el proceso");
                    exit(EXIT_FAILURE);
                }
                else if (!WIFEXITED(stat_loc))
                {
                    fprintf(stderr, "Error en el proceso, no ha salido normalmente");
                    exit(EXIT_FAILURE);
                }
                else if (WEXITSTATUS(stat_loc) != EXIT_SUCCESS)
                {
                    fprintf(stderr, "Error en el proceso, no ha devuelto EXIT_SUCCESS");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    if (fclose(arch_estadisticas))
    {
        perror("Error cerrando el archivo de salida de estadísticas");
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
