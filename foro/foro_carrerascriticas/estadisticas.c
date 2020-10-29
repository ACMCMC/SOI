#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void generar_archivos(int tam) {
    int i;
    FILE* fp;
    char tamano[13];
    sprintf(tamano, "%d", tam);
    fclose(fopen(strcat("arch_escr", tamano), "w"));
    fp = fopen(strcat("arch_lect", tamano), "w");

    for (i = 0; i < tam; i++) {
        fprintf(fp, "%c", (i % ('z' - 'a')) + 'a');
    }

    fclose(fp);
}

void run_test(int tam) {
    pid_t pid;
    int stat_loc;
    char tamano[13];
    sprintf(tamano, "%d", tam);
    generar_archivos(tam);
    pid = fork();
    if (pid < 0) {
            perror("Error ejecutando fork(). Abortando.");
            exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execlp("carrerascriticas.out", "carrerascriticas.out", strcat("arch_escr", tamano), strcat("arch_lect", tamano), (char*)NULL);
    } else {
        wait(&stat_loc);
    }
}

int main(int argc, char** argv) {
    int i, tam_inicial, tam_final, incremento, stat_loc;
    pid_t pid = -1;

    if (argc != 4) {
        fprintf(stderr, "Deben especificarse: tamaño inicial, incremento, tamaño final\n");
        exit(EXIT_SUCCESS);
    }

    tam_inicial = atoi(argv[1]);
    incremento = atoi(argv[2]);
    tam_final = atoi(argv[3]);

    for (i = tam_inicial; (i < tam_final) && (pid != 0); i += incremento) {
        pid = fork();
        if (pid < 0) {
            perror("Error ejecutando fork(). Abortando.");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            run_test(tam_inicial + incremento*i);
        }
    }

    if (pid != 0) {
        for (i = tam_inicial; (i < tam_final) && (pid != 0); i += incremento) {
            wait(&stat_loc);
        }
    }

    return(EXIT_SUCCESS);
}