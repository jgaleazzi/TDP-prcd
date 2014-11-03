#include "model.h"


int main(int argc, char * argv[]){
  Particle * particles;
  Movement *moves;
  int num_proc = 0;
  char *name = malloc(sizeof(char)*10);
  int err;
  sprintf(name,"proc_%d.txt",num_proc);   
  printf("%s\n",name);     
  nb_part = nb_lines(name); 

  FILE* file = fopen(name, "r");                                                     
  if(!file){                                                                         
    fprintf(stderr, "Cannot open file %s\n", name);                                  
  }
  particles = malloc(nb_part*sizeof(Particle));
  moves=malloc(nb_part*sizeof(Movement));
  load(particles,moves,file,nb_part);
  fclose(file);

  printf("part :%lf %lf %lf %lf\n",particles[0].mass,particles[0].x,particles[0].y,particles[0].dist);
  printf("move: %lf %lf %lf %lf\n",moves[0].v_x,moves[0].v_y,moves[0].a_x,moves[0].a_y);


  int i;
  for(i=0;i<10;i++){
    
    gravitation(particles,moves);
    update_moves(particles, moves);
    update_dt(particles, moves);

    char output[10];
    sprintf(output, "%d_out%d.txt", num_proc, i+1);
    FILE *file = fopen(output, "w+");
    int j;
    for(j=0; j<nb_part; j++){
      fprintf(file, "%lf %lf\n", particles[j].x, particles[j].y);

    }
    fclose(file);
  }
  free(particles);
  free(moves);
  return 0;
}
