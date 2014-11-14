#include <mpi.h>
#include "cblas.h"
#include "load.h"



void print_matrix(double *A, int size){
  int i,j;
  for(i = 0; i < size; ++i){
      for(j = 0; j < size; ++j){
	  printf("%lf ", A[j*size + i]);
	}
      printf("\n");
    }
}

int main(int agrc, char**argv){
  int myrank, comm_size,nb_bloc_1D,size,bloc_size;
  //MPI_Status status;
  char *name_A="A.txt";
  char *name_B="B.txt";
  FILE* file = fopen(name_A, "r");
  if(!file){
    fprintf(stderr, "Cannot open file %s\n", name_A);
  }
  
  fscanf(file,"%d %d",&size,&nb_bloc_1D);
  fclose(file);
  bloc_size=size/nb_bloc_1D;
  double* A = malloc(size*size*sizeof(double));
  double* B = malloc(size*size*sizeof(double));
  
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);


  if(myrank==0){

    load(name_A,A,size);
    load(name_B,B,size);
    //print_matrix(A,size);
  }
  


  int dims[2]={nb_bloc_1D,nb_bloc_1D};
  int periods[2]={1,0};
  int reorder = 0;
  MPI_Comm grid2D;
  
  MPI_Cart_create(MPI_COMM_WORLD, 2,dims,periods,reorder, &grid2D);
  
  int coord[2];
  int rank_loc;
  MPI_Cart_coords(grid2D,myrank,2,coord);
  MPI_Cart_rank(grid2D,coord,&rank_loc);
  printf("je suis le procs %d et coord (%d;%d) \n",myrank,coord[0],coord[1]);

  //we keep only the lines, corresponding to the 1st dimension
  int remain_dims_lines[2] = {0,1};
  MPI_Comm comm_line;
  int rang_local_ligne;
  MPI_Cart_sub( grid2D, remain_dims_lines, &comm_line);
  MPI_Comm_rank(comm_line, &rang_local_ligne);

  int remain_dims_column[2] = {0,1};
  MPI_Comm comm_column;
  int rang_local_colonne;
  MPI_Cart_sub( grid2D, remain_dims_column, &comm_column);
  MPI_Comm_rank(comm_column, &rang_local_colonne);

  int k,i,j;
  for(k=0;k<nb_bloc_1D;k++){
    if((rang_local_colonne == i) && (rang_local_ligne == (i+k)%nb_bloc_1D)){
      //MPI_Bcast(, bloc_size * bloc_size, MPI_DOUBLE, rang_local_ligne, comm_line);
  }

    // faire produit

    //3eme etape boucle 

  }





  MPI_Finalize();

  free(A);
  free(B);
  return 0;
}
