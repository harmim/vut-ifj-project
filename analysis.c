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
#include "symtable.h"

typedef struct parser_internal_data
{
	Token token;				/// Token
	
	bool scope_processed;		/// Defines if the main scope has been processed
	bool in_function;			/// Defines if the parser is in function
	Sym_table global_table;		/// Global symbol table
	Sym_table local_table;		/// Local symbol table

	TData* current_id;			/// ID of currently processed function
	TData* lhs_id;				/// ID of left-hand-side variable
	bool add_params_flag;		/// Defines if param rule should add or check it's params
	int param_index;			/// Index of currently checked param
} PData;


// forward declarations
int params(PData* data);
int param(PData* data);
int param_n(PData* data);
int statement(PData* data);
int def_var(PData* data);
int def_value(PData* data);
int arg(PData* data);
int arg_n(PData* data);
int value(PData* data);
int print(PData* data);
int expression(PData* data);

/**
 * Implementation of <prog> rule.
 *
 * @return Given exit code.
 */
int prog(PData* data)
{
	int result;

	// <prog> -> SCOPE EOL <statement> END SCOPE EOL <prog>
	if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_SCOPE)
	{
		// we are in scope
		data->in_function = false;
		
		// program may contain only 1 scope
		if (data->scope_processed)
			return SEM_ERR_OTHER;
		else
			data->scope_processed = true;

		// get next token and check EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = statement(data)) return result;
		// clear local symbol table
		sym_table_free(data->local_table);

		// check for END token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for SCOPE token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_SCOPE) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <prog> rule
		if (result = get_next_token(&data->token)) return result;
		return prog(data);
	}

	// <prog> -> DECLARE FUNCTION ID <params> AS TYPE EOL <prog>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_DECLARE)
	{
		// get next token and check for FUNCTION token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;

		// get next token and check for ID token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// add id to the global symbol table
		bool internal_error; 
		data->current_id = sym_table_add_symbol(data->global_table, data->token.attribute.string->str, &internal_error);
		if (!data->current_id)
		{
			if (internal_error) return ERROR_OTHER;
			else return SEM_ERR_UNDEFINED_VAR;
		}
		
		// get next token and execute <params> rule
		data->add_params_flag = true;
		if (result = get_next_token(&data->token)) return result;
		if (result = params(data)) return result;

		// check for AS token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type == TOKEN_TYPE_KEYWORD)
		{
			switch (data->token.attribute.keyword)
			{
			case KEYWORD_INTEGER:
				data->current_id->type = TYPE_INT; 
				break;

			case KEYWORD_DOUBLE:
				data->current_id->type = TYPE_DOUBLE;
				break;

			case KEYWORD_STRING:
				data->current_id->type = TYPE_STRING;
				break;

			default:
				return SYNTAX_ERR;
			}
		}
		else return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;
		
		// get next token and execute <prog> rule
		if (result = get_next_token(&data->token)) return result;
		return prog(data);		
	}

	// <prog> -> EOF
	else if (data->token.type == TOKEN_TYPE_EOF)
	{
		// check if all functions are defined
		for (int i = 0; i < MAX_SYMTABLE_SIZE; i++)
			for (Sym_table_item* it = data->global_table[i]; it != NULL; it = it->next)
				if (!it->data.defined) return SEM_ERR_UNDEFINED_VAR;

		return SYNTAX_OK;
	}

	// <prog> -> FUNCTION ID <params> AS TYPE EOL <statement> END FUNCTION EOL <prog>
	else
	{
		// get next token and check for FUNCTION token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;

		// get next token and check for ID token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		data->current_id = sym_table_search(&data->global_table, data->token.attribute.string->str);
		if (!data->current_id)
			return SEM_ERR_UNDEFINED_VAR;

		// get next token and execute <params> rule
		data->add_params_flag = false;
		if (result = get_next_token(&data->token)) return result;
		if (result = params(data)) return result;

		// check for AS token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type == TOKEN_TYPE_KEYWORD)
		{
			switch (data->token.attribute.keyword)
			{
			case KEYWORD_INTEGER:
				if (data->current_id->type != TYPE_INT) return SEM_ERR_TYPE_COMPAT;
				break;

			case KEYWORD_DOUBLE:
				if (data->current_id->type != TYPE_DOUBLE) return SEM_ERR_TYPE_COMPAT;
				break;

			case KEYWORD_STRING:
				if (data->current_id->type != TYPE_STRING) return SEM_ERR_TYPE_COMPAT;
				break;

			default:
				return SYNTAX_ERR;
			}
		}

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = statement(data)) return result;
		// clear local symtable
		sym_table_free(&data->local_table);

		// check for END token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for FUNCTION token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_FUNCTION) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// Current function is defined
		data->current_id->defined = true;

		// get next token and execute <prog> rule
		if (result = get_next_token(&data->token)) return result;
		return prog(data);
	}

	return SYNTAX_ERR;
}

