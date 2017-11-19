/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Code generator implementation.
 *
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include <stdlib.h>

#include "dynamic_string.h"
#include "code_generator.h"


// macros
#define ADD_INST(_inst)														\
	if (!dynamic_string_add_const_str(&code, (_inst "\n"))) return false

#define ADD_CODE(_code)														\
	if (!dynamic_string_add_const_str(&code, (_code))) return false

#define ADD_BLOCK_COMMENT(_comment)											\
	ADD_INST("\n# " _comment)


// built-in functions
// Length(s As String) As Integer
#define FUNCTION_LENGTH														\
	"\n # Built-in function Length"											\
	"\n LABEL $length"														\
	"\n PUSHFRAME"															\
	"\n DEFVAR LF@%retval"													\
	"\n STRLEN LF@%retval LF@%0"											\
	"\n POPFRAME"															\
	"\n RETURN"																\
	"\n"

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
	"\n RETURN"																\
	"\n"

// Asc(s As String, i As Integer) As Integer
#define FUNCTION_ASC														\
	"\n # Built-in function Asc"											\
	"\n LABEL $asc"															\
	"\n PUSHFRAME"															\
	"\n DEFVAR LF@%retval"													\
	"\n MOVE LF@%retval int@0"												\
	"\n DEFVAR LF@cond_length"												\
	"\n LT LF@cond_length LF@%1 int@0"										\
	"\n JUMPIFEQ $asc$return LF@cond_length bool@true"						\
	"\n DEFVAR LF@length_str"												\
	"\n CREATEFRAME"														\
	"\n DEFVAR TF@%0"														\
	"\n MOVE TF@%0 LF@%0"													\
	"\n CALL $length"														\
	"\n MOVE LF@length_str TF@%retval"										\
	"\n SUB LF@length_str LF@length_str int@1"								\
	"\n GT LF@cond_length LF@%1 LF@length_str"								\
	"\n JUMPIFEQ $asc$return LF@cond_length bool@true"						\
	"\n STRI2INT LF@%retval LF@%0 LF@%1"									\
	"\n LABEL $asc$return"													\
	"\n POPFRAME"															\
	"\n RETURN"																\
	"\n"

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
	"\n RETURN"																\
	"\n"


Dynamic_string code; /// String for generating code.


static bool define_build_in_functions()
{
	ADD_INST(FUNCTION_LENGTH);
	ADD_INST(FUNCTION_SUBSTR);
	ADD_INST(FUNCTION_ASC);
	ADD_INST(FUNCTION_CHR);

	return true;
}


static bool generate_file_header()
{
	ADD_BLOCK_COMMENT("Start of program");

	ADD_INST(".IFJcode17");

	ADD_INST("DEFVAR GF@%input_prompt");
	ADD_INST("MOVE GF@%input_prompt string@?\\032");

	ADD_INST("JUMP $$main");

	return true;
}


bool code_generator_start()
{
	if (!dynamic_string_init(&code)) return false;

	if (!generate_file_header()) return false;

	if (!define_build_in_functions()) return false;

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


bool generate_main_scope()
{
	ADD_BLOCK_COMMENT("Main scope");

	ADD_INST("LABEL $$main");
	ADD_INST("CREATEFRAME");
	ADD_INST("PUSHFRAME");

	return true;
}


bool generate_end_of_main_scope()
{
	ADD_BLOCK_COMMENT("End of main scope");

	ADD_INST("POPFRAME");
	ADD_INST("CLEARS");

	return true;
}


bool generate_function_head(char *function_id)
{
	ADD_BLOCK_COMMENT("Start of function");

	ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("\n");
	ADD_INST("PUSHFRAME");

	return true;
}


bool generate_end_of_function()
{
	ADD_INST("POPFRAME");
	ADD_INST("RETURN");

	return true;
}


bool generate_function_retval(Data_type type)
{
	ADD_INST("DEFVAR LF@%retval");

	ADD_CODE("MOVE LF@%retval ");
	switch (type)
	{
		case TYPE_INT:
			ADD_CODE("int@0");
			break;

		case TYPE_DOUBLE:
			ADD_CODE("int@0.0");
			break;

		case TYPE_STRING:
			ADD_CODE("string@");
			break;

		default:
			break;
	}
	ADD_CODE("\n");

	return true;
}


bool generate_declare_var(char *var_id)
{
	ADD_CODE("DEFVAR LF@"); ADD_CODE(var_id); ADD_CODE("\n");

	return true;
}


bool generate_call_function(char *function_id)
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


bool generate_declare_function_param(char *var_id, int index)
{
	ADD_CODE("DEFVAR LF@"); ADD_CODE(var_id); ADD_CODE("\n");

	char index_str[MAX_TERM_DIGITS];
	sprintf(index_str, "%d", index);
	ADD_CODE("MOVE LF@"); ADD_CODE(var_id); ADD_CODE(" LF@%"); ADD_CODE(index_str); ADD_CODE("\n");

	return true;
}


bool generate_before_pass_params_to_function()
{
	ADD_INST("CREATEFRAME");

	return true;
}


bool generate_pass_param_to_function(Token_type type, Token_attribute attribute, int index)
{
	char index_str[MAX_TERM_DIGITS];
	sprintf(index_str, "%d", index);
	ADD_CODE("DEFVAR TF@%"); ADD_CODE(index_str); ADD_CODE("\n");

	ADD_CODE("MOVE TF@%"); ADD_CODE(index_str); ADD_CODE(" ");
	char term_str[MAX_TERM_DIGITS], c;
	Dynamic_string tmp_string;
	dynamic_string_init(&tmp_string);
	switch (type)
	{
		case TOKEN_TYPE_INT_NUMBER:
			sprintf(term_str, "%d", attribute.integer);
			ADD_CODE("int@"); ADD_CODE(term_str);
			break;

		case TOKEN_TYPE_DOUBLE_NUMBER:
			sprintf(term_str, "%g", attribute.decimal);
			ADD_CODE("float@"); ADD_CODE(term_str);
			break;

		case TOKEN_TYPE_STRING:
			for (int i = 0; (c = (attribute.string->str)[i]) != '\0'; i++)
			{
				if (c == '#' || c == '\\' || c <= 32)
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
			ADD_CODE("LF@"); ADD_CODE(attribute.string->str);
			break;

		default:
			return false;
	}
	ADD_CODE("\n");
	dynamic_string_free(&tmp_string);

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
