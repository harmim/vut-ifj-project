/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Expression interface.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#ifndef _EXPRESSION_H
#define _EXPRESSION_H


#include "analysis.h"


/**
 * @enum Rules used for parsing.
 */
typedef enum
{
	NT_EQ_NT,		/// E -> E = E
	NT_NEQ_NT,		/// E -> E <> E
	NT_LEQ_NT,		/// E -> E <= E
	NT_LTN_NT,		/// E -> E < E
	NT_MEQ_NT,		/// E -> E => E
	NT_MTN_NT,		/// E -> E > E
	NT_PLUS_NT,		/// E -> E + E
	NT_MINUS_NT,	/// E -> E - E
	NT_IDIV_NT,		/// E -> E \ E
	NT_MUL_NT,		/// E -> E * E
	NT_DIV_NT,		/// E -> E / E
	LBR_NT_RBR,		/// E -> (E)
	OPERAND,		/// E -> i
	NOT_A_RULE		/// rule doesn't exist
} Prec_rules_enum;

/**
 * @enum Symbols used for precednece analysis.
 */
typedef enum
{
	PLUS,			/// +
	MINUS,			/// -
	MUL,			/// *
	DIV,			/// /
	IDIV,			/// \ /
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
