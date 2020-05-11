#include "funkcje.h"
#include "main.h"

void getInfoAboutOthers(int pojemnoscEkipy, int *pojemnoscWszystkich, int N)
{
    MPI_Status status;
    for(int i = 0; i < N; i++)
    {
        if(i != rank){
            MPI_Send( &pojemnoscEkipy, 1, MPI_INT, i, INFORMACJE_O_EKIPIE, MPI_COMM_WORLD );
            MPI_Recv( &pojemnoscWszystkich[i], 1, MPI_INT, i, INFORMACJE_O_EKIPIE, MPI_COMM_WORLD, &status);
        }
        //printf("Jestem %d: %d\n", rank, pojemnoscWszystkich[i]);
    }
}
void sendPacketToAll(packet_t *pkt, int tag)
{
    int freepkt=0;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}
    pkt->id = rank;
    for(int i=0;i< N; i++)
    {
        if(i != rank) MPI_Send( pkt, 1, MPI_PAKIET_T, i, tag, MPI_COMM_WORLD);
    }
    
    if (freepkt) free(pkt);
}

void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt=0;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}
    pkt->id = rank;
    MPI_Send( pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    if (freepkt) free(pkt);
}


