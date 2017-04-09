#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>


int compare (const void * a, const void * b)
{
  return ( *(unsigned short*)a - *(unsigned short*)b );
}


static void 
process(int halfsize, char* ims_name, char* imd_name)
{
	pnm ims = pnm_load(ims_name);
  	int width = pnm_get_width(ims);
  	int height = pnm_get_height(ims);

  	int size = 2 * halfsize + 1;
  	unsigned short* in_img = pnm_get_channel(ims, NULL, 0);
  	unsigned short res_img[width * height];
  	unsigned short window[size * size];

    for (int i = 0; i < height; ++i)
      for (int j = 0; j < width; ++j)
      {
      	int cpt = 0;
      	for (int k = 0 ; k < size; ++k)
      	{
      		for (int l = 0; l < size; ++l)
      		{
      			int new_i = i + k - halfsize;
      			int new_j = j + l - halfsize;

      			if (new_i >= 0 && new_i < width && new_j >= 0 && new_j < height)
      				window[cpt++] = in_img[new_i * width + new_j];
      		}
      	}

      	qsort(window, cpt, sizeof(unsigned short), compare);
      	res_img[i * width + j] = window[cpt / 2];
      }

	pnm imd = pnm_new(width, height, PnmRawPpm);
	pnm_set_channel(imd, res_img, 0);
	pnm_set_channel(imd, res_img, 1);
	pnm_set_channel(imd, res_img, 2);
	pnm_save(imd, PnmRawPpm, imd_name);
}

void usage (char *s){
  fprintf(stderr, "Usage: %s <halfsize> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[]){
	if (argc != PARAM+1) 
		usage(argv[0]);

	int   idx     	= 1;
	int   halfsize 	= atoi(argv[idx++]);
	char* ims     	= argv[idx++];
	char* imd     	= argv[idx++];

	process(halfsize, ims, imd);

	return EXIT_SUCCESS;
}