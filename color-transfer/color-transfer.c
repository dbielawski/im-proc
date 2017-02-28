/**
 * @file color-transfert
 * @brief transfert color from source image to target image.
 *        Method from Reinhard et al. : 
 *        Erik Reinhard, Michael Ashikhmin, Bruce Gooch and Peter Shirley, 
 *        'Color Transfer between Images', IEEE CGA special issue on 
 *        Applied Perception, Vol 21, No 5, pp 34-41, September - October 2001
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <bcl.h>

#define D 3

static float RGB2LMS[D][D] = {
  {0.3811, 0.5783, 0.0402}, 
  {0.1967, 0.7244, 0.0782},  
  {0.0241, 0.1288, 0.8444}
};

// static float LMS2LAB[D][D] = {
//   {0.0000, 0.0000, 0.0000}, 
//   {0.0000, 0.0000, 0.0000},  
//   {0.0000, 0.0000, 0.0000}
// };


// static float LAB2LMS[D][D] = {
//   {0.0000, 0.0000, 0.0000}, 
//   {0.0000, 0.0000, 0.0000},  
//   {0.0000, 0.0000, 0.0000}
// };

static float LMS2RGB[D][D] = {
  {4.4679, -3.5873, 0.1193}, 
  {-1.2186, 2.3809, -0.1624},  
  {0.0497, -0.2439, 1.2045}
};


int clamp(int min, int max, int value)
{
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

float* rgb2lms(unsigned short* RGB, int cols, int rows)
{
  float* LMS = malloc(sizeof(float) * cols * rows * 3);

  for (int i = 0; i < rows * 3; ++i)
  {
    for (int j = 0; j < cols * 3; ++j)
    {
      for (int k = 0; k < 3; ++k)
      {
        float tmp = 0.f;

        for (int l = 0; l < 3; ++l)
        {
          tmp += RGB2LMS[k][l] * RGB[i * cols + j + l];
        }

        if (tmp >= 1)
          tmp = log10(tmp);
        else if (LMS[i * cols + j + k] < 1)
          tmp = 0;

        LMS[i * cols + j + k] = tmp;
      }
    }
  }
  return LMS;
}

unsigned short* lms2rgb(float* LMS, int cols, int rows)
{
  unsigned short* RGB = malloc(sizeof(unsigned short) * cols * rows * 3);

  for (int i = 0; i < rows * 3; ++i)
  {
    for (int j = 0; j < cols * 3; ++j)
    {
      for (int k = 0; k < 3; ++k)
      {
        float tmp = 0.f;

        for (int l = 0; l < 3; ++l)
        {
          tmp += LMS2RGB[k][l] * LMS[i * cols + j + l];
        }

        RGB[i * cols + j + k] = clamp(0, 255, (unsigned short)tmp);
      }
    } 
  }
  return RGB;
}

static void process(char *ims, char *imt, char* imd)
{
  fprintf(stderr, "color-transfert: ");

  pnm im_out = pnm_load(ims);
  int cols = pnm_get_width(im_out);
  int rows = pnm_get_height(im_out);

  unsigned short* buffer = pnm_get_image(im_out);
  float* lms = rgb2lms(buffer, cols, rows);
  unsigned short* rgb = lms2rgb(lms, cols, rows);

  for (int i = 0; i < rows * 3; ++i)
    for (int j = 0; j < cols * 3; ++j)
      for (int k = 0; k < 3; ++k)
        buffer[i * cols + j + k] = rgb[i * cols + j + k];

  pnm_save(im_out, PnmRawPpm, imd);
  (void) ims;
  (void) imt;
  (void) imd;

  fprintf(stderr, "OK\n");
}

void usage (char *s){
  fprintf(stderr, "Usage: %s <ims> <imt> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define param 3
int main(int argc, char *argv[]){
  if (argc != param+1) 
    usage(argv[0]);
  process(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}
