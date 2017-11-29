/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Code generator implementation.
 *
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include <ctype.h>

#include "dynamic_string.h"
#include "scanner.h"
#include "code_generator.h"


/// macros
#define ADD_INST(_inst)														\
	if (!dynamic_string_add_const_str(&code, (_inst "\n"))) return false

#define ADD_CODE(_code)														\
	if (!dynamic_string_add_const_str(&code, (_code))) return false

#define ADD_CODE_INT(_code)													\
	do {																	\
		char str[MAX_DIGITS];												\
		sprintf(str, "%d", _code);											\
		ADD_CODE(str);														\
	} while (0)


#define MAX_DIGITS 40 /// Maximum digits for converting number to string.

/// built-in functions
// Length(s As String) As Integer
#define FUNCTION_LENGTH														\
	"\n # Built-in function Length"											\
	"\n LABEL $length"														\
	"\n PUSHFRAME"															\
	"\n DEFVAR LF@%retval"													\
	"\n STRLEN LF@%retval LF@%0"											\
	"\n POPFRAME"															\
	"\n RETURN"

// SubStr(s As String, i As Integer, n As Integer) As String
#define FUNCTION_SUBSTR														\
	"\n # Built-in function SubStr"											\
	"\n LABEL $substr"														\
	"\n PUSHFRAME"															\
	"\n DEFVAR LF@%retval"													\
	"\n MOVE LF@%retval string@"											\
	"\n DEFVAR LF@length_str"												\
	"\n CREATEFRAME"														\
	"\n DEFVAR TF@%0"														\
	"\n MOVE TF@%0 LF@%0"													\
	"\n CALL $length"														\
	"\n MOVE LF@length_str TF@%retval"										\
	"\n DEFVAR LF@ret_cond"													\
	"\n LT LF@ret_cond LF@length_str int@0"									\
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						\
	"\n EQ LF@ret_cond LF@length_str int@0"									\
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						\
	"\n LT LF@ret_cond LF@%1 int@0"											\
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						\
	"\n EQ LF@ret_cond LF@%1 int@0"											\
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						\
	"\n GT LF@ret_cond LF@%1 LF@length_str"									\
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						\
	"\n EQ LF@ret_cond LF@%2 int@0"											\
	"\n JUMPIFEQ $substr$return LF@ret_cond bool@true"						\
	"\n DEFVAR LF@max_n"													\
	"\n MOVE LF@max_n LF@length_str"										\
	"\n SUB LF@max_n LF@max_n LF@%1"										\
	"\n ADD LF@max_n LF@max_n int@1"										\
	"\n DEFVAR LF@edit_n_cond"												\
	"\n LT LF@edit_n_cond LF@%2 int@0"										\
	"\n JUMPIFEQ $substr$edit_n LF@edit_n_cond bool@true"					\
	"\n GT LF@edit_n_cond LF@%2 LF@max_n"									\
	"\n JUMPIFEQ $substr$edit_n LF@edit_n_cond bool@true"					\
	"\n JUMP $substr$process"												\
	"\n LABEL $substr$edit_n"												\
	"\n MOVE LF@%2 LF@max_n"												\
	"\n LABEL $substr$process"												\
	"\n DEFVAR LF@index"													\
	"\n MOVE LF@index LF@%1"												\
	"\n SUB LF@index LF@index int@1"										\
	"\n DEFVAR LF@char"														\
	"\n DEFVAR LF@process_loop_cond"										\
	"\n LABEL $substr$process_loop"											\
	"\n GETCHAR LF@char LF@%0 LF@index"										\
	"\n CONCAT LF@%retval LF@%retval LF@char"								\
	"\n ADD LF@index LF@index int@1"										\
	"\n SUB LF@%2 LF@%2 int@1"												\
	"\n GT LF@process_loop_cond LF@%2 int@0"								\
	"\n JUMPIFEQ $substr$process_loop LF@process_loop_cond bool@true"		\
	"\n LABEL $substr$return"												\
	"\n POPFRAME"															\
	"\n RETURN"

