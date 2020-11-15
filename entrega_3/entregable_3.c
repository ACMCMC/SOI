#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>

/*
 _______ .__   __. .___________..______       _______   _______      ___      .______    __       _______     ____   
|   ____||  \ |  | |           ||   _  \     |   ____| /  _____|    /   \     |   _  \  |  |     |   ____|   |___ \  
|  |__   |   \|  | `---|  |----`|  |_)  |    |  |__   |  |  __     /  ^  \    |  |_)  | |  |     |  |__        __) | 
|   __|  |  . `  |     |  |     |      /     |   __|  |  | |_ |   /  /_\  \   |   _  <  |  |     |   __|      |__ <  
|  |____ |  |\   |     |  |     |  |\  \----.|  |____ |  |__| |  /  _____  \  |  |_)  | |  `----.|  |____     ___) | 
|_______||__| \__|     |__|     | _| `._____||_______| \______| /__/     \__\ |______/  |_______||_______|   |____/  
                                                                                                                     
Aldán Creo Mariño, SOI 2020/21
*/

static void gestorAlarma(int senal);

double valorRaiz; // Aquí vamos guardando los valores del cálculo
int numAlarmas = 0, maxAlarmas;
long numActual;

int proximoPrimo(int primoAnterior);

int main()
{
    printf("Cuantas veces quieres capturar SIGALRM? ");
    scanf("%d", &maxAlarmas);
    if (signal(SIGALRM, gestorAlarma)==SIG_ERR) {
        perror("Error en signal()");
        exit(EXIT_FAILURE);
    }
    alarm(1); // Establecemos una nueva alarma dentro de 1 segundo. No hay que comprobar nada porque la documentación dice que no puede generar error
    for (numActual = 2; numAlarmas < maxAlarmas; numActual=proximoPrimo(numActual)) {
        valorRaiz = sqrt(numActual);
    }
    exit(EXIT_SUCCESS);
}

int proximoPrimo(int primoAnterior)
{
    int primo = 0;           // Flag que se pone a 1 cuando el número es primo
    int num = primoAnterior % 2 == 1 ? primoAnterior : primoAnterior - 1; // El número que estamos comprobando. Si la entrada es un numero par, le sumamos 1 para buscar el siguiente primo. Si es impar, sumamos 2, porque nigún par es primo.
    int i;
    while (!primo)
    {
        num+=2;
        primo = 1;
        for (i = 2; i <= num/2 && primo; i++) // Mientras no encontremos un divisor del número
        {
            if ((num % i) == 0)
            {
                primo = 0;
            }
        }
    }
    return num;
}

static void gestorAlarma(int senal)
{
    if (senal == SIGALRM) // Siempre se va a cumplir
        printf("%d. sqrt(%ld): %lf\n", numAlarmas+1, numActual, valorRaiz);
    if (++numAlarmas < maxAlarmas)
        alarm(1); // Establecemos una nueva alarma dentro de 1 segundo. No hay que comprobar nada porque la documentación dice que no puede generar error
}
