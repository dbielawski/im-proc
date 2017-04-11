#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>


static float S = 0.1f;

float gaussian(float k, int sigma)
{
	return exp(-k * k / (2.f * sigma * sigma));
}

float dist(int i, int j, int k, int l)
{
	return sqrt((i - k) * (i - k) + (j - l) * (j - l));
}

void precomputeColorDist(float* buffer, int sigma)
{
	for (int i = 0; i < 255; ++i)
		buffer[i] = gaussian(i, sigma);
}

void precomputePixelDist(float* buffer, int maxPixelDist, int sigma)
{
	for (int i = 0; i < maxPixelDist; ++i)
		buffer[i] = gaussian(i, sigma);
}

static void 
process(int sigma_s, int sigma_g, char* ims_name, char* imd_name)
{
	pnm ims = pnm_load(ims_name);
	int width = pnm_get_width(ims);
	int height = pnm_get_height(ims);

	unsigned short* in_img = pnm_get_channel(ims, NULL, 0);

	float res_img[width * height];

	int half_window_size = ceil(sqrt(- 2.f * sigma_s * sigma_s * log(S)));
	int window_size = half_window_size * 2;

	float LUT_G_g[255];
	precomputeColorDist(LUT_G_g, sigma_g);

	float LUT_G_s[half_window_size];
	precomputePixelDist(LUT_G_s, half_window_size, sigma_s);

	for (int i = 0; i < height; ++i)
      for (int j = 0; j < width; ++j)
      {
      	float num = 0.f;
      	float deno = 0.f;
		unsigned short Ip = in_img[i * width + j];

      	for (int k = 0 ; k < window_size; ++k)
      	{
      		for (int l = 0; l < window_size; ++l)
      		{
      			int new_i = i + k - half_window_size;
      			int new_j = j + l - half_window_size;

      			if (new_i >= 0 && new_i < width && new_j >= 0 && new_j < height)
      			{
      				unsigned short Iq = in_img[new_i * width + new_j];
      				float gaussianS = LUT_G_s[(int)dist(i, j, new_i, new_j)];
      				float gaussianG = LUT_G_g[abs(Ip - Iq)];
      				float gaussS_gaussG = gaussianS * gaussianG;

      				num += gaussS_gaussG * Iq;
      				deno += gaussS_gaussG;
      			}
      		}
      	}

  		res_img[i * width + j] = num / deno;
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
  fprintf(stderr, "Usage: %s <sigma_s> <sigma_g> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 4
int main(int argc, char *argv[]){
	if (argc != PARAM+1) 
		usage(argv[0]);

	int   idx     	= 1;
	int   sigma_s 	= atoi(argv[idx++]);
	int   sigma_g 	= atoi(argv[idx++]);
	char* ims     	= argv[idx++];
	char* imd     	= argv[idx++];

	process(sigma_s, sigma_g, ims, imd);

	return EXIT_SUCCESS;
}