// Asc(s As String, i As Integer) As Integer
#define FUNCTION_ASC														\
	"\n # Built-in function Asc"											\
	"\n LABEL $asc"															\
	"\n PUSHFRAME"															\
	"\n DEFVAR LF@%retval"													\
	"\n MOVE LF@%retval int@0"												\
	"\n DEFVAR LF@cond_length"												\
	"\n LT LF@cond_length LF@%1 int@1"										\
	"\n JUMPIFEQ $asc$return LF@cond_length bool@true"						\
	"\n DEFVAR LF@length_str"												\
	"\n CREATEFRAME"														\
	"\n DEFVAR TF@%0"														\
	"\n MOVE TF@%0 LF@%0"													\
	"\n CALL $length"														\
	"\n MOVE LF@length_str TF@%retval"										\
	"\n GT LF@cond_length LF@%1 LF@length_str"								\
	"\n JUMPIFEQ $asc$return LF@cond_length bool@true"						\
	"\n SUB LF@%1 LF@%1 int@1"												\
	"\n STRI2INT LF@%retval LF@%0 LF@%1"									\
	"\n LABEL $asc$return"													\
	"\n POPFRAME"															\
	"\n RETURN"

// Chr(i As Integer) As String
#define FUNCTION_CHR														\
	"\n # Built-in function Chr"											\
	"\n LABEL $chr"															\
	"\n PUSHFRAME"															\
	"\n DEFVAR LF@%retval"													\
	"\n MOVE LF@%retval string@"											\
	"\n DEFVAR LF@cond_range"												\
	"\n LT LF@cond_range LF@%0 int@0"										\
	"\n JUMPIFEQ $chr$return LF@cond_range bool@true"						\
	"\n GT LF@cond_range LF@%0 int@255"										\
	"\n JUMPIFEQ $chr$return LF@cond_range bool@true"						\
	"\n INT2CHAR LF@%retval LF@%0"											\
	"\n LABEL $chr$return"													\
	"\n POPFRAME"															\
	"\n RETURN"


Dynamic_string code; /// String for generating code.


/**
 * Defines built-in functions.
 *
 * @return True if it was successful, false otherwise.
 */
static bool define_built_in_functions()
{
	ADD_INST(FUNCTION_LENGTH);
	ADD_INST(FUNCTION_SUBSTR);
	ADD_INST(FUNCTION_ASC);
	ADD_INST(FUNCTION_CHR);

	return true;
}


/**
 * Generates file header, defines global variables and jump to main function.
 *
 * @return True if it was successful, false otherwise.
 */
static bool generate_file_header()
{
	ADD_INST("# Start of program");

	ADD_INST(".IFJcode17");

	ADD_INST("DEFVAR GF@%input_prompt");
	ADD_INST("MOVE GF@%input_prompt string@?\\032");

	ADD_INST("DEFVAR GF@%tmp_op1");
	ADD_INST("DEFVAR GF@%tmp_op2");
	ADD_INST("DEFVAR GF@%tmp_op3");

	ADD_INST("DEFVAR GF@%exp_result");

	ADD_INST("JUMP $$main");

	return true;
}


bool code_generator_start()
{
	if (!dynamic_string_init(&code)) return false;

	if (!generate_file_header()) return false;

	if (!define_built_in_functions()) return false;

	return true;
}


void code_generator_clear()
{
	dynamic_string_free(&code);
}


void code_generator_flush(FILE *destination_file)
{
	fputs(code.str, destination_file);
	code_generator_clear();
}


bool generate_main_scope_start()
{
	ADD_INST("\n# Main scope");

	ADD_INST("LABEL $$main");
	ADD_INST("CREATEFRAME");
	ADD_INST("PUSHFRAME");

	return true;
}


bool generate_main_scope_end()
{
	ADD_INST("# End of main scope");

	ADD_INST("POPFRAME");
	ADD_INST("CLEARS");

	return true;
}


bool generate_function_start(char *function_id)
{
	ADD_CODE("\n# Start of function "); ADD_CODE(function_id); ADD_CODE("\n");

	ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("\n");
	ADD_INST("PUSHFRAME");

	return true;
}


bool generate_function_end(char *function_id)
{
	ADD_CODE("# End of function "); ADD_CODE(function_id); ADD_CODE("\n");

	ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("%return\n");
	ADD_INST("POPFRAME");
	ADD_INST("RETURN");

	return true;
}


