/*
* header for bootloader
*/

#ifndef	__BOOT_H__
#define	__BOOT_H__

int atohex(const char *str);
int atoiMinMax(const char *str, int min, int max);

/* use declaration in stdlib.h */
//int atoi(const char *str);


#endif

