#include "GlobalDefinitions.h"

#include "String_Funcs.h"
#include "math.h"




//! ASCII representation of hexadecimal digits.
const char HEX_DIGITS[16] = "0123456789ABCDEF";

void strcpy_with_length_limit(char* outputString, char* inputString, u16_t max_length)
{
	if (strlen(inputString) >= (max_length-1))
	{
		memcpy( (char*) outputString, inputString, (max_length-1));
		outputString[max_length-1]=0;
	}
	else
	{
		strcpy(outputString, inputString);
	}
}
	
#if 0
char* strcpy_dynamic(char* outputString, char* inputString)
{
	outputString = (char*)realloc(outputString, strlen(inputString)+1);
	strcpy(outputString, inputString);
	return outputString;
}
#endif

char* new_dynamic_string(char* inputString)
{
	
	char* outputString = (char*)calloc(strlen(inputString)+1,1);
	strcpy(outputString, inputString);
	return outputString;
}

#if 0
char* strcat_dynamic(char* outputString, char* inputString)
{
	outputString = (char*)realloc(outputString, strlen(outputString)+strlen(inputString)+1);
	strcat(outputString, inputString);
	return outputString;
}
#endif

void ftoa(float value, char* result, u8_t decimalPoints)
{
	char leftHand[12];
	char rightHand[12];
	
	u32_t buffer;
	u8_t i;
	
	buffer = (u32_t)value;
	
	itoa(buffer,leftHand,10);
	
	value -= buffer;

	value = value*pow(10,decimalPoints);
	buffer = (u32_t)value;
	itoa(buffer,rightHand,10);
	
	strcpy(result, leftHand);
	strcat(result, ".");
	
	for (i=0; i< (decimalPoints-strlen(rightHand)); i++)
	{
		strcat(result, "0");
	}		
	strcat(result, rightHand);
}



void itoa_alt(s32_t value, char* result, u8_t base)
{
	u8_t sign = 0;

	// Max value is 32 bits, so should fit in 12 chars
	char output_string[12];
	u8_t counter = 11;
	u32_t value_cpy;

    if (  (base < 2) | (base > 16)) { *result = '\0'; return; }

    // If value is zero
    if (value == 0)
    {
    	strcpy(result, "0\0");
    	return;
    }

    // Check and store sign
    if (value >= 0)
    {
    	sign = 1;
    }
    else
    {
    	sign = 0;
    	value = -value;
    }

    // Terminate string
    output_string[11] = '\0';

    do
    {
    	counter--;
        if (counter > 11) { *result = '\0'; return; }
        
        value_cpy = value;
        value = value / base;
        if (value_cpy!=0)
            output_string[counter] = HEX_DIGITS[value_cpy - (value*base)];

	}while( (value_cpy!=0) );


	// add sign
	if (sign == 0)
	{	// is negative; put sign.
		output_string[counter] = '-';
	}
	else
	{
		counter++;
	}
	// copy string
	strcpy(result, output_string+counter);
	return;
}

char* converIntToString(s32_t value, u8_t base)
{
	u8_t sign = 0;
	char* result;

	// Max value is 32 bits, so should fit in 12 chars
	char output_string[12];
	u8_t counter = 11;
	u32_t value_cpy;

    if (  (base < 2) | (base > 16)) 
    { 
    	result=malloc(1);
    	*result = '\0'; 
    	return result; 
    }


    // If value is zero
    if (value == 0)
    {
    	result=malloc(2);
    	strcpy(result, "0\0");
    	return result;
    }

    // Check and store sign
    if (value >= 0)
    {
    	sign = 1;
    }
    else
    {
    	sign = 0;
    	value = -value;
    }

    // Terminate string
    output_string[11] = '\0';

    do
    {
    	counter--;
    	if (counter > 11) 
    	{ 
    		result=malloc(1);
    		*result = '\0'; 
    		return result;
    	}
    	
    	value_cpy = value;

    	value = value / base;
    	
    	if (value_cpy!=0)
    		output_string[counter] = HEX_DIGITS[value_cpy - (value*base)];

    }while( (value_cpy!=0) );

    // add sign
    if (sign == 0)
    {	// is negative; put sign.
    	output_string[counter] = '-';
    }
    else
    {
    	counter++;
    }

    // copy string
    result=malloc(strlen(output_string)+1);
    strcpy(result, output_string+counter);
    return result;
}

u32_t valueFromHex(char* string, uint8_t length)
{
	u8_t i;
	u32_t    value=0;
	
	
	for (i=0; i<length; i++)
	{
		value= value<<4;   // shift 4 bits to the left
		
		if ((string[i]<='9') & (string[i]>='0'))
		{
			value+=string[i]-'0';
		}
		if ((string[i]<='f') & (string[i]>='a'))
		{
			value+=string[i]-'a'+10;
		}
		if ((string[i]<='F') & (string[i]>='A'))
		{
			value+=string[i]-'A'+10;
		}
	}
	
	return value;
}

