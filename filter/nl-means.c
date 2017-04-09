#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pnm.h>

float gaussian(float k, int sigma)
{
  return exp(-k / 2.f * sigma * sigma);
}

static void 
process(int sigma, char* ims_name, char* imd_name)
{
  pnm ims = pnm_load(ims_name);
  int width = pnm_get_width(ims);
  int height = pnm_get_height(ims);

  unsigned short* in_img = pnm_get_channel(ims, NULL, 0);

  float tmp_img[width * height];
  float res_img[width * height];

  for (int i = 0; i < width * height; ++i)
    tmp_img[i] = (float)in_img[i];

  int window_size = 11;
  int halfsize_window = window_size / 2;
  int r = 2;
  int patch_size = 2 * r + 1;
  int halfsize_patch = patch_size / 2;


  float weight;
  float euclidian_distance;
  float normalization_factor;

  for (int i = 0; i < height; ++i)    // image: p
    for (int j = 0; j < width; ++j)
    {


      for (int k = 0; k < window_size; ++k)   // fenetre: q
        for (int l = 0; l < window_size; ++l)
        {

          int new_i = i + j - halfsize_window;
          int new_j = j + k - halfsize_window;

          if (new_i >= 0 && new_i < height && new_j >= 0 && new_j < width)
          {
            int cpt = 0;
            euclidian_distance = 0.f;

            for (int u = -r; u < r; ++u)  // patch
            {
              for (int v = -r; v < r; ++v)
              {

                if (i + u >= 0 && i + u < height && j + v >= 0 && j + v < width)
                {
                  euclidian_distance += (tmp_img[(i + u) * width + (j + v)]
                                        - tmp_img[(new_i + u) * width + (new_j + v)])
                                        * (tmp_img[(i + u) * width + (j + v)]
                                        - tmp_img[(new_i + u) * width + (new_j + v)]);
                  cpt++;                  
                }
              }
            }

            euclidian_distance /= cpt;

            weight = gaussian(euclidian_distance, sigma);
            normalization_factor += weight;
          }

        }

      res_img[i * width + j] = 1.f / normalization_factor * weight
                              * tmp_img[i * width + j];
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
  fprintf(stderr, "Usage: %s <sigma> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[]){
  if (argc != PARAM+1) 
    usage(argv[0]);

  int   idx       = 1;
  int   sigma  = atoi(argv[idx++]);
  char* ims       = argv[idx++];
  char* imd       = argv[idx++];

  process(sigma, ims, imd);

  return EXIT_SUCCESS;
}