#include "common.h"
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>


void safe_begin_timer(struct timespec *start)
{
	assert(clock_gettime(CLOCK_MONOTONIC_RAW, start) == 0);
}

double safe_end_timer(struct timespec *start)
{
	struct timespec stop, duration;

	assert(clock_gettime(CLOCK_MONOTONIC_RAW, &stop) == 0);

	if ((stop.tv_nsec - start->tv_nsec) < 0) 
	{
		duration.tv_sec = stop.tv_sec - start->tv_sec - 1;
		duration.tv_nsec = 1E9 + stop.tv_nsec - start->tv_nsec;
	} 
	else
	{
		duration.tv_sec = stop.tv_sec - start->tv_sec;
		duration.tv_nsec = stop.tv_nsec - start->tv_nsec;
	}

	return duration.tv_sec + duration.tv_nsec / 1E9;
}

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
