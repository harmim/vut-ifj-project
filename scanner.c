/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Scanner implementation.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */


#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "error.h"
#include "scanner.h"


FILE *source_file; /// Source file that will be scanned
Dynamic_string *dynamic_string; /// Dynamic string that will be written into


/**
 * Free resources and returns exit code.
 *
 * @param exit_code Exit code.
 * @param str Dynamic string to be freed.
 * @return Given exit code.
 */
static int free_resources(int exit_code, Dynamic_string *str)
{
	dynamic_string_free(str);

	return exit_code;
}


/**
 * Processing identifier, checks if it is keyword.
 *
 * @param str Identifier which is being compared.
 * @param token Pointer to output token.
 * @return 0 (SCANNER_TOKEN_OK) if token is OK, otherwise in case of lex error one of SCANNER_ERROR_... constant.
 */
static int process_identifier(Dynamic_string *str, Token *token)
{
	if (!dynamic_string_cmp_const_str(str, "and")) token->attribute.keyword = KEYWORD_AND;
	else if (!dynamic_string_cmp_const_str(str, "as")) token->attribute.keyword = KEYWORD_AS;
	else if (!dynamic_string_cmp_const_str(str, "boolean")) token->attribute.keyword = KEYWORD_BOOLEAN;
	else if (!dynamic_string_cmp_const_str(str, "continue")) token->attribute.keyword = KEYWORD_CONTINUE;
	else if (!dynamic_string_cmp_const_str(str, "declare")) token->attribute.keyword = KEYWORD_DECLARE;
	else if (!dynamic_string_cmp_const_str(str, "dim")) token->attribute.keyword = KEYWORD_DIM;
	else if (!dynamic_string_cmp_const_str(str, "do")) token->attribute.keyword = KEYWORD_DO;
	else if (!dynamic_string_cmp_const_str(str, "double")) token->attribute.keyword = KEYWORD_DOUBLE;
	else if (!dynamic_string_cmp_const_str(str, "else")) token->attribute.keyword = KEYWORD_ELSE;
	else if (!dynamic_string_cmp_const_str(str, "elseif")) token->attribute.keyword = KEYWORD_ELSEIF;
	else if (!dynamic_string_cmp_const_str(str, "end")) token->attribute.keyword = KEYWORD_END;
	else if (!dynamic_string_cmp_const_str(str, "exit")) token->attribute.keyword = KEYWORD_EXIT;
	else if (!dynamic_string_cmp_const_str(str, "false")) token->attribute.keyword = KEYWORD_FALSE;
	else if (!dynamic_string_cmp_const_str(str, "for")) token->attribute.keyword = KEYWORD_FOR;
	else if (!dynamic_string_cmp_const_str(str, "function")) token->attribute.keyword = KEYWORD_FUNCTION;
	else if (!dynamic_string_cmp_const_str(str, "if")) token->attribute.keyword = KEYWORD_IF;
	else if (!dynamic_string_cmp_const_str(str, "input")) token->attribute.keyword = KEYWORD_INPUT;
	else if (!dynamic_string_cmp_const_str(str, "integer")) token->attribute.keyword = KEYWORD_INTEGER;
	else if (!dynamic_string_cmp_const_str(str, "loop")) token->attribute.keyword = KEYWORD_LOOP;
	else if (!dynamic_string_cmp_const_str(str, "next")) token->attribute.keyword = KEYWORD_NEXT;
	else if (!dynamic_string_cmp_const_str(str, "not")) token->attribute.keyword = KEYWORD_NOT;
	else if (!dynamic_string_cmp_const_str(str, "or")) token->attribute.keyword = KEYWORD_OR;
	else if (!dynamic_string_cmp_const_str(str, "print")) token->attribute.keyword = KEYWORD_PRINT;
	else if (!dynamic_string_cmp_const_str(str, "return")) token->attribute.keyword = KEYWORD_RETURN;
	else if (!dynamic_string_cmp_const_str(str, "scope")) token->attribute.keyword = KEYWORD_SCOPE;
	else if (!dynamic_string_cmp_const_str(str, "shared")) token->attribute.keyword = KEYWORD_SHARED;
	else if (!dynamic_string_cmp_const_str(str, "string")) token->attribute.keyword = KEYWORD_STRING;
	else if (!dynamic_string_cmp_const_str(str, "static")) token->attribute.keyword = KEYWORD_STATIC;
	else if (!dynamic_string_cmp_const_str(str, "then")) token->attribute.keyword = KEYWORD_THEN;
	else if (!dynamic_string_cmp_const_str(str, "true")) token->attribute.keyword = KEYWORD_TRUE;
	else if (!dynamic_string_cmp_const_str(str, "while")) token->attribute.keyword = KEYWORD_WHILE;
	else token->type = TOKEN_TYPE_IDENTIFIER;

	if (token->type != TOKEN_TYPE_IDENTIFIER)
	{
		token->type = TOKEN_TYPE_KEYWORD;
		return free_resources(SCANNER_TOKEN_OK, str);
	}

	if (!dynamic_string_copy(str, token->attribute.string))
	{
		return free_resources(ERROR_INTERNAL, str);
	}

	return free_resources(SCANNER_TOKEN_OK, str);
}