u32_t valueFromIntString(char *inputString, u8_t length)
{
	u8_t k=0;
	u32_t intValue=0;
	
	for (k=0; k<length; k++)
	{
		if ((inputString[k]<='9') & (inputString[k]>='0'))
		{
			intValue*=10;
			intValue+=inputString[k]-'0';
		}
		else if (inputString[k]==' ')
		{ // ignore spaces
		}
		else
		{
			// If anything else than a number or a space, we've reached the end of the integers
			return intValue;
		}
	}
	return intValue;
}


u32_t getContentLengthFromHTTPHeader(char* HTTPHeader)
{
	u16_t i=0;
	u8_t headerFound=0;
	u32_t intValue=0;
	
	while ((i<strlen(HTTPHeader)) & (headerFound==0))
	{
		
		if ( !strncmp( &HTTPHeader[i], "Content-Length:", 15 ) )
		{
			i+=15;			// move ahead to the interesting stuff
			intValue=valueFromIntString(&HTTPHeader[i], 5);	// limit to 5 digits for now
		}
		else
		{
			i++;
		}
	}

	return intValue;
}

void replaceCharInString(char* inputString, char findCharacter, char replacementCharacter)
{
	u32_t i;
	
	for (i=0; i<strlen(inputString); i++)
	{
		if (inputString[i]==findCharacter)
		{
			inputString[i]=replacementCharacter;
		}
	}
}

u8_t numberOfOccurencesOfSubstringInString(char* theString, char* theSubString)
{
	u32_t i;
	u8_t count=0;
	
	for (i=0; i<(strlen(theString)-strlen(theSubString)+1); i++)
	{
		if (indexOfSubstringInString(&theString[i], theSubString))
		{
			count++;
			i+=indexOfSubstringInString(&theString[i], theSubString);
		}
	}
	return count;
}

u32_t indexOfSubstringInString(char* theString, char* theSubString)
{
	u32_t i=0;
	
	for (i=0; i<(strlen(theString)-strlen(theSubString)+1); i++)
	{
		if ( !strncmp( &theString[i], theSubString, strlen(theSubString) ) )
		{
			return i;
		}
	}
	
	return 0;
}


void cleanHTTPString(char *outputString, char* inputString)
{
	u8_t i;
	char aSingleCharacter[2];
	
	aSingleCharacter[1]='\0';
	
	for (i=0; i<strlen(inputString); i++)
	{
		if (inputString[i]=='%')
		{
			aSingleCharacter[0]=(char)valueFromHex(&inputString[i], 3);
			
			strcat(outputString, aSingleCharacter);
			i+=2;
		}
		else
		if (inputString[i]=='+')
		{
			strcat(outputString, " ");
		}
		else
		{
			aSingleCharacter[0]=inputString[i];
			strcat(outputString, aSingleCharacter);
		}
	}
}

void cleanHTTPStringInPlace(char* inputString)
{
	u16_t i;
	u16_t	newIndex=0;
	char aSingleCharacter[2];
	
	aSingleCharacter[1]='\0';
	
	for (i=0; i<strlen(inputString); i++)
	{
		if (inputString[i]=='%')
		{
			// convert %xx values to a single char

			inputString[newIndex]=(char)valueFromHex(&inputString[i], 3);
			newIndex++;
			i+=2;		// Skip over %xx
			
		}
		else
		if (inputString[i]=='+')
		{
			// Convert +'s to spaces
			inputString[newIndex]=' ';
			newIndex++;
		}
		else
		{
			inputString[newIndex]=inputString[i];
			newIndex++;
		}
	}
	inputString[newIndex]='\0';
	
// 	Shell_PrintLine_Debug( "\n\rString cleaned in place: ");
// 	Shell_PrintLine_Debug( inputString);
}


u8_t get_IndexOfU8Array(u8_t * U8_Array, u8_t U8_ArrayLenght, u8_t *ElementPointer)
{
	u8_t index = 0;
	
	for ( index=0; index < U8_ArrayLenght; index++)
	{
		if (ElementPointer == &U8_Array[index])
		{
			return index;
		}
	}
	
	return 0;
}

void removeElementInU8Array(u8_t *U8_Array, u8_t U8_ArrayLenght, u8_t *ElementToRemovePointer)
{
	u8_t index = 0;
	u8_t elementToRemoveIndex;
	
	elementToRemoveIndex = get_IndexOfU8Array(U8_Array, U8_ArrayLenght, (u8_t *)ElementToRemovePointer);
	
	for ( index=elementToRemoveIndex; index < U8_ArrayLenght; index++)
	{
		if ( index == (U8_ArrayLenght-1))
			U8_Array[index] = 0;
		else
			U8_Array[index] = U8_Array[index+1];

	}
}

