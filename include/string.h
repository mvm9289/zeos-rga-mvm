/*
 * string.h - Definició de funcions de tractament de strings.
 */

#ifndef __STRING_H__
#define __STRING_H__

#include <types.h>

int strlen(const char *buffer);
void strcpy(char *str1, const char *str2);
int strcmp (const char *str1, const char *str2);
void itoa(int num, char *buffer);

#endif  /* __STRING_H__ */
