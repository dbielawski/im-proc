/**
 * @file extract-subimage.c
 * @brief 
 */

#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

// i_offset nombre de ligne de decalage par rapport au top left
// j_offset nombre de colonne de decage par rapport au top left
// rows le nombre de ligne 
// cols nombre de colonne
void process(int i_offset, int j_offset, int rows, int cols, char* ims_name, char* imd_name)
{
  pnm ims = pnm_load(ims_name);
  unsigned short *input = pnm_get_image(ims);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  unsigned short *output = pnm_get_image(imd);

  for (int i = i_offset; i < rows + i_offset; i++)
    for (int j = j_offset; j < cols + j_offset; j++)
      for (int k = 0; k < 3; k++)
        *output++ = input[i * (pnm_get_width(ims)) * 3 + j * 3 + k];

  pnm_save(imd, PnmRawPpm, imd_name);
  pnm_free(imd);
}

void usage (char *s)
{
  fprintf(stderr,
	  "Usage: %s <i> <j> <rows> <cols> <ims> <imd>\n"
	  ,s);
  exit(EXIT_FAILURE);
}

#define PARAMS 6
int main(int argc, char *argv[])
{
  if (argc != PARAMS+1){
    usage(argv[0]);
  }
  /* code to modify */
  int i = atoi(argv[1]), j = atoi(argv[2]), rows = atoi(argv[3]), cols = atoi(argv[4]);
  char *ims = argv[5], *imd = argv[6];

  // printf("%i\n", x);
  // printf("%i\n", y);
  // printf("%i\n", rows);
  // printf("%i\n", cols);
  process(i, j, rows, cols, ims, imd);
  return EXIT_SUCCESS;
}
