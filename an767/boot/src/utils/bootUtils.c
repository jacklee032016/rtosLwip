
#include "compact.h"
#include "boot.h"

int atoi(const char *str)
{
	char key;
	int value = 0;

	while ( *str != 0 )
	{
		key = *str;

		if (key >= '0' && key <= '9')
		{
			value = (value * 10) + (key - '0');
		}
		else
		{
			return 0;
		}

		str++;
	}

	return value;
}

int atoiMinMax(const char *str, int min, int max)
{
	int value = atoi(str);

	if (value == 0)
		return 0;
	if (value < min || value > max)
	{
		printf("The number have to be between %d and %d"MUX_NEW_LINE, min, max);
		return 0;
	}

	return value;
}

int atohex(const char *str)
{
	char key;
	uint32_t dw = 0;
	int value = 0;

	for (dw = 0; dw < 8; dw++)
	{
		key = *str;

		if (key >= '0' && key <= '9')
		{
			value = (value * 16) + (key - '0');
		}
		else
		{
			if (key >= 'A' && key <= 'F')
			{
				value = (value * 16) + (key - 'A' + 10);
			}
			else
			{
				if (key >= 'a' && key <= 'f')
				{
					value = (value * 16) + (key - 'a' + 10);
				}
				else
				{
					printf("'%s' is not a hexa character!"MUX_NEW_LINE, str);
					return 0;
				}
			}
		}

		str++;
	}
	
	return value;
}

