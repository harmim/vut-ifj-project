/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Scanner interface.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 */


#ifndef _SCANNER_H
#define _SCANNER_H


#include <stdbool.h>

#include "dynamic_string.h"


// Error codes.
#define SCANNER_TOKEN_OK 0 /// Token is OK.
#define SCANNER_ERROR_LEX 1 /// Lex structure error.
#define SCANNER_ERROR_INTERNAL 99 /// Internal error, eg. malloc error etc.


// Scanner states.
#define SCANNER_STATE_START 200 /// Starting state every new token processing starts here and initializes other states
#define SCANNER_STATE_COMMENTARY 201 /// Line commentary, ignores every symbol, ends with EOL
#define SCANNER_STATE_BACKSLASH 202 /// Operator / OR start of block comment - next symbol must be '
#define SCANNER_STATE_BLOCK_COMMENTARY 203 /// Starts with /' and ignores every symbol except ' - this might be the end of block comment
#define SCANNER_STATE_BLOCK_COMMENTARY_LEAVE 204 /// Ends with '/ the ' is read, if the next symbol is /, leave, else if ', stay, else go back to state before
#define SCANNER_STATE_IDENTIFIER_OR_KEYWORD 205 /// Starts with letter or _, if next symbols are alphanumeric or _, add them to string, which is later compared with reserved words | Returns either keyword or string as ID
#define SCANNER_STATE_NUMBER 206 /// Start of number processing, accepts numbers, e/E and . | Can return integer number
#define SCANNER_STATE_NUMBER_POINT 207 /// If symbol was ., the number has type double
#define SCANNER_STATE_NUMBER_DOUBLE 208 /// The last symbol was number | Can return double number
#define SCANNER_STATE_NUMBER_EXPONENT 209 /// The last symbol was e or E, the number has type double, continues with optional symbols +/- or number
#define SCANNER_STATE_NUMBER_EXPONENT_SIGN 210 /// Optional symbol was read, continue with numbers only
#define SCANNER_STATE_NUMBER_EXPONENT_FINAL 211 /// Returns double number with exponent
#define SCANNER_STATE_STRING_START 212 /// String starts with !" else returns error
#define SCANNER_STATE_STRING 213 /// Sequence !" was read, ends with ", if ASCII value is lower than 32, returns error, these symbols can be written using escape sequence | Returns string
#define SCANNER_STATE_STRING_ESCAPE 214 /// If symbol \ was loaded, can replace char with escape sequence symbols
#define SCANNER_STATE_STRING_ESCAPE_ZEROONE 215 /// 0 or 1 was loaded, accepts only digits
#define SCANNER_STATE_STRING_ESCAPE_ZEROONE_ZERONINE 216 /// first 0 or 1 then 0 to 9 was loaded, accepts only digits, returns symbol with ASCII value
#define SCANNER_STATE_STRING_ESCAPE_TWO 217 /// 2 was loaded, accepts only 0 to 5 digits
#define SCANNER_STATE_STRING_ESCAPE_TWO_ZEROFOUR 218 /// first 2 then 0 to 9 was loaded, accepts only digits, returns symbol with ASCII value
#define SCANNER_STATE_STRING_ESCAPE_TWO_FIVE 219 /// first 2 then 5 was loaded, accepts only 0 to 5 digits, returns symbol with ASCII value
#define SCANNER_STATE_LESS_THAN 220 /// Starts with < | Returns <>, <= or <
#define SCANNER_STATE_MORE_THAN 221 /// Starts with > | Returns > or >=


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
 * @enum Type of token.
 */
enum token_type
{
	TOKEN_TYPE_EOF, /// End of file
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
};

/**
 * @union Token attribute.
 */
union token_attribute
{
	struct dynamic_string *string; /// String or identifier value.
	int integer; /// Integer value.
	enum keyword keyword; /// Keyword, one of the KEYWORD_... constant
	double decimal; /// Decimal value.
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
 * @return 0 (SCANNER_TOKEN_OK) if token is OK, otherwise in case of lex error one of SCANNER_ERROR_... constant.
 */
int get_next_token(FILE *source_file, struct token *token);


#endif //_SCANNER_H
