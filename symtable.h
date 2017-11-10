/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Symbol table interface.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */


#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include "dynamic_string.h"

#define MAX_SYMTABLE_SIZE 2048 /// Symbol table size

// sybol defines
#define NONEXISTING_SYMBOL -1 /// Non existing symbol
#define NOT_FUNCTION -2 /// Symbol is not function

// data types
#define TYPE_UNDEFINED 0 /// Integer data type
#define TYPE_INT 1 /// Integer data type
#define TYPE_DOUBLE 2 /// Double data type
#define TYPE_STRING 3 /// String data type

/**
 * @struct Item data representation.
 */
typedef struct htab_listitem_data {
	int type; /// Data type of symbol / return type of function
	Dynamic_string *params; /// parameters in string form
} TData;

/**
 * @struct Symbol table item representation.
 */
typedef struct htab_listitem {
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
 * @return Returns NULL if error or item is existing else returns pointer to added item.
 */
TData *sym_table_add_symbol(Sym_table *table, const char *key);

/**
 * Function appends parameter to symbol.
 *
 * @param token Pointer to output token.
 * @param data_type Data type of parameter of function.
 * @return Returns true if adding was succesfull else returns false.
 */
bool sym_table_add_param(TData *data, int data_type);

/**
 * Function returns number of parameters of function.
 *
 * @param table Pointer to table.
 * @param key Identifier of function or variable.
 * @return (NONEXISTING_SYMBOL) if symbol is not existing or (NOT_FUNCTION) if symbol is not function else returns number of parameters of function.
 */
int sym_table_number_of_params(Sym_table *table, const char *key);

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
