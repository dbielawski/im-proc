#include <math.h>

#include "morphology.h"

//enum {SQUARE, DIAMOND, DISK, LINE_V, DIAG_R, LINE_H, DIAG_L, CROSS, PLUS};

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
void diamond(pnm img, int hs)
{
  int size = 2 * hs + 1;
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
    {
      // en haut a gauche
      if (i <= hs && j <= hs)
      {
        if (j >= hs - i)
          pixels[i * size + j] = 255;
        else
          pixels[i * size + j] = 0;
      }

      // en bas a gauche
      if (i >= hs && j <= hs)
      {
        if (hs - j <  2 * hs - i)
          pixels[i * size + j] = 255;
        else
          pixels[i * size + j] = 0;
      }

      // en haut a droite
      if (i < hs && j > hs)
      {
        if (2 * hs - j >= hs - i)
          pixels[i * size + j] = 255;
        else
          pixels[i * size + j] = 0;
      }

      // en bas a droite
      if (i >= hs && j > hs)
      {
        if (i >= hs)
          pixels[i * size + j] = 255;
        else
          pixels[i * size + j] = 0;
      }
    }

  set_all_channels(img, pixels);
}

void disk(pnm img, int hs)
{
  int size = 2 * hs + 1;
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
    {
      int distance = sqrt((i - hs) * (i - hs) + (j - hs) * (j - hs));
      
      if (distance <= hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;
    }

  set_all_channels(img, pixels);
}

void line_v(pnm img, int hs)
{
  int size = 2 * hs + 1;
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (j == hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void diag_r(pnm img, int hs)
{
  int size = 2 * hs + 1;
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (size - j == i)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void line_h(pnm img, int hs)
{
  int size = 2 * hs + 1;
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (i == hs)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void diag_l(pnm img, int hs)
{
  int size = 2 * hs + 1;
  unsigned short* pixels = malloc(sizeof(unsigned short) * size * size);

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j)
      if (j == i)
        pixels[i * size + j] = 255;
      else
        pixels[i * size + j] = 0;

  set_all_channels(img, pixels);
}

void cross(pnm img, int hs)
{
  int size = 2 * hs + 1;
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

void plus(pnm img, int hs)
{
  int size = 2 * hs + 1;
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
  int size = 2 * hs + 1;
  pnm out = pnm_new(size, size, PnmRawPpm);

  if (s == 0)
    square(out, size);
  else if (s == 1)
    diamond(out, hs);
  else if (s == 2)
    disk(out, hs);
  else if (s == 3)
    line_v(out, hs);
  else if (s == 4)
    diag_r(out, hs);
  else if (s == 5)
    line_h(out, hs);
  else if (s == 6)
    diag_l(out, hs);
  else if (s == 7)
    cross(out, hs);
  else if (s == 8)
    plus(out, hs);
  else
    out = NULL;

  return out;
}

void 
lesser(unsigned short val, unsigned short* min){ 
  (void) val;
  (void) min;
}

void 
greater(unsigned short val, unsigned short* max){ 
  (void) val;
  (void) max;
}

void 
process(int s, 
	int hs, 
	pnm ims, 
	pnm imd, 
	void (*pf)(unsigned short, unsigned short*))
{  
  (void) s;
  (void) hs;
  (void) ims;
  (void) imd;
  (void) pf;
}
