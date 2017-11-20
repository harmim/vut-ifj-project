/**
 * Project: Implementace překladače imperativního jazyka IFJ17.
 *
 * @brief Main file of compiler.
 *
 * @author Timotej Halás <xhalas10@stud.fit.vutbr.cz>
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Matej Karas <xkaras34@stud.fit.vutbr.cz>
 */


#include <stdio.h>
#include <stdlib.h>

#include "analysis.h"
#include "code_generator.h"


// TODO: potom odstranit
#define DEBUG 1


/**
 * Main function.
 *
 * @return EXIT_SUCCESS (0), if compilation was seccessful, appropriate error code otherwise.
 */
int main()
{
	FILE* source_file;

#ifdef DEBUG
	if (!(source_file = fopen("test.bas", "r")))
	{
		source_file = stdin;
	}
#else
	source_file = stdin;
#endif

	set_source_file(source_file);

	int error_code;
	if ((error_code = analyse()))
	{
		code_generator_clear();
		return error_code;
	}

	code_generator_flush(stdout);

	return EXIT_SUCCESS;
}
