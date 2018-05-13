#include "image_processing.h"
#include "image_loader.h"
#include "common.h"
#include <math.h>
#include <pthread.h>


const char GX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
const char GY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

static void *filter_threaded(void *arg);


// common for threads
static image_t *image;
static pixel_t *data;
static int thread_count;


void *filter_threaded(void *arg)
{
	const int index = *(int *)arg;
	const int w = image->w;
	const float h = image->h / (float)thread_count;
	const int start_y = index ? h * index : 1;
	int end_y;

	if (index + 1 < thread_count)
		end_y = h * (index + 1);
	else
		end_y = image->h - 1;

	//struct timespec start_timer;
	//safe_begin_timer(&start_timer);

	for (int y = start_y; y < end_y; ++y) {
		pixel_t *row = image->data + (w * y);
		for (int x = 1; x < w - 1; ++x) {
			int gx = 0, gy = 0;
			pixel_t *px;

			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j) {
					px = data + (x-1+i + w * (y-1+j));

					int d = (px->r + px->g + px->b) / 3;

					gx += GX[i][j] * d;
					gy += GY[i][j] * d;
				}

			px = row + x;
			px->r = px->g = px->b = min(sqrt(gx * gx + gy * gy), 255);
		}
	}

	//printf("T%d processing time: %lf\n", index, safe_end_timer(&start_timer));

	pthread_exit(0);
}


void process_image(int num_threads, const char *ifpath, const char *ofpath)
{
	image = open_image(ifpath);
	data = image->data;
	image->data = safe_calloc(image->data_size, 1);

	if (num_threads*3 > image->h)
		num_threads = max(image->h / 3.0, 1);

	thread_count = num_threads;

	pthread_t *threads = safe_malloc(sizeof(pthread_t) * num_threads);
	int *ilist = safe_malloc(sizeof(int) * num_threads);

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	struct timespec start_timer;
	safe_begin_timer(&start_timer);

	for (int i = 0; i < num_threads; ++i) {
		ilist[i] = i;

		assert(pthread_create(
			&threads[i], &attr, filter_threaded, &ilist[i]) == 0);
	}

	for (int i = 0; i < num_threads; ++i)
		pthread_join(threads[i], NULL);

	printf("common processing time: %lf\n", safe_end_timer(&start_timer));

	assert(pthread_attr_destroy(&attr) == 0);

	safe_free(ilist);
	safe_free(threads);

	write_image(ofpath, image);

	safe_free(data);
	safe_free(image->data);
	safe_free(image);
}
