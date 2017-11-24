/**
* Project: Implementace překladače imperativního jazyka IFJ17.
*
* @brief Implementation of parsing of expressions.
*
* @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
* @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
*/


#include "scanner.h"
#include "expression.h"
#include "symstack.h"
#include "error.h"
#include "code_generator.h"


#define TABLE_SIZE 8

#define FREE_RESOURCES_RETURN(_return)								\
	do {															\
		symbol_stack_free(&stack);									\
		return _return;												\
	} while(0)


Symbol_stack stack;

/**
 * @enum Precedence table symbols.
 */
typedef enum
{
	S,    /// < SHIFT
	E,    /// = EQUAL
	R,    /// > REDUCE
	N     /// # ERROR
} Prec_table_sign_enum;

/**
 * @enum Indexes for precedence table.
 */
typedef enum
{
	I_PLUS_MINUS,		/// 0 +-
	I_MUL_DIV,			/// 1 */
	I_IDIV,				/// 2 \ /
	I_REL_OP,			/// 3 r
	I_LEFT_BRACKET,		/// 4 (
	I_RIGHT_BRACKET,	/// 5 )
	I_DATA,				/// 6 i
	I_DOLLAR			/// 7 $
} Prec_table_index_enum;

// Precedence table
int prec_table[TABLE_SIZE][TABLE_SIZE] =
{
//	|+- | */| \ | r | ( | ) | i | $ |
	{ R , S , S , R , S , R , S , R }, /// +-
	{ R , R , R , R , S , R , S , R }, /// */
	{ R , S , R , R , S , R , S , R }, /// \ /
	{ S , S , S , N , S , R , S , R }, /// r (realtion operators) = <> <= < >= >
	{ S , S , S , S , S , E , S , N }, /// (
	{ R , R , R , R , N , R , N , R }, /// )
	{ R , R , R , R , N , R , N , R }, /// i (id, int, double, string)
	{ S , S , S , S , S , N , S , N }  /// $
};


/**
 * Function converts symbol to precedence table index.
 *
 * @param symbol Symbol to be converted.
 * @return Returns precedence table index.
 */
static Prec_table_index_enum get_prec_table_index(Prec_table_symbol_enum symbol)
{
	switch (symbol)
	{
	case PLUS:
	case MINUS:
		return I_PLUS_MINUS;

	case MUL:
	case DIV:
		return I_MUL_DIV;

	case IDIV:
		return I_IDIV;

	case EQ:
	case NEQ:
	case LEQ:
	case LTN:
	case MEQ:
	case MTN:
		return I_REL_OP;

	case LEFT_BRACKET:
		return I_LEFT_BRACKET;

	case RIGHT_BRACKET:
		return I_RIGHT_BRACKET;

	case IDENTIFIER:
	case INT_NUMBER:
	case DOUBLE_NUMBER:
	case STRING:
		return I_DATA;

	default:
		return I_DOLLAR;
	}
}


/**
 * Function converts token type to symbol.
 *
 * @param token Pointer to token.
 * @return Returns dollar if symbol is not supported or converted symbol if symbol is supported.
 */
static Prec_table_symbol_enum get_symbol_from_token(Token* token)
{
	switch (token->type)
	{
	case TOKEN_TYPE_PLUS:
		return PLUS;
	case TOKEN_TYPE_MINUS:
		return MINUS;
	case TOKEN_TYPE_MUL:
		return MUL;
	case TOKEN_TYPE_DIV:
		return DIV;
	case TOKEN_TYPE_IDIV:
		return IDIV;
	case TOKEN_TYPE_ASSIGN:
		return EQ;
	case TOKEN_TYPE_NEQ:
		return NEQ;
	case TOKEN_TYPE_LEQ:
		return LEQ;
	case TOKEN_TYPE_LTN:
		return LTN;
	case TOKEN_TYPE_MEQ:
		return MEQ;
	case TOKEN_TYPE_MTN:
		return MTN;
	case TOKEN_TYPE_LEFT_BRACKET:
		return LEFT_BRACKET;
	case TOKEN_TYPE_RIGHT_BRACKET:
		return RIGHT_BRACKET;
	case TOKEN_TYPE_IDENTIFIER:
		return IDENTIFIER;
	case TOKEN_TYPE_INT_NUMBER:
		return INT_NUMBER;
	case TOKEN_TYPE_DOUBLE_NUMBER:
		return DOUBLE_NUMBER;
	case TOKEN_TYPE_STRING:
		return STRING;
	default:
		return DOLLAR;
	}
}


