#include "analysis.h"
#include "scanner.h"
#include <stdio.h>

Token token;
extern FILE* source_file;

int prog()
{
	int result;

	// <prog> -> SCOPE EOL <statement> END SCOPE EOL <prog>
	if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_SCOPE)
	{
		// get next token and check EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = statement()) return result;

		// get next token and check for END token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for SCOPE token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_SCOPE) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <prog> rule
		if (result = get_next_token(source_file, &token)) return result;
		return prog();
	}

	// <prog> -> DECLARE <func_head> EOL <prog>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_DECLARE)
	{
		// get next token and execute <func_head> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = func_head()) return result;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <prog> rule
		if (result = get_next_token(source_file, &token)) return result;
		return prog();		
	}

	// <prog> -> EOF
	else if (token.type == TOKEN_TYPE_EOF)
	{
		return SYNTAX_OK;
	}

	// <prog> -> <func_head> <statement> END FUNCTION EOL <prog>
	else
	{
		// get next token and execute <func_head> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = func_head()) return result;
		
		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = statement()) return result;

		// get next token and check for END token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for FUNCTION token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <prog> rule
		if (result = get_next_token(source_file, &token)) return result;
		return prog();
	}

	return SYNTAX_ERR;
}

int func_head()
{
	int result;

	// <func_head> -> FUNCTION ID <params> AS TYPE
	if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;
	
	// get next token and check for ID token
	if (result = get_next_token(source_file, &token)) return result;
	if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

	// get next token and execute <params> rule
	if (result = get_next_token(source_file, &token)) return result;
	if (result = params()) return result;

	// get next token and check for AS token
	if (result = get_next_token(source_file, &token)) return result;
	if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

	// get next token and check for TYPE token
	if (result = get_next_token(source_file, &token)) return result;
	if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
		token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

	return SYNTAX_OK;
}

int params()
{
	int result;

	// <params> -> ( <param> )
	if (token.type == TOKEN_TYPE_LEFT_BRACKET)
	{
		// get next token and execute <param> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = param()) return result;

		// get next token and check for ) token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_RIGHT_BRACKET) return SYNTAX_ERR;
	}

	// <params> -> ε
	
	return SYNTAX_OK;
}

int param()
{
	int result;
	// <param> -> ID AS TYPE <param_n>
	if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

	// get next token and check for AS token
	if (result = get_next_token(source_file, &token)) return result;
	if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

	// get next token and check for TYPE token
	if (result = get_next_token(source_file, &token)) return result;
	if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
		token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

	// get next token and execute <param_n> rule
	if (result = get_next_token(source_file, &token)) return result;
	if (result = param_n()) return result;

	return SYNTAX_OK;
}

int param_n()
{
	int result;

	// <param_n> -> , ID AS TYPE <param_n>
	if (token.type == TOKEN_TYPE_COMMA)
	{
		// get next token and check for ID token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for AS token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
			token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

		// get next token and execute <param_n> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = param_n()) return result;
	}

	// <param_n> -> ε

	return SYNTAX_OK;
}

int statement()
{
	int result;

	// <statement> -> DIM ID AS TYPE <def_var> EOL <statement>
	if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_DIM)
	{
		// get next token and check for ID token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for AS token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
			token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

		// get next token and execute <def_var> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = def_var()) return result;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;
		
		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		return statement();
	}

	// <statement> -> IF <expression> THEN EOL <statement> ELSE EOL <statement> END IF EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_IF)
	{
		// get next token and execute <expression> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = expression()) return result;

		// get next token and check for THEN token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_THEN) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = statement()) return result;

		// get next token and check for ELSE token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_ELSE) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = statement()) return result;

		// get next token and check for END token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for IF token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_IF) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		return statement();
	}

	// <statement> -> DO WHILE <expression> EOL <statement> LOOP EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_DO)
	{
		// get next token and check for WHILE token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_WHILE) return SYNTAX_ERR;

		// get next token and execute <expression> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = expression()) return result;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = statement()) return result;

		// get next token and check for LOOP token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_LOOP) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		return statement();
	}

	// <statement> -> ID = <expression> EOL <statement>
	else if (token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// get next token and check for = token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_ASSIGN) return SYNTAX_ERR;

		// get next token and execute <expression> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = expression()) return result;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		return statement();
	}

	// <statement> -> INPUT ID EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_INPUT)
	{
		// get next token and check for ID token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		return statement();
	}

	// <statement> -> PRINT <expression> ; <print> EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_PRINT)
	{
		// get next token and execute <expression> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = expression()) return result;

		// get next token and check for ; token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_SEMICOLON) return SYNTAX_ERR;

		// get next token and execute <print> rule
		if (result = get_next_token(source_file, &token)) return result;
		if (result = print()) return result;

		// get next token and check for EOL token
		if (result = get_next_token(source_file, &token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(source_file, &token)) return result;
		return statement();
	}

	// TODO
	// <statement> -> RETURN <expression> EOL <statement>

	// <statement> -> ε

	return SYNTAX_OK;
}