/**
 * Generates default value of variable.
 *
 * @param type Data type of variable.
 * @return True if it was successful, false otherwise.
 */
static bool generate_default_var_value(Data_type type)
{
	switch (type)
	{
		case TYPE_INT:
			ADD_CODE("int@0");
			break;

		case TYPE_DOUBLE:
			ADD_CODE("float@0.0");
			break;

		case TYPE_STRING:
			ADD_CODE("string@");
			break;

		case TYPE_BOOL:
			ADD_CODE("bool@false");

		default:
			return false;
	}

	return true;
}


bool generate_function_retval(Data_type type)
{
	ADD_INST("DEFVAR LF@%retval");

	ADD_CODE("MOVE LF@%retval ");
	if (!generate_default_var_value(type)) return false;
	ADD_CODE("\n");

	return true;
}


bool generate_function_call(char *function_id)
{
	ADD_CODE("CALL $"); ADD_CODE(function_id); ADD_CODE("\n");

	return true;
}


bool generate_function_retval_assign(char *l_val_id, Data_type l_type, Data_type ret_type)
{
	if (l_type == TYPE_INT && ret_type == TYPE_DOUBLE)
	{
		ADD_INST("FLOAT2R2EINT TF@%retval TF@%retval");
	}
	else if (l_type == TYPE_DOUBLE && ret_type == TYPE_INT)
	{
		ADD_INST("INT2FLOAT TF@%retval TF@%retval");
	}

	ADD_CODE("MOVE LF@"); ADD_CODE(l_val_id); ADD_CODE(" TF@%retval\n");

	return true;
}


bool generate_function_param_declare(char *param_id, int index)
{
	ADD_CODE("DEFVAR LF@"); ADD_CODE(param_id); ADD_CODE("\n");
	ADD_CODE("MOVE LF@"); ADD_CODE(param_id); ADD_CODE(" LF@%"); ADD_CODE_INT(index); ADD_CODE("\n");

	return true;
}


bool generate_function_before_pass_params()
{
	ADD_INST("CREATEFRAME");

	return true;
}


bool generate_function_convert_passed_param(Data_type from, Data_type to, int index)
{
	if (from == TYPE_DOUBLE && to == TYPE_INT)
	{
		ADD_CODE("FLOAT2R2EINT TF@%"); ADD_CODE_INT(index); ADD_CODE(" TF@%"); ADD_CODE_INT(index); ADD_CODE("\n");
	}
	else if (from == TYPE_INT && to == TYPE_DOUBLE)
	{
		ADD_CODE("INT2FLOAT TF@%"); ADD_CODE_INT(index); ADD_CODE(" TF@%"); ADD_CODE_INT(index); ADD_CODE("\n");
	}

	return true;
}


/**
 * Generates term value.
 *
 * @param token Token with term value.
 * @return True if it was successful, false otherwise.
 */
static bool generate_term_value(Token token)
{
	char term_str[MAX_DIGITS];
	unsigned char c;

	Dynamic_string tmp_string;
	if (!dynamic_string_init(&tmp_string)) return false;

	switch (token.type)
	{
		case TOKEN_TYPE_INT_NUMBER:
			sprintf(term_str, "%d", token.attribute.integer);
			ADD_CODE("int@"); ADD_CODE(term_str);
			break;

		case TOKEN_TYPE_DOUBLE_NUMBER:
			sprintf(term_str, "%g", token.attribute.decimal);
			ADD_CODE("float@"); ADD_CODE(term_str);
			break;

		case TOKEN_TYPE_STRING:
			for (int i = 0; (c = (unsigned char) (token.attribute.string->str)[i]) != '\0'; i++)
			{
				if (c == '#' || c == '\\' || c <= 32 || !isprint(c))
				{
					dynamic_string_add_char(&tmp_string, '\\');
					sprintf(term_str, "%03d", c);
					dynamic_string_add_const_str(&tmp_string, term_str);
				}
				else
				{
					dynamic_string_add_char(&tmp_string, c);
				}
			}
			ADD_CODE("string@"); ADD_CODE(tmp_string.str);
			break;

		case TOKEN_TYPE_IDENTIFIER:
			ADD_CODE("LF@"); ADD_CODE(token.attribute.string->str);
			break;

		default:
			dynamic_string_free(&tmp_string);
			return false;
	}

	dynamic_string_free(&tmp_string);

	return true;
}


