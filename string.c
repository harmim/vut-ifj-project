#include "string.h"


void strClear(string *s)
{
	s->str[0] = '\0';
	s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
	if (s1->length + 1 >= s1->allocSize)
	{
		// pamet nestaci, je potreba provest realokaci
		if ((s1->str = (char *) realloc(s1->str, (size_t) s1->length + 8)) == NULL)
		{
			return -1;
		}
		s1->allocSize = s1->length + 8;
	}
	s1->str[s1->length] = c;
	s1->length++;
	s1->str[s1->length] = '\0';
	return 1;
}

int strCmpConstStr(string *s1, char *s2)
// porovna nas retezec s konstantnim retezcem
{
	for (int i = 0; s1->str[i]; i++)
	{
		s1->str[i] = tolower(s1->str[i]);
		return strcmp(s1->str, s2);
	}
}
