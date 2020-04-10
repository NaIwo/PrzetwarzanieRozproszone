#include <mpi.h>
#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NOTFIND 1
#define FIND 2
#define len 7


/* 4 literowe
 * aa5UYq6trT5u.
 * bahAZ9Hk7SCf6
 * ddoo3WocSpthU
 * jkmD2RlhoMyuA
 * zzm4NUIIb7VIk
 * kkv864igyJC9o
 *
 * 5 literowe
 * aaSPfLTmjh3fU
 *
 * 6 literowe
 * aaLTdQr7DyHuU
 * */

char *stro="aaLTdQr7DyHuU";


int main(int argc, char **argv)
{
         MPI_Init(&argc, &argv);

         int size,rank;

        MPI_Status status;

         MPI_Comm_rank(MPI_COMM_WORLD, &rank);
         MPI_Comm_size(MPI_COMM_WORLD, &size);

         char cmp[len]={0};
         char odp[len]={0};

         char salt[3]={0};
         salt[0]=stro[0];
         salt[1]=stro[1];

        int a = 97;
        int z = 122;

        if (rank != 0)
        {
                int liczba = 0;
                MPI_Request request;
                int received;
                int ready = 0;
                char temp[len]={0};
                MPI_Irecv(&received, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &request);
                for(int pr = 0; pr <= 24; pr+= 8)
                {
                                        for(int i = a; i <= z ; i++)
                                        {
                                                        for(int j = a; j <= z; j++)
                                                        {
                                                                        for(int k = a; k <= z; k++)
                                                                        {
                                                                                for(int n = a; n <= z; n++)
                                                                                {
                                                                                        for(int m = a; m <= z; m++)
                                                                                        {
                                                                                        MPI_Test (&request, &ready, MPI_STATUS_IGNORE); // czy &?
                                                                                        if(ready){
                                                                                                        printf("OK, Koncze. \n");
                                                                                                        MPI_Finalize();
                                                                                                        exit(0);
                                                                                        }
                                                                                        temp[0] = rank + 96 + pr;
                                                                                        temp[1] = i;
                                                                                        temp[2] = j;
                                                                                        temp[3] = k;
                                                                                        temp[4] = n;
                                                                                        temp[5] = m;


                                                                                        if(rank + 96 + pr > z)
                                                                                        {
                                                                                                MPI_Finalize();
                                                                                                exit(0);
                                                                                        }

                                                                                        char * x=crypt(temp, salt);
                                                                                        if ((strcmp(x,stro))==0)
                                                                                        {
                                                                                                        printf("Udalo sie: %s %s %s\n", temp, x, stro);

                                                                                                        int end = 0;
                                                                                                        MPI_Send (&end, 1, MPI_INT, 0, FIND, MPI_COMM_WORLD); // czy &?

                                                                                                        MPI_Finalize();
                                                                                                        exit(0);
                                                                                        }

                                                                        }
                                                                                }
                                                                        }
                                                        }
                                        }
                                }

        }
        else {
                int liczba;
                int end = 1;
                printf("Let's start this s*it\n");
                        MPI_Recv (&end, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // czy &
        if(end == 0)
        {
                for(int i = 1; i < size; i++)
                {

                        MPI_Send (&end, 1, MPI_INT, i, 1, MPI_COMM_WORLD); // czy &?
                }
        }
        }

         /* Koniec fragmentu kodu */
        MPI_Finalize();
}