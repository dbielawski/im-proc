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

float find_max_amplitude(long int size, float* as)
{
  float max = 0.f;
  for (int i = 0; i < size; ++i)
  {
    if (max < as[i])
      max = as[i];
  }
  return max;
}

unsigned short clamp(int value, int min, int max)
{
  if (value < max && value > min)
    return value;
  else if (value < min)
    return min;
  else
    return max;
}

void centrage(int rows, int cols, unsigned short* img)
{
  long int size = rows * cols;
  for (int i = 0; i < size; ++i)
    img[i] = img[i] * pow(-1, i);
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

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      gray[i * cols + j] = pnm_get_component(ims, i, j, 0);

  fftw_complex* freq_repr = forward(rows, cols, gray);
  gray = backward(rows , cols, freq_repr);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      pnm_set_component(imd, i, j, 0, gray[i * cols + j]);
      pnm_set_component(imd, i, j, 1, gray[i * cols + j]);
      pnm_set_component(imd, i, j, 2, gray[i * cols + j]);
    }

  char new_name[100] = "FB-";
  strcat(new_name, process_name(name));
  pnm_save(imd, PnmRawPpm, new_name);
  pnm_free(imd);
  free(gray);
  fftw_free(freq_repr);

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
  (void)ims;
  (void)name;

  long int cols = pnm_get_width(ims);
  long int rows = pnm_get_height(ims);

  float *as = (float*)malloc(rows * cols * sizeof(float));
  float *ps = (float*)malloc(rows * cols * sizeof(float));

  unsigned short *gray = malloc(rows * cols * sizeof(unsigned short));
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      gray[i * cols + j] = pnm_get_component(ims, i, j, 0);

  fftw_complex *freq_repr = forward(rows, cols, gray);
  freq2spectra(rows, cols, freq_repr, as, ps);
  spectra2freq(rows, cols, as, ps, freq_repr);
  unsigned short* m_gray = backward(rows , cols, freq_repr);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      pnm_set_component(imd, i, j, 0, m_gray[i * cols + j]);
      pnm_set_component(imd, i, j, 1, m_gray[i * cols + j]);
      pnm_set_component(imd, i, j, 2, m_gray[i * cols + j]);
    }

  free(as);
  free(ps);
  fftw_free(freq_repr);
  free(gray);
  char new_name[100] = "FB-ASPS-";
  strcat(new_name, process_name(name));
  pnm_save(imd, PnmRawPpm, new_name);
  pnm_free(imd);

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
  (void)ims;
  (void)name;

  long int cols = pnm_get_width(ims);
  long int rows = pnm_get_height(ims);

  float *as = (float*)malloc(rows * cols * sizeof(float));
  float *ps = (float*)malloc(rows * cols * sizeof(float));

  unsigned short *gray = malloc(rows * cols * sizeof(unsigned short));
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      gray[i * cols + j] = pnm_get_component(ims, i, j, 0);

  centrage(rows, cols, gray);
  fftw_complex *freq_repr = forward(rows, cols, gray);
  freq2spectra(rows, cols, freq_repr, as, ps);

  pnm imd_as = pnm_new(cols, rows, PnmRawPpm);
  pnm imd_ps = pnm_new(cols, rows, PnmRawPpm);  

  float k = 0.1f;
  float as_max = find_max_amplitude(rows * cols, as);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      pnm_set_component(imd_as, i, j, 0,
        clamp(pow(as[i * cols + j] / as_max, k) * 255.f, 0, pnm_maxval));
      pnm_set_component(imd_as, i, j, 1,
        clamp(pow(as[i * cols + j] / as_max, k) * 255.f, 1, pnm_maxval));
      pnm_set_component(imd_as, i, j, 2,
        clamp(pow(as[i * cols + j] / as_max, k) * 255.f, 2, pnm_maxval));

      pnm_set_component(imd_ps, i, j, 0, ps[i * cols + j]);
      pnm_set_component(imd_ps, i, j, 1, ps[i * cols + j]);
      pnm_set_component(imd_ps, i, j, 2, ps[i * cols + j]);
    }


  free(as);
  free(ps);
  fftw_free(freq_repr);
  free(gray);

  char new_as[50] = "AS-";
  strcat(new_as, process_name(name));
  pnm_save(imd_as, PnmRawPpm, new_as);
    
  char new_ps[50] = "PS-";
  strcat(new_ps, process_name(name));
  pnm_save(imd_ps, PnmRawPpm, new_ps);

  pnm_free(imd_as);
  pnm_free(imd_ps);

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
