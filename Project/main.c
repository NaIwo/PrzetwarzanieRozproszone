#include "funkcje.h"
#include "main.h"
#include "WatekKom.h"

#include <pthread.h>

#define MAX(a, b) ((a)>(b) ? (a) : (b))

pthread_t threadKom;
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lamportMut = PTHREAD_MUTEX_INITIALIZER;
int END = 1;
MPI_Datatype MPI_PAKIET_T;
int size, rank;
int P, M, N;
int lamportValue; // wartość lamporta
int *pojemnoscWszystkich; // tablica wielkosci każdej z ekip
int kierunekTunelu; // aktualny kierunek tunelu
int *udzieloneZgody; // ma to na celu kontrolowanie przydzielonych zgód i ewentualne ich wycofanie w momencie otrtzymania REL
int *tablicaOtrzymanychZgod; // nazwa jest znacząca, tablica przechowuje indeksy procesów od których otrzymano zgody
state_t stan = naZiemi;
packet_t *msg; //kolejka żądań wszytskich procesów zawierająca ich wiadomości

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: %d\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
	        fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	        MPI_Finalize();
	        exit(-1);
	        break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	        break;
        case MPI_THREAD_SERIALIZED: 
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	        break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n");
	        break;
        default: printf("Nikt nic nie wie\n");
    }
}

