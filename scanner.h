/**************************************************************
 * Project:   IFJ2017
 *
 * Authors:   Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 *            Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 *
 *            Header file for scanner
 **************************************************************/

#ifndef _SCANNER_H
#define _SCANNER_H

#include "string.h"


typedef union
{
	int int_val;
	double decimal_val;
	char *ptr_val;
} attribute;


typedef struct
{
	int type;
	attribute attr;
} token;


token get_next_token(FILE *source_file);

#define STATE_START 0
#define STATE_COMMENTARY 1
#define STATE_BACKSLASH 2
#define STATE_BLOCK_COMMENTARY 3
#define STATE_BLOCK_COMMENTARY_LEAVE 4
#define STATE_IDENTIFIER_OR_KEYWORD 5
#define STATE_NUMBER 6
#define STATE_NUMBER_POINT 7
#define STATE_NUMBER_DOUBLE 8
#define STATE_NUMBER_EXPONENT 9
#define STATE_NUMBER_EXPONENT_SIGN 10
#define STATE_NUMBER_EXPONENT_FINAL 11
#define STATE_STRING_START 12
#define STATE_STRING 13
#define STATE_LESS_THAN 14
#define STATE_MORE_THAN 15


#define INT_NUMBER 200
#define DOUBLE_NUMBER 201
#define STRING 202

#define NEQ 301
#define LEQ 302
#define LTN 303
#define MEQ 304
#define MTN 305

#define END_OF_FILE 100
#define SCANNER_ERROR 101

#endif //SCANNER_SCANNER_H
