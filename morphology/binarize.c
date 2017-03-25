#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void process(int min, int max, char* ims_char, char* imd_char)
{
	fprintf(stderr,"binarize: ");

  	pnm ims = pnm_load(ims_char);
  	int height = pnm_get_height(ims);
  	int width = pnm_get_width(ims);
  

  	unsigned short* image = pnm_get_channel(ims, NULL, PnmRed);
  	unsigned short* image_out = malloc(height * width * sizeof(unsigned short));
  
	for(int i = 0; i < height; ++i)
		for(int j = 0; j < width; ++j)
    	{	
    		int index = i * width + j;
	      	unsigned short tmp = image[index];
	      	if(tmp >= min && tmp <= max)	
	      		image_out[index] = 255;
	      	else
	      		image_out[index] = 0;
      	}
 
	pnm imd = pnm_new(width, height, PnmRawPpm);
	pnm_set_channel(imd, image_out, 0);
	pnm_set_channel(imd, image_out, 1);
	pnm_set_channel(imd, image_out, 2);
	pnm_save(imd, PnmRawPpm, imd_char);

	fprintf(stderr,"OK\n");
}

void
usage(char* s)
{
  fprintf(stderr,"%s <min> <max> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

#define PARAM 4
int
main(int argc, char* argv[])
{
  if(argc != PARAM+1) 
    usage(argv[0]);

  process(atoi(argv[1]), atoi(argv[2]), argv[3], argv[4]);

  return EXIT_SUCCESS;
}