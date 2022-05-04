#ifndef _STRING_FUNCS_H_
#define _STRING_FUNCS_H_

#include "GlobalDefinitions.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

void itoa_alt(s32_t value, char* result, u8_t base);
void ftoa(float value, char* result, u8_t decimalPoints);

void cleanHTTPString(char *outputString, char* inputString);
uint32_t valueFromHex(char* string, uint8_t length);
u32_t valueFromIntString(char *inputString, uint8_t length);
u32_t getContentLengthFromHTTPHeader(char* HTTPHeader);
u32_t indexOfSubstringInString(char* theString, char* theSubString);
u8_t numberOfOccurencesOfSubstringInString(char* theString, char* theSubString);
void replaceCharInString(char* inputString, char findCharacter, char replacementCharacter);
void cleanHTTPStringInPlace(char* inputString);

char* converIntToString(s32_t value, u8_t base);
u8_t get_IndexOfU8Array(u8_t U8_Array[], u8_t U8_ArrayLenght, u8_t *ElementPointer);
void removeElementInU8Array(u8_t U8_Array[], u8_t U8_ArrayLenght, u8_t *ElementToRemovePointer);
u8_t insertElementInU8Array(u8_t U8_Array[], u8_t U8_ArrayLenght, u8_t ElementToInsert);

u8_t stringChecksum(char *aString);
void capitalizeStringInPlace(char *string);

char *str_strip (const char *string);
char **str_split (char *s, const char *ct, char **tab, uint8_t maxTabArray);
char *str_tolower (const char *ct);
char *str_toupper (const char *ct);
char *str_sub (const char *s, unsigned int start, unsigned int end);
char *substring(const char *s, unsigned int start, unsigned int end, char *stringToFill, uint8_t maxSubStringLen);
int str_indexOfStr (const char *cs, const char *ct);

int isAnInteger(char *s);

char* strcpy_dynamic(char* outputString, char* inputString);
char* strcat_dynamic(char* outputString, char* inputString);
char* new_dynamic_string(char* inputString);

void strcpy_with_length_limit(char* outputString, char* inputString, u16_t max_length);

//void SF_print_int_on_usart(volatile avr32_usart_t *usart, u32_t aNumber, u8_t base);

u8_t validateAndGetIpAddressFromString( char *ipAddressString, ip_addr_t *ReturnedValue);

u8_t stringsAreIdentical(char* string1, char* string2);

u8_t isValidHexString( const char *hexString );

int uintToHexString(u32_t num, char* buff);

#endif //_STRING_FUNCS_H_
