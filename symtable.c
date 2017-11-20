/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Symbol table implementation using hash table.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include <stdlib.h>
#include <string.h>

#include "symtable.h"


/**
 * Calculates index to table (hash).
 * GNU Hash ELF. Algorithm implementation used in UNIX ELF.
 * @link https://blogs.oracle.com/ali/gnu-hash-elf-sections
 * @link https://en.wikipedia.org/wiki/PJW_hash_function
 *
 * @param str String from which hash will be calculated.
 * @return Returns calculated hash.
 */
static unsigned long hash_function(const char *str)
{
	unsigned long hash = 0, x = 0;

	for (char c = *str; c != '\0'; c = *(++str))
	{
		hash = (hash << 4) + c;
		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
		}
		hash &= ~x;
	}

	return hash % MAX_SYMTABLE_SIZE;
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

	unsigned long index = hash_function(key);
	Sym_table_item *tmp_last = NULL;

	for (Sym_table_item *tmp = (*table)[index]; tmp != NULL; tmp = tmp->next)
	{
		if (!strcmp(key, tmp->key))
		{
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

	if (!(new_item->key = (char *)malloc((strlen(key) + 1) * sizeof(char))))
	{
		free(new_item);
		*alloc_failed = true;
		return NULL;
	}
	if (!(new_item->data.params = (Dynamic_string *)malloc(sizeof(Dynamic_string))))
	{
		free(new_item->key);
		free(new_item);
		*alloc_failed = true;
		return NULL;
	}
	if (!dynamic_string_init(new_item->data.params))
	{
		free(new_item->key);
		free(new_item);
		free(new_item->data.params);
		*alloc_failed = true;
		return NULL;
	}

	strcpy(new_item->key, key);
	new_item->data.identifier = new_item->key;
	new_item->data.type = TYPE_UNDEFINED;
	new_item->data.defined = false;
	new_item->data.global = false;
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

	switch (data_type)
	{
	case (TYPE_INT):
		if (!dynamic_string_add_char(data->params, 'i'))
		{
			return false;
		}
		break;

	case (TYPE_DOUBLE):
		if (!dynamic_string_add_char(data->params, 'd'))
		{
			return false;
		}
		break;

	case (TYPE_STRING):
		if (!dynamic_string_add_char(data->params, 's'))
		{
			return false;
		}
		break;

	default:
		break;
	}
	
	return true;
}


TData *sym_table_search(Sym_table *table, const char *key)
{
	if (table == NULL || key == NULL)
		return NULL;

	unsigned long index = hash_function(key);

	for (Sym_table_item *tmp = (*table)[index]; tmp != NULL; tmp = tmp->next)
	{
		if (!strcmp(key, tmp->key))
		{
			return &tmp->data;
		}
	}

	return NULL;
}


bool sym_table_remove_symbol(Sym_table *table, const char *key)
{
	if (table == NULL || key == NULL)
		return false;

	unsigned long index = hash_function(key);

	Sym_table_item *tmp_last = NULL;

	for (Sym_table_item *tmp = (*table)[index]; tmp != NULL; tmp = tmp->next)
	{
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

	for (int i = 0; i < MAX_SYMTABLE_SIZE; i++)
	{
		for (Sym_table_item *tmp = (*table)[i]; tmp != NULL; tmp = tmp_next)
		{
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
