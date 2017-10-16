/**************************************************************
 * Project:   IFJ2017
 *
 * Authors:   Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 *            Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 *
 *            Implementation of scanner
 **************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "scanner.h"
#include "string.h"


/**
 * Compares string with keyword
 * @param id string which is compared
 * @return reference value to keyword or just reference value of ID
 */
static int keyword_check(string *id)
{
	char *keyword[35] = {
		"as", "asc", "declare", "dim", "do", "double", "else", "end", "chr",
		"function", "if", "input", "integer", "length", "loop", "print", "return",
		"scope", "string", "substr", "then", "while", "and", "boolean", "continue",
		"elseif", "exit", "false", "for", "next", "not", "or", "shared", "static", "true"
	};
	for (int i = 100; i < 135; i++)
	{
		if (str_cmp_const_str(id, keyword[i]) == 0)
		{ return i; }
	}
	return 136;

}

/**
 * This is the main function of scanner, scans token after token and sends it further
 * @param attr number, string or ID
 * @return reference value which shows what token was read and is later used by parser or error if something went wrong
 */
token get_next_token(FILE *source_file)
{
	int state = STATE_START;
	token token = token;

	int is_double_number = 0; //flag
	int c;
	string *str;
	str_clear(str);

	while (1)
	{
		c = getc(source_file);

		switch (state)
		{
			case (STATE_START):
				if (isspace(c))
				{
					state = STATE_START;
				}

				else if (c == 39)
				{// '
					state = STATE_COMMENTARY;
				}
				else if (c == 47)
				{ // /
					state = STATE_BACKSLASH;
				}
				else if (isalpha(c))
				{
					str_add_char(str, (char) c);
					state = STATE_IDENTIFIER_OR_KEYWORD;
				}
				else if (isdigit(c))
				{
					str_add_char(str, (char) c);
					state = STATE_NUMBER;
				}
				else if (c == 33) // !
				{
					str_add_char(str, (char) c);
					state = STATE_STRING_START;
				}
				else if (c == 60)
				{ // <
					state = STATE_LESS_THAN;
				}
				else if (c == 62)
				{ // >
					state = STATE_MORE_THAN;
				}
				else if (c == '=')
				{
					return 61; // =
				}
				else if (c == 43)
				{
					return 43; // +
				}
				else if (c == 45)
				{
					return 45; // -
				}
				else if (c == 42)
				{
					return 42; // *
				}
				else if (c == 92)
				{
					return 92; // '\'
				}
				else if (c == 40)
				{
					return 40; // (
				}
				else if (c == 41)
				{
					return 41; // )
				}
				else if (c == 44)
				{
					return 44; // ,
				}
				else if (c == 59)
				{
					return 59; // ;
				}
				else if (c == EOF)
				{ return END_OF_FILE; }
				else
				{
					return SCANNER_ERROR;
				}
				break;

			case (STATE_COMMENTARY):
				if (c == 10)
				{ //EOL
					state = STATE_START;
				}
				break;

			case (STATE_BACKSLASH):
				if (c == 39)
				{
					state = STATE_BLOCK_COMMENTARY;
				}
				else
				{ return 47; } // /

			case (STATE_BLOCK_COMMENTARY):
				if (c == 39)
					state = STATE_BLOCK_COMMENTARY_LEAVE;
				else if (c == EOF)
					return SCANNER_ERROR;
				break;

			case (STATE_BLOCK_COMMENTARY_LEAVE):
				if (c == EOF)
					return SCANNER_ERROR;
				else if (c == 47)
					state = STATE_START;
				else state = STATE_BLOCK_COMMENTARY;
				break;

				// ještě tam můžet být znak _
			case (STATE_IDENTIFIER_OR_KEYWORD):
				if (isalnum(c))
					str_add_char(str, (char) c);
				else
				{
					ungetc(c, source_file);
					return keyword_check(str);
				}
				break;

			case (STATE_NUMBER):
				if (isdigit(c))
				{
					str_add_char(str, (char) c);
				}
				else if (c == 46) // .
				{
					state = STATE_NUMBER_POINT;
					str_add_char(str, (char) c);
					is_double_number = 1;
				}
				else if (tolower(c) == 101) //e
				{
					state = STATE_NUMBER_EXPONENT;
					str_add_char(str, (char) c);
				}
				else
				{
					ungetc(c, source_file);
					return INT_NUMBER;
				}
				break;

			case (STATE_NUMBER_POINT):
				if (isdigit(c))
				{
					state = STATE_NUMBER_DOUBLE;
					str_add_char(str, (char) c);
				}
				else return SCANNER_ERROR;
				break;

			case (STATE_NUMBER_DOUBLE):
				if (isdigit(c))
				{
					str_add_char(str, (char) c);
				}
				else if (tolower(c) == 101) //e
				{
					state = STATE_NUMBER_EXPONENT;
					str_add_char(str, (char) c);
				}
				else
				{
					ungetc(c, source_file);
					return DOUBLE_NUMBER;
				}
				break;

			case (STATE_NUMBER_EXPONENT):
				if (isdigit(c))
				{
					state = STATE_NUMBER_EXPONENT_FINAL;
					str_add_char(str, (char) c);
				}
				else if (c == 43 || c == 45) // + -
				{
					state = STATE_NUMBER_EXPONENT_SIGN;
					str_add_char(str, (char) c);
				}
				else return SCANNER_ERROR;
				break;

			case (STATE_NUMBER_EXPONENT_SIGN):
				if (isdigit(c))
				{
					state = STATE_NUMBER_EXPONENT_FINAL;
					str_add_char(str, (char) c);
				}
				else return SCANNER_ERROR;
				break;

			case (STATE_NUMBER_EXPONENT_FINAL):
				if (isdigit(c))
				{
					str_add_char(str, (char) c);
				}
				else
				{
					ungetc(c, source_file);
					char *endptr;
					if (is_double_number)
					{
						double val = strtod(str->str, &endptr);
						if (*endptr)
						{
							token.type = SCANNER_ERROR;
						}
						token.attr.decimal_val = val;
						token.type = DOUBLE_NUMBER;
					}
					else
					{
						int val = (int) strtol(str->str, &endptr, 10);
						if (*endptr)
						{
							token.type = SCANNER_ERROR;
						}
						token.attr.int_val = val;
						token.type = INT_NUMBER;
					}

					return token;
				}
				break;

			case (STATE_STRING_START):
				if (c == 34) // "
					state = STATE_STRING;
				else return SCANNER_ERROR;
				break;

			case (STATE_STRING):
				if (c <= 31) // lower ASCII can be written by using escape sequence
					return SCANNER_ERROR;
				else if (c == 34) // "
				{
					str_add_char(str, (char) c);
					return STRING;
				}
				else
				{
					str_add_char(str, (char) c);
				}
				break;

			case (STATE_LESS_THAN):
				if (c == 62) // >
					return NEQ;
				else if (c == 61) // =
					return LEQ;
				else
				{
					ungetc(c, source_file);
					return LTN;
				}

			case (STATE_MORE_THAN):
				if (c == 61) // =
					return MEQ;
				else
				{
					ungetc(c, source_file);
					token.type = MTN;
					return token;
				}
		}
	}
}
