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
int numAlarmas = 0, maxAlarmas; // Contadores: el número de alarmas que se han recibido; el número máximo de alarma que vamos a recibir antes de acabar
long numActual; // El valor actual del que vamos a hacer la raíz cuadrada

int proximoPrimo(int primoAnterior); // Función que, dado un número, nos devuelve el siguiente primo a él. Lo hace por una estrategia de fuerza bruta, pero no estamos buscando rendimiento.

int main()
{
    printf("Cuantas veces quieres capturar SIGALRM? "); // Le preguntamos al usuario antes de empezar a calcular, cuántas alarmas quiere recibir en total
    scanf("%d", &maxAlarmas);
    if (signal(SIGALRM, gestorAlarma)==SIG_ERR) { // La función que va a gestionar las alarmas (SIGALARM) es gestorAlarma
        perror("Error en signal()");
        exit(EXIT_FAILURE);
    }
    alarm(1); // Establecemos una nueva alarma dentro de 1 segundo. No hay que comprobar nada porque la documentación dice que no puede generar error. A partir de aquí, cada nueva alarma se establecerá desde gestorAlarma.
    for (numActual = 2; numAlarmas < maxAlarmas; numActual=proximoPrimo(numActual)) {
        valorRaiz = sqrt(numActual); // Vamos calculando los valores que nos pide el PDF
    }
    exit(EXIT_SUCCESS);
}

int proximoPrimo(int primoAnterior) // Función que calcula el siguiente primo de un número
{
    int primo = 0;           // Flag que se pone a 1 cuando el número es primo
    int num = primoAnterior % 2 == 1 ? primoAnterior : primoAnterior - 1; // El número que estamos comprobando. Si la entrada es un numero par, le sumamos 1 para buscar el siguiente primo. Si es impar, sumamos 2, porque nigún par es primo.
    int i;
    while (!primo) // Mientras no encontremos un primo...
    {
        num+=2; // Los primos van de 2 en 2, por lo menos, porque los números pares no pueden ser primos (excepto el 2, pero esta función calcula primos a partir del 2)
        primo = 1; // En principio, el número que estamos comprobando es primo. Vamos a calcular sus divisores hasta que encontremos alguno, en ese caso ya no será primo
        for (i = 2; i <= num/2 && primo; i++) // Mientras no encontremos un divisor del número
        {
            if ((num % i) == 0)
            {
                primo = 0; // Este número no es primo, cambiamos el flag y buscamos en el siguiente posible número
            }
        }
    }
    return num;
}

static void gestorAlarma(int senal) // Gestiona la señal SIGALRM
{
    if (senal == SIGALRM) // Siempre se va a cumplir
        printf("%d. sqrt(%ld): %lf\n", numAlarmas+1, numActual, valorRaiz);
    if (++numAlarmas < maxAlarmas)
        alarm(1); // Establecemos una nueva alarma dentro de 1 segundo. No hay que comprobar nada porque la documentación dice que no puede generar error
}
