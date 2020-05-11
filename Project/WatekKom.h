#ifndef WATEK_KOMUNIKACYJNY_H
#define WATEK_KOMUNIKACYJNY_H

/* wątek komunikacyjny: odbieranie wiadomości i reagowanie na nie poprzez zmiany stanu */
void *startKomWatek(void *ptr);
int czyWejde(int tunel);
int topProces(int id);
void wyczysc(int proces);
int zgodyPunkt5(int id);
int zgodyPunkt6(int id);
void rozpatrzNaNowo(int what);  // Funkcja, która na nowo rozpatrzy kolejkę po zmiania która nastąpiła o wyjściu z tunelu

#endif
