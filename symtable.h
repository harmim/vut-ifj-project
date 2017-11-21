/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Symbol table interface.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */


#ifndef _SYMTABLE_H
#define _SYMTABLE_H


#include <stdbool.h>

#include "dynamic_string.h"


#define MAX_SYMTABLE_SIZE 27457 /// Symbol table size. (prime number) Try keep the load factor at 75% or less.


/**
 * @enum Data types.
 */
typedef enum
{
	TYPE_UNDEFINED,	/// Data type undefined
	TYPE_INT,		/// Integer data type
	TYPE_DOUBLE,	/// Double data type
	TYPE_STRING,	/// String data type
	TYPE_BOOL,		/// Bool data type (actaully it's kinda imaginary)
} Data_type;

/**
 * @struct Item data representation.
 */
typedef struct
{
	Data_type type;			/// Data type of symbol / return type of function
	bool defined;			/// Defined if current function was defined
	Dynamic_string *params; /// parameters in string form
	char *identifier;		/// Data identifier (key).
	bool global;			/// Global (internal) variable.
} TData;

/**
 * @struct Symbol table item representation.
 */
typedef struct htab_listitem
{
	char *key; /// identifier
	TData data; /// data
	struct htab_listitem *next; /// ptr to next item
} Sym_table_item;

// Symbol table
typedef Sym_table_item* Sym_table[MAX_SYMTABLE_SIZE];


/**
 * Initialisation of symbol table.
 *
 * @param table Pointer to table.
 */
void sym_table_init(Sym_table *table);

/**
 * Appends item to symbol table of.
 *
 * @param table Pointer to table.
 * @param key Identifier of function or variable.
 * @param alloc_success True if allocation failed, otherwise true. 
 * @return Returns NULL if error or item is existing else returns pointer to added item.
 */
TData *sym_table_add_symbol(Sym_table *table, const char *key, bool* alloc_failed);

/**
 * Function appends parameter to symbol.
 *
 * @param token Pointer to output token.
 * @param data_type Data type of parameter of function.
 * @return Returns true if adding was succesfull else returns false.
 */
bool sym_table_add_param(TData *data, int data_type);

/**
 * Function finds symbol and renturns its data.
 *
 * @param table Pointer to table.
 * @param key Identifier of function or variable.
 * @return NULL if symbol is not existing or pointer to data of symbol if it was successfuly found.
 */
TData *sym_table_search(Sym_table *table, const char *key);

/**
 * Function removes symbol.
 *
 * @param table Pointer to table.
 * @param key Identifier of function or variable.
 * @return Returns true if removal was successfull esle returns false.
 */
bool sym_table_remove_symbol(Sym_table *table, const char *key);

/**
 * Function frees all used resources for symbol table.
 *
 * @param table Pointer to table.
 */
void sym_table_free(Sym_table *table);


#endif //_SYMTABLE_H
