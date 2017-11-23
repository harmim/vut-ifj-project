/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Syntactical and semantical analysis imeplementation.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include "dynamic_string.h"
#include "error.h"
#include "analysis.h"
#include "code_generator.h"
#include "symtable.h"


#define IS_VALUE(token)												\
	(token).type == TOKEN_TYPE_DOUBLE_NUMBER						\
	|| (token).type == TOKEN_TYPE_INT_NUMBER						\
	|| (token).type == TOKEN_TYPE_STRING							\
	|| (token).type == TOKEN_TYPE_IDENTIFIER

#define GET_TOKEN()													\
	if ((result = get_next_token(&data->token)) != SCANNER_TOKEN_OK)\
		return result

#define CHECK_TYPE(_type)											\
	if (data->token.type != (_type)) return SYNTAX_ERR

#define CHECK_RULE(rule)											\
	if ((result = rule(data))) return result

#define CHECK_KEYWORD(_keyword)										\
		if (														\
			data->token.type != TOKEN_TYPE_KEYWORD					\
			|| data->token.attribute.keyword != (_keyword)			\
		) return SYNTAX_ERR

#define GET_TOKEN_AND_CHECK_TYPE(_type)								\
	do {															\
		GET_TOKEN();												\
		CHECK_TYPE(_type);											\
	} while(0)

#define GET_TOKEN_AND_CHECK_RULE(rule)								\
	do {															\
		GET_TOKEN();												\
		CHECK_RULE(rule);											\
	} while(0)

#define GET_TOKEN_AND_CHECK_KEYWORD(_keyword)						\
	do {															\
		GET_TOKEN();												\
		CHECK_KEYWORD(_keyword);									\
	} while(0)


// forward declarations
static int params(PData* data);
static int param_n(PData* data);
static int statement(PData* data);
static int def_var(PData* data);
static int def_value(PData* data);
static int arg(PData* data);
static int arg_n(PData* data);
static int value(PData* data);
static int print(PData* data);


/**
 * Implementation of <prog> rule.
 *
 * @return Given exit code.
 */
