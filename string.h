#ifndef _STRING_H
#define _STRING_H


typedef struct
{
	char *str;        // misto pro dany retezec ukonceny znakem '\0'
	int length;        // skutecna delka retezce
	int allocSize;    // velikost alokovane pameti
} string;


void str_clear(string *s);

int str_add_char(string *s1, char c);

int str_cmp_const_str(string *s1, char *s2);

#endif
