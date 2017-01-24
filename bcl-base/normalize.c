/**
 * @file 
 * @author 
 * @brief ???
 */

#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

int find_max(unsigned short* input, int n);
int find_min(unsigned short* input, int n);
int normalize(int min, int max, int min_image, int max_image, int value);
unsigned short* extractChannel(unsigned short* input, int rows, int cols, int numChannel);

void process(int min, int max, char* ims_name, char* imd_name)
{
	pnm ims = pnm_load(ims_name);
	unsigned short *image = pnm_get_image(ims);

	int rows = pnm_get_height(ims);
	int cols = pnm_get_width(ims);

	pnm imd = pnm_new(cols, rows, PnmRawPpm);
	unsigned short *output = pnm_get_image(imd);

	unsigned short *extractedRouge = extractChannel(image, rows, cols, 0);
	unsigned short *extractedVert = extractChannel(image, rows, cols, 1);
	unsigned short *extractedBleu = extractChannel(image, rows, cols, 2);

	int min_rouge = find_min(extractedRouge, cols * rows);
	int max_rouge = find_max(extractedRouge, cols * rows);
	int min_vert = find_min(extractedVert, cols * rows);
	int max_vert = find_max(extractedVert, cols * rows);
	int min_bleu = find_min(extractedBleu, cols * rows);
	int max_bleu = find_max(extractedBleu, cols * rows);

	// printf("min %i\n", min);
	// printf("max %i\n", max);
	// printf("min_rouge %i\n", min_rouge);
	// printf("max_rouge %i\n", max_rouge);
	// printf("min_vert %i\n", min_vert);
	// printf("max_vert %i\n", max_vert);
	// printf("min_bleu %i\n", min_bleu);
	// printf("max_bleu %i\n", max_bleu);

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
		{
			int r = 0;
			int v = 0;
			int b = 0;

			r = image[i * cols * 3 + j * 3 + 0];
			v = image[i * cols * 3 + j * 3 + 1];
			b = image[i * cols * 3 + j * 3 + 2];

			*output++ = normalize(min, max, min_rouge, max_rouge, r);
			*output++ = normalize(min, max, min_vert, max_vert, v);
			*output++ = normalize(min, max, min_bleu, max_bleu, b);
		}

	free(extractedRouge);
	free(extractedVert);
	free(extractedBleu);

	pnm_save(imd, PnmRawPpm, imd_name);
	pnm_free(imd);
}

void usage (char *s)
{
  fprintf(stderr, "Usage: %s <min> <max> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}


int find_max(unsigned short* input, int n)
{
	int max = 0;
	for (int i = 0; i < n; ++i)
	{
		if (max < input[i])
			max = input[i];
	}
	return max;
}

int find_min(unsigned short* input, int n)
{
	int min = pnm_maxval;
	for (int i = 0; i < n; ++i)
	{
		if (min > input[i])
			min = input[i];
	}
	return min;
}

int normalize(int min, int max, int min_image, int max_image, int value)
{
	// if (value == 0) return 0;
	if (min_image == max_image) return 0;

	int a = ((max - min) / (max_image - min_image)) * value;
	int b = ((min * max_image) - (max * min_image)) / (max_image - min_image);
	return a + b;
}

// input: vecteur de taille rows * cols * 3
// retourne un vecteur de taille rows * cols, contenant le canal extrait
unsigned short *extractChannel(unsigned short* input, int rows, int cols, int numChannel)
{
	unsigned short *output = malloc(sizeof(unsigned short)*rows * cols);

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
		{
			output[i] = input[i * cols * 3 + j * 3 + numChannel];
		}
	return output;
}

#define PARAMS 4
int main(int argc, char *argv[])
{
  if (argc != PARAMS+1){
    usage(argv[0]);
  }
  /* code to modify */
  int min = atoi(argv[1]);
  int max = atoi(argv[2]);
  char *ims = argv[3];
  char *imd = argv[4];
 
  process(min, max, ims, imd);
  return EXIT_SUCCESS;
}