/**
 * Implementation of <params> rule.
 *
 * @return Given exit code.
 */
int params(PData* data)
{
	int result;

	// <params> -> ( <param> )
	if (data->token.type == TOKEN_TYPE_LEFT_BRACKET)
	{
		// There we will simulate derivation tree via iteration

		// get next token and execute <param> rule
		if (result = get_next_token(&data->token)) return result;	
		
		// <param> -> ID AS TYPE <param_n>
		if (data->token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for AS token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type == TOKEN_TYPE_KEYWORD)
		{
			// if we are in definition and in declaration there wasn't any arguments
			if (!data->add_params_flag && !data->current_id->params->length) return SEM_ERR_TYPE_COMPAT;

			switch (data->token.attribute.keyword)
			{
			case KEYWORD_INTEGER:
				if (data->add_params_flag)
				{
					if (!sym_table_add_param(data->current_id, 'i')) return ERROR_OTHER;
				}
				else if (data->current_id->params->str[0] != 'i') return SEM_ERR_TYPE_COMPAT;
				break;

			case KEYWORD_DOUBLE:
				if (data->add_params_flag)
				{
					if (!sym_table_add_param(data->current_id, 'd')) return ERROR_OTHER;
				}
				else if (data->current_id->params->str[0] != 'd') return SEM_ERR_TYPE_COMPAT;
				break;

			case KEYWORD_STRING:
				if (data->add_params_flag)
				{
					if (!sym_table_add_param(data->current_id, 's')) return ERROR_OTHER;
				}
				else if (data->current_id->params->str[0] != 's') return SEM_ERR_TYPE_COMPAT;
				break;

			default:
				return SYNTAX_ERR;
			}
		}
		else
			return SYNTAX_ERR;

		// get next token and execute <param_n> rule
		if (result = get_next_token(&data->token)) return result;

		// <param_n> -> , ID AS TYPE <param_n>
		int index = 1;
		while (data->token.type == TOKEN_TYPE_COMMA)
		{
			// get next token and check for ID token
			if (result = get_next_token(&data->token)) return result;
			if (data->token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

			// get next token and check for AS token
			if (result = get_next_token(&data->token)) return result;
			if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

			// get next token and check for TYPE token
			if (result = get_next_token(&data->token)) return result;
			if (data->token.type == TOKEN_TYPE_KEYWORD)
			{
				// if we are in definition and there is too many arguments
				if (!data->add_params_flag && data->current_id->params->length == index) return SEM_ERR_TYPE_COMPAT;

				switch (data->token.attribute.keyword)
				{
				case KEYWORD_INTEGER:
					if (data->add_params_flag)
					{
						if (!sym_table_add_param(data->current_id, 'i')) return ERROR_OTHER;
					}
					else if (data->current_id->params->str[index] != 'i') return SEM_ERR_TYPE_COMPAT;
					break;

				case KEYWORD_DOUBLE:
					if (data->add_params_flag)
					{
						if (!sym_table_add_param(data->current_id, 'd')) return ERROR_OTHER;
					}
					else if (data->current_id->params->str[index] != 'd') return SEM_ERR_TYPE_COMPAT;
					break;

				case KEYWORD_STRING:
					if (data->add_params_flag)
					{
						if (!sym_table_add_param(data->current_id, 's')) return ERROR_OTHER;
					}
					else if (data->current_id->params->str[index] != 's') return SEM_ERR_TYPE_COMPAT;
					break;

				default:
					return SYNTAX_ERR;
				}
			}
			else
				return SYNTAX_ERR;

			// get next token and execute <param_n> rule
			if (result = get_next_token(&data->token)) return result;
			index++;
		}

		// number of defined arguments is different from number of declared
		if (index != data->current_id->params->length) return SEM_ERR_TYPE_COMPAT;

		// check for ) token
		if (data->token.type != TOKEN_TYPE_RIGHT_BRACKET) return SYNTAX_ERR;

		// get next token
		if (result = get_next_token(&data->token)) return result;
	}
	else
		// parameter count missmatch
		if (data->current_id->params->length) return SEM_ERR_TYPE_COMPAT;

	// <params> -> ε
	
	return SYNTAX_OK;
}

/**
 * Implementation of <statement> rule.
 *
 * @return Given exit code.
 */
int statement(PData* data)
{
	int result;

	// <statement> -> DIM ID AS TYPE <def_var> EOL <statement>
	if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_DIM)
	{
		// get next token and check for ID token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// add id to the local symbol table
		bool internal_error;
		data->lhs_id = sym_table_add_symbol(&data->local_table, data->token.attribute.string->str, &internal_error);
		if (!data->lhs_id || sym_table_search(&data->global_table, data->token.attribute.string->str))
		{
			if (internal_error) return ERROR_OTHER;
			else return SEM_ERR_UNDEFINED_VAR;
		}

		// get next token and check for AS token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_AS) return SYNTAX_ERR;

		// get next token and check for TYPE token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type == TOKEN_TYPE_KEYWORD)
		{
			switch (data->token.attribute.keyword)
			{
			case KEYWORD_INTEGER:
				data->lhs_id->type = TYPE_INT;
				break;

			case KEYWORD_DOUBLE:
				data->lhs_id->type = TYPE_DOUBLE;
				break;

			case KEYWORD_STRING:
				data->lhs_id->type = TYPE_STRING;
				break;

			default:
				return SYNTAX_ERR;
			}
		}
		else
			return SYNTAX_ERR;
		
		// get next token and execute <def_var> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = def_var(data)) return result;

		// check for EOL token
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;
		
		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		return statement(data);
	}

	// <statement> -> IF <expression> THEN EOL <statement> ELSE EOL <statement> END IF EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_IF)
	{
		// get next token and execute <expression> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = expression(data)) return result;

		// check for THEN token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_THEN) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = statement(data)) return result;

		// check for ELSE token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_ELSE) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = statement(data)) return result;

		// check for END token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_END) return SYNTAX_ERR;

		// get next token and check for IF token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_IF) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		return statement(data);
	}

	// <statement> -> DO WHILE <expression> EOL <statement> LOOP EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_DO)
	{
		// get next token and check for WHILE token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_WHILE) return SYNTAX_ERR;

		// get next token and execute <expression> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = expression(data)) return result;

		// check for EOL token
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = statement(data)) return result;

		// check for LOOP token
		if (data->token.type != TOKEN_TYPE_KEYWORD || data->token.attribute.keyword != KEYWORD_LOOP) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		return statement(data);
	}

	// <statement> -> ID = <expression> EOL <statement>
	else if (data->token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// get next token and check for = token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_ASSIGN) return SYNTAX_ERR;

		// get next token and execute <expression> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = expression(data)) return result;

		// check for EOL token
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		return statement(data);
	}

	// <statement> -> INPUT ID EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_INPUT)
	{
		// get next token and check for ID token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_IDENTIFIER) return SYNTAX_ERR;

		// get next token and check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		return statement(data);
	}

	// <statement> -> PRINT <expression> ; <print> EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_PRINT)
	{
		// get next token and execute <expression> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = expression(data)) return result;

		// check for SEMICOLON token
		if (data->token.type != TOKEN_TYPE_SEMICOLON) return SYNTAX_ERR;

		// get next token and execute <print> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = print(data)) return result;

		// check for EOL token
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		return statement(data);
	}

	// <statement> -> RETURN <expression> EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_RETURN)
	{
		// scope doesn't have this type of rule
		if (!data->in_function) return SEM_ERR_OTHER;

		// get next token and execute <expression> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = expression(data)) return result;

		// check for EOL token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_EOL) return SYNTAX_ERR;

		// get next token and execute <statement> rule
		if (result = get_next_token(&data->token)) return result;
		return statement(data);
	}

	// <statement> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <def_var> rule.
 *
 * @return Given exit code.
 */
