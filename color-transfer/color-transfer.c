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

static float LMS2LAB[D][D] = {
  {0.5774, 0.5774, 0.5774}, 
  {0.4082, 0.4082, -0.8165},  
  {0.7071, -0.7071, 0.0000}
};

static float LAB2LMS[D][D] = {
  {0.5774, 0.4082, 0.7071}, 
  {0.5774, 0.4082, -0.7071},  
  {0.5774, -0.8165, 0.0000}
};

static float LMS2RGB[D][D] = {
  {4.4679, -3.5873, 0.1193}, 
  {-1.2186, 2.3809, -0.1624},  
  {0.0497, -0.2439, 1.2045}
};


static int clamp(int min, int max, int value)
{
  int x = value;
  if (value < min)
    x = min;
  else if (value > max)
    x = max;
  return x;
}

static float* rgb2lms(unsigned short* RGB, int cols, int rows)
{
  float* LMS = malloc(sizeof(float) * cols * rows * 3);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; ++k)
      {
        float tmp = 0.f;

        for (int l = 0; l < 3; ++l)
          tmp += RGB2LMS[k][l] * RGB[i * 3 * cols + (3 * j) + l];

        if (tmp >= 1.f)
          tmp = log10(tmp);
        else if (tmp < 1.f)
          tmp = 0.f;

        LMS[i * 3 * cols + (3 * j) + k] = tmp;
      }

  return LMS;
}

static float* lms2lab(float* LMS, int cols, int rows)
{
  float* LAB = malloc(sizeof(float) * cols * rows * 3);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; ++k)
      {
        float tmp = 0.f;

        for (int l = 0; l < 3; ++l)
          tmp += LMS2LAB[k][l] * LMS[i * 3 * cols + (3 * j) + l];

        LAB[i * 3 * cols + (3 * j) + k] = tmp;
        // LAB[i * 3 * cols + (3 * j) + k] =
        //   (unsigned short)clamp(0, 255, (unsigned short)tmp);
      }

  return LAB;
}

static float* lab2lms(float* LAB, int cols, int rows)
{
  float* LMS = malloc(sizeof(float) * cols * rows * 3);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; ++k)
      {
        float tmp = 0.f;

        for (int l = 0; l < 3; ++l)
          tmp += LAB2LMS[k][l] * LAB[i * 3 * cols + (3 * j) + l];

        tmp = pow(10, tmp);

        LMS[i * 3 * cols + (3 * j) + k] = tmp;

        // LMS[i * 3 * cols + (3 * j) + k] =
        //   (unsigned short)clamp(0, 255, (unsigned short)tmp);
      }

  return LMS;
}

static unsigned short* lms2rgb(float* LMS, int cols, int rows)
{
  unsigned short* RGB = malloc(sizeof(unsigned short) * cols * rows * 3);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; ++k)
      {
        float tmp = 0.f;

        for (int l = 0; l < 3; ++l)
          tmp += LMS2RGB[k][l] * LMS[i * 3 * cols + (3 * j) + l];

        RGB[i * 3 * cols + (3 * j) + k] =
          (unsigned short)clamp(0, 255, (unsigned short)tmp);
      }

  return RGB;
}

static float* means(float* lab, int cols, int rows)
{
  float* means = malloc(sizeof(float) * 3);

  for (int i = 0; i < 3; ++i)
    means[i] = 0.f;

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; ++k)
        means[k] += lab[i * 3 * cols + (3 * j) + k];

  for (int i = 0; i < 3; ++i)
    means[i] /= cols * rows;

  return means;
}

static float* standardDeviations(float* means, float* lab, int cols, int rows)
{
  float* deviations = malloc(sizeof(float) * 3);

  for (int i = 0; i < 3; ++i)
    deviations[i] = 0.f;

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; k++)
      {
        deviations[k] += lab[i * cols * 3 + j * 3 + k] - means[k] *
                        lab[i * cols * 3 + j * 3 + k] - means[k];
      }

  for (int i = 0; i < 3; ++i)
    deviations[i] = sqrt(deviations[i]);

  return deviations;
}

static float* transfer(float* deviation_s, float* deviation_t, float* lab_t, float* means_s, float* means_t, int cols, int rows)
{
  float* lab_d = malloc(sizeof(unsigned short) * cols * rows * 3);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; ++k)
      {
        lab_d[i * cols * 3 + j * 3 + k] = lab_t[i * cols * 3 + j * 3 + k] - means_t[k];
        lab_d[i * cols * 3 + j * 3 + k] *= deviation_s[k] / deviation_t[k];

        lab_d[i * cols * 3 + j * 3 + k] += means_s[k];
      }

  return lab_d;
}

static void fillBuffer(unsigned short* toFill, unsigned short* in, int cols , int rows)
{
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < 3; ++k)
        toFill[i * 3 * cols + (3 * j) + k] = in[i * 3 * cols + (3 * j) + k];
}

static void process(char *ims, char *imt, char* imd)
{
  fprintf(stderr, "color-transfer: ");

  pnm im_s = pnm_load(ims);
  pnm im_t = pnm_load(imt);

  int cols = pnm_get_width(im_s);
  int rows = pnm_get_height(im_t);

  unsigned short* img_s = pnm_get_image(im_s);
  unsigned short* img_t = pnm_get_image(im_t);

  float* lms_s = rgb2lms(img_s, cols, rows);
  float* lms_t = rgb2lms(img_t, cols, rows);

  float* lab_s = lms2lab(lms_s, cols, rows);
  float* lab_t = lms2lab(lms_t, cols, rows);

  float* means_s = means(lab_s, cols, rows);
  float* means_t = means(lab_t, cols, rows);

  // Deviation
  float* deviation_s = standardDeviations(means_s, lab_s, cols, rows);
  float* deviation_t = standardDeviations(means_t, lab_t, cols, rows);

  // Transfer
  float* lab_d = transfer(deviation_s, deviation_t, lab_t, means_s, means_t, cols, rows);

  float* lms_d = lab2lms(lab_d, cols, rows);
  unsigned short* rgb = lms2rgb(lms_d, cols, rows);

  pnm im_d = pnm_new(cols, rows, PnmRawPpm);
  unsigned short* buffer = pnm_get_image(im_d);
  fillBuffer(buffer, rgb, cols, rows);

  pnm_save(im_d, PnmRawPpm, imd);

  free(lab_s);
  free(lab_d);
  free(lms_s);
  free(lms_t);
  free(lms_d);
  free(rgb);
  free(buffer);
  pnm_free(im_s);
  pnm_free(im_t);

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
