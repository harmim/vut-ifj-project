/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Scanner implementation.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 */


#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"


static int process_identifier(struct dynamic_string *str, struct token *token)
{
	if (!dynamic_string_cmp_const_str(str, "and")) token->attribute.keyword = KEYWORD_AND;
	else if (!dynamic_string_cmp_const_str(str, "as")) token->attribute.keyword = KEYWORD_AS;
	else if (!dynamic_string_cmp_const_str(str, "asc")) token->attribute.keyword = KEYWORD_ASC;
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
	else if (!dynamic_string_cmp_const_str(str, "chr")) token->attribute.keyword = KEYWORD_CHR;
	else if (!dynamic_string_cmp_const_str(str, "function")) token->attribute.keyword = KEYWORD_FUNCTION;
	else if (!dynamic_string_cmp_const_str(str, "if")) token->attribute.keyword = KEYWORD_IF;
	else if (!dynamic_string_cmp_const_str(str, "input")) token->attribute.keyword = KEYWORD_INPUT;
	else if (!dynamic_string_cmp_const_str(str, "integer")) token->attribute.keyword = KEYWORD_INTEGER;
	else if (!dynamic_string_cmp_const_str(str, "lenght")) token->attribute.keyword = KEYWORD_LENGTH;
	else if (!dynamic_string_cmp_const_str(str, "loop")) token->attribute.keyword = KEYWORD_LOOP;
	else if (!dynamic_string_cmp_const_str(str, "next")) token->attribute.keyword = KEYWORD_NEXT;
	else if (!dynamic_string_cmp_const_str(str, "not")) token->attribute.keyword = KEYWORD_NOT;
	else if (!dynamic_string_cmp_const_str(str, "or")) token->attribute.keyword = KEYWORD_OR;
	else if (!dynamic_string_cmp_const_str(str, "print")) token->attribute.keyword = KEYWORD_PRINT;
	else if (!dynamic_string_cmp_const_str(str, "return")) token->attribute.keyword = KEYWORD_RETURN;
	else if (!dynamic_string_cmp_const_str(str, "scope")) token->attribute.keyword = KEYWORD_SCOPE;
	else if (!dynamic_string_cmp_const_str(str, "shared")) token->attribute.keyword = KEYWORD_SHARED;
	else if (!dynamic_string_cmp_const_str(str, "static")) token->attribute.keyword = KEYWORD_STATIC;
	else if (!dynamic_string_cmp_const_str(str, "substr")) token->attribute.keyword = KEYWORD_SUBSTR;
	else if (!dynamic_string_cmp_const_str(str, "then")) token->attribute.keyword = KEYWORD_THEN;
	else if (!dynamic_string_cmp_const_str(str, "true")) token->attribute.keyword = KEYWORD_TRUE;
	else if (!dynamic_string_cmp_const_str(str, "while")) token->attribute.keyword = KEYWORD_WHILE;
	else token->type = TOKEN_TYPE_IDENTIFIER;

	if (token->type != TOKEN_TYPE_IDENTIFIER)
	{
		token->type = TOKEN_TYPE_KEYWORD;
		return SCANNER_TOKEN_OK;
	}

	// TODO: uložit do tabulky symbolů hodnout str a do tokenu do ptr uložit ukazatel na tuto hodnotu v tabulce symboů

	return SCANNER_TOKEN_OK;
}


