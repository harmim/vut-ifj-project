/**
 * Project: Implementace pøekladaèe imperativního jazyka IFJ17.
 *
 * @brief Expression interface.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 */


#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "analysis.h"

/**
 * @enum Symbols used for precednece analysis.
 */
typedef enum prec_table_symbol_t
{
	PLUS,			/// +
	MINUS,			/// -
	MUL,			/// *
	DIV,			/// /
	IDIV,			/// \ 
	EQ,				/// =
	NEQ,			/// <>
	LEQ,			/// <=
	LTN,			/// <
	MEQ,			/// >=
	MTN,			/// >
	LEFT_BRACKET,	/// (
	RIGHT_BRACKET,	/// )
	IDENTIFIER,		/// ID 
	INT_NUMBER,		/// int 
	DOUBLE_NUMBER,	/// double
	STRING,			/// string
	DOLLAR,			/// $
	STOP,			/// stop symbol used when reducing
	NON_TERM		/// non-terminal
} Prec_table_symbol_enum;

/**
 * Implementation of <expression> rule. Parses expressions.
 *
 * @param data Pointer to parser's internal data.
 * @return Given exit code.
 */
int expression(PData* data);

#endif //_EXPRESSION_H
