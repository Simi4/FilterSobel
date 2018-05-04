#include "common.h"
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>


int safe_creat(const char *pathname, mode_t mode)
{
	int fd = creat(pathname, mode);

	assert(fd != -1);
	return fd;
}

int safe_open(const char *pathname, int flags)
{
	int fd = open(pathname, flags);

	assert(fd != -1);
	return fd;
}

void safe_read(int fd, void *buf, size_t count)
{
	assert(read(fd, buf, count) == count);
}

void *safe_calloc(size_t size, size_t cnt)
{
	void *buf = calloc(size, cnt);

	assert(buf != NULL);
	return buf;
}

void *safe_malloc(size_t size)
{
	void *buf = malloc(size);

	assert(buf != NULL);
	return buf;
}

void safe_free(void *buf)
{
	assert(buf != NULL);
	free(buf);
}

void safe_write(int fd, const void *buf, size_t count)
{
	assert(write(fd, buf, count) == count);
}

void safe_close(int fd)
{
	assert(close(fd) == 0);
}

char *safe_read_str(int fd)
{
	int pos = 0;
	char *s = safe_calloc(10, 1);
	char c;

	while (1) {
		safe_read(fd, &c, 1);

		if (isspace(c) != 0)
			break;

		s[pos] = c;

		pos++;
	}

	return s;
}

int safe_read_int(int fd)
{
	char *s = safe_read_str(fd);
	int res = atoi(s);

	safe_free(s);
	return res;
}

int min(int a, int b)
{
	return a < b ? a : b;
}

int max(int a, int b)
{
	return a > b ? a : b;
}
