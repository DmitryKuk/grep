#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OPT_STRING "Rc1"
// -R  Работать рекурсивно
// -c  Включить цветную печать
// -1  Короткая форма: не печетает более AROUND_MAX (см. grep.h) символов префикса и постфикса

struct arguments {
	const char *pname;
	
	char recursive;
	char colors;
	
	char short_form;
};

int arguments_init(int *argc_p, char ***argv_p, struct arguments *args);

#endif	// ARGUMENTS_H