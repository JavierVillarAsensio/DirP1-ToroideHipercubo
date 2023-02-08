#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>


#define DIMENSION 8
#define NODOS_TOROIDE DIMENSION*DIMENSION

#define ARRIBA 0
#define ABAJO 1
#define DERECHA 2
#define IZQUIERDA 3

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define ARCHIVO "datos.dat"

void coger_numeros(double *datos);
void enviar_numeros(double *datos);
void coger_vecinos(int vecinos[], int rango);
void toroide(int rango, double mi_numero);

int barrera = 0;

int main(int argc, char *argv[]){
    double *datos = malloc(NODOS_TOROIDE * sizeof(double));
    int rango, tam;
    double mi_numero;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rango);
    MPI_Comm_size(MPI_COMM_WORLD,&tam);

    if (rango == 0){
        coger_numeros(datos);
        enviar_numeros(datos);
    }

    MPI_Bcast(&barrera,1,MPI_INT,0,MPI_COMM_WORLD); 
        
    MPI_Recv(&mi_numero, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);

    toroide(rango, mi_numero);
    
    MPI_Finalize();
    free(datos);

    return EXIT_SUCCESS;
}

void coger_numeros(double *datos){
    FILE *arhivo;
    char *copia = malloc(1500 * sizeof(char));
    char *numero;
    int *posicion = 0;

    if ((arhivo = fopen(ARCHIVO, "r")) == 0){
        perror("Error abriendo archivo");
        exit(EXIT_FAILURE);
    }else{
        fscanf(arhivo, "%s", copia);

        datos[(*posicion)++] = atof(strtok(copia,","));

        while((numero = strtok(NULL, ",")) != NULL)
            datos[(*posicion)++] = atof(numero);
    }

    fclose(arhivo);
    free(copia);
}

void enviar_numeros(double *datos){    
    for(int i=0; i < NODOS_TOROIDE; i++)
        MPI_Send(&datos[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
}

void coger_vecinos(int vecinos[], int rango){
    int columna = rango % DIMENSION;
    int fila = rango / DIMENSION;

    switch(columna){
        case 0: 
            vecinos[IZQUIERDA] = (DIMENSION * (fila+1))-1;
            vecinos[DERECHA] = rango + 1;
            break;
        case DIMENSION-1:
            vecinos[IZQUIERDA] = rango -1;
            vecinos[DERECHA] = fila * DIMENSION;
            break;
        default:
            vecinos[IZQUIERDA] = rango -1;
            vecinos[DERECHA] = rango + 1;  
            break;
    }

    switch (fila){
        case 0:
            vecinos[ABAJO] = rango + DIMENSION;
            vecinos[ARRIBA] = (DIMENSION-1) * DIMENSION + rango;
            break;
        case DIMENSION-1:
            vecinos[ABAJO] = rango % DIMENSION;
            vecinos[ARRIBA] = rango - DIMENSION; 
            break;
        default:
            vecinos[ABAJO] = rango + DIMENSION;
            vecinos[ARRIBA] = rango - DIMENSION; 
            break;
    }
}

void toroide(int rango, double mi_numero){
    int vecinos[4];
    double numero_recibido;

    coger_vecinos(vecinos, rango);

    for(int i=1; i < DIMENSION; i++){
        MPI_Send(&mi_numero, 1, MPI_DOUBLE, vecinos[ABAJO], 1, MPI_COMM_WORLD);
        MPI_Recv(&numero_recibido,1,MPI_DOUBLE,vecinos[ARRIBA],1,MPI_COMM_WORLD,NULL);
        mi_numero = MIN(mi_numero, numero_recibido);

        MPI_Send(&mi_numero, 1, MPI_DOUBLE, vecinos[DERECHA], 1, MPI_COMM_WORLD);
        MPI_Recv(&numero_recibido,1,MPI_DOUBLE,vecinos[IZQUIERDA],1,MPI_COMM_WORLD,NULL);
        mi_numero = MIN(mi_numero, numero_recibido);

    }

    if(rango == 0)
        printf("El valor mínimo en toroide es: %.2f\n",mi_numero);   
}
