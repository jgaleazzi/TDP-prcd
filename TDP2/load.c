#include "model.h"


int nb_lines (char *name){
  FILE* file = fopen(name, "r");
  if(!file){
    fprintf(stderr, "Cannot open file %s\n", name);                                   
  }
  int n=0, c;
  while ((c = fgetc(file)) != EOF){
    if (c == '\n')
      n++;
  }
  fclose(file);
  return n;
}


void load(Particle *particle,FILE* file, int nb_line){
  int i,err;
  for(i=0;i<nb_line;i++){
    CHK(err=  fscanf(file, "%lf %lf %lf %lf",&particle[i].mass,&particle[i].x,&particle[i].y,&particle[i].v),"scanf");
  
  }
}


