#include <stdio.h>
#include <stdlib.h>
#include <bcl.h>


static void process(char *ims_1, char *ims_2, char* imd_name)
{
  fprintf(stderr, "substract: ");

	pnm im_1 = pnm_load(ims_1);
  pnm im_2 = pnm_load(ims_2);

  int width = pnm_get_width(im_1);
  int height = pnm_get_height(im_2);

  unsigned short* out = malloc(sizeof(unsigned short) * width * height);
  unsigned short* image_1 = pnm_get_channel(im_1, NULL, 0);
  unsigned short* image_2 = pnm_get_channel(im_2, NULL, 0);

  for (int i = 0; i < height; ++i)
    for (int j = 0; j < width; ++j)
    {
      int res = image_1[i * width +j] - image_2[i * width + j];

      if (res < 0)
        res = 0;

      out[i * width + j] = (unsigned short)res;
    }

  pnm imd = pnm_new(width, height, PnmRawPpm);
  pnm_set_channel(imd, out, 0);
  pnm_set_channel(imd, out, 1);
  pnm_set_channel(imd, out, 2);

  pnm_save(imd, PnmRawPpm, imd_name);

	fprintf(stderr, "OK\n");
}

void usage (char *s){
  fprintf(stderr, "Usage: %s <ims-1> <ims-2> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define param 3
int main(int argc, char *argv[]){
  if (argc != param+1) 
    usage(argv[0]);
  process(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}