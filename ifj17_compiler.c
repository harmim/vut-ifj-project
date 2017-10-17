/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Main file of compiler.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"


/**
 * Main function.
 *
 * @param argc Number of arguments.
 * @param argv Argument values.
 * @return EXIT_SUCCESS (0), if compilation was seccessful, appropriate error code otherwise.
 */
int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	// FIXME: This is only for scanner (get_next_token function) testing.
	// Example usage of scanner.
	struct dynamic_string str;
	if (!dynamic_string_init(&str))
	{
		return SCANNER_ERROR_INTERNAL;
	}
	while (true)
	{
		struct token token;
		int code = get_next_token(stdin, &token, &str); // scanner reads from standard input
		printf("Token type is %d.\n",token.type); // pomocny vypis
		if (code)
		{
			fprintf(stderr, "Scanner error!\n");

			return code; // scanner error occurred
		}

		if (token.type == TOKEN_TYPE_EOF)
		{
			break;
		}
	}
	dynamic_string_free(&str);


	return EXIT_SUCCESS; // compilation was successful
}
