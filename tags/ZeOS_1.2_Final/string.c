/*
 * string.c - 
 */

#include <string.h>

int strlen(char *buffer) {
    int i = 0;
    while(buffer[i] != '\0') ++i;
    return i;
}

void itoa(int num, char *buffer) {
    int i, j, sign;
    char aux;

    buffer[0]='\0';

    if((sign = num) < 0) num=-num;

    i=1;
    j=0;
    do {
        buffer[i++]=num%10 + '0';
        j++;
    } while ((num /= 10) > 0);

    if(sign < 0) buffer[i]='-';


    for (i = 0; i < j; i++, j--)
    {
        aux = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = aux;
    }
}
