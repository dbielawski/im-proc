#include <stdlib.h>
#include <math.h>

#include "fft.h"


void copy(int factor, pnm ims, char* imd_name)
{
	fprintf(stderr, "copy: ");

	int cols = pnm_get_width(ims);
	int rows = pnm_get_height(ims);

	int new_cols = cols * factor;
	int new_rows = rows * factor;

	unsigned short *gray =
		malloc(sizeof(unsigned short) * cols * rows);
	unsigned short *gray_copy =
		malloc(sizeof(unsigned short) * new_cols * new_rows);

	pnm_get_channel(ims, gray, 0);

	for (int i = 0; i < new_rows; ++i)
		for (int j = 0; j < new_cols; ++j)
			gray_copy[i * new_cols + j] = gray[(i / factor) * cols + j / factor];

	pnm imd = pnm_new(new_cols, new_rows, PnmRawPpm);
	pnm_set_channel(imd, gray_copy, 0);
	pnm_set_channel(imd, gray_copy, 1);
	pnm_set_channel(imd, gray_copy, 2);

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
	pnm_free(ims);
	free(gray);
	free(gray_copy);

	fprintf(stderr, "OK\n");
}

static void 
run(int factor, pnm ims, char* imd_name)
{
	copy(factor, ims, imd_name);
}

void 
usage (char *s)
{
	fprintf(stderr, "Usage: %s <factor> <ims> <imd>\n", s);
	exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[])
{  
	if (argc != 4)
		usage(argv[0]);

	pnm ims = pnm_load(argv[2]);
	run(atoi(argv[1]), ims, argv[3]);

	return EXIT_SUCCESS;
}