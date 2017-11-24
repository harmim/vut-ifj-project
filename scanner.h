/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Scanner interface.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 */


#ifndef _SCANNER_H
#define _SCANNER_H


#include <stdio.h>

#include "dynamic_string.h"


/**
 * @enum Reserved keywords.
 */
typedef enum
{
	KEYWORD_AND,
	KEYWORD_AS,
	KEYWORD_BOOLEAN,
	KEYWORD_CONTINUE,
	KEYWORD_DECLARE,
	KEYWORD_DIM,
	KEYWORD_DO,
	KEYWORD_DOUBLE,
	KEYWORD_ELSE,
	KEYWORD_ELSEIF,
	KEYWORD_END,
	KEYWORD_EXIT,
	KEYWORD_FALSE,
	KEYWORD_FOR,
	KEYWORD_FUNCTION,
	KEYWORD_IF,
	KEYWORD_INPUT,
	KEYWORD_INTEGER,
	KEYWORD_LOOP,
	KEYWORD_NEXT,
	KEYWORD_NOT,
	KEYWORD_OR,
	KEYWORD_PRINT,
	KEYWORD_RETURN,
	KEYWORD_SCOPE,
	KEYWORD_SHARED,
	KEYWORD_STRING,
	KEYWORD_STATIC,
	KEYWORD_THEN,
	KEYWORD_TRUE,
	KEYWORD_WHILE,
} Keyword;

/**
 * @enum Type of token.
 */
typedef enum
{
	TOKEN_TYPE_EOF, /// End of file
	TOKEN_TYPE_EOL, /// End of line
	TOKEN_TYPE_EMPTY, /// Empty
	TOKEN_TYPE_IDENTIFIER, /// Identifier
	TOKEN_TYPE_KEYWORD, /// Keyword

	TOKEN_TYPE_INT_NUMBER, /// Integer number
	TOKEN_TYPE_DOUBLE_NUMBER, /// Double number
	TOKEN_TYPE_STRING, /// String

	TOKEN_TYPE_NEQ, /// Not equal <>
	TOKEN_TYPE_LEQ, /// Less or equal <=
	TOKEN_TYPE_LTN, /// Less than <
	TOKEN_TYPE_MEQ, /// More or equal >=
	TOKEN_TYPE_MTN, /// More than >

	// operators
	TOKEN_TYPE_ASSIGN, /// Assign =
	TOKEN_TYPE_PLUS, /// Plus +
	TOKEN_TYPE_MINUS, /// Minus -
	TOKEN_TYPE_MUL, /// Multiplication *
	TOKEN_TYPE_DIV, /// Division / result always double
	TOKEN_TYPE_IDIV, /// Integer division \ only works with integers

	TOKEN_TYPE_LEFT_BRACKET, /// Left bracket (
	TOKEN_TYPE_RIGHT_BRACKET, /// Right bracket )
	TOKEN_TYPE_COMMA, /// Comma ,
	TOKEN_TYPE_SEMICOLON, /// Semicolon ;
} Token_type;

/**
 * @union Token attribute.
 */
typedef union
{
	Dynamic_string *string; /// String or identifier value.
	int integer; /// Integer value.
	Keyword keyword; /// Keyword, one of the KEYWORD_... constant
	double decimal; /// Decimal value.
} Token_attribute;

/**
 * @struct Token representation.
 */
typedef struct
{
	Token_type type; /// Token type, one of token_type constants.
	Token_attribute attribute; /// Attribute of token.
} Token;


/**
 * Sets source file to be scanned.
 *
 * @param f Pointer to source file.
 */
void set_source_file(FILE *f);

/**
 * Sets dynamic string to be written into.
 *
 * @param string Pointer to dynamic string.
 */
void set_dynamic_string(Dynamic_string *string);

/**
 * This is the main function of scanner, scans token after token and sends it further.
 *
 * @param token Pointer to output token.
 * @return 0 (SCANNER_TOKEN_OK) if token is OK, otherwise in case of lex error one of SCANNER_ERROR_... constant.
 */
int get_next_token(Token *token);


#endif //_SCANNER_H
