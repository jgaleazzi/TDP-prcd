
#include "load.h"
#include <mpi.h>

int main(int argc, char * argv[]){
  Particle * particles_owned, *buffer_1, *buffer_2;
  Movement *moves;
  int myrank, comm_size;
  MPI_Status status;

  MPI_Init(NULL, NULL);
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
  particles_owned = malloc(nb_part*sizeof(Particle));
  buffer_1 = malloc(nb_part*sizeof(Particle));
  buffer_2 = malloc(nb_part*sizeof(Particle));
  moves = malloc(nb_part*sizeof(Movement));
  load(particles_owned,moves,file,nb_part);
  fclose(file);

  //DEBUG                                                                                                                                                                        
  //  printf("part :%lf %lf %lf %lf\n",particles_owned[0].mass,particles_owned[0].x,particles_owned[0].y,particles_owned[0].dist);                                                                         
  //printf("move: %lf %lf %lf %lf\n",moves[0].v_x,moves[0].v_y,moves[0].a_x,moves[0].a_y);                                                                                       
  //DEBUG                                                                                                                                                                        

  int array_of_lengths[] = { 1, 1, 1, 1};
  MPI_Aint array_of_displs[4];
  MPI_Aint i1,i2;

  MPI_Get_address(&particles_owned[0],&i1);
  MPI_Get_address(&particles_owned[0].mass,&i2);
  array_of_displs[0]= i2 - i1;
  MPI_Get_address(&particles_owned[0].x,&i2);
  array_of_displs[1]= i2 - i1;
  MPI_Get_address(&particles_owned[0].y,&i2);
  array_of_displs[2]= i2 - i1;
  MPI_Get_address(&particles_owned[0].dist,&i2);
  array_of_displs[3]= i2 - i1;

  MPI_Datatype array_of_types[] = { MPI_DOUBLE, MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE};
  MPI_Datatype A_PARTICLE;
  MPI_Type_struct( 4,array_of_lengths,array_of_displs,array_of_types,&A_PARTICLE);
  MPI_Type_commit(&A_PARTICLE);

  MPI_Datatype PARTICLES;
  MPI_Type_vector(nb_part, 1, 1, A_PARTICLE, &PARTICLES);
  MPI_Type_commit(&PARTICLES);
  

  /*initialisation persistant communication*/
  MPI_Request buffer_send1, buffer_send2;
  MPI_Request buffer_recv1, buffer_recv2;
  MPI_Send_init(buffer_1, 1 ,PARTICLES , (myrank+1)%comm_size, 99, MPI_COMM_WORLD, &buffer_send1 );
  MPI_Recv_init(buffer_2, 1, PARTICLES,(myrank-1)%comm_size, 99, MPI_COMM_WORLD, &buffer_recv1);
  MPI_Send_init(buffer_2, 1 ,PARTICLES , (myrank+1)%comm_size, 99, MPI_COMM_WORLD, &buffer_send2 );
  MPI_Recv_init(buffer_1, 1, PARTICLES,(myrank-1)%comm_size, 99, MPI_COMM_WORLD, &buffer_recv2);

  int i,ring_index;
  for(i=0;i<NB_ITER;i++){
    //printf("myrank debut de boulce %d\n",myrank);    
    memcpy(buffer_1, particles_owned, nb_part*sizeof(Particle));
    for(ring_index;ring_index<comm_size; ring_index++){
      /* iteration number in ring  even*/
      if(ring_index%2==0){
	MPI_Start(&buffer_send1);
	MPI_Start(&buffer_recv1);
	if(ring_index==0){
	  gravitation_inter(particles_owned,moves);
	}
	else{
	  gravitation(particles_owned,buffer_1,moves);
	}
	MPI_Wait(&buffer_send1, &status);
	MPI_Wait(&buffer_recv1, &status);
      }
      /* iteration number in ring odd*/
      else{
	MPI_Start(&buffer_send2);
	MPI_Start(&buffer_recv2);
	gravitation(particles_owned,buffer_1,moves);
	
	MPI_Wait(&buffer_send2, &status);
	MPI_Wait(&buffer_recv2, &status);
      
      }    
    }
    update_moves(particles_owned, moves);

  
    char output[10];
    //    printf("myrank avant ecriture de boulce %d\n",myrank);

    sprintf(output, "test/%d_out.txt", myrank);
    FILE *file = fopen(output, "a");
    int j;
    for(j=0; j<nb_part; j++){
      fprintf(file, "%lf %lf\n", particles_owned[j].x, particles_owned[j].y);
      }
    double my_dt = update_dt(particles_owned, moves);
    MPI_Allreduce(&my_dt, &dt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    fclose(file);

    //   printf("myrank fin de boulce %d\n",myrank);
  }
 
  MPI_Type_free(&A_PARTICLE);
  MPI_Type_free(&PARTICLES);
  MPI_Finalize();
 

  free(particles_owned);
  free(moves);
  free(buffer_1);
  free(buffer_2);
  return 0;
}
  