/**
 * Function converts token type to data type.
 *
 * @param token Pointer to token.
 * @param data Pointer to parser's internal data.
 * @return Returns data type of actual token.
 */
static Data_type get_data_type(Token* token, PData* data)
{
	TData* symbol;

	switch (token->type)
	{
	case TOKEN_TYPE_IDENTIFIER:
		symbol = sym_table_search(&data->local_table, token->attribute.string->str);
		if (symbol == NULL)
			return TYPE_UNDEFINED;
		return symbol->type;

	case TOKEN_TYPE_INT_NUMBER:
		return TYPE_INT;

	case TOKEN_TYPE_DOUBLE_NUMBER:
		return TYPE_DOUBLE;

	case TOKEN_TYPE_STRING:
		return TYPE_STRING;
	default:
		return TYPE_UNDEFINED;
	}
}


/**
 * Function function counts number of symbols after stop symbol on stack.
 *
 * @param stop_found Pointer to bool variable which will be changed to true if stop was found else to false.
 * @return Number of charatcters after stop symbol. Is valid only when stop_found was set to true.
 */
static int num_of_symbols_after_stop(bool* stop_found)
{
	Symbol_stack_item* tmp = symbol_stack_top(&stack);
	int count = 0;

	while (tmp != NULL)
	{
		if (tmp->symbol != STOP)
		{
			*stop_found = false;
			count++;
		}
		else
		{
			*stop_found = true;
			break;
		}

		tmp = tmp->next;
	}

	return count;
}


/**
 * Function tests if symbols in parameters are valid according to rules.
 *
 * @param num Number of valid symbols in parameter.
 * @param op1 Symbol 1.
 * @param op2 Symbol 2.
 * @param op3 Symbol 3.
 * @return NOT_A_RULE if no rule is found or returns rule which is valid.
 */
static Prec_rules_enum test_rule(int num, Symbol_stack_item* op1, Symbol_stack_item* op2, Symbol_stack_item* op3)
{
	switch (num)
	{
	case 1:
		// rule E -> i
		if (op1->symbol == IDENTIFIER || op1->symbol == INT_NUMBER || op1->symbol == DOUBLE_NUMBER || op1->symbol == STRING)
			return OPERAND;

		return NOT_A_RULE;

	case 3:
		// rule E -> (E)
		if (op1->symbol == LEFT_BRACKET && op2->symbol == NON_TERM && op3->symbol == RIGHT_BRACKET)
			return LBR_NT_RBR;

		if (op1->symbol == NON_TERM && op3->symbol == NON_TERM)
		{
			switch (op2->symbol)
			{
			// rule E -> E + E
			case PLUS:
				return NT_PLUS_NT;

			// rule E -> E - E
			case MINUS:
				return NT_MINUS_NT;

			// rule E -> E * E
			case MUL:
				return NT_MUL_NT;

			// rule E -> E / E
			case DIV:
				return NT_DIV_NT;

			// rule E -> E \ E
			case IDIV:
				return NT_IDIV_NT;

			// rule E -> E = E
			case EQ:
				return NT_EQ_NT;

			// rule E -> E <> E
			case NEQ:
				return NT_NEQ_NT;

			// rule E -> E <= E
			case LEQ:
				return NT_LEQ_NT;

			// rule E -> E < E
			case LTN:
				return NT_LTN_NT;

			// rule E -> E >= E
			case MEQ:
				return NT_MEQ_NT;

			// rule E -> E > E
			case MTN:
				return NT_MTN_NT;

			// invalid operator
			default:
				return NOT_A_RULE;
			}
		}
		return NOT_A_RULE;
	}
	return NOT_A_RULE;
}


/**
 * Function checks semantics of operands according to rule.
 *
 * @param rule Pointer to table.
 * @param op1 Symbol 1.
 * @param op2 Symbol 2.
 * @param op3 Symbol 3.
 * @param final_type Sets data type which will be after executing rule.
 * @return Given exit code.
 */