int def_var(PData* data)
{
	int result;

	// <def_var> -> = <def_val>
	if (data->token.type == TOKEN_TYPE_ASSIGN)
	{
		// get next token and execute <def_val> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = def_value(data)) return result;
	}

	// <def_var> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <def_value> rule.
 *
 * @return Given exit code.
 */
int def_value(PData* data)
{
	int result;

	// <def_value> -> ID( <arg> )
	if (data->token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// get next token and check for ( token
		if (result = get_next_token(&data->token)) return result;
		if (data->token.type != TOKEN_TYPE_LEFT_BRACKET) return SYNTAX_ERR;

		// get next token and execute <arg> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = arg(data)) return result;

		// check for ) token
		if (data->token.type != TOKEN_TYPE_RIGHT_BRACKET) return SYNTAX_ERR;

		// get next token
		if (result = get_next_token(&data->token)) return result;
	}

	// <def_value> -> <expression>	
	if (result = expression(data)) return result;

	return SYNTAX_OK;
}

/**
 * Implementation of <arg> rule.
 *
 * @return Given exit code.
 */
int arg(PData* data)
{
	int result;

	// <arg> -> <value> <arg_n>
	if (result = value(data)) return result;
	if (result = arg_n(data)) return result;

	// <arg> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <arg_n> rule.
 *
 * @return Given exit code.
 */
int arg_n(PData* data)
{
	int result;

	// <arg_n> -> , <value> <arg_n>
	if (data->token.type == TOKEN_TYPE_COMMA)
	{
		// get next token and execute <value> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = value(data)) return result;

		// get next token and execute <arg_n> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = arg_n(data)) return result;
	}

	// <arg_n> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <value> rule.
 *
 * @return Given exit code.
 */
int value(PData* data)
{
	int result;

	// <value> -> double_value
	if (data->token.type == TOKEN_TYPE_DOUBLE_NUMBER)
	{
		// get next token
		if (result = get_next_token(&data->token)) return result;
	}

	// <value> -> int_value
	else if (data->token.type == TOKEN_TYPE_INT_NUMBER)
	{
		// get next token
		if (result = get_next_token(&data->token)) return result;
	}
	// <value> -> string
	else if (data->token.type == TOKEN_TYPE_STRING)
	{
		// get next token
		if (result = get_next_token(&data->token)) return result;
	}
	// <value> -> ID
	else if (data->token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// get next token
		if (result = get_next_token(&data->token)) return result;
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
int print(PData* data)
{
	int result;

	// <print> -> <expression> ; <print>
	if (result = expression(data)) return result;

	if (data->token.type == TOKEN_TYPE_SEMICOLON)
	{
		// get next token and execute <print> rule
		if (result = get_next_token(&data->token)) return result;
		if (result = print(data)) return result;
	}

	// <print> -> ε

	return SYNTAX_OK;
}

int expression(PData* data)
{
	return SYNTAX_OK;
}

/**
* Initialize variable needed for analysis. 
*/
void init_variables(PData* data)
{
	data->scope_processed = false;
	data->in_function = false;
	
	sym_table_init(&data->global_table);
	sym_table_init(&data->local_table);

	data->current_id = NULL;
	data->add_params_flag = false;
}

void free_variables(PData* data)
{
	sym_table_free(&data->global_table);
	sym_table_free(&data->local_table);
}

int analyse() 
{
	int result;
	Dynamic_string string;
	PData parser_data;

	if (!dynamic_string_init(&string))
		return ERROR_OTHER;

	set_dynamic_string(&string);
	init_variables(&parser_data);

	if (result = get_next_token(&parser_data.token))
		fprintf(stderr, "Scanner error!\n");
	else
		result = prog(&parser_data);
	
	dynamic_string_free(&string);
	free_variables(&parser_data);

	return result;
}
