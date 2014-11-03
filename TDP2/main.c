#include "model.h"


int main(int argc, char * argv[]){
  Particle * particle;
  Movement *move;
  int num_proc = 0;
  char *name = malloc(sizeof(char)*10);
  int err;
  sprintf(name,"proc_%d.txt",num_proc);   
  printf("%s\n",name);     
  int nb_part = nb_lines(name); 

  FILE* file = fopen(name, "r");                                                     
  if(!file){                                                                         
    fprintf(stderr, "Cannot open file %s\n", name);                                  
  }
  particle = malloc(nb_part*sizeof(Particle));
  move=malloc(nb_part*sizeof(Movement));
  load(particle,move,file,nb_part);
  fclose(file);

  printf("part :%lf %lf %lf %lf\n",particle[0].mass,particle[0].x,particle[0].y,particle[0].dist);
  printf("move: %lf %lf %lf %lf\n",move[0].v_x,move[0].v_y,move[0].a_x,move[0].a_y);

  free(particle);
  free(move);
  return 0;
}
