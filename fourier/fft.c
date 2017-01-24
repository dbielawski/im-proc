#include <float.h>
#include <stdlib.h>
#include <math.h>

#include "fft.h"

fftw_complex
*forward(int rows, int cols, unsigned short* g_img)
{
  fftw_complex* in;
  fftw_complex* out;
  fftw_plan plan;

  long int size = rows * cols;

  in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);

  for (long int i = 0; i < size; ++i)
  {
    in[i] = g_img[i];
  }

  plan = fftw_plan_dft_2d(rows , cols, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  fftw_destroy_plan(plan);
  fftw_free(in);

  return out;
}

unsigned short 
*backward(int rows, int cols, fftw_complex* freq_repr)
{
  long int size = rows * cols;
  unsigned short *img = malloc(size * sizeof(unsigned short));

  fftw_plan plan;
  fftw_complex* out;
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);

  plan = fftw_plan_dft_2d(rows , cols, freq_repr, out, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  for (int i = 0; i < size; ++i)
  {
    double real = creal(out[i]);
    img[i] = (unsigned short)(real / size);
  }

  fftw_destroy_plan(plan);
  fftw_free(out);

  return img;
}

void
freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as, float* ps) 
{
  (void)rows;
  (void)cols;
  (void)freq_repr;
  (void)as;
  (void)ps;
}


void 
spectra2freq(int rows, int cols, float* as, float* ps, fftw_complex* freq_repr)
{
  (void)rows;
  (void)cols;
  (void)freq_repr;
  (void)as;
  (void)ps;
}
