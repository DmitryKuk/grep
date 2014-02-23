#ifndef GREP_H
#define GREP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "arguments.h"

#define ALLOCATION_ERROR "Allocation error"

#define CANT_GET_STAT_	"Can't get stat: "
#define CANT_OPEN_DIR_	"Can't open directory: "
#define CANT_OPEN_FILE_	"Can't open file: "
#define CANT_READ_FILE_	"Can't read from file: "
#define CANT_PROC_DIR_	"Can't process directory (without '-R'): "


#define OK			0

#define ERROR_STAT	1
#define ERROR_OPEN	2
#define ERROR_READ	3
#define ERROR_ALLOC	4
#define ERROR_PROC	5

#define AROUND_MAX 40

int grep(const struct arguments *args, const char *text, const char *path);

#endif	// GREP_H