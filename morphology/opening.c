#include <stdlib.h>
#include <stdio.h>

#include "morphology.h"


void
usage(char* s)
{
  fprintf(stderr,"%s <se> <hs> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

#define PARAM 4
int
main(int argc, char* argv[])
{
	if(argc != PARAM+1)
		usage(argv[0]);

	int   shape    = atoi(argv[1]);
	int   hs       = atoi(argv[2]);
  char* ims_name = argv[3];
  char* imd_name = argv[4];

  pnm ims = pnm_load(ims_name);
  int width = pnm_get_width(ims);
 	int height = pnm_get_height(ims);
 	pnm imd = pnm_new(width, height, PnmRawPpm);

 	process(shape, hs, ims, imd, lesser);
  pnm ope = pnm_new(width, height, PnmRawPpm);
  process(shape, hs, imd, ope, greater);

 	pnm_save(ope, PnmRawPpm, imd_name);

 	pnm_free(ims);
  pnm_free(imd);
 	pnm_free(ope);

  return EXIT_SUCCESS;
};