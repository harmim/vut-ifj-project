/**
 * Project: Implementace pøekladaèe imperativního jazyka IFJ17.
 *
 * @brief Error definitions.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 */


#ifndef _ERROR_H
#define _ERROR_H

// Error codes.
#define SCANNER_TOKEN_OK			0 /// Token is OK.
#define SCANNER_ERROR_LEX			1 /// Lex structure error.
#define SYNTAX_ERR					2 /// Syntax error.
#define SEM_ERR_UNDEFINED_VAR		3 /// Semantic error - undefined variable.
#define SEM_ERR_TYPE_COMPAT			4 /// Semantic error - types uncompatible.
#define SEM_ERR_OTHER				6 /// Semantic error - other.
#define ERROR_OTHER					98 /// Internal error, eg. malloc error etc.
#define SCANNER_ERROR_INTERNAL		99 /// Internal error, eg. malloc error etc.

#endif //_ERROR_H
