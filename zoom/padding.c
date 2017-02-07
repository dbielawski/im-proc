#include <stdlib.h>
#include <math.h>

#include "fft.h"


void padding(int factor, pnm ims, char* imd_name)
{
	fprintf(stderr, "padding: ");

	int cols = pnm_get_width(ims);
	int rows = pnm_get_height(ims);

	int new_cols = cols * factor;
	int new_rows = rows * factor;

	unsigned short *gray =
		malloc(sizeof(unsigned short) * cols * rows);

	pnm_get_channel(ims, gray, 0);

	fftw_complex* gray_comp = forward(rows, cols, gray);
	fftw_complex* padding_comp =
		(fftw_complex*)fftw_malloc(sizeof(fftw_complex) * new_rows * new_cols);

	int mid_new_cols = new_cols / 2;
	int mid_new_rows = new_rows / 2;

	int mid_cols = cols / 2;
	int mid_rows = rows / 2;

	for (int i = 0; i < new_rows; ++i)
		for (int j = 0; j < new_cols; ++j)
		{
			if ((i >= (mid_new_cols - mid_cols)) &&
				(i < (mid_new_cols + mid_cols)) &&
				(j >= (mid_new_rows - mid_rows)) &&
				(j < (mid_new_rows + mid_rows)))
			{
				// TODO: pas beau
				padding_comp[i * new_cols + j] = gray_comp[ (i - (mid_new_rows - mid_rows)) * cols + (j - (mid_new_cols - mid_cols))];
			}
			else
				padding_comp[i * new_cols + j] = 0;
		}

	unsigned short* gray_padding = backward(new_rows, new_cols, padding_comp, factor);
	
	pnm imd = pnm_new(new_cols, new_rows, PnmRawPpm);
	pnm_set_channel(imd, gray_padding, 0);
	pnm_set_channel(imd, gray_padding, 1);
	pnm_set_channel(imd, gray_padding, 2);

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
	pnm_free(ims);
	free(gray);
	free(gray_padding);
	fftw_free(gray_comp);
	fftw_free(padding_comp);

	fprintf(stderr, "OK\n");
}

static void 
run(int factor, pnm ims, char* imd_name)
{
	padding(factor, ims, imd_name);
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