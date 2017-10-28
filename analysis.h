/**
* Project: Implementace překladače imperativního jazyka IFJ17.
*
* @brief Syntactical and semantical analysis
* @author Matej karas <xkaras34@stud.fit.vutbr.cz>
*/


#ifndef _ANALYSIS_H
#define _ANALYSIS_H

#define SYNTAX_OK					0
#define SYNTAX_ERR					2
#define SEM_ERR_UNDEFINED_VAR		3
#define SEM_ERR_TYPE_COMPAT			4
#define SEM_ERR_OTHER				6

// function prototypes

int prog();
int func_head();
int params();
int param();
int param_n();
int statement();
int def_var();
int arg();
int arg_n();
int print();


#endif