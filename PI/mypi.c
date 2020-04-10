#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

#define ROOT 0
#define MSG_TAG 100

int main(int argc,char **argv)
{
    int size,tid;
    int R = 10000;

    MPI_Init(&argc, &argv); 

    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Comm_rank( MPI_COMM_WORLD, &tid );

    srand( tid );

    int res;
    int pkt = strtol(argv[1], NULL, 10); 

    if ( tid == 0 ) {
	MPI_Status status;
        int i;
        long double pi;
        int insideAllCircles = 0;
	// pewnie jakiś for tutaj
    for (i = 1; i<size; i++)
    {
        MPI_Recv( &res, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        insideAllCircles += res;
    }
    insideAllCircles *= 4;
    pi = (long double)insideAllCircles / ((size - 1) * pkt * 2);
        printf("Przybliżenie pi po zebraniu danych od %d procesów wynosi %llf\n", i-1, pi);
    } else {
        int insideCircle = 0;
        for (int i = 0; i < pkt; i++)
        {
            int x = rand() % R;
            int y = rand() % R;
            if((x*x + y*y) < (R*R))
            {
                insideCircle ++;
            }
        }
        srand( tid + size);
        for (int i = 0; i < pkt; i++)
        {
            int x = rand() % R;
            int y = rand() % R;
            if((x*x + y*y) < (R*R))
            {
                insideCircle ++;
            }
        }

	MPI_Send( &insideCircle, 1, MPI_INT, ROOT, MSG_TAG, MPI_COMM_WORLD );
    }

    MPI_Finalize();
}
