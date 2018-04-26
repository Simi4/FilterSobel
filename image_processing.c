#include "image_processing.h"
#include "image_loader.h"
#include "common.h"
#include <math.h>
#include <pthread.h>


static float mulX(const pixel_t *pixels, const int w, const int x, const int y);
static float mulY(const pixel_t *pixels, const int w, const int x, const int y);
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
	int start_y = h * index;
	int end_y;

	if (index + 1 < thread_count)
		end_y = h * (index + 1) + 3;
	else
		end_y = image->h;

	for (int x = 1; x < w - 1; ++x) {
		for (int y = start_y + 1; y < end_y - 1; ++y) {
			float gx = mulX(data, w, x, y);
			float gy = mulY(data, w, x, y);
			int res = min(sqrt(gx * gx + gy * gy), 255);

			image->data[x + w * y].r = res;
			image->data[x + w * y].g = res;
			image->data[x + w * y].b = res;
		}
	}

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

	for (int i = 0; i < num_threads; i++) {
		ilist[i] = i;
		pthread_create(&threads[i], NULL, filter_threaded, &ilist[i]);
	}

	for (int i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);

	safe_free(ilist);
	safe_free(threads);

	write_image(ofpath, image);

	safe_free(data);
	safe_free(image->data);
	safe_free(image);
}


float mulX(const pixel_t *pixels, const int w, const int x, const int y)
{
	static int GX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
	float result = 0;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			const pixel_t *px = pixels + (x-1+i + w * (y-1+j));

			result += GX[i][j] * (px->r + px->g + px->b) / 3;
		}
	return result;
}


float mulY(const pixel_t *pixels, const int w, const int x, const int y)
{
	static int GY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };
	float result = 0;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			const pixel_t *px = pixels + (x-1+i + w * (y-1+j));

			result += GY[i][j] * (px->r + px->g + px->b) / 3;
		}
	return result;
}
