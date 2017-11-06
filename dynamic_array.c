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
}

bool vector_init(Vector* vec)
{
	vector_init(vec, DEFAULT_VECTOR_SIZE);
}

bool vector_init(Vector* vec, unsigned size)
{
	if ((vec->arr = (Token*)malloc(sizeof(Token) * size)))
		return false;

	vec->alloc_size = size;
	vec->length = 0;
}

bool vector_push_back(Vector* vec, Token* token)
{
	if (vec->length + 1 > vec->alloc_size && !vector_realloc(vec))
		return false;

	vec->arr[vec->length++] = *token;
}

void vector_pop_back(Vector* vec)
{
	if (vec->length)
		vec->length--;
}

Token* vector_back(Vector* vec)
{
	return (vec->length) ? &vec->arr[vec->length - 1] : NULL;
}