static int prog(PData* data)
{
	int result;

	// <prog> -> SCOPE EOL <statement> END SCOPE <prog>
	if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_SCOPE)
	{
		// we are in scope
		data->in_function = false;
		data->current_id = NULL;
		data->label_index = 0;

		// program may contain only 1 scope
		if (data->scope_processed)
			return SEM_ERR_OTHER;
		else
			data->scope_processed = true;

		GENERATE_CODE(generate_main_scope);

		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);
		GET_TOKEN_AND_CHECK_RULE(statement);
		CHECK_KEYWORD(KEYWORD_END);
		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_SCOPE);

		GENERATE_CODE(generate_end_of_main_scope);

		// clear local symbol table
		sym_table_free(&data->local_table);

		// get next token and execute <prog> rule
		GET_TOKEN();
		return prog(data);
	}

	// <prog> -> DECLARE FUNCTION ID ( <params> ) AS TYPE <prog>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_DECLARE)
	{
		data->in_declaration = true;
		data->non_declared_function = true;

		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_FUNCTION);
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_IDENTIFIER);
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_LEFT_BRACKET);
		
		// add id to the global symbol table
		bool internal_error; 
		data->current_id = sym_table_add_symbol(&data->global_table, data->token.attribute.string->str, &internal_error);
		if (!data->current_id)
		{
			if (internal_error) return ERROR_INTERNAL;
			else return SEM_ERR_UNDEFINED_VAR;
		}

		GET_TOKEN_AND_CHECK_RULE(params);
		CHECK_TYPE(TOKEN_TYPE_RIGHT_BRACKET);
		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_AS);

		// get next token and check for TYPE token
		GET_TOKEN();

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
		
		// get next token and execute <prog> rule
		GET_TOKEN();
		return prog(data);
	}

	// <prog> -> EOF
	else if (data->token.type == TOKEN_TYPE_EOF)
	{
		// check if all functions are defined
		for (int i = 0; i < MAX_SYMTABLE_SIZE; i++)
			for (Sym_table_item* it = data->global_table[i]; it != NULL; it = it->next)
				if (!it->data.defined) return SEM_ERR_UNDEFINED_VAR;

		if (!data->scope_processed) return SEM_ERR_OTHER;

		return SYNTAX_OK;
	}

	// <prog> -> EOL <prog>
	else if (data->token.type == TOKEN_TYPE_EOL)
	{
		GET_TOKEN();
		return prog(data);
	}

	// <prog> -> FUNCTION ID ( <params> ) AS TYPE EOL <statement> END FUNCTION <prog>
	else
	{
		data->in_function = true;
		data->in_declaration = false;
		data->label_index = 0;

		CHECK_KEYWORD(KEYWORD_FUNCTION);
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_IDENTIFIER);

		// if function wasn't declared add it to the global symbol table
		bool internal_error;
		data->current_id = sym_table_add_symbol(&data->global_table, data->token.attribute.string->str, &internal_error);
		if (data->current_id) 
			data->non_declared_function = true;
		else
		{
			if (internal_error) return ERROR_INTERNAL;
			data->non_declared_function = false;
			data->current_id = sym_table_search(&data->global_table, data->token.attribute.string->str);
		}

		GENERATE_CODE(generate_function_head, data->current_id->identifier);

		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_LEFT_BRACKET);
		GET_TOKEN_AND_CHECK_RULE(params);
		CHECK_TYPE(TOKEN_TYPE_RIGHT_BRACKET);
		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_AS);

		// get next token and check for TYPE token
		GET_TOKEN();
		if (data->token.type == TOKEN_TYPE_KEYWORD)
		{
			switch (data->token.attribute.keyword)
			{
			case KEYWORD_INTEGER:
				if (!data->non_declared_function && data->current_id->type != TYPE_INT) 
					return SEM_ERR_UNDEFINED_VAR;

				data->current_id->type = TYPE_INT;
				break;

			case KEYWORD_DOUBLE:
				if (!data->non_declared_function && data->current_id->type != TYPE_DOUBLE) 
					return SEM_ERR_UNDEFINED_VAR;

				data->current_id->type = TYPE_DOUBLE;
				break;

			case KEYWORD_STRING:
				if (!data->non_declared_function && data->current_id->type != TYPE_STRING) 
					return SEM_ERR_UNDEFINED_VAR;

				data->current_id->type = TYPE_STRING;
				break;

			default:
				return SYNTAX_ERR;
			}
		}
		else
			return SYNTAX_ERR;

		GENERATE_CODE(generate_function_retval, data->current_id->type);

		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);
		GET_TOKEN_AND_CHECK_RULE(statement);
		CHECK_KEYWORD(KEYWORD_END);
		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_FUNCTION);

		GENERATE_CODE(generate_end_of_function, data->current_id->identifier);

		// Current function is defined
		data->current_id->defined = true;

		// clear local symtable
		sym_table_free(&data->local_table);

		// get next token and execute <prog> rule
		GET_TOKEN();
		return prog(data);
	}

	return SYNTAX_ERR;
}


/**
 * Implementation of <type> rule.
 *
 * @return Given exit code.
 */
static int type(PData* data)
{
	if (data->token.type == TOKEN_TYPE_KEYWORD)
	{
		switch (data->token.attribute.keyword)
		{
		case KEYWORD_INTEGER:
			if (data->non_declared_function)
			{
				if (!sym_table_add_param(data->current_id, TYPE_INT)) return ERROR_INTERNAL;
			}
			else if (data->current_id->params->str[data->param_index] != 'i') return SEM_ERR_UNDEFINED_VAR;

			if (!data->in_declaration) data->rhs_id->type = TYPE_INT;
			break;

		case KEYWORD_DOUBLE:
			if (data->non_declared_function)
			{
				if (!sym_table_add_param(data->current_id, TYPE_DOUBLE)) return ERROR_INTERNAL;
			}
			else if (data->current_id->params->str[data->param_index] != 'd') return SEM_ERR_UNDEFINED_VAR;

			if (!data->in_declaration) data->rhs_id->type = TYPE_DOUBLE;
			break;

		case KEYWORD_STRING:
			if (data->non_declared_function)
			{
				if (!sym_table_add_param(data->current_id, TYPE_STRING)) return ERROR_INTERNAL;
			}
			else if (data->current_id->params->str[data->param_index] != 's') return SEM_ERR_UNDEFINED_VAR;

			if (!data->in_declaration) data->rhs_id->type = TYPE_STRING;
			break;

		default:
			return SYNTAX_ERR;
		}
	}
	else
		return SYNTAX_ERR;

	return SYNTAX_OK;
}

/**
 * Implementation of <params> rule.
 *
 * @return Given exit code.
 */
