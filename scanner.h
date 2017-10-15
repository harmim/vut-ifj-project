/**************************************************************
 * Project:   IFJ2017
 *
 * Authors:   Vojtěch Hertl <xhertl04@stud.fit.vutbr.cz>
 *            Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 *            Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 *            Matej Karas <xkaras34@stud.fit.vutbr.cz>
 *
 *            Header file for scanner
 **************************************************************/

#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H

#endif //SCANNER_SCANNER_H
//#include "string.h"

typedef struct
{
    char* str;		// misto pro dany retezec ukonceny znakem '\0'
    int length;		// skutecna delka retezce
    int allocSize;	// velikost alokovane pameti
} string;

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCmpConstStr(string *s1, char* s2);
int keyword_check(string *id);
void setSourceFile(FILE *f);
int getNextToken(string *attr);

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