static int check_semantics(Prec_rules_enum rule, Symbol_stack_item* op1, Symbol_stack_item* op2, Symbol_stack_item* op3, Data_type* final_type)
{
	bool retype_op1 = false;
	bool retype_op3 = false;

	if (rule == OPERAND)
	{
		if (op1->data_type == TYPE_UNDEFINED)
			return SEM_ERR_UNDEFINED_VAR;

		if (op1->data_type == TYPE_BOOL)
			return SEM_ERR_TYPE_COMPAT;
	}

	if (rule == LBR_NT_RBR)
	{
		if (op2->data_type == TYPE_UNDEFINED)
			return SEM_ERR_UNDEFINED_VAR;
	}

	if (rule != OPERAND && rule != LBR_NT_RBR)
	{
		if (op1->data_type == TYPE_UNDEFINED || op3->data_type == TYPE_UNDEFINED)
			return SEM_ERR_UNDEFINED_VAR;

		if (op1->data_type == TYPE_BOOL || op3->data_type == TYPE_BOOL)
			return SEM_ERR_TYPE_COMPAT;
	}

	switch (rule)
	{
	case OPERAND:
		*final_type = op1->data_type;
		break;

	case LBR_NT_RBR:
		*final_type = op2->data_type;
		break;

	case NT_PLUS_NT:
	case NT_MINUS_NT:
	case NT_MUL_NT:
		if (op1->data_type == TYPE_STRING && op3->data_type == TYPE_STRING && rule == NT_PLUS_NT)
		{
			*final_type = TYPE_STRING;
			break;
		}

		if (op1->data_type == TYPE_INT && op3->data_type == TYPE_INT)
		{
			*final_type = TYPE_INT;
			break;
		}

		if (op1->data_type == TYPE_STRING || op3->data_type == TYPE_STRING)
			return SEM_ERR_TYPE_COMPAT;

		*final_type = TYPE_DOUBLE;

		if (op1->data_type == TYPE_INT)
			retype_op1 = true;

		if (op3->data_type == TYPE_INT)
			retype_op3 = true;

		break;

	case NT_DIV_NT:
		*final_type = TYPE_DOUBLE;

		if (op1->data_type == TYPE_STRING || op3->data_type == TYPE_STRING)
			return SEM_ERR_TYPE_COMPAT;

		if (op1->data_type == TYPE_INT)
			retype_op1 = true;

		if (op3->data_type == TYPE_INT)
			retype_op3 = true;

		break;

	case NT_IDIV_NT:
		*final_type = TYPE_INT;

		if (op1->data_type != TYPE_INT || op3->data_type != TYPE_INT)
			return SEM_ERR_TYPE_COMPAT;

		break;

	case NT_EQ_NT:
	case NT_NEQ_NT:
	case NT_LEQ_NT:
	case NT_LTN_NT:
	case NT_MEQ_NT:
	case NT_MTN_NT:
		*final_type = TYPE_BOOL;

		if (op1->data_type == TYPE_INT && op3->data_type == TYPE_DOUBLE)
			retype_op1 = true;

		else if (op1->data_type == TYPE_DOUBLE && op3->data_type == TYPE_INT)
			retype_op3 = true;

		else if (op1->data_type != op3->data_type)
			return SEM_ERR_TYPE_COMPAT;

		break;

	default:
		break;
	}

	if (retype_op1)
	{
		GENERATE_CODE(generate_stack_op2_to_double);
	}

	if (retype_op3)
	{
		GENERATE_CODE(generate_stack_op1_to_double);
	}

	return SYNTAX_OK;
}


/**
 * Function reduces symbols after STOP symbol if rule for reducing is found.
 *
 * @param data Pointer to table.
 * @return Given exit code.
 */
