#include "arguments.h"

int arguments_init(int *argc_p, char ***argv_p, struct arguments *args)
{
	memset(args, 0, sizeof(struct arguments));
	args->pname = **argv_p;
	if (isatty(STDOUT_FILENO)) args->colors = 1;
	
	int ch;
	while ((ch = getopt(*argc_p, *argv_p, OPT_STRING)) != -1)
		switch (ch) {
		case 'R':
			args->recursive = 1;
			break;
		case 'c':
			args->colors = 1;
			break;
		case '1':
			args->short_form = 1;
			break;
		case '?': default:
			fprintf(stderr, "Invalid argument!\n");
			return 1;
		}
	*argc_p -= optind;
	*argv_p += optind;
	return 0;
}