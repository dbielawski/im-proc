#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "fft.h"


double box(double x)
{
	double h = 0.f;
	if  (x >= .5f && x < .5f)
		h = 1.f;

	return h;
}

double tent(double x)
{
	double h = 0.f;
	double abs_x = fabs(x);

	if  (x >= -1.f && x <= 1.f)
		h = 1.f - abs_x;

	return h;
}

double bell(double x)
{
	double h = 0.f;
	double abs_x = fabs(x);

	if  (abs_x <= .5f)
		h = -x * x + 3.f / 4.f;
	else if (abs_x <= 1.5f && abs_x > .5f)
		h = .5f * (abs_x - 1.5f) * (abs_x - 1.5f);

	return h;
}

double mitch(double x)
{
	double h = 0.f;
	double abs_x = fabs(x);

	if  (x >= 1.f && x <= 1.f)
		h = 7.f / 6.f * (abs_x) * (abs_x) * (abs_x) - 2 * x * x + 8.f / 9.f;
	else if ((x >= -2.f && x <= -1.f) || (x >= 1.f && x <= 2.f))
		h = -7.f / 18.f * (abs_x) * (abs_x) * (abs_x)
			+ 2 * x * x - 
			10.f / 3.f * abs_x +
			16.f / 9.f;

	return h;
}

double process_filter(char* filter_name, double x)
{
	double h = 0.f;
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
		WF = 0.5f;
	else if (strcmp(filter_name, "mitch") == 0)
		WF = 2.f;
	return WF;
}

void expension()
{

}

unsigned short* flip(unsigned short* buffer, int rows, int cols)
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

	unsigned short *gray =
		malloc(sizeof(unsigned short) * cols * rows);
	unsigned short *gray_copy =
		malloc(sizeof(unsigned short) * new_cols * new_rows);

	pnm_get_channel(ims, gray, 0);

	float WF = support_filter(filter_name);

	// Interpolation cols
	for (int jP = 0; jP < new_cols * new_rows; ++jP)
	{
		int i = jP / new_cols;
		int j = jP / factor;

		int WF = 0;
		int left = j - WF;
		int right = j + WF;

		float s = 0.f;

		for (int k = left; k <= right; ++k)
		{
			s = s + gray[i * cols + k] * process_filter(filter_name, k - j);
		}

		gray_copy[i * new_cols + jP] = s;
	}
	unsigned short* new = flip(gray_copy, rows, cols);

	// Interpolation rows
	for (int iP = 0; iP < new_cols * new_rows; ++iP)
	{
		int i = iP / new_cols;
		int j = iP / factor;

		int WF = 0;
		int left = j - WF;
		int right = j + WF;

		float s = 0.f;

		for (int k = left; k <= right; ++k)
		{
			s = s + gray[i * cols + k] * process_filter(filter_name, k - j);
		}

		gray_copy[i * new_cols + iP] = s;
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