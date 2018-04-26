#pragma once
#include <stdint.h>


struct __attribute__((__packed__)) pixel_s {
	uint8_t r, g, b;
};
#define pixel_t struct pixel_s


struct image_s {
	uint32_t w, h, data_size;
	pixel_t *data;
};
#define image_t struct image_s


image_t *open_image(const char *path);
void write_image(const char *path, image_t *image);
