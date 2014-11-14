#include "load.h"

/* file loads datas from txt files*/
void load(const char *name, double *matrix, int size){
  FILE* file = fopen(name, "r");
  if(!file){
    fprintf(stderr, "Cannot open file %s\n", name);
  }
  int tmp,tmp1;
  fscanf(file,"%d %d",&tmp,&tmp1);

  int i,j, err;
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      //Column Major
      CHK(err= fscanf(file, "%lf ",&matrix[j*size + i]),"scanf");
    }
  }
  fclose(file);
}
