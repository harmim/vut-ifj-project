/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Code generator interface.
 *
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H


#include <stdio.h>
#include <stdbool.h>

#include "symtable.h"
#include "scanner.h"
#include "expression.h"


#define MAX_TERM_DIGITS 40 /// Maximum digits terms.


/**
 * Inicialization of code generator.
 * Define built-in functions, etc.
 *
 * @return True if it was successful, false otherwise.
 */
bool code_generator_start();

/**
 * Clear resources.
 */
void code_generator_clear();

/**
 * Flush generated code to destination file.
 *
 * @param destination_file Pointer to destination file.
 */
void code_generator_flush(FILE *destination_file);

bool generate_main_scope();

bool generate_end_of_main_scope();

bool generate_function_head(char *function_id);

bool generate_end_of_function(char *function_id);

bool generate_function_retval(Data_type type);

bool generate_declare_var(char *var_id);

bool generate_var_default_value(char *var_id, Data_type type);

bool generate_call_function(char *function_id);

bool generate_function_retval_assign(char *l_val_id, Data_type l_type, Data_type ret_type);

bool generate_declare_function_param(char *var_id, int index);

bool generate_before_pass_params_to_function();

bool generate_pass_param_to_function(Token token, int index);

bool generate_input(char *var_id, Data_type type);

bool generate_push(Token token);

bool generate_stack_operation(Prec_rules_enum rule);

bool generate_concat_stack_strings();

bool generate_save_expression_result(char *var_id, Data_type ret_type, Data_type l_type, char *frame);

bool generate_stack_op1_to_double();

bool generate_stack_op2_to_double();

bool generate_print();

bool generate_function_return(char *function_id);

bool generate_if_head();

bool generate_if_start(char *function_id, int label_index, int label_deep);

bool generate_if_else_part(char *function_id, int label_index, int label_deep);

bool generate_if_end(char *function_id, int label_index, int label_deep);

bool generate_while_head(char *function_id, int label_index, int label_deep);

bool generate_while_start(char *function_id, int prev_label_index, int label_deep);

bool generate_while_end(char *function_id, int label_index, int label_deep);


#endif //_CODE_GENERATOR_H
