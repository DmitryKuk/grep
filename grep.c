#include "grep.h"

static char equal(const char *buf, size_t buf_pos, const char *src_buf, size_t size)
{
	size_t i;
	for (i = 0; buf_pos < size; ++i, ++buf_pos)
		if (buf[buf_pos] != src_buf[i]) return 0;
	for (buf_pos = 0; i < size; ++i, ++buf_pos)
		if (buf[buf_pos] != src_buf[i]) return 0;
	
	// Это менее сложно, но более гибко (общий случай для любого закольцованного буфера).
	// Плохое решение в данном случае, т.к. медленнее.
	//for (i = 0; i < size; ++i, ++buf_pos) {
	//	if (buf_pos == size) buf_pos = 0;
	//	if (buf[buf_pos] != src_buf[i]) return 0;
	//}
	return 1;
}


static int grep_file(const struct arguments *args, const char *text, size_t text_len, const char *path)
{
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		fprintf(stderr, "%s%s", CANT_OPEN_FILE_, path);
		perror(NULL);
		return ERROR_OPEN;
	}
	
	char *data = malloc(text_len);
	if (data == NULL) {
		perror(ALLOCATION_ERROR);
		fclose(file);
		return ERROR_ALLOC;
	}
	
	if (fread(data, sizeof(char), text_len * sizeof(char), file) != text_len && errno != 0) {
		fprintf(stderr, "%s%s: ", CANT_READ_FILE_, path);
		perror(NULL);
		free(data);
		fclose(file);
		return ERROR_READ;
	}
	
	size_t data_pos = 0;
	off_t curr_pos = 0, line_beg_pos = 0;	// Для whence = SEEK_SET (смещение от начала файла)
	
	do {
		if (equal(data, data_pos, text, text_len)) {	// Текст найден
			// Печать имени файла
			printf("%s: ", path);
			
			
			// Печать префикса
			{
				off_t prefix_beg_pos = line_beg_pos;
				if (args->short_form && curr_pos > AROUND_MAX + prefix_beg_pos)
					prefix_beg_pos = curr_pos - AROUND_MAX;
				fseeko(file, prefix_beg_pos, SEEK_SET);
				
				while (prefix_beg_pos++ < curr_pos) {
					int ch = getc(file);
					putchar(ch);
				}
			}
			
			
			// Печать найденного текста с цветом
			if (args->colors)
				printf("\033[32m");	// Зелёный цвет
			
			{
				size_t i = data_pos;
				for (i = data_pos; i < text_len; ++i)
					putchar(data[i]);
				for (i = 0; i < data_pos; ++i)
					putchar(data[i]);
			}
			
			if (args->colors)
				printf("\033[m");	// Сброс цвета
			
			
			// Печать постфикса
			{
				off_t postfix_beg_pos = curr_pos + text_len;
				fseeko(file, postfix_beg_pos, SEEK_SET);
				int ch;
				if (args->short_form) {
					off_t postfix_end_pos = postfix_beg_pos + AROUND_MAX;
					while (!feof(file) && (ch = getc(file)) != '\n' && ch != '\v'
							&& postfix_beg_pos++ < postfix_end_pos)
						putchar(ch);
				} else {
					while (!feof(file) && (ch = getc(file)) != '\n' && ch != '\v')
						putchar(ch);
				}
			}
			
			putchar('\n');
			fseeko(file, curr_pos + text_len, SEEK_SET);
		}
		
		++curr_pos;
		if (data[data_pos] == '\n' || data[data_pos] == '\v')
			line_beg_pos = curr_pos;
		data[data_pos] = getc(file);
		if (++data_pos == text_len) data_pos = 0;
	} while (!feof(file));
	
	free(data);
	fclose(file);
	return OK;
}


static int grep_dispather(const struct arguments *args, const char *text, size_t text_len, const char *path);


static int grep_dir(const struct arguments *args, const char *text, size_t text_len, const char *path)
{
	DIR *dir = opendir(path);
	if (dir == NULL) {
		fprintf(stderr, "%s%s: ", CANT_OPEN_DIR_, path);
		perror(NULL);
		return ERROR_OPEN;
	}
	
	size_t new_path_len = strlen(path), new_path_cap = text_len + ((text[text_len - 1] == '/')? 0: 1) + 2;
	char *new_path = malloc(new_path_cap + 1);
	if (new_path == NULL) {
		perror(ALLOCATION_ERROR);
		closedir(dir);
		return ERROR_ALLOC;
	}
	strcpy(new_path, path);
	if (new_path[new_path_len - 1] != '/') {
		new_path[new_path_len] = '/';
		new_path[++new_path_len] = '\0';
	}
	
	// Получение inode родителя и себя
	ino_t parent_ino, self_ino;
	{
		struct stat st1, st2;
		strcpy(new_path + new_path_len, ".");
		stat(new_path, &st1);
		strcpy(new_path + new_path_len + 1, ".");
		stat(new_path, &st2);
		self_ino = st1.st_ino;
		parent_ino = st2.st_ino;
	}
	
	struct dirent *dp;
	while ((dp = readdir(dir)) != NULL) {
		// Пропуск ссылок на родителя и себя
		if (dp->d_ino == parent_ino || dp->d_ino == self_ino)
			continue;
		
		// Допись имени файла
		size_t name_len = strlen(dp->d_name);
		
		if (new_path_len + name_len > new_path_cap) {
			new_path_cap = new_path_len + name_len;
			char *tmp = realloc(new_path, new_path_cap + 1);
			if (tmp == NULL) {
				perror(ALLOCATION_ERROR);
				free(new_path);
				closedir(dir);
				return ERROR_ALLOC;
			}
			
			new_path = tmp;
		}
		
		strcpy(new_path + new_path_len, dp->d_name);
		
		grep_dispather(args, text, text_len, new_path);
	}
	
	free(new_path);
	closedir(dir);
	return OK;
}


static int grep_dispather(const struct arguments *args, const char *text, size_t text_len, const char *path)
{
	struct stat stat_buf;
	if (stat(path, &stat_buf)) {
		fprintf(stderr, "%s%s: ", CANT_GET_STAT_, path);
		perror(NULL);
		return ERROR_STAT;
	}
	
	if (stat_buf.st_mode & S_IFDIR) {
		if (args->recursive)
			return grep_dir(args, text, text_len, path);
		else {
			fprintf(stderr, "%s%s\n", CANT_PROC_DIR_, path);
			return ERROR_PROC;
		}
	} else
		return grep_file(args, text, text_len, path);
}


int grep(const struct arguments *args, const char *text, const char *path)
{
	return grep_dispather(args, text, strlen(text), path);
}