#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "image_processing.h"


void usage(void)
{
	puts("Image filter");
	puts("Example use:");
	puts("./bin --if in.bmp --of out.bmp -t 4");
}


int main(int argc, char *argv[])
{
	struct option long_opt[] = {
		{"if",      1, 0, 'i'},
		{"of",      1, 0, 'o'},
		{"threads", 1, 0, 't'},
		{"help",    0, 0, 'h'},
		{0,         0, 0, 0  }
	};

	int num_threads = 0;
	char *ifpath, *ofpath;

	ifpath = ofpath = NULL;

	while (1) {
		int c = getopt_long(argc, argv, "i:o:t:h", long_opt, NULL);

		if (c == -1)
			break;

		//printf("option '%c' selected, optarg: %s\n", c, optarg);
		switch (c) {
		case 'i':
			ifpath = optarg;
			break;

		case 'o':
			ofpath = optarg;
			break;

		case 't':
			num_threads = atoi(optarg);
			break;

		default:
			usage();
			exit(EXIT_SUCCESS);
		}
	}

	if (!num_threads || !ifpath || !ofpath) {
		usage();
		exit(EXIT_SUCCESS);
	}

	process_image(num_threads, ifpath, ofpath);

	exit(EXIT_SUCCESS);
}
