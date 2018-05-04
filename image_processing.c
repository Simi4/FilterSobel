#include "image_processing.h"
#include "image_loader.h"
#include "common.h"
#include <math.h>
#include <pthread.h>
#include <time.h>


static int GX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
static int GY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

static int mulX(int x, int y);
static int mulY(int x, int y);
static void *filter_threaded(void *arg);


// common for threads
static image_t *image;
static pixel_t *data;
static int thread_count;


void *filter_threaded(void *arg)
{
	int index = *(int *)arg;
	int w = image->w;
	float h = image->h / (float)thread_count;
	int start_y = index ? h * index : 1;
	int end_y;

	if (index + 1 < thread_count)
		end_y = h * (index + 1);
	else
		end_y = image->h - 1;

	clock_t start_clock = clock();

	for (int x = 1; x < w - 1; ++x) {
		for (int y = start_y; y < end_y; ++y) {
			int gx = mulX(x, y);
			int gy = mulY(x, y);
			int res = min(sqrt(gx * gx + gy * gy), 255);

			image->data[x + w * y].r = res;
			image->data[x + w * y].g = res;
			image->data[x + w * y].b = res;
		}
	}

	clock_t end_clock = clock() - start_clock;

	printf("T%d processing time: %f\n",
		index, end_clock / (float)CLOCKS_PER_SEC);

	pthread_exit(0);
}


void process_image(int num_threads, const char *ifpath, const char *ofpath)
{
	image = open_image(ifpath);
	data = image->data;
	image->data = safe_calloc(image->data_size, 1);

	if (num_threads*3 > image->h)
		num_threads = max(image->h / 3, 1);

	thread_count = num_threads;

	pthread_t *threads = safe_calloc(sizeof(pthread_t), num_threads);
	int *ilist = safe_malloc(sizeof(int) * num_threads);

	clock_t start_clock = clock();

	for (int i = 0; i < num_threads; i++) {
		ilist[i] = i;
		assert(pthread_create(
			&threads[i], NULL, filter_threaded, &ilist[i]) == 0);
	}

	for (int i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);

	clock_t end_clock = clock() - start_clock;

	printf("common processing time: %f\n",
		end_clock / (float)CLOCKS_PER_SEC);

	safe_free(ilist);
	safe_free(threads);

	write_image(ofpath, image);

	safe_free(data);
	safe_free(image->data);
	safe_free(image);
}


int mulX(int x, int y)
{
	int result = 0;
	int w = image->w;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			pixel_t *px = data + (x-1+i + w * (y-1+j));

			result += GX[i][j] * (px->r + px->g + px->b) / 3;
		}
	return result;
}


int mulY(int x, int y)
{
	int result = 0;
	int w = image->w;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			pixel_t *px = data + (x-1+i + w * (y-1+j));

			result += GY[i][j] * (px->r + px->g + px->b) / 3;
		}
	return result;
}