#if 0
U8 insertElementInU8Array(U8 U8_Array[], u8_t U8_ArrayLenght, u8_t ElementToInsert)
{
	u8_t index = 0;
	u8_t index2 = 0;

	if (U8_ArrayLenght == MATRIX_DISPLAY_SIZE) // Cannot insert more element
		return 0;

	
	
	for ( index=0; index < U8_ArrayLenght; index++)
	{
		if (U8_Array[index] > ElementToInsert) // Then insert it before
		{
			for ( index2=(U8_ArrayLenght+1); index2 > index; index2--)
			{
				U8_Array[index2] = U8_Array[index2-1];
			}
			
			U8_Array[index] = ElementToInsert;
			return index;
		}
	}
	// Add it at the end
	U8_Array[index] = ElementToInsert;
	return index;
}
#endif

u8_t stringChecksum(char *aString)
{
	u16_t i;
	u8_t checksum=0;
	
	for (i=0; i<strlen(aString); i++)
	{
		checksum -= aString[i];
	}
	
	return checksum;
}


void capitalizeStringInPlace(char *string)
{
	u16_t i;

	for (i=0; i<strlen(string); i++)
	{
		if ((string[i]>='a') & (string[i]<='z'))
    		string[i]-='a'-'A';
	}
}

// Remove space characters at the beginning & the end of the string and also when there is 2 or more successive space characters
char *str_strip (const char *string)
{
	char *strip = NULL;

	if (string != NULL)
	{
		strip = malloc (sizeof (*strip) * (strlen (string) + 1));
		if (strip != NULL)
		{
			int i, j;
			int ps = 0;

			for (i = 0, j = 0; string[i]; i++)
			{
				if (string[i] == ' ')
				{
					if (j==0)
					{
						continue;
					}
					
					if (ps == 0)
					{
						strip[j] = string[i];
						ps = 1;
						j++;
					}
				}
				else
				{
					strip[j] = string[i];
					ps = 0;
					j++;
				}
			}
			
			if (strip[j-1] == ' ')
			strip[j-1] = '\0';
			else
			strip[j] = '\0';
		}
		else
		{
			return NULL;
		}
	}
	return strip;
}

// Split a string according to the specified separator, and copy each substring in a table of string.
// The last table item is NULL
char **str_split (char *s, const char *ct, char **tab, uint8_t maxTabArray)
{
	//char **tab = NULL;

	if (s != NULL && ct != NULL)
	{
		int i;
		char *cs = NULL;

		for (i = 0; (cs = strtok (s, ct)); i++)
		{
			tab[i] = cs;
			s = NULL;
			
			if (i == (maxTabArray-2))
			{
				i=i+1;
				Shell_PrintLine_Debug( "ERROR in str_split (MAX ARRAY REACHED)");
				break;
			}
		}
		tab[i] = NULL;
	}
	return tab;
}

char *str_tolower (const char *ct)
{
	char *s = NULL;

	if (ct != NULL)
	{
		int i;

		/* (1) */
		s = malloc (sizeof (*s) * (strlen (ct) + 1));
		if (s != NULL)
		{
			/* (2) */
			for (i = 0; ct[i]; i++)
			{
				s[i] = tolower (ct[i]);
			}
			s[i] = '\0';
		}
	}
	return s;
}

char *str_toupper (const char *ct)
{
	char *s = NULL;

	if (ct != NULL)
	{
		int i;

		/* (1) */
		s = malloc (sizeof (*s) * (strlen (ct) + 1));
		if (s != NULL)
		{
			/* (2) */
			for (i = 0; ct[i]; i++)
			{
				s[i] = toupper (ct[i]);
			}
			s[i] = '\0';
		}
	}
	return s;
}

char *str_sub (const char *s, unsigned int start, unsigned int end)
{
	char *new_s = NULL;

	if (s != NULL )
	{
		/* (1)*/
		new_s = malloc (sizeof (*new_s) * (end - start + 2));
		if (new_s != NULL)
		{
			int i;

			/* (2) */
			for (i = start; i <= end; i++)
			{
				/* (3) */
				new_s[i-start] = s[i];
			}
			new_s[i-start] = '\0';
		}
		else
		{
			free (new_s);
			Shell_PrintLine_Debug( "ERROR in str_sub\r\n");
			return NULL;
		}
	}
	return new_s;
}

