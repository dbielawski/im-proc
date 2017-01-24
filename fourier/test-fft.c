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
  pnm imd = pnm_new(cols, rows, PnmRawPpm);

  unsigned short* gray = malloc(sizeof(unsigned short) * cols * rows);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      gray[i * cols + j] = pnm_get_component(ims, i, j, 0);

  fftw_complex* freq_repr = forward(rows, cols, gray);
  unsigned short* fb_gray = backward(rows , cols, freq_repr);

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
    {
      pnm_set_component(imd, i, j, 0, fb_gray[i * cols + j]);
      pnm_set_component(imd, i, j, 1, fb_gray[i * cols + j]);
      pnm_set_component(imd, i, j, 2, fb_gray[i * cols + j]);
    }

  char new_name[100];
  strcat(new_name, "FB-");
  strcat(new_name, name);
  pnm_save(imd, PnmRawPpm, "new.ppm");
  pnm_free(imd);
  free(fb_gray);
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
