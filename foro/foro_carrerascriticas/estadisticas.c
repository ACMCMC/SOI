#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void generar_archivos(int tam)
{
    int i;
    FILE *fp;
    char tam_string[13], nombre_arch_escr[20], nombre_arch_lect[20];
    sprintf(tam_string, "%d", tam);
    strcpy(nombre_arch_escr, "arch_escr");
    strncat(nombre_arch_escr, tam_string, 12);
    strcpy(nombre_arch_lect, "arch_lect");
    strncat(nombre_arch_lect, tam_string, 12);
    fp = fopen(nombre_arch_escr, "w");
    if (fp == NULL) {  //Generamos un archivo vacío
        perror("Error abriendo el archivo de escritura");
        exit(EXIT_FAILURE);
    }
    if (fclose(fp)) {
        perror("Error cerrando el archivo de escritura");
        exit(EXIT_FAILURE);
    }
    fp = fopen(nombre_arch_lect, "w");
    if (fp == NULL) {
        perror("Error abriendo el archivo de lectura");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < tam; i++)
    {
        fprintf(fp, "%c", (i % ('z' + 1 - 'a')) + 'a');
    }

    if (fclose(fp)) {
        perror("Error cerrando el archivo de lectura");
        exit(EXIT_FAILURE);
    }
}

void run_test(int tam, int num_inicial_proc, int incremento_proc, int num_final_proc)
{
    pid_t pid;
    int stat_loc, num_actual_proc;
    char tam_string[13], num_actual_proc_string[13];
    sprintf(tam_string, "%d", tam);
    printf("Haciendo test con tamano de archivo %d, num inicial de procesos %d, incremento %d, num final %d\n", tam, num_inicial_proc, incremento_proc, num_final_proc);
    generar_archivos(tam);
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
            if (execlp("carrerascriticas.out", "carrerascriticas.out", strcat("arch_escr", tam_string), strcat("arch_lect", tam_string), num_actual_proc_string, (char *)NULL) < 0) {
                perror("Error ejecutando execlp");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            wait(&stat_loc);
        }
    }
}

int main(int argc, char **argv)
{
    int i, tam_inicial, tam_final, incremento_tam, stat_loc, num_inicial_proc, incremento_proc, num_final_proc;
    pid_t pid = -1;

    if (argc != 7)
    {
        fprintf(stderr, "Deben especificarse: tamaño inicial, incremento de tamaño, tamaño final, numero inicial de procesos, incremento del numero de procesos, numero final de procesos\n");
        exit(EXIT_SUCCESS);
    }

    tam_inicial = atoi(argv[1]);
    incremento_tam = atoi(argv[2]);
    tam_final = atoi(argv[3]);
    num_inicial_proc = atoi(argv[4]);
    incremento_proc = atoi(argv[5]);
    num_final_proc = atoi(argv[6]);

    for (i = tam_inicial; (i <= tam_final) && (pid != 0); i += incremento_tam)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("Error ejecutando fork(). Abortando.");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            run_test(i, num_inicial_proc, incremento_proc, num_final_proc);
        }
    }

    if (pid != 0)
    {
        for (i = tam_inicial; (i <= tam_final) && (pid != 0); i += incremento_tam)
        {
            wait(&stat_loc);
        }
    }

    return (EXIT_SUCCESS);
}