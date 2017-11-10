/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Syntactical and semantical analysis imeplementation.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */


#include <stdio.h>

#include "error.h"
#include "scanner.h"
#include "analysis.h"

Token token;

bool scope_processed;
bool in_function;

// forward declarations
int statement();
int func_head();
int expression();
int def_var();
int print();
int param_n();
int def_value();
int arg_n();
int value();
int arg();
int params();
int param();

/**
 * Implementation of <prog> rule.
 *
 * @return Given exit code.
 */
int prog()
{
	int result;

	// <prog> -> SCOPE EOL <statement> END SCOPE EOL <prog>
	if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_SCOPE)
	{
		// we are in scope
		in_function = false;
		
		// program may contain only 1 scope
		if (scope_processed)
			return SEM_ERR_OTHER;
		else
			scope_processed = true;

		// get next token and check EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		if (result = statement()) return result;

		// check for END token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for SCOPE token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_SCOPE) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <prog> rule
		if (result = get_next_token(&token)) return result;
		return prog();
	}

	// <prog> -> DECLARE FUNCTION ID <params> AS TYPE EOL <prog>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_DECLARE)
	{
		// get next token and check for FUNCTION token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;

		// get next token and check for ID token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and execute <params> rule
		if (result = get_next_token(&token)) return result;
		if (result = params()) return result;

		// check for AS token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
			token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <prog> rule
		if (result = get_next_token(&token)) return result;
		return prog();		
	}

	// <prog> -> EOF
	else if (token.type == TOKEN_TYPE_EOF)
	{
		return SYNTAX_OK;
	}

	// <prog> -> FUNCTION ID <params> AS TYPE EOL <statement> END FUNCTION EOL <prog>
	else
	{
		// get next token and check for FUNCTION token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;

		// get next token and check for ID token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and execute <params> rule
		if (result = get_next_token(&token)) return result;
		if (result = params()) return result;

		// check for AS token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
			token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		if (result = statement()) return result;

		// check for END token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for FUNCTION token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <prog> rule
		if (result = get_next_token(&token)) return result;
		return prog();
	}

	return SYNTAX_ERR;
}

/**
 * Implementation of <params> rule.
 *
 * @return Given exit code.
 */
int params()
{
	int result;

	// <params> -> ( <param> )
	if (token.type == TOKEN_TYPE_LEFT_BRACKET)
	{
		// get next token and execute <param> rule
		if (result = get_next_token(&token)) return result;
		if (result = param()) return result;

		// check for ) token
		if (token.type != TOKEN_TYPE_RIGHT_BRACKET) return SYNTAX_ERR;

		// get next token
		if (result = get_next_token(&token)) return result;
	}

	// <params> -> ε
	
	return SYNTAX_OK;
}

/**
 * Implementation of <param> rule.
 *
 * @return Given exit code.
 */
int param()
{
	int result;

	// <param> -> ID AS TYPE <param_n>
	if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

	// get next token and check for AS token
	if (result = get_next_token(&token)) return result;
	if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

	// get next token and check for TYPE token
	if (result = get_next_token(&token)) return result;
	if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
		token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

	// get next token and execute <param_n> rule
	if (result = get_next_token(&token)) return result;
	if (result = param_n()) return result;

	return SYNTAX_OK;
}

/**
 * Implementation of <param_n> rule.
 *
 * @return Given exit code.
 */
int param_n()
{
	int result;

	// <param_n> -> , ID AS TYPE <param_n>
	if (token.type == TOKEN_TYPE_COMMA)
	{
		// get next token and check for ID token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for AS token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
			token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

		// get next token and execute <param_n> rule
		if (result = get_next_token(&token)) return result;
		if (result = param_n()) return result;
	}

	// <param_n> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <statement> rule.
 *
 * @return Given exit code.
 */
int statement()
{
	int result;

	// <statement> -> DIM ID AS TYPE <def_var> EOL <statement>
	if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_DIM)
	{
		// get next token and check for ID token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for AS token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || !(token.attribute.keyword == KEYWORD_INTEGER ||
			token.attribute.keyword == KEYWORD_DOUBLE || token.attribute.keyword == KEYWORD_STRING)) return SYNTAX_ERR;

		// get next token and execute <def_var> rule
		if (result = get_next_token(&token)) return result;
		if (result = def_var()) return result;

		// check for EOL token
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;
		
		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		return statement();
	}

	// <statement> -> IF <expression> THEN EOL <statement> ELSE EOL <statement> END IF EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_IF)
	{
		// get next token and execute <expression> rule
		if (result = get_next_token(&token)) return result;
		if (result = expression()) return result;

		// check for THEN token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_THEN) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		if (result = statement()) return result;

		// check for ELSE token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_ELSE) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		if (result = statement()) return result;

		// check for END token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for IF token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_IF) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		return statement();
	}

	// <statement> -> DO WHILE <expression> EOL <statement> LOOP EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_DO)
	{
		// get next token and check for WHILE token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_WHILE) return SYNTAX_ERR;

		// get next token and execute <expression> rule
		if (result = get_next_token(&token)) return result;
		if (result = expression()) return result;

		// check for EOL token
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		if (result = statement()) return result;

		// check for LOOP token
		if (token.type != TOKEN_TYPE_KEYWORD || token.attribute.keyword != KEYWORD_LOOP) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		return statement();
	}

	// <statement> -> ID = <expression> EOL <statement>
	else if (token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// get next token and check for = token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_ASSIGN) return SYNTAX_ERR;

		// get next token and execute <expression> rule
		if (result = get_next_token(&token)) return result;
		if (result = expression()) return result;

		// check for EOL token
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		return statement();
	}

	// <statement> -> INPUT ID EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_INPUT)
	{
		// get next token and check for ID token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		return statement();
	}

	// <statement> -> PRINT <expression> ; <print> EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_PRINT)
	{
		// get next token and execute <expression> rule
		if (result = get_next_token(&token)) return result;
		if (result = expression()) return result;

		// check for SEMICOLON token
		if (token.type != TOKEN_TYPE_SEMICOLON) return SYNTAX_ERR;

		// get next token and execute <print> rule
		if (result = get_next_token(&token)) return result;
		if (result = print()) return result;

		// check for EOL token
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		return statement();
	}

	// <statement> -> RETURN <expression> EOL <statement>
	else if (token.type == TOKEN_TYPE_KEYWORD && token.attribute.keyword == KEYWORD_RETURN)
	{
		// scope doesn't have this type of rule
		if (!in_function) return SEM_ERR_OTHER;

		// get next token and execute <expression> rule
		if (result = get_next_token(&token)) return result;
		if (result = expression()) return result;

		// check for EOL token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&token)) return result;
		return statement();
	}

	// <statement> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <def_var> rule.
 *
 * @return Given exit code.
 */
