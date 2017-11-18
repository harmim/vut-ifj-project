/**
 * Project: Implementace pøekladaèe imperativního jazyka IFJ17.
 *
 * @brief Stack of symbols implementation.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 */


#include <stdlib.h>
#include <stdio.h>

#include "symstack.h"

void symbol_stack_init(Symbol_stack* stack)
{
	stack->top = NULL;
}

bool symbol_stack_push(Symbol_stack* stack, Prec_table_symbol_enum symbol, Data_type type)
{
	Symbol_stack_item* new_item = (Symbol_stack_item*)malloc(sizeof(Symbol_stack_item));

	if (new_item == NULL)
		return false;

	new_item->symbol = symbol;
	new_item->data_type = type;
	new_item->next = stack->top;

	stack->top = new_item;

	return true;
}

bool symbol_stack_pop(Symbol_stack* stack)
{
	if (stack->top != NULL) 
	{
		Symbol_stack_item* tmp = stack->top;
		stack->top = tmp->next;
		free(tmp);

		return true;
	}
	return false;
}

void symbol_stack_pop_count(Symbol_stack* stack, int count)
{
	for (int i = 0; i < count; i++)
	{
		symbol_stack_pop(stack);
	}
}

Symbol_stack_item* symbol_stack_top_terminal(Symbol_stack* stack)
{
	for (Symbol_stack_item* tmp = stack->top; tmp != NULL; tmp = tmp->next)
	{
		if (tmp->symbol < STOP)
			return tmp;
	}

	return NULL;
}

bool symbol_stack_insert_after_top_terminal(Symbol_stack* stack, Prec_table_symbol_enum symbol, Data_type type)
{
	Symbol_stack_item* prev = NULL;

	for (Symbol_stack_item* tmp = stack->top; tmp != NULL; tmp = tmp->next)
	{
		if (tmp->symbol < STOP)
		{
			Symbol_stack_item* new_item = (Symbol_stack_item*)malloc(sizeof(Symbol_stack_item));

			if (new_item == NULL)
				return false;

			new_item->symbol = symbol;
			new_item->data_type = type;

			if (prev == NULL)
			{
				new_item->next = stack->top;
				stack->top = new_item;
			}
			else
			{
				new_item->next = prev->next;
				prev->next = new_item;
			}

			return true;
		}		

		prev = tmp;
	}

	return false;
}

Symbol_stack_item* symbol_stack_top(Symbol_stack* stack)
{
		return stack->top;
}

void symbol_stack_free(Symbol_stack* stack)
{
	while (symbol_stack_pop(stack));
}
