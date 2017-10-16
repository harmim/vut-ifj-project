/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Scanner interface.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 */


#ifndef _SCANNER_H
#define _SCANNER_H

#include "dynamic_string.h"


// Error codes.
#define SCANNER_TOKEN_OK 0 /// Token is OK.
#define SCANNER_ERROR_LEX 1 /// Lex structure error.
#define SCANNER_ERROR_INTERNAL 99 /// Internal error, eg. malloc error etc.


// Scanner states.
#define SCANNER_STATE_START 200
#define SCANNER_STATE_COMMENTARY 201
#define SCANNER_STATE_BACKSLASH 202
#define SCANNER_STATE_BLOCK_COMMENTARY 203
#define SCANNER_STATE_BLOCK_COMMENTARY_LEAVE 204
#define SCANNER_STATE_IDENTIFIER_OR_KEYWORD 205
#define SCANNER_STATE_NUMBER 206
#define SCANNER_STATE_NUMBER_POINT 207
#define SCANNER_STATE_STATE_NUMBER_DOUBLE 208
#define SCANNER_STATE_NUMBER_EXPONENT 209
#define SCANNER_STATE_NUMBER_EXPONENT_SIGN 210
#define SCANNER_STATE_NUMBER_EXPONENT_FINAL 211
#define SCANNER_STATE_STRING_START 212
#define SCANNER_STATE_STRING 213
#define SCANNER_STATE_LESS_THAN 214
#define SCANNER_STATE_MORE_THAN 215


/**
 * @enum Reserved keywords.
 */
enum keyword
{
	KEYWORD_AND,
	KEYWORD_AS,
	KEYWORD_ASC,
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
	KEYWORD_CHR,
	KEYWORD_FUNCTION,
	KEYWORD_IF,
	KEYWORD_INPUT,
	KEYWORD_INTEGER,
	KEYWORD_LENGTH,
	KEYWORD_LOOP,
	KEYWORD_NEXT,
	KEYWORD_NOT,
	KEYWORD_OR,
	KEYWORD_PRINT,
	KEYWORD_RETURN,
	KEYWORD_SCOPE,
	KEYWORD_SHARED,
	KEYWORD_STATIC,
	KEYWORD_SUBSTR,
	KEYWORD_THEN,
	KEYWORD_TRUE,
	KEYWORD_WHILE,
};

/**
 * TODO: Comment each token type.
 * @enum Type of token.
 */
enum token_type
{
	TOKEN_TYPE_EOF,
	TOKEN_TYPE_EMPTY,
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_KEYWORD,

	TOKEN_TYPE_INT_NUMBER,
	TOKEN_TYPE_DOUBLE_NUMBER,
	TOKEN_TYPE_STRING,

	TOKEN_TYPE_NEQ,
	TOKEN_TYPE_LEQ,
	TOKEN_TYPE_LTN,
	TOKEN_TYPE_MEQ,
	TOKEN_TYPE_MTN,

	// operators
	TOKEN_TYPE_ASSIGN,
	TOKEN_TYPE_PLUS,
	TOKEN_TYPE_MINUS,
	TOKEN_TYPE_MUL,
	TOKEN_TYPE_DIV,
	TOKEN_TYPE_IDIV,

	TOKEN_TYPE_LEFT_BRACKET,
	TOKEN_TYPE_RIGHT_BRACKET,
	TOKEN_TYPE_RIGHT_COMMA,
	TOKEN_TYPE_SEMICOLEN,
};

/**
 * @union Token attribute.
 */
union token_attribute
{
	int integer; /// Integer value.
	enum keyword keyword; /// Keyword, one of the KEYWORD_... constant
	double decimal; /// Decimal value.
	// FIXME: not char *, but pointer to table of symbols
	char *identifier; /// Pointer to table of symbols in case of identifier.
	struct dynamic_string *str; /// String value.
};

/**
 * @struct Token representation.
 */
struct token
{
	enum token_type type; /// Token type, one of token_type constants.
	union token_attribute attribute; /// Attribute of token.
};


/**
 * This is the main function of scanner, scans token after token and sends it further.
 *
 * @param source_file Source file to be scanned.
 * @param token Pointer to output token.
 * @param str Dynamic string for lex processing.
 * @return 0 (SCANNER_TOKEN_OK) if token is OK, otherwise in case of lex error one of SCANNER_ERROR_... constant.
 */
int get_next_token(FILE *source_file, struct token *token, struct dynamic_string *str);


#endif //_SCANNER_H
