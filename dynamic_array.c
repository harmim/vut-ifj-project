/**
 * Project: Implementace pøekladaèe imperativního jazyka IFJ17.
 *
 * @brief Dynamic array implementation.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */

#include "dynamic_array.h"
#include <stdlib.h>

#define DEFAULT_VECTOR_SIZE 1024

bool vector_realloc(Vector* vec)
{
	vec->alloc_size *= 2;
	Token* arr = realloc(vec->arr, vec->alloc_size);
	if (!arr)
		return false;

	free(vec->arr);
	vec->arr = arr;

	return true;
}

bool vector_init(Vector* vec, unsigned size)
{
	if ((vec->arr = (Token*)malloc(sizeof(Token) * size)))
		return false;

	vec->alloc_size = size;
	vec->length = 0;

	return true;
}

bool vector_push_back(Vector* vec, Token* token)
{
	if (vec->length + 1 > vec->alloc_size && !vector_realloc(vec))
		return false;

	vec->arr[vec->length].type = token->type;
	switch (token->type)
	{
	case TOKEN_TYPE_KEYWORD:
		vec->arr[vec->length].attribute.keyword = token->attribute.keyword;
		break;

	case TOKEN_TYPE_IDENTIFIER:
	case TOKEN_TYPE_STRING:
		if ((vec->arr[vec->length].attribute.string = (Dynamic_string*)malloc(sizeof(Dynamic_string))) == NULL)
			return false;
		if (!dynamic_string_init(vec->arr[vec->length].attribute.string))
		{
			free(vec->arr[vec->length].attribute.string);
			return false;
		}
		if (!dynamic_string_copy(token->attribute.string, vec->arr[vec->length].attribute.string))
		{
			dynamic_string_free(vec->arr[vec->length].attribute.string);
			free(vec->arr[vec->length].attribute.string);
			return false;
		}
		break;

	case TOKEN_TYPE_DOUBLE_NUMBER:
		vec->arr[vec->length].attribute.decimal = token->attribute.decimal;
		break;

	case TOKEN_TYPE_INT_NUMBER:
		vec->arr[vec->length].attribute.integer = token->attribute.integer;
		break;

	default:
		break;
	};

	vec->length++;

	return true;
}

bool vector_pop_back(Vector* vec)
{
	if (vec->length)
	{
		if (vec->arr[vec->length].type == TOKEN_TYPE_IDENTIFIER || vec->arr[vec->length].type == TOKEN_TYPE_STRING)
		{
			dynamic_string_free(vec->arr[vec->length].attribute.string);
			free(vec->arr[vec->length].attribute.string);
		}

		free(vec->arr);

		vec->length--;

		return true;
	}

	return false;
}

Token* vector_back(Vector* vec)
{
	return (vec->length) ? &vec->arr[vec->length - 1] : NULL;
}

void vector_free(Vector* vec)
{
	while (vector_pop_back(vec));
}
