# Explicación de lo que ocurre

Se crean los hilos correctamente, y el valor de i se propaga de forma adecuada a los hilos que creamos. Lo que sucede es que, de vez en cuando, el hilo principal acaba antes de los hilos que crea, y eso implica que a veces no llegan a trabajar todos los que deberían. Esto se puede solucionar haciendo que el hilo principal espere a que acabe el resto antes de finalizar. El siguiente fragmento de código maneja esta cuestión:

```c
for (i = 0; i < NUMBER_THREADS; i++) {
    if (pthread_join(p[i], NULL)) {
        perror("Error ejecutando pthread_join");
    }
}
```