static int params(PData* data)
{
	int result;
	data->param_index = 0;

	// <params> -> ID AS <type> <param_n>
	if (data->token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// if there is function named as parameter
		if (sym_table_search(&data->global_table, data->token.attribute.string->str))
			return SEM_ERR_UNDEFINED_VAR;

		// if we are in defintion, we need to add parameters to the local symbol table
		if (!data->in_declaration)
		{
			bool internal_error;
			if (!(data->rhs_id = sym_table_add_symbol(&data->local_table, data->token.attribute.string->str, &internal_error)))
			{
				if (internal_error) return ERROR_INTERNAL;
				else return SEM_ERR_UNDEFINED_VAR;
			}

			GENERATE_CODE(generate_declare_function_param, data->rhs_id->identifier, data->param_index);
		}

		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_AS);
		GET_TOKEN_AND_CHECK_RULE(type);
		GET_TOKEN_AND_CHECK_RULE(param_n);

		if (data->param_index + 1 != data->current_id->params->length) 
			return SEM_ERR_UNDEFINED_VAR;
	} 
	else if (!data->in_declaration && data->current_id->params->length)
		return SEM_ERR_UNDEFINED_VAR;

	// <params> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <param_n> rule.
 *
 * @return Given exit code.
 */
static int param_n(PData* data)
{
	int result;

	// <param_n> -> , ID AS <type> <param_n>
	if (data->token.type == TOKEN_TYPE_COMMA)
	{
		data->param_index++;

		if (!data->in_declaration && !data->non_declared_function && data->param_index == data->current_id->params->length)
			return SEM_ERR_UNDEFINED_VAR;

		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_IDENTIFIER);

		// if we are in defintion, we need to add ID to the local symbol table
		if (!data->in_declaration)
		{
			bool internal_error;
			if (!(data->rhs_id = sym_table_add_symbol(&data->local_table, data->token.attribute.string->str, &internal_error)))
			{
				if (internal_error) return ERROR_INTERNAL;
				else return SEM_ERR_UNDEFINED_VAR;
			}

			GENERATE_CODE(generate_declare_function_param, data->rhs_id->identifier, data->param_index);
		}

		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_AS);
		GET_TOKEN_AND_CHECK_RULE(type);

		// get next token and execute <param_n> rule
		GET_TOKEN();
		return param_n(data);
	}

	// <param_n> -> ε
	
	return SYNTAX_OK;
}

/**
 * Implementation of <statement> rule.
 *
 * @return Given exit code.
 */
