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


#define ERROR_INTERNAL 99 /// Internal error, eg. malloc error etc.


/**
 * Main function.
 *
 * @return EXIT_SUCCESS (0), if compilation was seccessful, appropriate error code otherwise.
 */
int main(void)
{
	// FIXME: This is only for scanner (get_next_token function) testing.
	// Example usage of scanner.
	while (true)
	{
		struct token token;
		struct dynamic_string string;
		if (!dynamic_string_init(&string))
		{
			return ERROR_INTERNAL;
		}
		token.attribute.string = &string;
		int code = get_next_token(stdin, &token); // scanner reads from standard input

		if (code)
		{
			fprintf(stderr, "Scanner error!\n");
			dynamic_string_free(&string);

			return code; // scanner error occurred
		}

		printf("Token type is %d.\n", token.type);

		if (token.type == TOKEN_TYPE_KEYWORD)
		{
			printf("Token keyword is %d.\n", token.attribute.keyword);
		}
		else if (token.type == TOKEN_TYPE_IDENTIFIER)
		{
			printf("Token identifier is %s.\n", token.attribute.string->str);
		}
		else if (token.type == TOKEN_TYPE_INT_NUMBER)
		{
			printf("Token int is %d.\n", token.attribute.integer);
		}
		else if (token.type == TOKEN_TYPE_DOUBLE_NUMBER)
		{
			printf("Token double is %f.\n", token.attribute.decimal);
		}
		else if (token.type == TOKEN_TYPE_STRING)
		{
			printf("Token string is %s.\n", token.attribute.string->str);
		}

		if (token.type == TOKEN_TYPE_EOF)
		{
			dynamic_string_free(&string);
			break;
		}
		dynamic_string_free(&string);
	}


	return EXIT_SUCCESS; // compilation was successful
}
