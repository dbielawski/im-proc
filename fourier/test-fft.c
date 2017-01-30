/**
 * @file test-fft.c
 * @brief test the behaviors of functions in fft module
 *
 */
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fft.h"

#define PI 3.14159265358979323846

char* process_name(char* name)
{
  char *slash = strrchr(name, '/');
  if (slash)
  {
    slash++;

    int len = (strlen(slash));
    char* res = malloc((strlen(slash) + 1) * sizeof(char));
    strncpy(res, slash, len);
    res[len] = '\0';
    return res;
  }
  return name;
}

float find_max(long int size, float* buffer)
{
  float max = 0.f;
  for (int i = 0; i < size; ++i)
  {
    if (max < buffer[i])
      max = buffer[i];
  }
  return max;
}

int clamp(int value, int min, int max)
{
  if (value < max && value > min)
    return value;
  else if (value < min)
    return min;
  else
    return max;
}

/**
 * @brief test the forward and backward functions
 * @param pnm ims, the source image
 * @param char* name, the image file name
 */
static void
test_for_backward(pnm ims, char* name)
{
  fprintf(stderr, "test_for_backward: ");

  long int cols = pnm_get_width(ims);
  long int rows = pnm_get_height(ims);

  unsigned short* gray = malloc(sizeof(unsigned short) * cols * rows);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);

  pnm_get_channel(ims, gray, 0);

  fftw_complex* freq_repr = forward(rows, cols, gray);
  free(gray);
  gray = backward(rows , cols, freq_repr);

  pnm_set_channel(imd, gray, 0);
  pnm_set_channel(imd, gray, 1);
  pnm_set_channel(imd, gray, 2);

  char new_name[100] = "FB-";
  char* extracted_name = process_name(name);
  strcat(new_name, extracted_name);
  pnm_save(imd, PnmRawPpm, new_name);

  free(gray);
  fftw_free(freq_repr);
  free(extracted_name);
  pnm_free(imd);

  fprintf(stderr, "OK\n");
}


/**
 * @brief test image reconstruction from of amplitude and phase spectrum
 * @param pnm ims: the source image
 * @param char *name: the image file name
 */
static void
test_reconstruction(pnm ims, char* name)
{
  fprintf(stderr, "test_reconstruction: ");

  long int cols = pnm_get_width(ims);
  long int rows = pnm_get_height(ims);

  float *as = (float*)malloc(rows * cols * sizeof(float));
  float *ps = (float*)malloc(rows * cols * sizeof(float));

  unsigned short *gray = malloc(rows * cols * sizeof(unsigned short));
  pnm_get_channel(ims, gray, 0);

  fftw_complex *freq_repr = forward(rows, cols, gray);
  freq2spectra(rows, cols, freq_repr, as, ps);
  spectra2freq(rows, cols, as, ps, freq_repr);
  unsigned short* m_gray = backward(rows , cols, freq_repr);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  pnm_set_channel(imd, m_gray, 0);
  pnm_set_channel(imd, m_gray, 1);
  pnm_set_channel(imd, m_gray, 2);

  free(as);
  free(ps);
  fftw_free(freq_repr);
  free(gray);
  char new_name[100] = "FB-ASPS-";
  char* extracted_name = process_name(name);
  strcat(new_name, extracted_name);
  pnm_save(imd, PnmRawPpm, new_name);
  pnm_free(imd);
  free(extracted_name);

  fprintf(stderr, "OK\n");
}


/**
 * @brief test construction of amplitude and phase images in ppm files
 * @param pnm ims, the source image
 * @param char* name, the image file name
 */
