#include "load.h"
#include "mpi.h"

int main(int argc, char * argv[]){
  Particle * particles;
  Movement *moves;
  int myrank, comm_size;

  MPI_INIT(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  //TODO : adapter la taille du filename en fonction du nombre de digits                                                                                                         
  char *name = malloc(sizeof(char)*10);
  int err;


  sprintf(name,"proc_%d.txt",myrank);
  //DEBUG                                                                                                                                                                        
  //printf("%s\n",name);                                                                                                                                                         
  //DEBUG                                                                                                                                                                        

  nb_part = nb_lines(name);

  FILE* file = fopen(name, "r");
  if(!file){
    fprintf(stderr, "Cannot open file %s\n", name);
  }
  particles = malloc(nb_part*sizeof(Particle));
  moves=malloc(nb_part*sizeof(Movement));
  load(particles,moves,file,nb_part);
  fclose(file);

  //DEBUG                                                                                                                                                                        
  //printf("part :%lf %lf %lf %lf\n",particles[0].mass,particles[0].x,particles[0].y,particles[0].dist);                                                                         
  //printf("move: %lf %lf %lf %lf\n",moves[0].v_x,moves[0].v_y,moves[0].a_x,moves[0].a_y);                                                                                       
  //DEBUG                                                                                                                                                                        

  int buf_size = NB_PARTS%comm_size ? NB_PARTS/comm_size + 1 : NB_PARTS/comm_size;
  int buffer0[buf_size];
  int buffer1[buf_size];
  int i;
  int pic_counter = 1;
  for(i=0;i<NB_ITER;i++){

   int* send_p;
    int* rcv;
    int j = 0;

    //1st step : each proc computes on its own particles                                                                                                                         

    for(j = 0; j < comm_size; ++j)
      {
        if(j == 0)
          {

          }

      }

    gravitation(particles,moves);
    update_moves(particles, moves);
    update_dt(particles, moves);

    if(!(i%PIC_FREQ))
      {
        char output[10];
        sprintf(output, "%d_out%d.txt", myrank, pic_counter);
        FILE *file = fopen(output, "w+");
        int j;
        for(j=0; j<nb_part; j++){
          fprintf(file, "%lf %lf\n", particles[j].x, particles[j].y);
	}
        fclose(file);
	++pic_counter;
      }
  }
  free(particles);
  free(moves);

  MPI_Finalize();
  return 0;
}
