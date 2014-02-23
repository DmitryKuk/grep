#include <stdio.h>
#include <unistd.h>

#include "grep.h"
#include "arguments.h"
#include "usage.h"

int main(int argc, char **argv)
{
	struct arguments args;
	if (arguments_init(&argc, &argv, &args)) {
		usage(args.pname);
		return 1;
	}
	
	if (argc < 2) {
		usage(args.pname);
		return 1;
	}
	
	const char *text = *(argv++);
	
	while (--argc)
		grep(&args, text, *(argv++));
	return 0;
}