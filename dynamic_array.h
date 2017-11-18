/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Dynamic array interface
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */


#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H

#include <stdbool.h>
#include "scanner.h"

/**
 * @struct Representation of dynamic array.
 */
typedef struct vector
{
	Token* arr; /// dynamic array of vectors

	unsigned int length; /// lenght of array
	unsigned int alloc_size; /// allocated size of array
} Vector;

/**
 * Inicialization of dynamic array.
 *
 * @param vec Pointer to dynamic array.
 * @param size Size of array to allocate
 * @return True if inicialization was successful, false otherwise.
 */
bool vector_init(Vector* vec, unsigned size);

/**
 * Add Token to the end of array
 *
 * @param vec Pointer to dynamic array.
 * @param token Token to be pushed onto array
 * @return True if push was successful, false otherwise
 */
bool vector_push_back(Vector* vec, Token* token);

/**
 * Remove Token from the end of array
 *
 * @param vec Pointer to dynamic array.
 * @return True if pop was successful, false otherwise
 */
bool vector_pop_back(Vector* vec);

/**
 * Return last token
 *
 * @param vec Pointer to dynamic array.
 * @return Last token
 */
Token* vector_back(Vector* vec);

/**
 * Free vector array.
 *
 * @param vec Pointer to dynamic array.
 */
void vector_free(Vector* vec);

#endif //_DYNAMIC_ARRAY_H
