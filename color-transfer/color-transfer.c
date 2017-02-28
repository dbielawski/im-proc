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


// // Chemin inverse
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

  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      LMS[i * cols + j + 0] += 0.f;
      LMS[i * cols + j + 1] += 0.f;
      LMS[i * cols + j + 2] += 0.f;

      for (int k = 0; k < 3; ++k)
      {
        for (int l = 0; l < 3; ++l)
        {
          LMS[i * cols + j + 0] += RGB2LMS[k][l] * RGB[i * cols + j + 0];
          LMS[i * cols + j + 1] += RGB2LMS[k][l] * RGB[i * cols + j + 1];
          LMS[i * cols + j + 2] += RGB2LMS[k][l] * RGB[i * cols + j + 2];          
        }
      }

      if (LMS[i * cols + j + 0] >= 1)
        LMS[i * cols + j + 0] = log10(LMS[i * cols + j + 0]);

      if (LMS[i * cols + j + 1] >= 1)
        LMS[i * cols + j + 1] = log10(LMS[i * cols + j + 1]);

      if (LMS[i * cols + j + 2] >= 1)
        LMS[i * cols + j + 2] = log10(LMS[i * cols + j + 2]);
    }
  }
  return LMS;
}

unsigned short* lms2rgb(float* LMS, int cols, int rows)
{
  float* RGB = malloc(sizeof(float) * cols * rows * 3);

  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      RGB[i * cols + j + 0] += 0.f;
      RGB[i * cols + j + 1] += 0.f;
      RGB[i * cols + j + 2] += 0.f;

      for (int k = 0; k < 3; ++k)
      {
        for (int l = 0; l < 3; ++l)
        {
          RGB[i * cols + j + 0] += LMS2RGB[k][l] * LMS[i * cols + j + 0];
          RGB[i * cols + j + 1] += LMS2RGB[k][l] * LMS[i * cols + j + 1];
          RGB[i * cols + j + 2] += LMS2RGB[k][l] * LMS[i * cols + j + 2];          
        }
      }
    } 
  }

  unsigned short* RGB_I = malloc(sizeof(unsigned short) * cols * rows * 3);
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      // RGB_I[i * cols + j + 0] = clamp(0, 255, RGB[i * cols + j + 0]);
      // RGB_I[i * cols + j + 1] = clamp(0, 255, RGB[i * cols + j + 1]);
      // RGB_I[i * cols + j + 2] = clamp(0, 255, RGB[i * cols + j + 2]);

      RGB_I[i * cols + j + 0] = 0;
      RGB_I[i * cols + j + 1] = 0;
      RGB_I[i * cols + j + 2] = 0;
    } 
  }
  free(RGB);
  return RGB_I;
}

static void process(char *ims, char *imt, char* imd){

  pnm ims_ = pnm_load(ims);
  unsigned short* buffer = pnm_get_image(ims_);
  float* lms = rgb2lms(buffer, pnm_get_width(ims_), pnm_get_height(ims_));
  unsigned short* rgb = lms2rgb(lms, pnm_get_width(ims_), pnm_get_height(ims_));
  buffer = rgb;


  pnm_save(ims_, PnmRawPpm, "a.ppm");
  (void) ims;
  (void) imt;
  (void) imd;
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
