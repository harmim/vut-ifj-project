/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Main file of compiler.
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 */

#include <stdio.h>

#include "analysis.h"
#include "scanner.h"

/**
 * Main function.
 *
 * @return EXIT_SUCCESS (0), if compilation was seccessful, appropriate error code otherwise.
 */
int main(void)
{
	int result;
	FILE* file;
	if (!(file = fopen("test.bas", "r")))
		return -1;
	
	set_source_file(file);
	result = analyse(); // compilation was successful	
	fclose(file);

	return result;
}
