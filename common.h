#pragma once
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>


void safe_begin_timer(struct timespec *start);

double safe_end_timer(struct timespec *start);

int safe_creat(const char *pathname, mode_t mode);

int safe_open(const char *pathname, int flags);

void safe_read(int fd, void *buf, size_t count);

void *safe_malloc(size_t size);

void *safe_calloc(size_t size, size_t cnt);

void safe_free(void *buf);

void safe_write(int fd, const void *buf, size_t count);

void safe_close(int fd);

char *safe_read_str(int fd);

int safe_read_int(int fd);

int min(int a, int b);

int max(int a, int b);
