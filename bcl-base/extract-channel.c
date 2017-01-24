/**
 * @file 
 * @author 
 * @brief ???
 */

#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>


void process(int num, char* ims_name, char* imd_name)
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
				*output++ = input[i * cols * 3 + j * 3 + num];

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
}

void 
usage (char *s)
{
  fprintf(stderr,
	  "Usage: %s <channel={0,2}> <ims> <imd>\n"
	  ,s);
  exit(EXIT_FAILURE);
}


#define PARAMS 3
int 
main(int argc, char *argv[])
{
  if (argc != PARAMS+1){
    usage(argv[0]);
  }
  /* code to modify */
  int num=atoi(argv[1]);
  char *ims = argv[2], *imd = argv[3];
 
  process(num, ims, imd);
  return EXIT_SUCCESS;
}