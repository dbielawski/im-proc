#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <bcl.h>

static const float DISC_TEMPO = 0.25f;


float c1(int s, int lambda)
{
	return (float)(1.f / (1.f + (s / lambda * 1.f) * (s / lambda * 1.f)));
}

float c2(int s, int lambda)
{
	return (float)exp(-(s / lambda * 1.f) * (s / lambda * 1.f));
}

static void 
process(int n, int lambda, int function,  char* ims_name, char* imd_name)
{
  pnm ims = pnm_load(ims_name);
  int width = pnm_get_width(ims);
  int height = pnm_get_height(ims);

  unsigned short* in_img = pnm_get_channel(ims, NULL, 0);

  float res_img[width * height];
  float tmp_img[width * height];

  for (int i = 0; i < width * height; ++i)
    tmp_img[i] = (float)in_img[i];

  for (int k = 0; k < n; ++k)
  {
    for (int i = 0; i < height; ++i)
      for (int j = 0; j < width; ++j)
      {
        float top;
        float left;
        float right;
        float bottom;

        if (i == 0)
          top = (float)tmp_img[(i + 1) * width + j];
        else
          top = (float)tmp_img[(i - 1) * width + j];

        if (i == height - 1)
          bottom = (float)tmp_img[(i - 1) * width + j];
        else
          bottom = (float)tmp_img[(i + 1) * width + j];

        if (j == width - 1)
          right = (float)tmp_img[i * width + j - 1];
        else
          right = (float)tmp_img[i * width + j + 1];

        if (j == 0)
          left = (float)tmp_img[i * width + j + 1];
        else
          left = (float)tmp_img[i * width + j - 1];

      	float gradient_right = right - tmp_img[i * width + j];
      	float gradient_bottom = bottom - tmp_img[i * width + j];

      	float norm = sqrt((gradient_right * gradient_right)
      		+ (gradient_bottom * gradient_bottom));


      	if (function == 1)
      		norm = c1(norm, lambda);
  		  else if (function == 2)
      		norm = c2(norm, lambda);

      	float div = tmp_img[i * width + j] - left + tmp_img[i * width + j] - bottom;
        printf("%f\n", div);

      	res_img[i * width + j] = div + DISC_TEMPO * norm;
      }

      for (int i = 0 ; i < width * height; ++i)
        tmp_img[i] = res_img[i];
  }

  unsigned short res[width * height];
  for (int i = 0; i < width * height; ++i)
  {
    if (res_img[i] > 255.f)
      res_img[i] = 255.f;
    else if (res_img[i] < 0.f)
      res_img[i] = 0.f;

    res[i] = (unsigned short) res_img[i];
  }

  pnm imd = pnm_new(width, height, PnmRawPpm);
  pnm_set_channel(imd, res, 0);
  pnm_set_channel(imd, res, 1);
  pnm_set_channel(imd, res, 2);
  pnm_save(imd, PnmRawPpm, imd_name);
}

void usage (char *s){
  fprintf(stderr, "Usage: %s <n> <lambda> <function> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 5
int main(int argc, char *argv[]){
	if (argc != PARAM+1) 
		usage(argv[0]);

	int   idx     	= 1;
	int   n       	= atoi(argv[idx++]);
	int lambda 		= atoi(argv[idx++]);
	int function 	= atoi(argv[idx++]);
	char* ims     	= argv[idx++];
	char* imd     	= argv[idx++];

	process(n, lambda, function, ims, imd);

	return EXIT_SUCCESS;
}