char *substring(const char *s, unsigned int start, unsigned int end, char *stringToFill, uint8_t maxSubStringLen)
{
	//char *new_s = NULL;

	if (s != NULL )
	{
		/* (1)*/
		//new_s = malloc (sizeof (*new_s) * (end - start + 2));
		if (maxSubStringLen >= (end - start + 2))
		//if (new_s != NULL)
		{
			int i;

			/* (2) */
			for (i = start; i <= end; i++)
			{
				/* (3) */
				stringToFill[i-start] = s[i];
			}
			stringToFill[i-start] = '\0';
		}
		else
		{
			//free (new_s);
			Shell_PrintLine_Debug( "ERROR in substring(): substring > maxSubStringLen\r\n");
			return stringToFill;
		}
	}
	return stringToFill;
}

int str_indexOfStr (const char *cs, const char *ct)
{
	int index = -1;

	if (cs != NULL && ct != NULL)
	{
		char *ptr_pos = NULL;

		ptr_pos = strstr (cs, ct);
		if (ptr_pos != NULL)
		{
			index = ptr_pos - cs;
		}
	}
	return index;
}

int isAnInteger(char *s)
{
	u8_t sLength = strlen(s);
	u8_t i = 0;
	
	for (i=0; i<sLength; i++)
	{
		if ( !isdigit(s[i])	)
		 return 0;
	}
	
	return 1;
}

#if 0
void SF_print_int_on_usart(volatile avr32_usart_t *usart, u32_t aNumber, u8_t base)
{
	char intString[14];
	itoa(aNumber, intString, base);
	
	usart_write_line(usart, intString);
}
#endif

u8_t validateAndGetIpAddressFromString( char *ipAddressString, ip_addr_t *ReturnedValue)
{
	char* ipAddressStringCopy;
	u8_t index = 0;
	char *parts[6];
	u8_t addressValueArray[4];

	ipAddressStringCopy = malloc(  sizeof(char) * ( strlen(ipAddressString) + 1) );
	strcpy(ipAddressStringCopy, ipAddressString);
	
	// Split the string with the '.' delimiter
	str_split(ipAddressStringCopy,".", parts, 6);
	
	// Validate that we have exactly 4 parts
	while(parts[index] != NULL)
	{
		if (index >= 4)
		{
			free(ipAddressStringCopy);
			return false; 
		}
		
		// Validate that each part is a number
		if( isAnInteger(parts[index]) == true )
		{
			u32_t tempIntValue = valueFromIntString(parts[index], strlen(parts[index]) );
			
			// Validate that each part is between 0 and 255 inclusively
			if ( (tempIntValue >= 0) && ( tempIntValue <= 255)  )
			{
				addressValueArray[index] = tempIntValue;
			}
			else
			{
				free(ipAddressStringCopy);
				return false; 
			}				
			
		}
		else
		{
			free(ipAddressStringCopy);
			return false; 
		}			
		
		index++;
		
	}
	
	free(ipAddressStringCopy);
	
	NH_IPAddressFromIntArray( addressValueArray, ReturnedValue);
	//UV_setIPAddress(tempAddress);
	
	return true;
}

u8_t stringsAreIdentical(char* string1, char* string2)
{
	if (strlen(string1) != strlen(string2))
	{
		return 0;
	}
	
	if (!strncmp( string1, string2, strlen(string1)))
	{
		return 1;
	}
	
	return 0;
}

u8_t isValidHexString( const char *hexString )
{
	u8_t i;
	
	// if the string length is odd so it is not a valid Hex string
	if ( (strlen(hexString) % 2) != 0 )
		return false;
		
	for (i = 0; i < strlen(hexString); i++ )
	{
		if ( ((hexString[i]<='9') && (hexString[i]>='0')) ||  ((hexString[i]<='f') && (hexString[i]>='a')) || ((hexString[i]<='F') && (hexString[i]>='A')) )
		{
			// ok
		}
		else
			return false;
		
	}
	
	return true;
}

int uintToHexString(u32_t num, char* buff)
{
	int len=0,k=0;
	char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7','8', '9' ,'A', 'B', 'C', 'D', 'E', 'F' };
	
	do //for every 4 bits
	{
		//get the equivalent hex digit
		buff[len] = hex[num & 0xF];
		len++;
		num>>=4;
	}while(num != 0);
	
	//since we get the digits in the wrong order reverse the digits in the buffer
	for(; k < len/2; k++)
	{//xor swapping
		buff[k] ^= buff[len-k-1];
		buff[len-k-1] ^= buff[k];
		buff[k] ^= buff[len-k-1];
	}
	
	// If odd then add '0' at the beginning
	if ( (len % 2) == 1 )
	{
		for(k=len; k > 0; k--)
			buff[k] = buff[k-1];

		buff[0] = '0';
		len++;
	}
	//null terminate the buffer and return the length in digits
	buff[len]='\0';
	return len;
}
