#ifndef GLOBALH
#define GLOBALH


#define ROOT 0
#define INFORMACJE_O_EKIPIE 1
#define MSG_TAG 2

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

extern int P, M, N; //P - pojemność tunelu, M - liczba tuneli, N - liczba bogaczy 
extern int rank;
extern int size;
extern int END;
typedef enum {wTunelu, naZiemi, wRaju, czekamNaPozwolenie, Koniec} state_t;
extern state_t stan;
typedef struct {
    int id;         // id procesu
    int ts;       // timestamp (zegar lamporta) 
    int nr;        // Numer tunelu ktorym chcemy przejsc
    int kierunek;     // Kierunek 0 - DIM, 1 - EARTH 
    int typ;      // typ wiadomosci - REQ - 1, ACK - 2, REV - 3, REL - 4
} packet_t;
extern MPI_Datatype MPI_PAKIET_T;

void zwiekszLamporta(int value);
void zmienStan(state_t);


    #define REQ 1
    #define ACK 2
    #define REV 3
    #define REL 4


    extern int lamportValue; // wartość lamporta
    extern int *pojemnoscWszystkich;  // tablica wielkosci każdej z ekip
    extern int kierunekTunelu; // na początku każdy tunel skierowany jest w stronę raju
    extern int *udzieloneZgody;  // ma to na celu kontrolowanie przydzielonych zgód i ewentualne ich wycofanie w momencie otrtzymania REL
    extern int *tablicaOtrzymanychZgod; // nazwa jest znacząca, tablica przechowuje indeksy procesów od których otrzymano zgody
    extern packet_t *msg;
    extern int proba;

#endif