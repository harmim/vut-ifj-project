/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Dynamic string interface.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H


#include <stdbool.h>


#define DYNAMIC_STRING_LEN_INC 8 /// Inicialization lenght of string.


/**
 * @struct Representation of dynamic string.
 */
typedef struct
{
	char *str; /// string ened by '\0' byte
	unsigned int length; /// lenght of string
	unsigned int alloc_size; /// number of chars alocated for string
} Dynamic_string;


/**
 * Inicialization of dynamic string.
 *
 * @param s Pointer to dynamic string.
 * @return True if inicialization was successful, false otherwise.
 */
bool dynamic_string_init(Dynamic_string *s);

/**
 * Frees alocated memory for dynamic string.
 *
 * @param s Pointer to dynamic string.
 */
void dynamic_string_free(Dynamic_string *s);

/**
 * Clear dynamic string content.
 *
 * @param s Pointer to dynamic string.
 */
void dynamic_string_clear(Dynamic_string *s);

/**
 * Add char to the end of dynamic string.
 *
 * @param s Dynamic string.
 * @param c Char to add.
 * @return True if it was successful, false otherwise.
 */
bool dynamic_string_add_char(Dynamic_string *s, char c);

/**
 * Add constant string to the end of dynamic string.
 *
 * @param s Dynamic string.
 * @param const_string Constant string.
 * @return True if it was successful, false otherwise.
 */
bool dynamic_string_add_const_str(Dynamic_string *s, const char *const_string);

/**
 * Compare dynamic string with constant string.
 *
 * @param dynamic_string Dynamic string.
 * @param const_string Constant string.
 * @return Returns 1, 0, or -1, according as the s1 is greater than, equal to, or less than the s2.
 */
int dynamic_string_cmp_const_str(Dynamic_string *dynamic_string, const char *const_string);

/**
 * Copy src string to dest string.
 *
 * @param src Source string.
 * @param dest Destination string.
 * @return True if it was successful, false otherwise.
 */
bool dynamic_string_copy(Dynamic_string *src, Dynamic_string *dest);


#endif // _DYNAMIC_STRING_H