/**
 * Processing integer.
 *
 * @param str Integer which is being processed.
 * @param token Pointer to output token.
 * @return 0 (SCANNER_TOKEN_OK) if token is OK, otherwise in case of lex error one of SCANNER_ERROR_... constant.
 */
static int process_integer(Dynamic_string *str, Token *token)
{
	char *endptr;

	int val = (int) strtol(str->str, &endptr, 10);
	if (*endptr)
	{
		return free_resources(ERROR_INTERNAL, str);
	}

	token->attribute.integer = val;
	token->type = TOKEN_TYPE_INT_NUMBER;

	return free_resources(SCANNER_TOKEN_OK, str);
}


/**
 * Processing decimal.
 *
 * @param str Decimal which is being processed.
 * @param token Pointer to output token.
 * @return 0 (SCANNER_TOKEN_OK) if token is OK, otherwise in case of lex error one of SCANNER_ERROR_... constant.
 */
static int process_decimal(Dynamic_string *str, Token *token)
{
	char *endptr;

	double val = strtod(str->str, &endptr);
	if (*endptr)
	{
		return free_resources(ERROR_INTERNAL, str);
	}

	token->attribute.decimal = val;
	token->type = TOKEN_TYPE_DOUBLE_NUMBER;

	return free_resources(SCANNER_TOKEN_OK, str);
}


void set_source_file(FILE *f)
{
	source_file = f;
}


void set_dynamic_string(Dynamic_string *string)
{
	dynamic_string = string;
}