bool generate_function_pass_param(Token token, int index)
{
	ADD_CODE("DEFVAR TF@%"); ADD_CODE_INT(index); ADD_CODE("\n");

	ADD_CODE("MOVE TF@%"); ADD_CODE_INT(index); ADD_CODE(" ");
	if (!generate_term_value(token)) return false;
	ADD_CODE("\n");

	return true;
}


bool generate_function_return(char *function_id)
{
	ADD_INST("MOVE LF@%retval GF@%exp_result");
	ADD_CODE("JUMP $"); ADD_CODE(function_id); ADD_CODE("%return\n");

	return true;
}


bool generate_var_declare(char *var_id)
{
	ADD_CODE("DEFVAR LF@"); ADD_CODE(var_id); ADD_CODE("\n");

	return true;
}


bool generate_var_default_value(char *var_id, Data_type type)
{
	ADD_CODE("MOVE LF@"); ADD_CODE(var_id); ADD_CODE(" ");
	if (!generate_default_var_value(type)) return false;
	ADD_CODE("\n");

	return true;
}


bool generate_input(char *var_id, Data_type type)
{
	ADD_INST("WRITE GF@%input_prompt");

	ADD_CODE("READ LF@"); ADD_CODE(var_id); ADD_CODE(" ");
	switch (type)
	{
		case TYPE_INT:
			ADD_CODE("int");
			break;

		case TYPE_DOUBLE:
			ADD_CODE("float");
			break;

		case TYPE_STRING:
			ADD_CODE("string");
			break;

		default:
			return false;
	}
	ADD_CODE("\n");

	return true;
}


bool generate_print()
{
	ADD_INST("WRITE GF@%exp_result");

	return true;
}


bool generate_push(Token token)
{
	ADD_CODE("PUSHS ");
	if (!generate_term_value(token)) return false;
	ADD_CODE("\n");

	return true;
}


bool generate_stack_operation(Prec_rules_enum rule)
{
	switch (rule)
	{
		case NT_PLUS_NT:
			ADD_INST("ADDS");
			break;

		case NT_MINUS_NT:
			ADD_INST("SUBS");
			break;

		case NT_MUL_NT:
			ADD_INST("MULS");
			break;

		case NT_DIV_NT:
			ADD_INST("DIVS");
			break;

		case NT_IDIV_NT:
			ADD_INST("POPS GF@%tmp_op1");
			ADD_INST("INT2FLOATS");
			ADD_INST("PUSHS GF@%tmp_op1");
			ADD_INST("INT2FLOATS");
			ADD_INST("DIVS");
			ADD_INST("FLOAT2INTS");
			break;

		case NT_EQ_NT:
			ADD_INST("EQS");
			break;

		case NT_NEQ_NT:
			ADD_INST("EQS");
			ADD_INST("NOTS");
			break;

		case NT_LEQ_NT:
			ADD_INST("POPS GF@%tmp_op1");
			ADD_INST("POPS GF@%tmp_op2");
			ADD_INST("PUSHS GF@%tmp_op2");
			ADD_INST("PUSHS GF@%tmp_op1");
			ADD_INST("LTS");
			ADD_INST("PUSHS GF@%tmp_op2");
			ADD_INST("PUSHS GF@%tmp_op1");
			ADD_INST("EQS");
			ADD_INST("ORS");
			break;

		case NT_LTN_NT:
			ADD_INST("LTS");
			break;

		case NT_MEQ_NT:
			ADD_INST("POPS GF@%tmp_op1");
			ADD_INST("POPS GF@%tmp_op2");
			ADD_INST("PUSHS GF@%tmp_op2");
			ADD_INST("PUSHS GF@%tmp_op1");
			ADD_INST("GTS");
			ADD_INST("PUSHS GF@%tmp_op2");
			ADD_INST("PUSHS GF@%tmp_op1");
			ADD_INST("EQS");
			ADD_INST("ORS");
			break;

		case NT_MTN_NT:
			ADD_INST("GTS");
			break;

		default:
			break;
	}

	return true;
}


