/**
 * @file 
 * @author 
 * @brief ???
 */

#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>


void process(char* ims_name, char* imd_name)
{
	pnm ims = pnm_load(ims_name);
	unsigned short *input = pnm_get_image(ims);

	int rows = pnm_get_height(ims);
	int cols = pnm_get_width(ims);

	pnm imd = pnm_new(cols, rows, PnmRawPpm);
	unsigned short *output = pnm_get_image(imd);

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			for (int k = 0; k < 3; ++k)
			{
				int r = 0;
				int g = 0;
				int b = 0;
				r = input[i * cols * 3 + j * 3 + 0];
				g = input[i * cols * 3 + j * 3 + 1];
				b = input[i * cols * 3 + j * 3 + 2];
				
				int value = (r + g + b) / 3;
				*output++ = value;
			}

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
}

void usage (char *s)
{
  fprintf(stderr, "Usage: %s <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}


#define PARAMS 2
int main(int argc, char *argv[])
{
  if (argc != PARAMS+1){
    usage(argv[0]);
  }
  /* code to modify */
  char *ims = argv[1];
  char *imd = argv[2];
 
  process(ims, imd);
  return EXIT_SUCCESS;
}