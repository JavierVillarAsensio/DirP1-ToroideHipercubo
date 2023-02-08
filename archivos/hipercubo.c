#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>


#define DIMENSION 3
#define NODOS_CUBO (int)pow(2,DIMENSION)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define XOR(x, y) (x ^ y)

#define ARCHIVO "datos.dat"

void coger_numeros(double *datos);
void enviar_numeros(double *datos);
void hipercubo(int rango, double mi_numero);

int barrera = 0;

int main(int argc, char *argv[]){
    double *datos = malloc(NODOS_CUBO * sizeof(double));
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

    hipercubo(rango, mi_numero);
    
    MPI_Finalize();
    free(datos);

    return EXIT_SUCCESS;
}

void coger_numeros(double *datos){
    FILE *archivo;
    char *copia = malloc(1500 * sizeof(char));
    char *numero;
    int *posicion = 0;

    if ((archivo = fopen(ARCHIVO, "r")) == 0){
        perror("Error abriendo archivo");
        exit(EXIT_FAILURE);
    }else{
        fscanf(archivo, "%s", copia);

        datos[(*posicion)++] = atof(strtok(copia,","));

        while((numero = strtok(NULL, ",")) != NULL)
            datos[(*posicion)++] = atof(numero);
    }

    fclose(archivo);
    free(copia);
}

void enviar_numeros(double *datos){
    for(int i=0; i < NODOS_CUBO; i++)
        MPI_Send(&datos[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
}

void hipercubo(int rango, double mi_numero){
    double numero_recibido;
    int vecino;

    for(int i=0; i < DIMENSION; i++){
        vecino = XOR(rango, (int)pow(2,i));

        MPI_Send(&mi_numero, 1, MPI_DOUBLE, vecino, 1, MPI_COMM_WORLD);
        MPI_Recv(&numero_recibido, 1, MPI_DOUBLE, vecino, 1,MPI_COMM_WORLD, NULL);

        mi_numero = MAX(mi_numero, numero_recibido);
    }

    if(rango == 0)
        printf("El mayor número en hipercubo es: %.2f\n",mi_numero);   
}
