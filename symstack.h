/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Stack of tokens interface.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#ifndef _SYMSTACK_H
#define _SYMSTACK_H


#include <stdbool.h>

#include "expression.h"
#include "symtable.h"


/**
 * @struct Stack item represetation.
 */
typedef struct stack_item
{
	Prec_table_symbol_enum symbol; /// Symbol of stack item.
	Data_type data_type; /// Data type used for semantic analysis.
	struct stack_item *next; /// Pointer to next stack item.
} Symbol_stack_item;

/**
 * @struct Stack representation.
 */
typedef struct
{
	Symbol_stack_item *top; /// Pointer to stack item on top of stack.
} Symbol_stack;


/**
 * Function initializes stack.
 *
 * @param stack Pointer to stack.
 */
void symbol_stack_init(Symbol_stack* stack);

/**
 * Function pushes symbol to stack and sets its data type.
 *
 * @param stack Pointer to stack.
 * @param symbol Symbol to be pushed.
 * @param type Data type to be set.
 * @return True if successfull else false.
 */
bool symbol_stack_push(Symbol_stack* stack, Prec_table_symbol_enum symbol, Data_type type);

/**
 * Function pops top symbol from stack.
 *
 * @param stack Pointer to stack.
 * @return True if successfull else false.
 */
bool symbol_stack_pop(Symbol_stack* stack);

/**
 * Function pops stack more times.
 *
 * @param stack Pointer to stack.
 * @param count How many times stack will be popped.
 */
void symbol_stack_pop_count(Symbol_stack* stack, int count);

/**
 * Function returns top termial.
 *
 * @param stack Pointer to stack.
 * @return Returns pointer to top terminal.
 */
Symbol_stack_item* symbol_stack_top_terminal(Symbol_stack* stack);

/**
 * Function inserts symbol after top terminal.
 *
 * @param stack Pointer to stack.
 * @param symbol Symbol to be pushed.
 * @param type Data type to be set.
 * @return True if successfull else false.
 */
bool symbol_stack_insert_after_top_terminal(Symbol_stack* stack, Prec_table_symbol_enum symbol, Data_type type);

/**
 * Function returns top symbol.
 *
 * @param stack Pointer to stack.
 * @return Pointer to symbol on top of stack.
 */
Symbol_stack_item* symbol_stack_top(Symbol_stack* stack);

/**
 * Function frees resources used for stack.
 *
 * @param stack Pointer to stack.
 */
void symbol_stack_free(Symbol_stack* stack);


#endif //_SYMSTACK_H
