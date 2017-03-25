#include <math.h>

#include "morphology.h"

enum {SQUARE, DIAMOND, DISK, LINE_V, DIAG_R, LINE_H, DIAG_L, CROSS, PLUS};

void set_all_channels(pnm im, unsigned short* buffer)
{
  for (int i = 0; i < 3; ++i)
    pnm_set_channel(im, buffer, i);
}

void square(pnm img, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      pixels[i * size + j] = 255;

  set_all_channels(img, pixels);
}

// FIXME:
void diamond(pnm img, int hs, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
    {
      int distance = abs(j - hs) + abs(i - hs);

      if (distance < hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;
    }
  
  set_all_channels(img, pixels);
}

void disk(pnm img, int hs, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
    {
      float distance = sqrt((i - hs) * (i - hs) + (j - hs) * (j - hs));
      
      if (distance <= hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;
    }

  set_all_channels(img, pixels);
}

void line_v(pnm img, int hs, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (j == hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void diag_r(pnm img, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (size - j == i)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void line_h(pnm img, int hs, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (i == hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void diag_l(pnm img, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (j == i)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void cross(pnm img, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
    {
      if (j == i)
        pixels[i * size + j] = 255;
      else if (size - j == i)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;      
    }

  set_all_channels(img, pixels);
}

void plus(pnm img, int hs, int size)
{
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
    {
      if (i == hs)
        pixels[i * size + j] = 255;
      else  if (j == hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;
    }

  set_all_channels(img, pixels);
}

pnm
se(int s, int hs){
  int size = (2 * hs + 1) * (2 * hs + 1);
  hs = size / 2;
  pnm out = pnm_new(size, size, PnmRawPpm);

  if (s == SQUARE)
    square(out, size);
  else if (s == DIAMOND)
    diamond(out, hs, size);
  else if (s == DISK)
    disk(out, hs, size);
  else if (s == LINE_V)
    line_v(out, hs, size);
  else if (s == DIAG_R)
    diag_r(out, size);
  else if (s == LINE_H)
    line_h(out, hs, size);
  else if (s == DIAG_L)
    diag_l(out, size);
  else if (s == CROSS)
    cross(out, size);
  else if (s == PLUS)
    plus(out, hs, size);
  else
    out = NULL;

  return out;
}

void 
lesser(unsigned short val, unsigned short* min){ 
  if (*min > val)
    *min = val;
}

void 
greater(unsigned short val, unsigned short* max){ 
  if (*max < val)
    *max = val;
}

void 
process(int s, 
	int hs, 
	pnm ims, // input image
	pnm imd, // output image
	void (*pf)(unsigned short, unsigned short*))
{
  fprintf(stderr, "process: ");
  pnm struct_elem = se(s, hs);

  if (struct_elem == NULL)
  {
    printf("ERROR: structing element is null\n");
    exit(-1);
  }

  int size = (2 * hs + 1) * (2 * hs + 1);
  hs = size / 2;
  int height = pnm_get_height(ims);
  int width = pnm_get_width(ims);

  unsigned short* input_image = pnm_get_channel(ims, NULL, 0);
  unsigned short* out_image = malloc(sizeof(unsigned short) * width * height);
  unsigned short* st_image = pnm_get_channel(struct_elem, NULL, 0);

  for (int i = 0; i < height; ++i)
    for (int j = 0; j < width; ++j)
    {
      unsigned short value = input_image[i * width + j];

      for (int k = 0; k < size; ++k)
      {
        for (int l = 0; l < size; ++l)
        {
          if (st_image[k * size + l] == 255)
          {
            // Pourquoi - et pas + ?
            int new_i = i + k - hs;
            int new_j = j + l - hs;

            if (new_i >= 0 && new_i < width && new_j >= 0 && new_j < height)
              pf(input_image[new_i * width + new_j], &value);
          }
        }
      }

      out_image[i * width + j] = value;
    }

  set_all_channels(imd, out_image);
  
  free(out_image);
  free(st_image);
  fprintf(stderr, "OK\n");
}
