#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pnm.h>


static void 
process(int sigma, char* ims_name, char* imd_name)
{

}

void usage (char *s){
  fprintf(stderr, "Usage: %s <sigma> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[]){
  if (argc != PARAM+1) 
    usage(argv[0]);

  int   idx       = 1;
  int   sigma  = atoi(argv[idx++]);
  char* ims       = argv[idx++];
  char* imd       = argv[idx++];

  //process(halfsize, ims, imd);

  return EXIT_SUCCESS;
}