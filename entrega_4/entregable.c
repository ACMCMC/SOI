
#define NUM_CORES 4
#define NUM_HILOS 2

void realizar_calculos();
void realizar_calculo(int i);

int j; // El id de este hilo

int main() {

}

void realizar_calculos() {
    int i;
    for (i = j; i < d; i+=NUM_HILOS)
        realizar_calculo(i);
}

void realizar_calculo(int i) {

}