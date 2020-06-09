#ifndef FUNCH
#define FUNCH

#include "main.h"

void getInfoAboutOthers(int pojemnoscEkipy, int *pojemnoscWszystkich, int N);
void sendPacketToAll(packet_t *pkt, int tag);
void sendPacket(packet_t *pkt, int destination, int tag);

#endif