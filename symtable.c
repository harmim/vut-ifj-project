/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Symbol table implementation using hash table.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include <stdlib.h>
#include <string.h>

#include "symtable.h"


 /**
  * Calculates index to table (hash).
  *
  * @param str String from which hash will be calculated.
  * @return Returns calculated hash.
  */
unsigned hash_function(const char *str)
{
	unsigned int index = 0;
	const unsigned char *char_ptr;

	for (char_ptr = (const unsigned char*)str; *char_ptr != '\0'; char_ptr++)
	{
		index = 65599 * index + *char_ptr;
	}

	return index % MAX_SYMTABLE_SIZE;
}

void sym_table_init(Sym_table *table)
{
	if (table == NULL)
		return;

	for (int i = 0; i < MAX_SYMTABLE_SIZE; i++)
	{
		(*table)[i] = NULL;
	}
}


TData *sym_table_add_symbol(Sym_table *table, const char *key, bool* alloc_failed)
{
	*alloc_failed = false;

	if (table == NULL || key == NULL)
	{
		*alloc_failed = true;
		return NULL;
	}

	unsigned index = hash_function(key);
	Sym_table_item *tmp_last = NULL;

	for (Sym_table_item *tmp = (*table)[index]; tmp != NULL; tmp = tmp->next) {
		if (!strcmp(key, tmp->key)) {
			return NULL;
		}

		tmp_last = tmp;
	}

	Sym_table_item *new_item = (Sym_table_item *)malloc(sizeof(Sym_table_item));
	if (new_item == NULL)
	{
		*alloc_failed = true;
		return NULL;
	}

	new_item->key = (char *)malloc((strlen(key) + 1) * sizeof(char));

	if (new_item->key == NULL) {
		free(new_item);
		*alloc_failed = true;
		return NULL;
	}

	strcpy(new_item->key, key);
	new_item->data.type = TYPE_UNDEFINED;
	new_item->data.params = NULL;
	new_item->data.defined = false;
	new_item->next = NULL;

	if (tmp_last == NULL)
		(*table)[index] = new_item;
	else
		tmp_last->next = new_item;

	return &new_item->data;
}

bool sym_table_add_param(TData *data, int data_type)
{
	if (data == NULL)
		return false;

	if (data->params == NULL)
	{
		if (!(data->params = (Dynamic_string *)malloc(sizeof(Dynamic_string))))
			return false;
		dynamic_string_init(data->params);
	}		

	switch (data_type)
	{
	case (TYPE_INT):
		dynamic_string_add_char(data->params, 'i');
		break;

	case (TYPE_DOUBLE):
		dynamic_string_add_char(data->params, 'd');
		break;

	case (TYPE_STRING):
		dynamic_string_add_char(data->params, 's');
		break;

	default:
		break;
	}
	
	return true;
}

int sym_table_number_of_params(Sym_table *table, const char *key)
{
	if (table == NULL || key == NULL)
		return -3;

	TData *data = sym_table_search(table, key);

	if (data == NULL)
		return NONEXISTING_SYMBOL;
	if (data->params == NULL)
		return NOT_FUNCTION;

	return (int)data->params->length;
}

TData *sym_table_search(Sym_table *table, const char *key)
{
	if (table == NULL || key == NULL)
		return NULL;

	unsigned index = hash_function(key);

	for (Sym_table_item *tmp = (*table)[index]; tmp != NULL; tmp = tmp->next) {
		if (!strcmp(key, tmp->key)) {
			return &tmp->data;
		}
	}

	return NULL;
}

bool sym_table_remove_symbol(Sym_table *table, const char *key)
{
	if (table == NULL || key == NULL)
		return false;

	unsigned index = hash_function(key);

	Sym_table_item *tmp_last = NULL;

	for (Sym_table_item *tmp = (*table)[index]; tmp != NULL; tmp = tmp->next) {
		if (!strcmp(key, tmp->key)) 
		{
			if (tmp_last == NULL) 
			{
				(*table)[index] = (*table)[index]->next;
			}
			else 
			{
				tmp_last->next = tmp->next;
			}

			free(tmp->key);

			if (tmp->data.params != NULL)
			{
				dynamic_string_free(tmp->data.params);
				free(tmp->data.params);
			}

			free(tmp);

			return true;
		}
		tmp_last = tmp;
	}

	return false;
}

void sym_table_free(Sym_table *table)
{
	if (table == NULL)
		return;

	Sym_table_item *tmp_next = NULL;

	for (int i = 0; i < MAX_SYMTABLE_SIZE; i++) {
		for (Sym_table_item *tmp = (*table)[i]; tmp != NULL; tmp = tmp_next) {
			tmp_next = tmp->next;
			free(tmp->key);

			if (tmp->data.params != NULL)
			{
				dynamic_string_free(tmp->data.params);
				free(tmp->data.params);
			}

			free(tmp);
		}

		(*table)[i] = NULL;
	}
}
