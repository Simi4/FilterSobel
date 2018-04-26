#include "image_loader.h"
#include "common.h"


image_t *open_image(const char *path)
{
	int fd = safe_open(path, O_RDONLY);
	char *header = safe_read_str(fd);

	assert(strncmp(header, "P6", 2) == 0);
	safe_free(header);

	image_t *image = safe_malloc(sizeof(image_t));

	image->w = safe_read_int(fd);
	image->h = safe_read_int(fd);

	assert(safe_read_int(fd) == 255);

	image->data_size = image->w * image->h * sizeof(pixel_t);

	image->data = safe_malloc(image->data_size);

	safe_read(fd, image->data, image->data_size);

	return image;
}


void write_image(const char *path, image_t *image)
{
	int fd = safe_creat(path, 0700);

	char descr[20] = "";

	sprintf(descr, "P6\n%d %d\n255\n", image->w, image->h);

	safe_write(fd, descr, strlen(descr));
	safe_write(fd, image->data, image->data_size);

	safe_close(fd);
}
