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

/**
 * Generates start of main scope.
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_main_scope_start();

/**
 * Generates end of main scope.
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_main_scope_end();

/**
 * Generates start of function.
 *
 * @param function_id Function identifier.
 * @return True if it was successful, false otherwise.
 */
bool generate_function_start(char *function_id);

/**
 * Generates end of function.
 *
 * @param function_id Function identifier.
 * @return True if it was successful, false otherwise.
 */
bool generate_function_end(char *function_id);

/**
 * Generates declaration and definition of function return value.
 *
 * @param type Data type of function return value.
 * @return True if it was successful, false otherwise.
 */
bool generate_function_retval(Data_type type);

/**
 * Generates function call (jump to the function).
 *
 * @param function_id Function identifier.
 * @return True if it was successful, false otherwise.
 */
bool generate_function_call(char *function_id);

/**
 * Generates assignment function return value of L-variable.
 *
 * @param l_val_id Identifier of L-variable.
 * @param l_type L-variable data type.
 * @param ret_type Data type of function return value.
 * @return True if it was successful, false otherwise.
 */
bool generate_function_retval_assign(char *l_val_id, Data_type l_type, Data_type ret_type);

/**
 * Generates local variable for function parameter.
 *
 * @param param_id Parameter identifier.
 * @param index Parameter index (position).
 * @return True if it was successful, false otherwise.
 */
bool generate_function_param_declare(char *param_id, int index);

/**
 * Generates code for preparation for pass parameters to function.
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_function_before_pass_params();

/**
 * Generates passing parameter to function.
 *
 * @param token Token with passed parameter value.
 * @param index Parameter index (position).
 * @return True if it was successful, false otherwise.
 */
bool generate_function_pass_param(Token token, int index);

/**
 * Generates convert passed parameter data type.
 *
 * @param from Data type to convert from.
 * @param to Data type to conver to.
 * @param index Parameter index (position).
 * @return True if it was successful, false otherwise.
 */
bool generate_function_convert_passed_param(Data_type from, Data_type to, int index);

/**
 * Generates return statement in function.
 *
 * @param function_id Function identifier.
 * @return True if it was successful, false otherwise.
 */
bool generate_function_return(char *function_id);

/**
 * Generates variable declaration.
 *
 * @param var_id Variable identifier.
 * @return True if it was successful, false otherwise.
 */
bool generate_var_declare(char *var_id);

/**
 * Generates assignment default value to variable.
 *
 * @param var_id Variable identifier.
 * @param type Data type of variable.
 * @return True if it was successful, false otherwise.
 */
bool generate_var_default_value(char *var_id, Data_type type);

/**
 * Generates input statement.
 *
 * @param var_id Identifier of variable read value.
 * @param type Data type of read value.
 * @return True if it was successful, false otherwise.
 */
bool generate_input(char *var_id, Data_type type);

/**
 * Generates print of expression result.
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_print();

/**
 * Generates push value to data stack.
 *
 * @param token Token with value to be pushed.
 * @return True if it was successful, false otherwise.
 */
bool generate_push(Token token);

/**
 * Generates operation with top data stack items.
 *
 * @param rule Expression rule.
 * @return True if it was successful, false otherwise.
 */
bool generate_stack_operation(Prec_rules_enum rule);

/**
 * Generates concatition of top data stack items.
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_concat_stack_strings();

/**
 * Generates save expression result from data stack.
 *
 * @param var_id Variable identifier for expression result.
 * @param ret_type Data type of expression in data stack.
 * @param l_type Data type of variable for save result.
 * @param frame Variable frame of given variable.
 * @return True if it was successful, false otherwise.
 */
bool generate_save_expression_result(char *var_id, Data_type ret_type, Data_type l_type, char *frame);

/**
 * Generates convert data stack top item to double.
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_stack_op1_to_double();

/**
 * Generates convert data stack top-1 item to double.
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_stack_op2_to_double();

/**
 * Generates If-Then head (before processing If-Then expression).
 *
 * @return True if it was successful, false otherwise.
 */
bool generate_if_head();

/**
 * Generates If-Then start (after processing If-Then expression).
 *
 * @param function_id Identifier of function in which is this statement.
 * @param label_index Index of label.
 * @param label_deep Deep of label.
 * @return True if it was successful, false otherwise.
 */
bool generate_if_start(char *function_id, int label_index, int label_deep);

/**
 * Generates If-Then Else part (after processing If-Then statement).
 *
 * @param function_id Identifier of function in which is this statement.
 * @param label_index Index of label.
 * @param label_deep Deep of label.
 * @return True if it was successful, false otherwise.
 */
bool generate_if_else_part(char *function_id, int label_index, int label_deep);

/**
 * Generates If-Then end (after processing If-Then Else statement).
 *
 * @param function_id Identifier of function in which is this statement.
 * @param label_index Index of label.
 * @param label_deep Deep of label.
 * @return True if it was successful, false otherwise.
 */
bool generate_if_end(char *function_id, int label_index, int label_deep);

/**
 * Generates Do-While head (before processing expression).
 *
 * @param function_id Identifier of function in which is this statement.
 * @param label_index Index of label.
 * @param label_deep Deep of label.
 * @return True if it was successful, false otherwise.
 */
bool generate_while_head(char *function_id, int label_index, int label_deep);

/**
 * Generates Do-While head (after processing expression).
 *
 * @param function_id Identifier of function in which is this statement.
 * @param label_index Index of label.
 * @param label_deep Deep of label.
 * @return True if it was successful, false otherwise.
 */
bool generate_while_start(char *function_id, int label_index, int label_deep);

/**
 * Generates Do-While head (after processing statement).
 *
 * @param function_id Identifier of function in which is this statement.
 * @param label_index Index of label.
 * @param label_deep Deep of label.
 * @return True if it was successful, false otherwise.
 */
bool generate_while_end(char *function_id, int label_index, int label_deep);


#endif //_CODE_GENERATOR_H