int def_var()
{
	int result;

	// <def_var> -> = <def_val>
	if (token.type == TOKEN_TYPE_ASSIGN)
	{
		// get next token and execute <def_val> rule
		if (result = get_next_token(&token)) return result;
		if (result = def_value()) return result;
	}

	// <def_var> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <def_value> rule.
 *
 * @return Given exit code.
 */
int def_value()
{
	int result;

	// <def_value> -> ID( <arg> )
	if (token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// get next token and check for ( token
		if (result = get_next_token(&token)) return result;
		if (token.type != TOKEN_TYPE_LEFT_BRACKET) return SYNTAX_ERR;

		// get next token and execute <arg> rule
		if (result = get_next_token(&token)) return result;
		if (result = arg()) return result;

		// check for ) token
		if (token.type != TOKEN_TYPE_RIGHT_BRACKET) return SYNTAX_ERR;

		// get next token
		if (result = get_next_token(&token)) return result;
	}

	// <def_value> -> <expression>	
	if (result = expression()) return result;

	return SYNTAX_OK;
}

/**
 * Implementation of <arg> rule.
 *
 * @return Given exit code.
 */
int arg()
{
	int result;

	// <arg> -> <value> <arg_n>
	if (result = value()) return result;
	if (result = arg_n()) return result;

	// <arg> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <arg_n> rule.
 *
 * @return Given exit code.
 */
int arg_n()
{
	int result;

	// <arg_n> -> , <value> <arg_n>
	if (token.type == TOKEN_TYPE_COMMA)
	{
		// get next token and execute <value> rule
		if (result = get_next_token(&token)) return result;
		if (result = value()) return result;

		// get next token and execute <arg_n> rule
		if (result = get_next_token(&token)) return result;
		if (result = arg_n()) return result;
	}

	// <arg_n> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <value> rule.
 *
 * @return Given exit code.
 */
int value()
{
	int result;

	// <value> -> double_value
	if (token.type == TOKEN_TYPE_DOUBLE_NUMBER)
	{
		// get next token
		if (result = get_next_token(&token)) return result;
	}

	// <value> -> int_value
	else if (token.type == TOKEN_TYPE_INT_NUMBER)
	{
		// get next token
		if (result = get_next_token(&token)) return result;
	}
	// <value> -> string
	else if (token.type == TOKEN_TYPE_STRING)
	{
		// get next token
		if (result = get_next_token(&token)) return result;
	}
	// <value> -> ID
	else if (token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// get next token
		if (result = get_next_token(&token)) return result;
	}

	else
	{
		return SYNTAX_ERR;
	}

	return SYNTAX_OK;
}

/**
 * Implementation of <print> rule.
 *
 * @return Given exit code.
 */
int print()
{
	int result;

	// <print> -> <expression> ; <print>
	if (result = expression()) return result;

	if (token.type == TOKEN_TYPE_SEMICOLON)
	{
		// get next token and execute <print> rule
		if (result = get_next_token(&token)) return result;
		if (result = print()) return result;
	}

	// <print> -> ε

	return SYNTAX_OK;
}

/**
* Initialize variable needed for analysis. 
*/
void init_variables()
{
	scope_processed = false;
	in_function = false;
}

int analyse() 
{
	int result;

	Dynamic_string string;

	if (!dynamic_string_init(&string))
	{
		return ERROR_OTHER;
	}

	set_dynamic_string(&string);
	init_variables();

	if (result = get_next_token(&token))
	{
		fprintf(stderr, "Scanner error!\n");
		dynamic_string_free(&string);
	}
	else
	{
		result = prog();
	}
	
	dynamic_string_free(&string);

	return result;
}


int expression()
{
	return SYNTAX_OK;
}