int get_next_token(Token *token)
{
	if (source_file == NULL)
	{
		return ERROR_INTERNAL;
	}

	if (dynamic_string == NULL)
	{
		return ERROR_INTERNAL;
	}

	token->attribute.string = dynamic_string;

	// inicialization
	Dynamic_string string;
	Dynamic_string *str = &string;
	if (!dynamic_string_init(str))
	{
		return ERROR_INTERNAL;
	}

	int state = SCANNER_STATE_START;
	token->type = TOKEN_TYPE_EMPTY;

	char c, *endptr, strnum[4] = { 0 };

	// reading chars from source_file
	while (true)
	{
		c = (char) getc(source_file);

		switch (state)
		{
			case (SCANNER_STATE_START):
				if (c == '\n')
				{
					state = SCANNER_STATE_EOL;
				}
				else if (isspace(c))
				{
					state = SCANNER_STATE_START;
				}
				else if (c == '\'')
				{
					state = SCANNER_STATE_COMMENTARY;
				}
				else if (c == '/')
				{
					state = SCANNER_STATE_BACKSLASH;
				}
				else if (isalpha(c) || c == '_')
				{
					if (!dynamic_string_add_char(str, (char) tolower(c)))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
					state = SCANNER_STATE_IDENTIFIER_OR_KEYWORD;
				}
				else if (isdigit(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
					state = SCANNER_STATE_NUMBER;
				}
				else if (c == '!')
				{
					state = SCANNER_STATE_STRING_START;
				}
				else if (c == '<')
				{
					state = SCANNER_STATE_LESS_THAN;
				}
				else if (c == '>')
				{
					state = SCANNER_STATE_MORE_THAN;
				}
				else if (c == '=')
				{
					token->type = TOKEN_TYPE_ASSIGN;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == '+')
				{
					token->type = TOKEN_TYPE_PLUS;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == '-')
				{
					token->type = TOKEN_TYPE_MINUS;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == '*')
				{
					token->type = TOKEN_TYPE_MUL;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == '\\')
				{
					token->type = TOKEN_TYPE_IDIV;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == '(')
				{
					token->type = TOKEN_TYPE_LEFT_BRACKET;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == ')')
				{
					token->type = TOKEN_TYPE_RIGHT_BRACKET;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == ',')
				{
					token->type = TOKEN_TYPE_COMMA;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == ';')
				{
					token->type = TOKEN_TYPE_SEMICOLON;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else if (c == EOF)
				{
					token->type = TOKEN_TYPE_EOF;
					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_COMMENTARY):
				if (c == '\n' || c == EOF)
				{
					state = SCANNER_STATE_START;
					ungetc(c, source_file);
				}

				break;

			case (SCANNER_STATE_BACKSLASH):
				if (c == '\'')
				{
					state = SCANNER_STATE_BLOCK_COMMENTARY;
				}
				else
				{
					ungetc(c, source_file);
					token->type = TOKEN_TYPE_DIV;
					return free_resources(SCANNER_TOKEN_OK, str);
				}

				break;

			case (SCANNER_STATE_BLOCK_COMMENTARY):
				if (c == '\'')
				{
					state = SCANNER_STATE_BLOCK_COMMENTARY_LEAVE;
				}
				else if (c == EOF)
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_BLOCK_COMMENTARY_LEAVE):
				if (c == EOF)
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}
				else if (c == '/')
				{
					state = SCANNER_STATE_START;
				}
				else if (c == '\'')
				{
					state = SCANNER_STATE_BLOCK_COMMENTARY_LEAVE;
				}
				else
				{
					state = SCANNER_STATE_BLOCK_COMMENTARY;
				}

				break;

			case (SCANNER_STATE_IDENTIFIER_OR_KEYWORD):
				if (isalnum(c) || c == '_')
				{
					if (!dynamic_string_add_char(str, (char) tolower(c)))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					ungetc(c, source_file);
					return process_identifier(str, token);
				}

				break;

			case (SCANNER_STATE_NUMBER):
				if (isdigit(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else if (c == '.')
				{
					state = SCANNER_STATE_NUMBER_POINT;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else if (tolower(c) == 'e')
				{
					state = SCANNER_STATE_NUMBER_EXPONENT;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					ungetc(c, source_file);
					return process_integer(str, token);
				}

				break;

			case (SCANNER_STATE_NUMBER_POINT):
				if (isdigit(c))
				{
					state = SCANNER_STATE_NUMBER_DOUBLE;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_NUMBER_DOUBLE):
				if (isdigit(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else if (tolower(c) == 'e')
				{
					state = SCANNER_STATE_NUMBER_EXPONENT;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					ungetc(c, source_file);
					return process_decimal(str, token);
				}

				break;

			case (SCANNER_STATE_NUMBER_EXPONENT):
				if (isdigit(c))
				{
					state = SCANNER_STATE_NUMBER_EXPONENT_FINAL;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else if (c == '+' || c == '-')
				{
					state = SCANNER_STATE_NUMBER_EXPONENT_SIGN;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_NUMBER_EXPONENT_SIGN):
				if (isdigit(c))
				{
					state = SCANNER_STATE_NUMBER_EXPONENT_FINAL;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_NUMBER_EXPONENT_FINAL):
				if (isdigit(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					ungetc(c, source_file);
					return process_decimal(str, token);
				}

				break;

			case (SCANNER_STATE_STRING_START):
				if (c == '"')
				{
					state = SCANNER_STATE_STRING;
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_STRING):
				if (c < 32)
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}
				else if (c == '\\')
				{
					state = SCANNER_STATE_STRING_ESCAPE;
				}
				else if (c == '"')
				{
					if (!dynamic_string_copy(str, token->attribute.string))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
					token->type = TOKEN_TYPE_STRING;

					return free_resources(SCANNER_TOKEN_OK, str);
				}
				else
				{
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}

				break;

			case (SCANNER_STATE_STRING_ESCAPE):
				if (c < 32)
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}
				else if (c == 'n')
				{
					c = '\n';
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
					state = SCANNER_STATE_STRING;
				}
				else if (c == '"')
				{
					c = '"';
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
					state = SCANNER_STATE_STRING;
				}
				else if (c == 't')
				{
					c = '\t';
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
					state = SCANNER_STATE_STRING;
				}
				else if (c == '\\')
				{
					c = '\\';
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
					state = SCANNER_STATE_STRING;
				}
				else if (c == '0' || c == '1')
				{
					strnum[0] = c;
					state = SCANNER_STATE_STRING_ESCAPE_ZEROONE;
				}
				else if (c == '2')
				{
					strnum[0] = c;
					state = SCANNER_STATE_STRING_ESCAPE_TWO;
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_STRING_ESCAPE_ZEROONE):
				if (isdigit(c))
				{
					strnum[1] = c;
					state = SCANNER_STATE_STRING_ESCAPE_ZEROONE_ZERONINE;
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_STRING_ESCAPE_ZEROONE_ZERONINE):
				if (isdigit(c))
				{
					strnum[2] = c;
					state = SCANNER_STATE_STRING;

					int val = (int) strtol(strnum, &endptr, 10);
					if (*endptr)
					{
						return free_resources(ERROR_INTERNAL, str);
					}

					c = (char) val;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_STRING_ESCAPE_TWO):
				if (isdigit(c))
				{
					if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4')
					{
						strnum[1] = c;
						state = SCANNER_STATE_STRING_ESCAPE_TWO_ZEROFOUR;
					}
					else if (c == '5')
					{
						strnum[1] = c;
						state = SCANNER_STATE_STRING_ESCAPE_TWO_FIVE;
					}
					else
					{
						return free_resources(SCANNER_ERROR_LEX, str);
					}
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_STRING_ESCAPE_TWO_ZEROFOUR):
				if (isdigit(c))
				{
					strnum[2] = c;
					state = SCANNER_STATE_STRING;

					int val = (int) strtol(strnum, &endptr, 10);
					if (*endptr)
					{
						return free_resources(ERROR_INTERNAL, str);
					}

					c = (char) val;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_STRING_ESCAPE_TWO_FIVE):
				if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5')
				{
					strnum[2] = c;
					state = SCANNER_STATE_STRING;

					int val = (int) strtol(strnum, &endptr, 10);
					if (*endptr)
					{
						return free_resources(ERROR_INTERNAL, str);
					}

					c = (char) val;
					if (!dynamic_string_add_char(str, c))
					{
						return free_resources(ERROR_INTERNAL, str);
					}
				}
				else
				{
					return free_resources(SCANNER_ERROR_LEX, str);
				}

				break;

			case (SCANNER_STATE_LESS_THAN):
				if (c == '>')
				{
					token->type = TOKEN_TYPE_NEQ;
				}
				else if (c == '=')
				{
					token->type = TOKEN_TYPE_LEQ;
				}
				else
				{
					ungetc(c, source_file);
					token->type = TOKEN_TYPE_LTN;
				}

				return free_resources(SCANNER_TOKEN_OK, str);

			case (SCANNER_STATE_MORE_THAN):
				if (c == '=')
				{
					token->type = TOKEN_TYPE_MEQ;
				}
				else
				{
					ungetc(c, source_file);
					token->type = TOKEN_TYPE_MTN;
				}

				return free_resources(SCANNER_TOKEN_OK, str);

			case (SCANNER_STATE_EOL):
				if (isspace(c))
				{
					break;
				}

				ungetc(c, source_file);
				token->type = TOKEN_TYPE_EOL;
				return free_resources(SCANNER_TOKEN_OK, str);
		}
	}
}
