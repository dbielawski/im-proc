/**
 * @file 
 * @author 
 * @brief ???
 */

#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

int normalize(int min, int max, int min_image, int max_image, int value);

void process(int min, int max, char* ims_name, char* imd_name)
{
	pnm ims = pnm_load(ims_name);
	unsigned short *image = pnm_get_image(ims);

	int rows = pnm_get_height(ims);
	int cols = pnm_get_width(ims);

	pnm imd = pnm_new(cols, rows, PnmRawPpm);

	int min_img = 0;
	int max_img = pnm_maxval;

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
		{
			int r = 0;
			int v = 0;
			int b = 0;

			r = image[i * cols * 3 + j * 3 + 0];
			v = image[i * cols * 3 + j * 3 + 1];
			b = image[i * cols * 3 + j * 3 + 2];

			pnm_set_component(imd, i, j, 0,
				normalize(min, max, min_img, max_img, r));
			pnm_set_component(imd, i, j, 1,
				normalize(min, max, min_img, max_img, v));
			pnm_set_component(imd, i, j, 2,
				normalize(min, max, min_img, max_img, b));
		}

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
}

void usage (char *s)
{
  fprintf(stderr, "Usage: %s <min> <max> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

int normalize(int min, int max, int min_image, int max_image, int value)
{
	// if (value == 0) return 0;
	if (min_image == max_image) return 0;

	int a = ((max - min) * 1.f / (max_image - min_image)) * value * 1.f;
	int b = ((min * max_image) - (max * min_image)) *1.f / (max_image - min_image) * 1.f;
	return a + b;
}

#define PARAMS 4
int main(int argc, char *argv[])
{
  if (argc != PARAMS+1){
    usage(argv[0]);
  }
  /* code to modify */
  int min = atoi(argv[1]);
  int max = atoi(argv[2]);
  char *ims = argv[3];
  char *imd = argv[4];
 
  process(min, max, ims, imd);
  return EXIT_SUCCESS;
}