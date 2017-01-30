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
  float max = buffer[0];
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

  unsigned short* rouge = malloc(sizeof(unsigned short) * cols * rows);
  unsigned short* vert = malloc(sizeof(unsigned short) * cols * rows);
  unsigned short* bleu = malloc(sizeof(unsigned short) * cols * rows);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);

  pnm_get_channel(ims, rouge, 0);
  pnm_get_channel(ims, vert, 1);
  pnm_get_channel(ims, bleu, 2);

  fftw_complex* freq_repr_r = forward(rows, cols, rouge);
  fftw_complex* freq_repr_v = forward(rows, cols, vert);
  fftw_complex* freq_repr_b = forward(rows, cols, bleu);
  free(rouge);
  free(vert);
  free(bleu);
  rouge = backward(rows , cols, freq_repr_r);
  vert = backward(rows , cols, freq_repr_v);
  bleu = backward(rows , cols, freq_repr_b);

  pnm_set_channel(imd, rouge, 0);
  pnm_set_channel(imd, vert, 1);
  pnm_set_channel(imd, bleu, 2);

  char new_name[100] = "FB-";
  char* extracted_name = process_name(name);
  strcat(new_name, extracted_name);
  pnm_save(imd, PnmRawPpm, new_name);

  free(rouge);
  free(vert);
  free(bleu);
  fftw_free(freq_repr_r);
  fftw_free(freq_repr_v);
  fftw_free(freq_repr_b);
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

  float *as_r = (float*)malloc(rows * cols * sizeof(float));
  float *as_v = (float*)malloc(rows * cols * sizeof(float));
  float *as_b = (float*)malloc(rows * cols * sizeof(float));

  float *ps_r = (float*)malloc(rows * cols * sizeof(float));
  float *ps_v = (float*)malloc(rows * cols * sizeof(float));
  float *ps_b = (float*)malloc(rows * cols * sizeof(float));

  unsigned short *rouge = malloc(rows * cols * sizeof(unsigned short));
  unsigned short *vert = malloc(rows * cols * sizeof(unsigned short));
  unsigned short *bleu = malloc(rows * cols * sizeof(unsigned short));
  pnm_get_channel(ims, rouge, 0);
  pnm_get_channel(ims, vert, 1);
  pnm_get_channel(ims, bleu, 2);

  fftw_complex *freq_repr_r = forward(rows, cols, rouge);
  fftw_complex *freq_repr_v = forward(rows, cols, vert);
  fftw_complex *freq_repr_b = forward(rows, cols, bleu);

  freq2spectra(rows, cols, freq_repr_r, as_r, ps_r);
  freq2spectra(rows, cols, freq_repr_v, as_v, ps_v);
  freq2spectra(rows, cols, freq_repr_b, as_b, ps_b);

  spectra2freq(rows, cols, as_r, ps_r, freq_repr_r);
  spectra2freq(rows, cols, as_v, ps_v, freq_repr_v);
  spectra2freq(rows, cols, as_b, ps_b, freq_repr_b);

  unsigned short* m_rouge = backward(rows , cols, freq_repr_r);
  unsigned short* m_vert = backward(rows , cols, freq_repr_v);
  unsigned short* m_bleu = backward(rows , cols, freq_repr_b);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  pnm_set_channel(imd, m_rouge, 0);
  pnm_set_channel(imd, m_vert, 1);
  pnm_set_channel(imd, m_bleu, 2);

  free(as_r);
  free(as_v);
  free(as_b);
  free(ps_r);
  free(ps_v);
  free(ps_b);
  fftw_free(freq_repr_r);
  fftw_free(freq_repr_v);
  fftw_free(freq_repr_b);
  free(rouge);
  free(vert);
  free(bleu);
  free(m_rouge);
  free(m_vert);
  free(m_bleu);
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

  float *as_r = (float*)malloc(rows * cols * sizeof(float));
  float *as_v = (float*)malloc(rows * cols * sizeof(float));
  float *as_b = (float*)malloc(rows * cols * sizeof(float));

  float *ps_r = (float*)malloc(rows * cols * sizeof(float));
  float *ps_v = (float*)malloc(rows * cols * sizeof(float));
  float *ps_b = (float*)malloc(rows * cols * sizeof(float));

  pnm imd_as = pnm_new(cols, rows, PnmRawPpm);
  pnm imd_ps = pnm_new(cols, rows, PnmRawPpm);  

  unsigned short *rouge = malloc(rows * cols * sizeof(unsigned short));
  unsigned short *vert = malloc(rows * cols * sizeof(unsigned short));
  unsigned short *bleu = malloc(rows * cols * sizeof(unsigned short));

  pnm_get_channel(ims, rouge, 0);
  pnm_get_channel(ims, vert, 1);
  pnm_get_channel(ims, bleu, 2);

  fftw_complex *freq_repr_r = forward(rows, cols, rouge);
  fftw_complex *freq_repr_v = forward(rows, cols, vert);
  fftw_complex *freq_repr_b = forward(rows, cols, bleu);

  freq2spectra(rows, cols, freq_repr_r, as_r, ps_r);
  freq2spectra(rows, cols, freq_repr_v, as_v, ps_v);
  freq2spectra(rows, cols, freq_repr_b, as_b, ps_b);

  float k = 0.15f;
  float as_max_r = find_max(rows * cols, as_r);
  float as_max_v = find_max(rows * cols, as_v);
  float as_max_b = find_max(rows * cols, as_b);

  unsigned short* img_as_r = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_as_v = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_as_b = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_ps_r = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_ps_v = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_ps_b = malloc(rows * cols * sizeof(unsigned short));

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      img_as_r[i * cols + j] =
        clamp(pow(as_r[i * cols + j] / as_max_r, k) * 255.f, 0, pnm_maxval);
      img_as_v[i * cols + j] =
        clamp(pow(as_v[i * cols + j] / as_max_v, k) * 255.f, 0, pnm_maxval);
      img_as_b[i * cols + j] =
        clamp(pow(as_b[i * cols + j] / as_max_b, k) * 255.f, 0, pnm_maxval);

      img_ps_r[i * cols + j] =
        clamp(ps_r[i * cols + j], 0, pnm_maxval);
      img_ps_v[i * cols + j] =
        clamp(ps_v[i * cols + j], 0, pnm_maxval);
      img_ps_b[i * cols + j] =
        clamp(ps_b[i * cols + j], 0, pnm_maxval);
    }

  pnm_set_channel(imd_as, img_as_r, 0);
  pnm_set_channel(imd_as, img_as_v, 1);
  pnm_set_channel(imd_as, img_as_b, 2);

  pnm_set_channel(imd_ps, img_ps_r, 0);
  pnm_set_channel(imd_ps, img_ps_v, 1);
  pnm_set_channel(imd_ps, img_ps_b, 2);

  free(as_r);
  free(as_v);
  free(as_b);
  free(ps_r);
  free(ps_v);
  free(ps_b);
  fftw_free(freq_repr_r);
  fftw_free(freq_repr_v);
  fftw_free(freq_repr_b);
  free(rouge);
  free(vert);
  free(bleu);

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
  free(img_as_r);
  free(img_as_v);
  free(img_as_b);
  free(img_ps_r);
  free(img_ps_v);
  free(img_ps_b);

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

  long int cols = pnm_get_width(ims);
  long int rows = pnm_get_height(ims);

  float *as_r = (float*)malloc(rows * cols * sizeof(float));
  float *as_v = (float*)malloc(rows * cols * sizeof(float));
  float *as_b = (float*)malloc(rows * cols * sizeof(float));

  float *ps_r = (float*)malloc(rows * cols * sizeof(float));
  float *ps_v = (float*)malloc(rows * cols * sizeof(float));
  float *ps_b = (float*)malloc(rows * cols * sizeof(float));

  pnm imd_freq = pnm_new(cols, rows, PnmRawPpm);
  pnm imd_as = pnm_new(cols, rows, PnmRawPpm);

  unsigned short *rouge = malloc(rows * cols * sizeof(unsigned short));
  unsigned short *vert = malloc(rows * cols * sizeof(unsigned short));
  unsigned short *bleu = malloc(rows * cols * sizeof(unsigned short));

  pnm_get_channel(ims, rouge, 0);
  pnm_get_channel(ims, vert, 1);
  pnm_get_channel(ims, bleu, 2);

  fftw_complex *freq_repr_r = forward(rows, cols, rouge);
  fftw_complex *freq_repr_v = forward(rows, cols, vert);
  fftw_complex *freq_repr_b = forward(rows, cols, bleu);

  freq2spectra(rows, cols, freq_repr_r, as_r, ps_r);
  freq2spectra(rows, cols, freq_repr_v, as_v, ps_v);
  freq2spectra(rows, cols, freq_repr_b, as_b, ps_b);
  
  float as_max_r = find_max(rows * cols, as_r);
  float as_max_v = find_max(rows * cols, as_v);
  float as_max_b = find_max(rows * cols, as_b);
  float new_amp_r = as_max_r * .25f;
  float new_amp_v = as_max_v * .25f;
  float new_amp_b = as_max_b * .25f;

  int mid_i = rows / 2;
  int mid_j = cols / 2;

  // horizontale
  as_r[cols * mid_i + mid_j - 8] = new_amp_r;
  as_r[cols * mid_i + mid_j + 8] = new_amp_r;
  // verticale
  as_r[cols * (mid_i - 8) + mid_j] = new_amp_r;
  as_r[cols * (mid_i + 8) + mid_j] = new_amp_r;

  // horizontale
  as_r[cols * mid_i + mid_j - 8] = new_amp_v;
  as_r[cols * mid_i + mid_j + 8] = new_amp_v;
  // verticale
  as_r[cols * (mid_i - 8) + mid_j] = new_amp_v;
  as_r[cols * (mid_i + 8) + mid_j] = new_amp_v;

  // horizontale
  as_r[cols * mid_i + mid_j - 8] = new_amp_b;
  as_r[cols * mid_i + mid_j + 8] = new_amp_b;
  // verticale
  as_r[cols * (mid_i - 8) + mid_j] = new_amp_b;
  as_r[cols * (mid_i + 8) + mid_j] = new_amp_b;

  spectra2freq(rows, cols, as_r, ps_r, freq_repr_r);
  spectra2freq(rows, cols, as_v, ps_v, freq_repr_b);
  spectra2freq(rows, cols, as_b, ps_b, freq_repr_v);
  unsigned short* img_freq_r = backward(rows, cols, freq_repr_r);
  unsigned short* img_freq_v = backward(rows, cols, freq_repr_v);
  unsigned short* img_freq_b = backward(rows, cols, freq_repr_b);

  unsigned short* img_as_r = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_as_v = malloc(rows * cols * sizeof(unsigned short));
  unsigned short* img_as_b = malloc(rows * cols * sizeof(unsigned short));

  float k = 0.15f;
  as_max_r = find_max(rows * cols, as_r);
  as_max_v = find_max(rows * cols, as_v);
  as_max_b = find_max(rows * cols, as_b);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      img_as_r[i * cols + j] =
        clamp(pow(as_r[i * cols + j] / as_max_r, k) * 255.f, 0, pnm_maxval);
      img_as_v[i * cols + j] =
        clamp(pow(as_v[i * cols + j] / as_max_v, k) * 255.f, 0, pnm_maxval);
      img_as_b[i * cols + j] =
        clamp(pow(as_b[i * cols + j] / as_max_b, k) * 255.f, 0, pnm_maxval);
    }

  pnm_set_channel(imd_freq, img_freq_r, 0);
  pnm_set_channel(imd_freq, img_freq_v, 1);
  pnm_set_channel(imd_freq, img_freq_b, 2);

  pnm_set_channel(imd_as, img_as_r, 0);
  pnm_set_channel(imd_as, img_as_v, 1);
  pnm_set_channel(imd_as, img_as_b, 2);

  free(as_r);
  free(as_v);
  free(as_b);
  free(ps_r);
  free(ps_v);
  free(ps_b);
  fftw_free(freq_repr_r);
  fftw_free(freq_repr_v);
  fftw_free(freq_repr_b);
  free(rouge);
  free(vert);
  free(bleu);

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
  free(img_freq_r);
  free(img_freq_v);
  free(img_freq_b);
  free(img_as_r);
  free(img_as_v);
  free(img_as_b);

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
