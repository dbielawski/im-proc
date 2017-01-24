/**
 * @file print-ppm.c
 * @brief print to standard output a color ppm file
 */

#include <stdlib.h>
#include <stdio.h>

static void
process(int r, int g, int b, int rows, int cols){

  printf("P3\n");
  printf("%i %i\n", cols, rows);
  printf("255\n");
  for (int i = 0; i < rows * cols; ++i)
  {
    printf("%i %i %i ", r, g, b);
    if (0 == ((i + 1) % cols))
      printf("\n");
  }
  printf("\n");
}

void 
usage (char *s)
{
  fprintf(stderr,
	  "Usage: %s <r={0,255}> <g={0,255}> <b={0,255}> <rows> <cols>\n"
	  ,s);
  exit(EXIT_FAILURE);
}

#define PARAMS 5
int 
main(int argc, char *argv[])
{
  if (argc != PARAMS+1){
    usage(argv[0]);
  }
  /* code to modify */
  int r=atoi(argv[1]), g=atoi(argv[2]), b=atoi(argv[3]), rows=atoi(argv[4]), cols=atoi(argv[5]);
 
  process(r, g, b, rows, cols);
  return EXIT_SUCCESS;
}


