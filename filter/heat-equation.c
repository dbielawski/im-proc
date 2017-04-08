#include <stdlib.h>
#include <stdio.h>


#include <bcl.h>

static const float DISC_TEMPO = 0.25f;

static float
Laplacien(float* buffer, int width, int height, int i, int j)
{
  float top;
  float left;
  float right;
  float bottom;

  if (i == 0)
    top = (float)buffer[(i + 1) * width + j];
  else
    top = (float)buffer[(i - 1) * width + j];

  if (i == height - 1)
    bottom = (float)buffer[(i - 1) * width + j];
  else
    bottom = (float)buffer[(i + 1) * width + j];

  if (j == width - 1)
    right = (float)buffer[i * width + j - 1];
  else
    right = (float)buffer[i * width + j + 1];

  if (j == 0)
    left = (float)buffer[i * width + j + 1];
  else
    left = (float)buffer[i * width + j - 1];

  return top + left + bottom + right - 4.f * buffer[i * width + j];
}

static void 
process(int n, char* ims_name, char* imd_name)
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
        float l = Laplacien(tmp_img, width, height, i, j);

        res_img[i * width + j] = tmp_img[i * width + j] + DISC_TEMPO * l;
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
  fprintf(stderr, "Usage: %s <n> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[]){
  if (argc != PARAM+1) 
    usage(argv[0]);

  int   idx     = 1;
  int   n       = atoi(argv[idx++]);
  char* ims     = argv[idx++];
  char* imd     = argv[idx++];

  process(n, ims, imd);

  return EXIT_SUCCESS;
}
