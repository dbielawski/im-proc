#include <float.h>
#include <stdlib.h>
#include <math.h>

#include "fft.h"

void centrage(int rows, int cols, fftw_complex* img)
{
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      if ((i + j) % 2 == 1)
        img[i * cols + j] *= -1;
}

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

  centrage(rows, cols, in);

  plan = fftw_plan_dft_2d(rows , cols, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  fftw_destroy_plan(plan);
  fftw_free(in);

  return out;
}

unsigned short 
*backward(int rows, int cols, fftw_complex* freq_repr, int factor)
{
  long int size = rows * cols;
  long int normalizeFactor = rows / factor * cols / factor;
  unsigned short *image = malloc(size * sizeof(unsigned short));

  fftw_plan plan;
  fftw_complex* out;
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);

  plan = fftw_plan_dft_2d(rows , cols, freq_repr, out,
    FFTW_BACKWARD,
    FFTW_ESTIMATE);

  fftw_execute(plan);

  centrage(rows, cols, out);

  for (int i = 0; i < size; ++i)
  {
    double real = creal(out[i]);
    real = (real / normalizeFactor);
    (void)factor;

    double v = real;
    if (real > 255)
      v = 255;
    else if (real < 0)
      v = 0;
    image[i] = (unsigned short)v;
  }

  fftw_destroy_plan(plan);
  fftw_free(out);

  return image;
}

void
freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as, float* ps)
{
  long int size = rows * cols;

  for (int i = 0; i < size; ++i)
  {
    as[i] = cabs(freq_repr[i]);
    ps[i] = carg(freq_repr[i]);
  }
}

void 
spectra2freq(int rows, int cols, float* as, float* ps, fftw_complex* freq_repr)
{
  long int size = rows * cols;

  for (int i = 0; i < size; ++i)
    freq_repr[i] = as[i] * cos(ps[i]) + I * as[i] * sin(ps[i]);
}