static int reduce_by_rule(PData* data)
{
	(void) data;

	int result;

	Symbol_stack_item* op1 = NULL;
	Symbol_stack_item* op2 = NULL;
	Symbol_stack_item* op3 = NULL;
	Data_type final_type;
	Prec_rules_enum rule_for_code_gen;
	bool found = false;

	int count = num_of_symbols_after_stop(&found);


	if (count == 1 && found)
	{
		op1 = stack.top;
		rule_for_code_gen = test_rule(count, op1, NULL, NULL);
	}
	else if (count == 3 && found)
	{
		op1 = stack.top->next->next;
		op2 = stack.top->next;
		op3 = stack.top;
		rule_for_code_gen = test_rule(count, op1, op2, op3);
	}
	else
		return SYNTAX_ERR;

	if (rule_for_code_gen == NOT_A_RULE)
	{
		return SYNTAX_ERR;
	}
	else
	{
		if ((result = check_semantics(rule_for_code_gen, op1, op2, op3, &final_type)))
			return result;

		if (rule_for_code_gen == NT_PLUS_NT && final_type == TYPE_STRING)
		{
			GENERATE_CODE(generate_concat_stack_strings);
		}
		else GENERATE_CODE(generate_stack_operation, rule_for_code_gen);

		symbol_stack_pop_count(&stack, count + 1);
		symbol_stack_push(&stack, NON_TERM, final_type);
	}

	return SYNTAX_OK;
}


int expression(PData* data)
{
	int result = SYNTAX_ERR;

	symbol_stack_init(&stack);

	if (!symbol_stack_push(&stack, DOLLAR, TYPE_UNDEFINED))
		FREE_RESOURCES_RETURN(ERROR_INTERNAL);

	Symbol_stack_item* top_stack_terminal;
	Prec_table_symbol_enum actual_symbol;

	bool success = false;

	do
	{
		actual_symbol = get_symbol_from_token(&data->token);
		top_stack_terminal = symbol_stack_top_terminal(&stack);

		if (top_stack_terminal == NULL)
			FREE_RESOURCES_RETURN(ERROR_INTERNAL);

		switch (prec_table[get_prec_table_index(top_stack_terminal->symbol)][get_prec_table_index(actual_symbol)])
		{
		case S:
			if (!symbol_stack_insert_after_top_terminal(&stack, STOP, TYPE_UNDEFINED))
				FREE_RESOURCES_RETURN(ERROR_INTERNAL);

			if(!symbol_stack_push(&stack, actual_symbol, get_data_type(&data->token, data)))
				FREE_RESOURCES_RETURN(ERROR_INTERNAL);

			if (actual_symbol == IDENTIFIER || actual_symbol == INT_NUMBER || actual_symbol == DOUBLE_NUMBER || actual_symbol == STRING)
			{
				GENERATE_CODE(generate_push, data->token);
			}

			if ((result = get_next_token(&data->token)))
				FREE_RESOURCES_RETURN(result);
			break;

		case E:
			symbol_stack_push(&stack, actual_symbol, get_data_type(&data->token, data));

			if ((result = get_next_token(&data->token)))
				FREE_RESOURCES_RETURN(result);
			break;

		case R:
			if ((result = reduce_by_rule(data)))
				FREE_RESOURCES_RETURN(result);
			break;

		case N:
			if (actual_symbol == DOLLAR && top_stack_terminal->symbol == DOLLAR)
				success = true;
			else
				FREE_RESOURCES_RETURN(SYNTAX_ERR);
			break;
		}
	} while (!success);

	Symbol_stack_item* final_non_terminal = symbol_stack_top(&stack);
	if (final_non_terminal == NULL)
		FREE_RESOURCES_RETURN(ERROR_INTERNAL);
	if (final_non_terminal->symbol != NON_TERM)
		FREE_RESOURCES_RETURN(SYNTAX_ERR);

	if (data->lhs_id != NULL)
	{
		char *frame = "LF";
		if (data->lhs_id->global) frame = "GF";

		switch (data->lhs_id->type)
		{
		case TYPE_INT:
			if (final_non_terminal->data_type == TYPE_STRING)
				FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

			GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_INT, frame);
			break;

		case TYPE_DOUBLE:
			if (final_non_terminal->data_type == TYPE_STRING)
				FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

			GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_DOUBLE, frame);
			break;

		case TYPE_STRING:
			if (final_non_terminal->data_type != TYPE_STRING)
				FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

			GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, TYPE_STRING, TYPE_STRING, frame);
			break;

		case TYPE_UNDEFINED:
			GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_UNDEFINED, frame);
			break;

		case TYPE_BOOL:
			if (final_non_terminal->data_type != TYPE_BOOL)
				FREE_RESOURCES_RETURN(SEM_ERR_TYPE_COMPAT);

			GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, TYPE_BOOL, frame);
			break;

		default:
			break;
		}
	}

	FREE_RESOURCES_RETURN(SYNTAX_OK);
}
