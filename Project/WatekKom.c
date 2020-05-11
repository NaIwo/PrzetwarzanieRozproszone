#include "main.h"
#include "WatekKom.h"
#include "funkcje.h"


int topProces(int id)
{
    int min = msg[id].ts;
    int index = id;
    for (int i=0; i<N; i++)
    {
        if(msg[i].ts < min && msg[i].ts != -1){
            min = msg[i].ts;
            index = i;
        } 
    }
    return index;
}

int czyWejde(int tunel) // podobna funckja, ale wcelu łatwiejszego debugowania, zmiena nazwy i lekko inne działanie
{
    int min = msg[rank].ts;
    int index = rank;
    for (int i=0; i<N; i++)
    {
        if(msg[i].ts < min && msg[i].kierunek == tunel && msg[i].ts != -1){
            min = msg[i].ts;
            index = i;
        } 
    }
    return index;
}

void wyczysc(int proces)
{
        udzieloneZgody[proces] = 0;
        msg[proces].id = -1;
        msg[proces].ts = -1;
        msg[proces].nr = -1;
        msg[proces].kierunek = -1;
        msg[proces].typ = -1;
}
int sumaOdKtorychNieOtrzymal(){
    int sum = 0;
    for (int i=0; i<N; i++)
    {
        if(tablicaOtrzymanychZgod[i] == 0 && i != rank){
            sum += pojemnoscWszystkich[i];
        }
    }
    return sum;
}
int zgodyPunkt5(int id)
{
    if(stan != wTunelu && stan != Koniec && msg[id].ts != -1)
    {
        if(stan != czekamNaPozwolenie) return 1; //Na podstawie podpunktu a)
        if(msg[rank].kierunek == msg[id].kierunek && msg[rank].nr != msg[id].nr) return 1; //Na podstawie podpunktu b)
        if(msg[rank].kierunek == msg[id].kierunek && msg[rank].nr == msg[id].nr){ //Na podstawie podpunktu c)
            if(msg[id].ts < msg[rank].ts) return 1;
            else if(msg[id].ts == msg[rank].ts && id < rank) return 1;
        }
        if(msg[rank].kierunek != msg[id].kierunek){ //Na podstawie podpunktu d)
            if(msg[id].ts < msg[rank].ts) return 1;
            else if(msg[id].ts == msg[rank].ts && id < rank) return 1;
        }

    }
    return 0; // w każdym innym wypadku
}
int zgodyPunkt6(int id)
{
    if(stan != wTunelu && stan != Koniec && msg[id].ts != -1)
    {
        if(stan != czekamNaPozwolenie){ //Na podstawie podpunktu a)
            if(id == topProces(id)) return 1;
        } 
        if(msg[rank].kierunek == msg[id].kierunek && msg[rank].nr != msg[id].nr){ //Na podstawie podpunktu b)
            if(id == topProces(id)) return 1;
        }
        if(msg[rank].kierunek == msg[id].kierunek && msg[rank].nr == msg[id].nr){ //Na podstawie podpunktu c)
            if(id == topProces(id)) return 1;
        }
        if(msg[rank].kierunek != msg[id].kierunek){ //Na podstawie podpunktu d)
            if(id == topProces(id)) return 1;
        }
    }
    return 0; // w każdym innym wypadku
}
void rozpatrzNaNowo(int what)  // Funkcja, która na nowo rozpatrzy kolejkę po zmiania która nastąpiła o wyjściu z tunelu
{
    int czyWyslacZgode;
    packet_t *pkt = malloc( sizeof(packet_t)); // Pomocnicza
    for(int i=0; i<N; i++){ //Wychodze z sekcji wiec przegladam i ewentualnie wysylam zgody
        if(msg[i].id != -1 && udzieloneZgody[i] == 0 && i != rank){
            if(msg[i].kierunek == kierunekTunelu) // zgody na podstawie punktu 5.
                {
                    czyWyslacZgode = zgodyPunkt5(i);
                }
                else{ //Zgody na podstawie punktu 6.
                    czyWyslacZgode = zgodyPunkt6(i);
                }

                if(czyWyslacZgode){ // jesli jest mozliwosc wyslania ACK to tego dokonuje
                    pkt->ts = -1;
                    pkt->nr = -1;
                    pkt->kierunek = -1; 
                    pkt->typ = ACK; // ACK - wysylam akceptacje do odpowiedniego procesu
                    if(what == 0) printf("[%d]{%d} - Po zmianie kierunku wysylam pozwolenie do %d.\n", rank, lamportValue, msg[i].id);
                    else if(what == 1) printf("[%d]{%d} - Po wyjsciu z tunelu wysylam pozwolenie do %d.\n", rank, lamportValue, msg[i].id);
                    else printf("[%d]{%d} - Inny wyszedl z tunelu. Rozpatruje na nowo i wysylam pozwolenie do %d.\n", rank, lamportValue, msg[i].id);
                    udzieloneZgody[i] = 1;
                    sendPacket( pkt, i, MSG_TAG );
                } //END small IF
            }//END big IF
        }// END FOR
    free(pkt);
}
void *startKomWatek(void *ptr)
{

    MPI_Status status;
    packet_t pakiet;
    int maxLamportRec = 0;
    int czyWyslacZgode;
    int liczbaZgod = 0;
    packet_t *pkt = malloc( sizeof(packet_t)); // Pomocnicza

    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while(END){
        printf("[%d]{%d} - Czekam na jakieś informajce ~ narrator. \n", rank, lamportValue);
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MSG_TAG, MPI_COMM_WORLD, &status);
        switch ( pakiet.typ ) {
	    case REQ: 
                udzieloneZgody[pakiet.id] = 0;
                msg[pakiet.id] = pakiet; // dodaje żądanie do kolejki żądań
                if(pakiet.ts > lamportValue){
                    zwiekszLamporta(pakiet.ts); // ZWIĘKSZAM ZEGAR LAMPORTA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    printf("[%d]{%d} - Zwiekszylem zegar lamporta na:  %d.\n", rank, lamportValue, lamportValue);
                }
                
                if(pakiet.kierunek == kierunekTunelu) // zgody na podstawie punktu 5.
                {
                    czyWyslacZgode = zgodyPunkt5(pakiet.id);
                }
                else{ //Zgody na podstawie punktu 6.
                    czyWyslacZgode = zgodyPunkt6(pakiet.id);
                }


                if(czyWyslacZgode){ // jesli jest mozliwosc wyslania ACK to tego dokonuje
                    pkt->ts = lamportValue;
                    pkt->nr = -1;
                    pkt->kierunek = -1; 
                    pkt->typ = ACK; // ACK - wysylam akceptacje do odpowiedniego procesu
                    printf("[%d]{%d} - Wysylam pozwolenie do %d.\n", rank, lamportValue, pakiet.id);
                    udzieloneZgody[pakiet.id] = 1;
                    sendPacket( pkt, pakiet.id, MSG_TAG );
                }
                
	    break;
	    case ACK: 
                printf("[%d]{%d} - Dostalem pozwolenie od %d.\n",rank, lamportValue, pakiet.id);
                liczbaZgod++;
                tablicaOtrzymanychZgod[pakiet.id] = 1;
                int index = czyWejde(1 - msg[rank].kierunek); // sprawdzam czy nie ma kogos kto jest wyżej w kolejce żądań i chce iść w przeciwną stronę
                if(P - sumaOdKtorychNieOtrzymal() >= pojemnoscWszystkich[rank] && stan != wTunelu){ // warunek 7a
                    if(index != rank && stan == czekamNaPozwolenie && index != -1) { // warunek 7a
                            printf("[%d]{%d} - Niestety, proces %d chce isc w inna strone i posiada wyzszy piorytet.\n",rank, lamportValue, index);
                        }
                    else if(stan == czekamNaPozwolenie){ // warunek 7b
                        printf("[%d]{%d} - Dostalem wszystkie pozwolenia wiec wejde do tunelu.\n",rank, lamportValue);
                        for (int i=0; i<N; i++)
                        {
                                tablicaOtrzymanychZgod[i] = 0;
                        }
                        zmienStan(wTunelu);
                        if(msg[rank].kierunek != kierunekTunelu){ // jesli zmienil kierunek tuneli
                            kierunekTunelu = 1 - kierunekTunelu;
                            pkt->ts = -1;
                            pkt->nr = -1;
                            pkt->kierunek = kierunekTunelu; 
                            pkt->typ = REV; //REV - zmieniam kierunek tunelu
                            sendPacketToAll(pkt, MSG_TAG); // ToAll oznacza, że wysyłam od razu do wszystkich bogaczy
                            printf("[%d]{%d} - Zmienilem kierunek tuneli. Wyslalem REV. \n",rank, lamportValue);
                        }
                    }
                }
	    break;
        case REV:
                kierunekTunelu = pakiet.kierunek; //zmieniamy kierunek tuneli
                liczbaZgod = 0; // cofamy zgody ktore otrzymalismy
                for (int i=0; i<N; i++)
                {
                        udzieloneZgody[i] = 0;
                        tablicaOtrzymanychZgod[i] = 0;
                }
                printf("[%d]{%d} - Proces %d zmienil kierunek tuneli wiec wycofuje zgody. \n",rank, lamportValue, pakiet.id);

                rozpatrzNaNowo(0);// rozpatruje kolejke na nowo i jesli to możliwe, wysyłam zgdoy
        break;
	    case REL: 
                //trzeba usunac dany proces z kolejki bo wyszedl z tunelu
                wyczysc(pakiet.id);
                printf("[%d]{%d} - Proces %d wyszedl wiec usuwam z kolejki.\n",rank, lamportValue, pakiet.id);
                rozpatrzNaNowo(2); // rozpatruje kolejke na nowo i jesli to możliwe, wysyłam zgdoy



                
	    break;
	    default:
	    break;
        }
    }
      free(pkt);
    
}