void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    printf("[%d] - Czekam na wątek w celu zakonczenia.\n", rank );
    pthread_join(threadKom,NULL);
    printf("[%d] - [*] KONCZE SWOJ ZYWOT. FAREWELL. [*]\n", rank );
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}
void makeSimpleInit(int *argc, char ***argv){
    int provided;
    MPI_Init_thread(argc, argv,MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    const int nitems=5; 
    int blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype typy[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[5]; 
    offsets[0] = offsetof(packet_t, id);
    offsets[1] = offsetof(packet_t, ts);
    offsets[2] = offsetof(packet_t, nr);
    offsets[3] = offsetof(packet_t, kierunek);
    offsets[4] = offsetof(packet_t, typ);

    MPI_Type_create_struct(nitems, blocklengths, offsets, typy, &MPI_PAKIET_T);
    MPI_Type_commit(&MPI_PAKIET_T);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    msg = malloc( sizeof(packet_t) * N );
    for(int i=0; i<N; i++){
        msg[i].id = -1;
        msg[i].ts = -1;
        msg[i].nr = -1;
        msg[i].kierunek = -1;
        msg[i].typ = -1;
    }

    pthread_create( &threadKom, NULL, startKomWatek , 0); // Utworzenie wątku komunikacyjnego
}
void zmienStan(state_t nowy)
{
    pthread_mutex_lock( &stateMut );
    if (stan==Koniec) { 
	pthread_mutex_unlock( &stateMut );
        return;
    }
    stan = nowy;
    pthread_mutex_unlock( &stateMut );
}
void zwiekszLamporta(int value)
{
    pthread_mutex_lock( &lamportMut );
    if (stan==Koniec) { 
	pthread_mutex_unlock( &lamportMut );
        return;
    }
    if(value == -1)
        lamportValue ++;
    else
        lamportValue = value; 
    pthread_mutex_unlock( &lamportMut );
}

int main(int argc,char **argv)
{    
    MPI_Status status;

    
    //Na wstępie sprawdzam czy poprawnie zainicjowano program, czy podano odpowiednią ilość parametrów
    if(argc != 3){ 
        printf("Blednie zainicjowano program (Podaj odpowiendia ilosc parametrow) \n");
        return 0;
    }

    P = atoi( argv[1] );
    M = atoi( argv[2] );
 
    
    pojemnoscWszystkich = malloc( sizeof (int) * N); // informacje o wielkośći wszystkich ekip
    kierunekTunelu = 0; // zainicjowanie kierunku tunalu na 0 - w stronę raju, 1 - strona ziemi
    lamportValue = 0; // ustawienie wartośći zegaru lamporta na 0

    udzieloneZgody = malloc( sizeof (int) * N);
    memset(udzieloneZgody, 0, sizeof(int) * N);

    tablicaOtrzymanychZgod = malloc(sizeof(int) * N);
    memset(tablicaOtrzymanychZgod, 0, sizeof(int) * N);

    makeSimpleInit(&argc, &argv); // inicjacja pewnych wartośći
    
    N = size;

    packet_t *pkt = malloc( sizeof(packet_t)); // Instancja struktury przesyłanej wiadomości

    int proba = 50; 

    srand( time(NULL) + rank );
    pojemnoscWszystkich[rank] = rand() % P/2 + 1;; // ustawienie wartośći wielkości własnej ekipy bogacza

    getInfoAboutOthers(pojemnoscWszystkich[rank], pojemnoscWszystkich, N); // zebranie informacji o pojemnośći pozostałych ekip

    printf("[%d] Moja pojemność - %d\n", rank, pojemnoscWszystkich[rank]);

    printf("[%d]{%d} - Zaczniemy za 3 sekundy ... \n\n", rank, lamportValue);
    sleep(3);
    printf("[%d]{%d} - START! \n", rank, lamportValue);

    while(1)
    {
        int los = rand()%100;
        if((stan == naZiemi || stan == wRaju) && los >= proba) // jeśli nie ubiega się o przejście, po prostu sobie jest, więc zakładamy, że może jest chętny na przejście (to przypuszczenie implementujemy jako wartość losowa)
        {
            pkt->nr = rand()%M;
            if(stan == naZiemi) printf("[%d]{%d} - Moja kolej! Chce przejsc do raju! Tunel: %d\n", rank, lamportValue, pkt->nr);
            else printf("[%d]{%d} - Moja kolej! Chce przejsc na ziemie! Tunel: %d\n", rank, lamportValue, pkt->nr);

            zwiekszLamporta(-1); // Zwiększenie zegaru lamporta
            printf("[%d]{%d} - Zwiekszylem zegar lamporta na:  %d.\n", rank, lamportValue, lamportValue);

            //wartość pkt->id dodawana jest w funkcji wysyłającej więc pomijam przypisanie w tym miejscu
            pkt->ts = lamportValue;
            pkt->id = rank;
            pkt->kierunek = stan == naZiemi ? 0 : 1; // 0 - chce isc do raju, 1 - chce isc na ziemie
            pkt->typ = REQ; // REQ bo wysyłam zapytanie, żądanie do innych, o tym, że chce przejść
            msg[rank] = *pkt; // przypisuję do tablicy żądań własne żądanie

            if(N!=1) {
                zmienStan(czekamNaPozwolenie);
                sendPacketToAll(pkt, MSG_TAG); // ToAll oznacza, że wysyłam od razu do wszystkich bogaczy
            } else {
                zmienStan(wTunelu);
            }
            
        }
        if(stan == wTunelu) {
            int ide = rand()%1 + 4;
            printf("[%d]{%d} - Jestem w tunelu i bede szedl %d sekund.\n",rank, lamportValue, ide);

            sleep(ide);

            pkt->ts = -1;
            pkt->nr = -1;
            if(stan == naZiemi) pkt->kierunek = 0; 
            else if(stan == wRaju) pkt->kierunek = 1; 
            pkt->typ = REL; //REL - wychodze z tunelu
            msg[rank] = *pkt; 

            zmienStan(msg[rank].kierunek == 0 ? wRaju : naZiemi);
            sendPacketToAll(pkt, MSG_TAG); // ToAll oznacza, że wysyłam od razu do wszystkich bogaczy

            if(stan == naZiemi) printf("[%d]{%d} - Wychodze z tunelu! Jestem na ziemi. \n", rank, lamportValue);
            else if(stan == wRaju) printf("[%d]{%d} - Wychodze z tunelu! Jestem w raju. \n", rank, lamportValue);

            msg[rank].id = -1;
            msg[rank].ts = -1;
            msg[rank].nr = -1;
            msg[rank].kierunek = -1;
            msg[rank].typ = -1;
        

        for(int i=0; i<N; i++){
            tablicaOtrzymanychZgod[i] = 0;
            } // usuwam otrzymane zgody ponieważ będę się od nowa ubiegał
            rozpatrzNaNowo(1); // Funkcja, która na nowo rozpatrzy kolejkę po zmiania która nastąpiła o wyjściu z tunelu
        }
        if(stan == czekamNaPozwolenie){
            printf("[%d]{%d} - Czekam na pozwolenie.\n", rank, lamportValue);
        }
        if((stan == naZiemi || stan == wRaju) && los < proba) {
            printf("[%d]{%d} - Egzystuje sobie i nic nie chce. \n", rank, lamportValue);
        }
        sleep(1); // zamrożenie stanu
    }
    zmienStan(Koniec);
    END = 0;
    sleep(10); // czekam aż drugi wątek znów zawiśnie na MPI_Recv w celu odblokowania i zakończenia programu
    sendPacket( pkt, (rank+1)%size, MSG_TAG );
    sleep(15); // czekam aż drugi wątek znów zawiśnie na MPI_Recv w celu odblokowania i zakończenia programu
    sendPacket( pkt, (rank+1)%size, MSG_TAG );

    //free(args);
    finalizuj();
    free(pojemnoscWszystkich);
    free(udzieloneZgody);
    free(tablicaOtrzymanychZgod);
    free(msg);
    return 0;
    
}
