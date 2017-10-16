/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Dynamic string.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include <stdlib.h>
#include <string.h>

#include "dynamic_string.h"


void dynamic_string_clear(struct dynamic_string *s)
{
	s->length = 0;
	s->str[s->length] = '\0';
}


bool dynamic_string_init(struct dynamic_string *s)
{
	if (!(s->str = (char *) malloc(STR_LEN_INC)))
	{
		return false;
	}

	dynamic_string_clear(s);
	s->alloc_size = STR_LEN_INC;

	return true;
}


void dynamic_string_free(struct dynamic_string *s)
{
	free(s->str);
}


bool dynamic_string_add_char(struct dynamic_string *s, char c)
{
	if (s->length + 1 >= s->alloc_size)
	{
		unsigned int new_size = s->length + STR_LEN_INC;
		if (!(s->str = (char *) realloc(s->str, new_size)))
		{
			return false;
		}
		s->alloc_size = new_size;
	}

	s->str[s->length++] = c;
	s->str[s->length] = '\0';

	return true;
}


int dynamic_string_cmp_const_str(struct dynamic_string *dynamic_string, char *const_string)
{
	return strcmp(dynamic_string->str, const_string);
}


bool dynamic_string_copy(struct dynamic_string *src, struct dynamic_string *dest)
{
	unsigned int newLength = src->length;
	if (newLength >= dest->alloc_size)
	{
		if (!(dest->str = (char *) realloc(dest->str, newLength + 1)))
		{
			return false;
		}
		dest->alloc_size = newLength + 1;
	}

	strcpy(dest->str, src->str);
	dest->length = newLength;

	return true;
}
