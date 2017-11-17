/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Main file of compiler.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "scanner.h"
#include "analysis.h"
#include "symtable.h"

/**
 * Main function.
 *
 * @return EXIT_SUCCESS (0), if compilation was seccessful, appropriate error code otherwise.
 */
int main(void)
{
	FILE* file;
	if (!(file = fopen("test.bas", "r")))
		return -1;
	
	set_source_file(file);

	return analyse(); // compilation was successful
}
