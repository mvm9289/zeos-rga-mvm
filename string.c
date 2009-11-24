/*
 * string.c - 
 */

#include <string.h>

int strlen(const char *buffer) {
    int i = 0;
    while(buffer[i] != '\0') ++i;
    return i;
}

void strcpy(char *str1, const char *str2) {
    int i;
    for(i=0; str2[i] != '\0'; i++)
        str1[i]=str2[i];
    str1[i]='\0';
}

int strcmp(const char *str1, const char *str2) {
    int i;

	if(strlen(str1) != strlen(str2)) return 0;
    for(i=0; str1[i] != '\0'; i++)
        if (str1[i]!=str2[i]) return 0;

    return 1;
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