int get_next_token(FILE *source_file, struct token *token, struct dynamic_string *str)
{
	int state = SCANNER_STATE_START;
	token->type = TOKEN_TYPE_EMPTY;
	dynamic_string_clear(str);

	bool is_double_number = false; // flag
	char c, *endptr;

	while (true)
	{
		c = (char) tolower(getc(source_file));

		switch (state)
		{
			case (SCANNER_STATE_START):
				if (isspace(c))
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
				else if (isalpha(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
					state = SCANNER_STATE_IDENTIFIER_OR_KEYWORD;
				}
				else if (isdigit(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
					state = SCANNER_STATE_NUMBER;
				}
				else if (c == '!')
				{
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
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
					return SCANNER_TOKEN_OK;
				}
				else if (c == '+')
				{
					token->type = TOKEN_TYPE_PLUS;
					return SCANNER_TOKEN_OK;
				}
				else if (c == '-')
				{
					token->type = TOKEN_TYPE_MINUS;
					return SCANNER_TOKEN_OK;
				}
				else if (c == '*')
				{
					token->type = TOKEN_TYPE_MUL;
					return SCANNER_TOKEN_OK;
				}
				else if (c == '\\')
				{
					token->type = TOKEN_TYPE_IDIV;
					return SCANNER_TOKEN_OK;
				}
				else if (c == '(')
				{
					token->type = TOKEN_TYPE_LEFT_BRACKET;
					return SCANNER_TOKEN_OK;
				}
				else if (c == ')')
				{
					token->type = TOKEN_TYPE_RIGHT_BRACKET;
					return SCANNER_TOKEN_OK;
				}
				else if (c == ',')
				{
					token->type = TOKEN_TYPE_RIGHT_COMMA;
					return SCANNER_TOKEN_OK;
				}
				else if (c == ';')
				{
					token->type = TOKEN_TYPE_SEMICOLEN;
					return SCANNER_TOKEN_OK;
				}
				else if (c == EOF)
				{
					token->type = TOKEN_TYPE_EOF;
					return SCANNER_TOKEN_OK;
				}
				else
				{
					return SCANNER_ERROR_LEX;
				}

				break;

			case (SCANNER_STATE_COMMENTARY):
				if (c == '\n')
				{
					state = SCANNER_STATE_START;
				}

				break;

			case (SCANNER_STATE_BACKSLASH):
				if (c == '\'')
				{
					state = SCANNER_STATE_BLOCK_COMMENTARY;
				}
				else
				{
					token->type = TOKEN_TYPE_DIV;
					return SCANNER_TOKEN_OK;
				}

				break;

			case (SCANNER_STATE_BLOCK_COMMENTARY):
				if (c == '\'')
				{
					state = SCANNER_STATE_BLOCK_COMMENTARY_LEAVE;
				}
				else if (c == EOF)
				{
					return SCANNER_ERROR_LEX;
				}

				break;

			case (SCANNER_STATE_BLOCK_COMMENTARY_LEAVE):
				if (c == EOF)
				{
					return SCANNER_ERROR_LEX;
				}
				else if (c == '/')
				{
					state = SCANNER_STATE_START;
				}
				else
				{
					state = SCANNER_STATE_BLOCK_COMMENTARY;
				}

				break;

			case (SCANNER_STATE_IDENTIFIER_OR_KEYWORD):
				if (isalnum(c) || c == '_')
				{
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
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
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else if (c == '.')
				{
					state = SCANNER_STATE_NUMBER_POINT;
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
					is_double_number = true;
				}
				else if (c == 'e')
				{
					state = SCANNER_STATE_NUMBER_EXPONENT;
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else
				{
					ungetc(c, source_file);
					int val = (int) strtol(str->str, &endptr, 10);
					if (*endptr)
					{
						return SCANNER_ERROR_INTERNAL;
					}
					token->attribute.integer = val;
					token->type = TOKEN_TYPE_INT_NUMBER;

					return SCANNER_TOKEN_OK;
				}

				break;

			case (SCANNER_STATE_NUMBER_POINT):
				if (isdigit(c))
				{
					state = SCANNER_STATE_STATE_NUMBER_DOUBLE;
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else
				{
					return SCANNER_ERROR_LEX;
				}

				break;

			case (SCANNER_STATE_STATE_NUMBER_DOUBLE):
				if (isdigit(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else if (c == 'e')
				{
					state = SCANNER_STATE_NUMBER_EXPONENT;
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else
				{
					ungetc(c, source_file);
					double val = strtod(str->str, &endptr);
					if (*endptr)
					{
						return SCANNER_ERROR_INTERNAL;
					}
					token->attribute.decimal = val;
					token->type = TOKEN_TYPE_DOUBLE_NUMBER;

					return SCANNER_TOKEN_OK;
				}

				break;

			case (SCANNER_STATE_NUMBER_EXPONENT):
				if (isdigit(c))
				{
					state = SCANNER_STATE_NUMBER_EXPONENT_FINAL;
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else if (c == '+' || c == '-') // + -
				{
					state = SCANNER_STATE_NUMBER_EXPONENT_SIGN;
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else
				{
					return SCANNER_ERROR_LEX;
				}

				break;

			case (SCANNER_STATE_NUMBER_EXPONENT_SIGN):
				if (isdigit(c))
				{
					state = SCANNER_STATE_NUMBER_EXPONENT_FINAL;
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else
				{
					return SCANNER_ERROR_LEX;
				}

				break;

			case (SCANNER_STATE_NUMBER_EXPONENT_FINAL):
				if (isdigit(c))
				{
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
				}
				else
				{
					ungetc(c, source_file);
					if (is_double_number)
					{
						double val = strtod(str->str, &endptr);
						if (*endptr)
						{
							return SCANNER_ERROR_INTERNAL;
						}
						token->attribute.decimal = val;
						token->type = TOKEN_TYPE_DOUBLE_NUMBER;
					}
					else
					{
						int val = (int) strtol(str->str, &endptr, 10);
						if (*endptr)
						{
							return SCANNER_ERROR_INTERNAL;
						}
						token->attribute.integer = val;
						token->type = TOKEN_TYPE_INT_NUMBER;
					}

					return SCANNER_TOKEN_OK;
				}

				break;

			case (SCANNER_STATE_STRING_START):
				if (c == '"')
				{
					state = SCANNER_STATE_STRING;
				}
				else
				{
					return SCANNER_ERROR_LEX;
				}

				break;

			case (SCANNER_STATE_STRING):
				if (c <= 31) // lower ASCII can be written by using escape sequence
				{
					return SCANNER_ERROR_LEX;
				}
				else if (c == '"')
				{
					if (
						!dynamic_string_add_char(str, c)
							|| !dynamic_string_init(token->attribute.str)
							|| !dynamic_string_copy(str, token->attribute.str)
						)
					{
						return SCANNER_ERROR_INTERNAL;
					}

					token->type = TOKEN_TYPE_STRING;

					return SCANNER_TOKEN_OK;
				}
				else
				{
					if (!dynamic_string_add_char(str, c))
					{
						return SCANNER_ERROR_INTERNAL;
					}
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

				return SCANNER_ERROR_LEX;

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

				return SCANNER_ERROR_LEX;
		}
	}
}
