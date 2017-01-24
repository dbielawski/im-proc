/**
 * @file 
 * @author 
 * @brief ???
 */

#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>


void process(char* ims0_name, char* ims1_name,char* ims2_name, char* imd_name)
{
	pnm ims0 = pnm_load(ims0_name);
	unsigned short *input0 = pnm_get_image(ims0);

	pnm ims1 = pnm_load(ims1_name);
	unsigned short *input1 = pnm_get_image(ims1);

	pnm ims2 = pnm_load(ims2_name);
	unsigned short *input2 = pnm_get_image(ims2);

	int rows = pnm_get_height(ims0);
	int cols = pnm_get_width(ims0);

	pnm imd = pnm_new(cols, rows, PnmRawPpm);
	unsigned short *output = pnm_get_image(imd);

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			for (int k = 0; k < 3; ++k)
			{
				int value = 0;
				if (k == 0)
					value = input0[i * cols * 3 + j * 3 + 0];
				else if (k == 1)
					value = input1[i * cols * 3 + j * 3 + 1];
				else if (k == 2)
					value = input2[i * cols * 3 + j * 3 + 2];

				*output++ = value;
			}

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
}

void usage (char *s)
{
  fprintf(stderr, "Usage: %s <ims0> <ims1> <ims2> <imd>\n", s);
  exit(EXIT_FAILURE);
}


#define PARAMS 4
int main(int argc, char *argv[])
{
  if (argc != PARAMS+1){
    usage(argv[0]);
  }
  /* code to modify */
  char *ims0 = argv[1], *ims1 = argv[2], *ims2 = argv[3];
  char *imd = argv[4];
 
  process(ims0, ims1, ims2, imd);
  return EXIT_SUCCESS;
}