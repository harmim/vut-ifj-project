/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Syntactical and semantical analysis interface.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */


#ifndef _ANALYSIS_H
#define _ANALYSIS_H

#define SYNTAX_OK 0 /// Syntax is ok.

#include "scanner.h"
#include "symtable.h"

/**
 * @struct Parser's internal data representation.
 */
typedef struct parser_internal_data
{
	Sym_table global_table;		/// Global symbol table
	Sym_table local_table;		/// Local symbol table

	Token token;				/// Token

	TData* current_id;			/// ID of currently processed function
	TData* lhs_id;				/// ID of left-hand-side variable
	TData* rhs_id;				/// ID of right-hand-side function (expression?)

	Data_type expr_ret_type;	/// Return value of expression rule evaluating
	unsigned param_index;		/// Index of currently checked param

	bool scope_processed;		/// Defines if the main scope has been processed
	bool in_function;			/// Defines if the parser is in function
	bool in_declaration;		/// Defines if param rule should add or check it's params
	bool non_declared_function;	/// Function that has been only defined
} PData;

/**
 * Starts syntactic and semantic anlysis.
 */
int analyse();

#endif //_ANALYSIS_H
