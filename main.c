#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

#define FILE_ERROR 1


int main(int argc, char *argv[])
{
	FILE *source_file;

	if (argc == 1)
	{
		printf("Neni zadan vstupni soubor\n");
		return FILE_ERROR;
	}

	if (!(source_file = fopen(argv[1], "r")))
	{
		printf("Soubor se nepodarilo otevrit\n");
		return FILE_ERROR;
	}

	while (true)
	{
		token token = get_next_token(source_file);

		if (token.type == SCANNER_ERROR)
		{
			return 10;
		}

		if (token.type == END_OF_FILE)
		{
			break;
		}
	}

	return EXIT_SUCCESS;
}
