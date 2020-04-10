#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char *stro="kkv864igyJC9o";


int main(int argc, char **argv)
{
            int size,rank;
            char cmp[5]={0};
                char salt[3]={0};

                salt[0]=stro[0];
                salt[1]=stro[1];

                /* Ten fragment kodu nalezy _jakos_ zrównoleglić */
                /* pewnie będzie potrzebna jakaś pętla... */
                int a = 97;
                int z = 122;
                for(int i = a; i <= z; i++){
                        for(int j = a; j <= z; j++){
                                for(int k = a; k <= z; k++){
                                        for(int o = a; o <= z; o++){
                                                cmp[0] = i;
                                                cmp[1] = j;
                                                cmp[2] = k;
                                                cmp[3] = o;
                                                char * x=crypt(cmp, salt);
                                                if ((strcmp(x,stro))==0){
                                                        /* w docelowej wersji przeslac odgadnięte hasło masterowi */
                                                        printf("Udalo sie: %s %s %s\n", cmp, x, stro);
                                                        exit(0);
                                                }
                                        }
                                }
                        }
                }


                /* Koniec fragmentu kodu */
}