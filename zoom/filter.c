#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "fft.h"


float box(float x)
{
	float h = 0.f;
	if  (x >= -.5f && x < .5f)
		h = 1.f;

	return h;
}

float tent(float x)
{
	float h = 0.f;
	float abs_x = fabs(x);

	if  (x >= -1.f && x <= 1.f)
		h = 1.f - abs_x;

	return h;
}

float bell(float x)
{
	float h = 0.f;
	float abs_x = fabs(x);

	if  (abs_x <= .5f)
		h = -x * x + 3.f / 4.f;
	else if (abs_x <= 1.5f && abs_x > .5f)
		h = .5f * (abs_x - 1.5f) * (abs_x - 1.5f);

	return h;
}

// TODO: fix mitch
float mitch(float x)
{
	float h = 0.f;
	float abs_x = fabs(x);

	if  (x >= 1.f && x <= 1.f)
		h = 7.f / 6.f * (abs_x) * (abs_x) * (abs_x) - 2.f * x * x + 8.f / 9.f;
	else if ((x >= -2.f && x <= -1.f) || (x >= 1.f && x <= 2.f))
		h = -7.f / 18.f * (abs_x) * (abs_x) * (abs_x)
			+ 2.f * x * x - 
			10.f / 3.f * abs_x +
			16.f / 9.f;

	return h;
}

float process_filter(char* filter_name, float x)
{
	float h = 0.f;
	if (strcmp(filter_name, "box") == 0)
		h = box(x);
	else if (strcmp(filter_name, "tent") == 0)
		h = tent(x);
	else if (strcmp(filter_name, "bell") == 0)
		h = bell(x);
	else if (strcmp(filter_name, "mitch") == 0)
		h = mitch(x);
	return h;
}

float support_filter(char* filter_name)
{
	float WF = 0.f;

	if (strcmp(filter_name, "box") == 0)
		WF = 0.5f;
	else if (strcmp(filter_name, "tent") == 0)
		WF = 1.0f;
	else if (strcmp(filter_name, "bell") == 0)
		WF = 1.5f;
	else if (strcmp(filter_name, "mitch") == 0)
		WF = 2.f;
	return WF;
}

void expension(unsigned short* in, int cols_in, int rows_in,
	unsigned short* out, int cols_out, int rows_out, char* filter_name, int factor)
{
	for (int i = 0; i < rows_in; ++i)
		for (int jP = 0; jP < cols_out; ++jP)
		{
			float j = jP / (factor * 1.f);
			float WF = support_filter(filter_name);
			int left = j - WF;
			int right = j + WF;

			float s = 0.f;

			for (int k = left; k <= right; ++k)
			{
				float h = process_filter(filter_name, k - j);
				int col = k;

		        if (col < 0)
		        	col = 0;
		        else if (k >= cols_in)
		        	col = cols_in - 1;

	        	s += in[i * cols_in + col] * h;
			}
			out[i * cols_out + jP] = (unsigned short)s;
		}
}

unsigned short* flip(unsigned short* buffer, int cols, int rows)
{
	int size = rows * cols;
	unsigned short* flipped_buffer = malloc(size * sizeof(unsigned short));

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			flipped_buffer[i * cols + j] = buffer[j * rows + i];

	return flipped_buffer;
}

void filter(int factor, char* filter_name, pnm ims, char* imd_name)
{
	fprintf(stderr, "filter: ");

	int cols = pnm_get_width(ims);
	int rows = pnm_get_height(ims);

	int new_cols = cols * factor;
	int new_rows = rows * factor;	

	unsigned short* gray = pnm_get_channel(ims, NULL, 0);
	unsigned short* gray_copy_w = malloc(sizeof(unsigned) * new_cols * rows);
	unsigned short* gray_copy = malloc(sizeof(unsigned) * new_cols * new_rows);

	// expension(gray, cols, rows,	gray_copy_w, new_cols, rows, filter_name, factor);
	// flip(gray_copy_w, new_cols, rows);
	// expension(gray_copy_w, new_cols, rows,	gray_copy, new_cols, new_rows, filter_name, factor);
	// flip(gray_copy, new_cols, new_rows);

	float WF = support_filter(filter_name);

	for (int i = 0; i < rows; ++i)
		for (int jP = 0; jP < new_cols; ++jP)
		{
			float j = jP / (factor * 1.f);
			int left = j - WF;
			int right = j + WF;

			float s = 0.f;

			for (int k = left; k <= right; ++k)
			{
				float h = process_filter(filter_name, k - j);
				int col = k;

		        if (col < 0)
		        	col = 0;
		        else if (k >= cols)
		        	col = cols - 1;

	        	s += gray[i * cols + col] * h;
			}
			gray_copy_w[i * new_cols + jP] = (unsigned short)s;
		}

	for (int iP = 0; iP < new_rows; ++iP)
		for (int j = 0; j < new_cols; ++j)
		{
			float i = iP / (factor * 1.f);
			int above = i - WF;
			int below = i + WF;

			float s = 0.f;

			for (int k = above; k <= below; ++k)
			{
				float h = process_filter(filter_name, k - i);
				int row = k;

				if (k < 0)
					row = 0;
		        else if (k >= rows)
		        	row = rows - 1;

	        	s += gray_copy_w[row * new_cols + j] * h;
			}
			gray_copy[iP * new_cols + j] = (unsigned short)s;
		}

	pnm imd = pnm_new(new_cols, new_rows, PnmRawPpm);
	pnm_set_channel(imd, gray_copy, 0);
	pnm_set_channel(imd, gray_copy, 1);
	pnm_set_channel(imd, gray_copy, 2);

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
	pnm_free(ims);
	free(gray);
	free(gray_copy);
	free(gray_copy_w);

	fprintf(stderr, "OK\n");
}

static void 
run(int factor, char* filter_name, pnm ims, char* imd_name)
{
	filter(factor, filter_name, ims, imd_name);
}

void 
usage (char *s)
{
	fprintf(stderr, "Usage: %s <factor> <filter-name> <ims> <imd>\n", s);
	exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[])
{  
	if (argc != 5)
		usage(argv[0]);

	pnm ims = pnm_load(argv[3]);
	run(atoi(argv[1]), argv[2], ims, argv[4]);

	return EXIT_SUCCESS;
}