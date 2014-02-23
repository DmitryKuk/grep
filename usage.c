#include "usage.h"

void usage(const char *pname)
{
	fprintf(stderr, "Usage: %s [-%s] TEXT [FILE1 ...]\n", pname, OPT_STRING);
}