static int statement(PData* data)
{
	int result;

	// <statement> -> DIM ID AS TYPE <def_var> EOL <statement>
	if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_DIM)
	{
		if (data->in_while_or_if) return SYNTAX_ERR;

		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_IDENTIFIER);

		// add id to the local symbol table
		bool internal_error;
		data->lhs_id = sym_table_add_symbol(&data->local_table, data->token.attribute.string->str, &internal_error);
		if (!data->lhs_id || sym_table_search(&data->global_table, data->token.attribute.string->str))
		{
			if (internal_error) return ERROR_INTERNAL;
			else return SEM_ERR_UNDEFINED_VAR;
		}

		GENERATE_CODE(generate_declare_var, data->lhs_id->identifier);

		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_AS);

		// get next token and check for TYPE token
		GET_TOKEN();
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

		GET_TOKEN_AND_CHECK_RULE(def_var);
		CHECK_TYPE(TOKEN_TYPE_EOL);

		// get next token and execute <statement> rule
		GET_TOKEN();
		return statement(data);
	}

	// <statement> -> IF <expression> THEN EOL <statement> ELSE EOL <statement> END IF EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_IF)
	{
		data->label_deep++;
		data->in_while_or_if = true;

		data->lhs_id = sym_table_search(&data->global_table, "%exp_result");
		if (!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
		data->lhs_id->type = TYPE_BOOL;

		char *function_id = data->current_id ? data->current_id->identifier : "";
		int current_label_index = data->label_index;
		data->label_index += 2;

		GENERATE_CODE(generate_if_head);

		GET_TOKEN_AND_CHECK_RULE(expression);
		CHECK_KEYWORD(KEYWORD_THEN);
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);

		GENERATE_CODE(generate_if_start, function_id, current_label_index, data->label_deep);

		GET_TOKEN_AND_CHECK_RULE(statement);
		CHECK_KEYWORD(KEYWORD_ELSE);
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);

		GENERATE_CODE(generate_if_else_part, function_id, current_label_index, data->label_deep);

		GET_TOKEN_AND_CHECK_RULE(statement);

		CHECK_KEYWORD(KEYWORD_END);
		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_IF);
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);

		GENERATE_CODE(generate_if_end, function_id, current_label_index + 1, data->label_deep);

		data->label_deep--;
		data->in_while_or_if = false;

		// get next token and execute <statement> rule
		GET_TOKEN();
		return statement(data);
	}

	// <statement> -> DO WHILE <expression> EOL <statement> LOOP EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_DO)
	{
		data->label_deep++;
		data->in_while_or_if = true;

		GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_WHILE);

		data->lhs_id = sym_table_search(&data->global_table, "%exp_result");
		if (!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
		data->lhs_id->type = TYPE_BOOL;

		char *function_id = data->current_id ? data->current_id->identifier : "";
		int current_label_index = data->label_index;
		data->label_index += 2;

		GENERATE_CODE(generate_while_head, function_id, current_label_index, data->label_deep);

		GET_TOKEN_AND_CHECK_RULE(expression);
		CHECK_TYPE(TOKEN_TYPE_EOL);

		GENERATE_CODE(generate_while_start, function_id, current_label_index + 1, data->label_deep);

		GET_TOKEN_AND_CHECK_RULE(statement);

		CHECK_KEYWORD(KEYWORD_LOOP);
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);

		GENERATE_CODE(generate_while_end, function_id, current_label_index + 1, data->label_deep);

		data->label_deep--;
		data->in_while_or_if = false;

		// get next token and execute <statement> rule
		GET_TOKEN();
		return statement(data);
	}

	// <statement> -> ID = <def_value> EOL <statement>
	else if (data->token.type == TOKEN_TYPE_IDENTIFIER)
	{
		// check for existence of variable
		data->lhs_id = sym_table_search(&data->local_table, data->token.attribute.string->str);
		if (!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
		
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_ASSIGN);
		GET_TOKEN_AND_CHECK_RULE(def_value);
		CHECK_TYPE(TOKEN_TYPE_EOL);

		// get next token and execute <statement> rule
		GET_TOKEN();
		return statement(data);
	}

	// <statement> -> INPUT ID EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_INPUT)
	{
		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_IDENTIFIER);

		data->lhs_id = sym_table_search(&data->local_table, data->token.attribute.string->str);
		if (!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;

		if (data->lhs_id->type != TYPE_INT && data->lhs_id->type != TYPE_DOUBLE && data->lhs_id->type != TYPE_STRING)
		{
			return SEM_ERR_OTHER;
		}

		GENERATE_CODE(generate_input, data->lhs_id->identifier, data->lhs_id->type);

		GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_EOL);

		// get next token and execute <statement> rule
		GET_TOKEN();
		return statement(data);
	}

	// <statement> -> PRINT <expression> ; <print> EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_PRINT)
	{
		data->lhs_id = sym_table_search(&data->global_table, "%exp_result");
		if (!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
		data->lhs_id->type = TYPE_UNDEFINED;

		GET_TOKEN_AND_CHECK_RULE(expression);
		CHECK_TYPE(TOKEN_TYPE_SEMICOLON);

		GENERATE_CODE(generate_print);

		GET_TOKEN_AND_CHECK_RULE(print);
		CHECK_TYPE(TOKEN_TYPE_EOL);

		// get next token and execute <statement> rule
		GET_TOKEN();
		return statement(data);
	}

	// <statement> -> RETURN <expression> EOL <statement>
	else if (data->token.type == TOKEN_TYPE_KEYWORD && data->token.attribute.keyword == KEYWORD_RETURN)
	{
		// scope doesn't have this type of rule
		if (!data->in_function) return SYNTAX_ERR;

		data->lhs_id = sym_table_search(&data->global_table, "%exp_result");
		if (!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
		data->lhs_id->type = data->current_id->type;

		GET_TOKEN_AND_CHECK_RULE(expression);

		GENERATE_CODE(generate_function_return, data->current_id->identifier);

		CHECK_TYPE(TOKEN_TYPE_EOL);

		// get next token and execute <statement> rule
		GET_TOKEN();
		return statement(data);
	}

	// <statement> -> ε
	else if (data->token.type == TOKEN_TYPE_EOL)
	{
		GET_TOKEN();
		return statement(data);
	}

	return SYNTAX_OK;
}

/**
 * Implementation of <def_var> rule.
 *
 * @return Given exit code.
 */
static int def_var(PData* data)
{
	int result;

	// <def_var> -> = <def_val>
	if (data->token.type == TOKEN_TYPE_ASSIGN)
	{
		GET_TOKEN_AND_CHECK_RULE(def_value);
	}

	// <def_var> -> ε
	else
	{
		GENERATE_CODE(generate_var_default_value, data->lhs_id->identifier, data->lhs_id->type);
	}

	return SYNTAX_OK;
}

/**
 * Implementation of <def_value> rule.
 *
 * @return Given exit code.
 */
static int def_value(PData* data)
{
	int result;

	// <def_value> -> ID ( <arg> )
	if (data->token.type == TOKEN_TYPE_IDENTIFIER)
	{
		data->rhs_id = sym_table_search(&data->global_table, data->token.attribute.string->str);

		if (data->rhs_id)
		{
			// check type compatibilty
			// if either one expression is string, we cannot implicitly convert
			if (data->lhs_id->type != data->rhs_id->type)
				if (data->lhs_id->type == TYPE_STRING || data->rhs_id->type == TYPE_STRING)
					return SEM_ERR_TYPE_COMPAT;

			GENERATE_CODE(generate_before_pass_params_to_function);

			GET_TOKEN_AND_CHECK_TYPE(TOKEN_TYPE_LEFT_BRACKET);
			GET_TOKEN_AND_CHECK_RULE(arg);
			CHECK_TYPE(TOKEN_TYPE_RIGHT_BRACKET);
			GET_TOKEN();

			if (data->rhs_id->params->length != data->param_index) return SEM_ERR_TYPE_COMPAT;

			GENERATE_CODE(generate_call_function, data->rhs_id->identifier);
			GENERATE_CODE(generate_function_retval_assign, data->lhs_id->identifier, data->lhs_id->type, data->rhs_id->type);

			return SYNTAX_OK;
		}
	}

	// <def_value> -> <expression>
	CHECK_RULE(expression);

	return SYNTAX_OK;
}

/**
 * Implementation of <arg> rule.
 *
 * @return Given exit code.
 */
static int arg(PData* data)
{
	int result;

	// currently processed argument
	data->param_index = 0;

	// <arg> -> <value> <arg_n>
	// if token is value
	if (IS_VALUE(data->token))
	{
		CHECK_RULE(value);
		GET_TOKEN_AND_CHECK_RULE(arg_n);
	}

	// <arg> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <arg_n> rule.
 *
 * @return Given exit code.
 */
static int arg_n(PData* data)
{
	int result;

	// <arg_n> -> , <value> <arg_n>
	if (data->token.type == TOKEN_TYPE_COMMA)
	{
		GET_TOKEN_AND_CHECK_RULE(value);
		GET_TOKEN_AND_CHECK_RULE(arg_n);
	}

	// <arg_n> -> ε

	return SYNTAX_OK;
}

/**
 * Implementation of <value> rule.
 *
 * @return Given exit code.
 */
static int value(PData* data)
{
	// check number of arguments
	if (data->rhs_id->params->length == data->param_index)
		return SEM_ERR_TYPE_COMPAT;

	switch (data->token.type)
	{
	// <value> -> DOUBLE_NUMBER
	case TOKEN_TYPE_DOUBLE_NUMBER:
		if (data->rhs_id->params->str[data->param_index] == 's')
			return SEM_ERR_TYPE_COMPAT;

		break;

	// <value> -> INT_NUMBER
	case TOKEN_TYPE_INT_NUMBER:
		if (data->rhs_id->params->str[data->param_index] == 's')
			return SEM_ERR_TYPE_COMPAT;

		break;

	// <value> -> STRING
	case TOKEN_TYPE_STRING:
		if (data->rhs_id->params->str[data->param_index] != 's')
			return SEM_ERR_TYPE_COMPAT;

		break;

	// <value> -> IDENTIFIER
	case TOKEN_TYPE_IDENTIFIER:;	// ;	C evil magic
		TData* id = sym_table_search(&data->local_table, data->token.attribute.string->str);
		if (!id) return SEM_ERR_UNDEFINED_VAR;

		switch (id->type)
		{
		case TYPE_INT:
			if (data->rhs_id->params->str[data->param_index] == 's')
				return SEM_ERR_TYPE_COMPAT;

			break;

		case TYPE_DOUBLE:
			if (data->rhs_id->params->str[data->param_index] == 's')
				return SEM_ERR_TYPE_COMPAT;

			break;

		case TYPE_STRING:
			if (data->rhs_id->params->str[data->param_index] != 's')
				return SEM_ERR_TYPE_COMPAT;

			break;

		default: // shouldn't get here
			return ERROR_INTERNAL;
		}

		break;

	default:
		return SYNTAX_ERR;
	}

	GENERATE_CODE(generate_pass_param_to_function, data->token, data->param_index);

	// increment argument position
	data->param_index++;

	return SYNTAX_OK;
}

/**
 * Implementation of <print> rule.
 *
 * @return Given exit code.
 */
static int print(PData* data)
{
	int result;

	// <print> -> ε
	if (data->token.type == TOKEN_TYPE_EOL)
	{
		return SYNTAX_OK;
	}

	data->lhs_id = sym_table_search(&data->global_table, "%exp_result");
	if (!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
	data->lhs_id->type = TYPE_UNDEFINED;

	// <print> -> <expression> ; <print>
	CHECK_RULE(expression);
	CHECK_TYPE(TOKEN_TYPE_SEMICOLON);

	GENERATE_CODE(generate_print);

	GET_TOKEN_AND_CHECK_RULE(print);

	return SYNTAX_OK;
}


/**
 * Initialize variable needed for analysis. 
 *
 * @return True if inicialization was successful, false otherwise.
 */
static bool init_variables(PData* data)
{
	sym_table_init(&data->global_table);
	sym_table_init(&data->local_table);

	data->current_id = NULL;
	data->lhs_id = NULL;
	data->rhs_id = NULL;

	data->param_index = 0;
	data->label_index = 0;
	data->label_deep = -1;

	data->scope_processed = false;
	data->in_function = false;
	data->in_declaration = false;
	data->in_while_or_if = false;
	data->non_declared_function = false;

	// init default functions

	bool internal_error;
	TData* id;

	// Length(s As String) As Integer
	id = sym_table_add_symbol(&data->global_table, "length", &internal_error);
	if (internal_error) return false;

	id->defined = true;
	id->type = TYPE_INT;
	if (!sym_table_add_param(id, TYPE_STRING)) return false;

	// SubStr(s As String, i As Integer, n As Integer) As String
	id = sym_table_add_symbol(&data->global_table, "substr", &internal_error);
	if (internal_error) return false;

	id->defined = true;
	id->type = TYPE_STRING;
	if (!sym_table_add_param(id, TYPE_STRING)) return false;
	if (!sym_table_add_param(id, TYPE_INT)) return false;
	if (!sym_table_add_param(id, TYPE_INT)) return false;

	// Asc(s As String, i As Integer) As Integer
	id = sym_table_add_symbol(&data->global_table, "asc", &internal_error);
	if (internal_error) return false;

	id->defined = true;
	id->type = TYPE_INT;
	if (!sym_table_add_param(id, TYPE_STRING)) return false;
	if (!sym_table_add_param(id, TYPE_INT)) return false;

	// Chr(i As Integer) As String
	id = sym_table_add_symbol(&data->global_table, "chr", &internal_error);
	if (internal_error) return false;

	id->defined = true;
	id->type = TYPE_STRING;
	if (!sym_table_add_param(id, TYPE_INT)) return false;

	// Global variable %exp_result for storing result of expression.
	id = sym_table_add_symbol(&data->global_table, "%exp_result", &internal_error);
	if (internal_error) return false;
	id->defined = true;
	id->type = TYPE_UNDEFINED;
	id->global = true;

	return true;
}


/**
 * Frees symbol tables
 */
static void free_variables(PData* data)
{
	sym_table_free(&data->global_table);
	sym_table_free(&data->local_table);
}


int analyse()
{
	int result;

	Dynamic_string string;
	if (!dynamic_string_init(&string)) return ERROR_INTERNAL;
	set_dynamic_string(&string);

	PData parser_data;
	if (!init_variables(&parser_data))
	{
		dynamic_string_free(&string);
		return ERROR_INTERNAL;
	}

	if ((result = get_next_token(&parser_data.token)) == SCANNER_TOKEN_OK)
	{
		if (!code_generator_start())
		{
			dynamic_string_free(&string);
			free_variables(&parser_data);
			return ERROR_INTERNAL;
		}

		result = prog(&parser_data);
	}

	dynamic_string_free(&string);
	free_variables(&parser_data);

	return result;
}
