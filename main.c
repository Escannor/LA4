#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Regresa una matriz de [jugadores][5] con las cartas
int ** GetCards(int num_jugadores){   //2-4 jugadores
    srand(time(NULL)); // randomize seed

    int **r;
    int i, j, a;

    r = malloc(sizeof(int*) * num_jugadores);

    for(i = 0; i < num_jugadores; i++) {
        r[i] = malloc(sizeof(int*) * 5);
    }

    int cartas[13] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

    for ( i = 0; i < num_jugadores; i++) {
        for ( j = 0; j < 5; j++) {
            do {
                a = rand() % 13;
                r[i][j] = a+2;
            } while ( cartas[a] == 0 );

            cartas[a] --;
            printf("[%d] a=%d", r[i][j], a);
            printf("\n");
        }
        printf("\n");
    }

    return r;
}

/*

int ** GetArray2(){
    int array[2][2];
    array[0][0] = 1;
    array[0][1] = 2;
    array[1][0] = 3;
    array[1][1] = 4;

    return array[0][0];
}*/


int main() {

    int **cards;
    int i, j;

    unsigned short int num_jugadores = 3;

    cards = GetCards(num_jugadores);

    for ( i = 0; i < num_jugadores; i++) {
        for ( j = 0; j < 5; j++) {
            printf("[%d]", cards[i][j]);
        }
        printf("\n");
    }


    return 0;

}


/*
int **matrix_sum(int matrix1[][3], int matrix2[][3]){
    int i, j;
    int **matrix3;
    matrix3 = malloc(sizeof(int*) * 3);

    for(i = 0; i < 3; i++) {
        matrix3[i] = malloc(sizeof(int*) * 3);
    }

    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            matrix3[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    return matrix3;
}



int main(){
    int x[3][3], y[3][3];
    int **a;
    int i,j;


    printf("Enter the matrix1: \n");
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            scanf("%d",&x[i][j]);
        }
    }


    printf("Enter the matrix2: \n");
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            scanf("%d",&y[i][j]);
        }
    }


    a = matrix_sum(x,y); //asigning
    printf("The sum of the matrix is: \n");
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            printf("%d",a[i][j]);
            printf("\t");
        }
        printf("\n");
    }

    //free the memory
    for(i = 0; i < 3; i++) {
        free(a[i]);
    }
    free(a);
    return 0;
}*/