static void
test_display(pnm ims, char* name)
{
  fprintf(stderr, "test_display: ");

  long int cols = pnm_get_width(ims);
  long int rows = pnm_get_height(ims);

  float *as = (float*)malloc(rows * cols * sizeof(float));
  float *ps = (float*)malloc(rows * cols * sizeof(float));

  pnm imd_as = pnm_new(cols, rows, PnmRawPpm);
  pnm imd_ps = pnm_new(cols, rows, PnmRawPpm);  

  unsigned short *gray = malloc(rows * cols * sizeof(unsigned short));
  pnm_get_channel(ims, gray, 0);

  fftw_complex *freq_repr = forward(rows, cols, gray);
  freq2spectra(rows, cols, freq_repr, as, ps);

  float k = 0.15f;
  float as_max = find_max(rows * cols, as);

  unsigned short* img_as = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_ps = malloc(rows * cols * sizeof(unsigned short));

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      img_as[i * cols + j] =
      clamp(pow(as[i * cols + j] / as_max, k) * 255.f, 0, pnm_maxval);

      img_ps[i * cols + j] =
      clamp(ps[i * cols + j], 0, pnm_maxval);
    }

  pnm_set_channel(imd_as, img_as, 0);
  pnm_set_channel(imd_as, img_as, 1);
  pnm_set_channel(imd_as, img_as, 2);

  pnm_set_channel(imd_ps, img_ps, 0);
  pnm_set_channel(imd_ps, img_ps, 1);
  pnm_set_channel(imd_ps, img_ps, 2);

  free(as);
  free(ps);
  fftw_free(freq_repr);
  free(gray);

  char new_as[50] = "AS-";
  char* extracted_name = process_name(name);
  strcat(new_as, extracted_name);
  pnm_save(imd_as, PnmRawPpm, new_as);
  free(extracted_name);
    
  char new_ps[50] = "PS-";
  extracted_name = process_name(name);
  strcat(new_ps, extracted_name);
  pnm_save(imd_ps, PnmRawPpm, new_ps);
  free(extracted_name);

  pnm_free(imd_as);
  pnm_free(imd_ps);
  free(img_as);
  free(img_ps);

  fprintf(stderr, "OK\n");
}

/**
 * @brief test the modification of amplitude and phase spectrum and 
 *        construct output images
 * @param pnm ims, the source image
 * @param char* name, the image file name
 */
static void
test_modification(pnm ims, char* name)
{
  fprintf(stderr, "test_modification: ");

  (void)ims;
  (void)name;

  long int cols = pnm_get_width(ims);
  long int rows = pnm_get_height(ims);

  float *as = (float*)malloc(rows * cols * sizeof(float));
  float *ps = (float*)malloc(rows * cols * sizeof(float));

  pnm imd_freq = pnm_new(cols, rows, PnmRawPpm);
  pnm imd_as = pnm_new(cols, rows, PnmRawPpm);

  unsigned short *gray = malloc(rows * cols * sizeof(unsigned short));
  pnm_get_channel(ims, gray, 0);

  fftw_complex *freq_repr = forward(rows, cols, gray);
  freq2spectra(rows, cols, freq_repr, as, ps);
  
  float as_max = find_max(rows * cols, as);
  float new_amp = as_max * .25f;

  int mid_i = rows / 2;
  int mid_j = cols / 2;
  // horizontale
  as[cols * mid_i + mid_j - 8] = new_amp;
  as[cols * mid_i + mid_j + 8] = new_amp;
  // verticale
  as[cols * (mid_i - 8) + mid_j] = new_amp;
  as[cols * (mid_i + 8) + mid_j] = new_amp;

  spectra2freq(rows, cols, as, ps, freq_repr);  
  unsigned short* img_freq = backward(rows, cols, freq_repr);

  unsigned short* img_as = malloc(rows * cols * sizeof(unsigned short));

  float k = 0.15f;
  as_max = find_max(rows * cols, as);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      img_as[i * cols + j] =
      clamp(pow(as[i * cols + j] / as_max, k) * 255.f, 0, pnm_maxval);
    }

  pnm_set_channel(imd_freq, img_freq, 0);
  pnm_set_channel(imd_freq, img_freq, 1);
  pnm_set_channel(imd_freq, img_freq, 2);

  pnm_set_channel(imd_as, img_as, 0);
  pnm_set_channel(imd_as, img_as, 1);
  pnm_set_channel(imd_as, img_as, 2);

  free(as);
  free(ps);
  fftw_free(freq_repr);
  free(gray);

  char new_freq[50] = "FREQ-";
  char* extracted_name = process_name(name);
  strcat(new_freq, extracted_name);
  pnm_save(imd_freq, PnmRawPpm, new_freq);
  free(extracted_name);
    
  char new_as[50] = "FAS-";
  extracted_name = process_name(name);
  strcat(new_as, extracted_name);
  pnm_save(imd_as, PnmRawPpm, new_as);
  free(extracted_name);

  pnm_free(imd_freq);
  pnm_free(imd_as);
  free(img_freq);
  free(img_as);

  fprintf(stderr, "OK\n");
}


static void 
run(pnm ims, char* name)
{
  test_for_backward(ims, name);
  test_reconstruction(ims, name);
  test_display(ims, name);
  test_modification(ims, name);
}


void 
usage (char *s)
{
  fprintf(stderr, "Usage: %s <ims> \n", s);
  exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[])
{  
  if (argc != 2)
    usage(argv[0]);
  pnm ims = pnm_load(argv[1]);
  run(ims, argv[1]);
  pnm_free(ims);

  return EXIT_SUCCESS;
}