bool generate_concat_stack_strings()
{
	ADD_INST("POPS GF@%tmp_op3");
	ADD_INST("POPS GF@%tmp_op2");
	ADD_INST("CONCAT GF@%tmp_op1 GF@%tmp_op2 GF@%tmp_op3");
	ADD_INST("PUSHS GF@%tmp_op1");

	return true;
}


bool generate_save_expression_result(char *var_id, Data_type ret_type, Data_type l_type, char *frame)
{
	if (l_type == TYPE_INT && ret_type == TYPE_DOUBLE)
	{
		ADD_INST("FLOAT2R2EINTS");
	}
	else if (l_type == TYPE_DOUBLE && ret_type == TYPE_INT)
	{
		ADD_INST("INT2FLOATS");
	}

	ADD_CODE("POPS "); ADD_CODE(frame); ADD_CODE("@"); ADD_CODE(var_id); ADD_CODE("\n");

	return true;
}


bool generate_stack_op1_to_double()
{
	ADD_INST("INT2FLOATS");

	return true;
}


bool generate_stack_op1_to_integer()
{
	ADD_INST("FLOAT2R2EINTS");

	return true;
}


bool generate_stack_op2_to_double()
{
	ADD_INST("POPS GF@%tmp_op1");
	ADD_INST("INT2FLOATS");
	ADD_INST("PUSHS GF@%tmp_op1");

	return true;
}


bool generate_stack_op2_to_integer()
{
	ADD_INST("POPS GF@%tmp_op1");
	ADD_INST("FLOAT2R2EINTS");
	ADD_INST("PUSHS GF@%tmp_op1");

	return true;
}


/**
 * Generates label.
 *
 * @param function_id Identifier of function in which is this label.
 * @param label_index Index of label.
 * @param label_deep Deep of label.
 * @return True if it was successful, false otherwise.
 */
static bool generate_label(char *function_id, int label_index, int label_deep)
{
	ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_deep);
	ADD_CODE("%"); ADD_CODE_INT(label_index); ADD_CODE("\n");

	return true;
}


bool generate_if_head()
{
	ADD_INST("\n# If Then");

	return true;
}


bool generate_if_start(char *function_id, int label_index, int label_deep)
{
	ADD_CODE("JUMPIFEQ $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_deep);
	ADD_CODE("%"); ADD_CODE_INT(label_index); ADD_CODE(" GF@%exp_result bool@false\n");

	return true;
}


bool generate_if_else_part(char *function_id, int label_index, int label_deep)
{
	ADD_CODE("JUMP $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_deep);
	ADD_CODE("%"); ADD_CODE_INT(label_index + 1); ADD_CODE("\n");

	ADD_INST("# Else");

	if (!generate_label(function_id, label_index, label_deep)) return false;

	return true;
}


bool generate_if_end(char *function_id, int label_index, int label_deep)
{
	ADD_INST("# End If");

	if (!generate_label(function_id, label_index, label_deep)) return false;

	return true;
}


bool generate_while_head(char *function_id, int label_index, int label_deep)
{
	ADD_INST("\n# Do While");

	if (!generate_label(function_id, label_index, label_deep)) return false;

	return true;
}


bool generate_while_start(char *function_id, int label_index, int label_deep)
{
	ADD_CODE("JUMPIFEQ $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_deep);
	ADD_CODE("%"); ADD_CODE_INT(label_index); ADD_CODE(" GF@%exp_result bool@false"); ADD_CODE("\n");

	return true;
}


bool generate_while_end(char *function_id, int label_index, int label_deep)
{
	ADD_CODE("JUMP $"); ADD_CODE(function_id); ADD_CODE("%"); ADD_CODE_INT(label_deep);
	ADD_CODE("%"); ADD_CODE_INT(label_index - 1); ADD_CODE("\n");

	ADD_INST("# Loop");

	if (!generate_label(function_id, label_index, label_deep)) return false;

	return true;
}
