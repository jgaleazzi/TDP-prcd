#include <mpi.h>
#include <string.h>
#include "cblas.h"

#include "load.h"
#include "matrix.h"

int main(int agrc, char**argv){
  int myrank, nb_proc,nb_block_1D,size,block_size;
  int k,i,j;
  MPI_Status status;
  char *name_A="A.txt";
  char *name_B="B.txt";
  FILE* file = fopen(name_A, "r");

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);


  if(!file){
    fprintf(stderr, "Cannot open file %s\n", name_A);
  }
  fscanf(file,"%d %d",&size,&block_size);
  fclose(file);

  nb_block_1D=size/block_size;

  double *A;
  double *B;
  double *C;
  A=malloc(size*size*sizeof(double));
  B=malloc(size*size*sizeof(double));
  C=malloc(size*size*sizeof(double));


  if(myrank==0){
    load(name_A,A,size);
    load(name_B,B,size);
  }

  if(nb_block_1D * nb_block_1D != nb_proc){
    if(myrank == 0)
      puts("A square number of process is required\n");
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  int sendcounts[nb_proc];
  int displs[nb_proc];
  double local_A[block_size*block_size];
  double block_B[block_size*block_size];

  for( i = 0; i < nb_proc; i++) {
    sendcounts[i] = 1;
    displs[i] = (i%nb_block_1D) + (i/nb_block_1D) * nb_block_1D * block_size;
  }

  MPI_Datatype MATRIX_BLOC;
  MPI_Type_vector(block_size, block_size, nb_block_1D*block_size, MPI_DOUBLE, &MATRIX_BLOC);
  MPI_Type_create_resized(MATRIX_BLOC, 0, block_size * sizeof(double), &MATRIX_BLOC);
  MPI_Type_commit(&MATRIX_BLOC);
  
  MPI_Scatterv(A, sendcounts,displs, MATRIX_BLOC, local_A, block_size*block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatterv(B, sendcounts,displs, MATRIX_BLOC, block_B, block_size*block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  int dims[2]={nb_block_1D,nb_block_1D};
  int periods[2]={1,1};
  int reorder = 0;
  MPI_Comm grid2D;
  
  //Creation of a grid, in order to easily create line & column communicators
  MPI_Cart_create(MPI_COMM_WORLD, 2,dims,periods,reorder, &grid2D);
  
  int coord[2];
  int rank_loc;
  MPI_Cart_coords(grid2D,myrank,2,coord);
  MPI_Cart_rank(grid2D,coord,&rank_loc);

  //creation of line communicators
  //we keep only the lines, corresponding to the 1st dimension
  int remain_dims_lines[2] = {1,0};
  MPI_Comm comm_line;
  int local_line_rank;
  MPI_Cart_sub(grid2D, remain_dims_lines, &comm_line);
  MPI_Comm_rank(comm_line, &local_line_rank);

  //creation of column communicators
  //we keep only the columns, corresponding to the 1st dimension
  int remain_dims_column[2] = {0,1};
  MPI_Comm comm_column;
  int local_rank_column;
   MPI_Cart_sub(grid2D, remain_dims_column, &comm_column);
  MPI_Comm_rank(comm_column, &local_rank_column);

  MPI_Request send_req;
  MPI_Request recv_req;

  double bloc_C[block_size*block_size];
  double current_A[block_size*block_size];
  
  //MAIN LOOP
  double time_p;
  time_p = MPI_Wtime();
  for(k=0;k<nb_block_1D;k++){

    //STEP 1
    if(local_line_rank == (local_rank_column+k)%nb_block_1D){
      memcpy(current_A, local_A, block_size*block_size*sizeof(double));
    }
      MPI_Bcast(current_A, block_size*block_size, MPI_DOUBLE, (local_rank_column+k)%nb_block_1D, comm_line); 

    //STEP 2
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, block_size, block_size,
                 block_size, 1.0, current_A,
                 block_size, block_B, block_size,
                 1.0, bloc_C, block_size);

    //STEP 3
    //+nb_block_1D added in the modulo, because the C-modulo of a negative number returns a negative number  
    MPI_Sendrecv_replace(block_B, block_size*block_size, MPI_DOUBLE, (local_rank_column-1+nb_block_1D)%nb_block_1D, 100, (local_rank_column+1)%nb_block_1D, 100, comm_column, &status);
  }
  time_p = MPI_Wtime() - time_p;
  MPI_Gatherv(bloc_C, block_size*block_size, MPI_DOUBLE,
	      C, sendcounts, displs,MATRIX_BLOC, 0, MPI_COMM_WORLD);

  //ajout reduce
  if (myrank == 0){
    printf("Time on %d procs: %lf sec\n", nb_proc, time_p);
  }
  /*  if(myrank==0){
    print_matrix(C, block_size*nb_block_1D);
    }*/

  MPI_Type_free(& MATRIX_BLOC);
  MPI_Finalize();

  free(A);
  free(B);
  free(C);
  return 0;
}
