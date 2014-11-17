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
  int myrank, nb_proc,nb_bloc_1D,size,bloc_size;
  int k,i,j;
  //MPI_Status status;
  char *name_A="A.txt";
  char *name_B="B.txt";
  FILE* file = fopen(name_A, "r");
  if(!file){
    fprintf(stderr, "Cannot open file %s\n", name_A);
  }
  
  fscanf(file,"%d %d",&size,&bloc_size);
  fclose(file);


  nb_bloc_1D=size/bloc_size;
  printf("bloc_size: %d nb_bloc_1: %d\n",bloc_size,nb_bloc_1D);
  double* A = malloc(size*size*sizeof(double));
  double* B = malloc(size*size*sizeof(double));
  double* C = malloc(size*size*sizeof(double));
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);



  if(myrank==0){
    load(name_A,A,size);
    load(name_B,B,size);
    //    print_matrix(A,size);
  }


  if(nb_bloc_1D * nb_bloc_1D != nb_proc){
    if(myrank == 0)
      puts("I need a square number of processes !");
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  int sendcounts[nb_proc];
  int displs[nb_proc];
  double matrix_local_A[bloc_size*bloc_size];
  double matrix_local_B[bloc_size*bloc_size];

  for( i = 0; i < nb_proc; i++) {
    sendcounts[i] = 1;
    displs[i] = (i%nb_bloc_1D) + (i/nb_bloc_1D) * nb_bloc_1D * bloc_size;
    if(myrank == 0)
      printf("%d\n", displs[i]);
  }


  MPI_Datatype MATRIX_BLOC;
  MPI_Type_vector(bloc_size, bloc_size, nb_bloc_1D*bloc_size, MPI_DOUBLE, &MATRIX_BLOC);
  MPI_Type_create_resized(MATRIX_BLOC, 0, bloc_size * sizeof(double), &MATRIX_BLOC);
  MPI_Type_commit(&MATRIX_BLOC);

  
  MPI_Scatterv(A, sendcounts,displs, MATRIX_BLOC, matrix_local_A, bloc_size*bloc_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatterv(B, sendcounts,displs, MATRIX_BLOC, matrix_local_B, bloc_size*bloc_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  //  print_matrix(matrix_local_A,bloc_size);

  int dims[2]={nb_bloc_1D,nb_bloc_1D};
  int periods[2]={1,0};
  int reorder = 0;
  MPI_Comm grid2D;
  
  MPI_Cart_create(MPI_COMM_WORLD, 2,dims,periods,reorder, &grid2D);
  
  int coord[2];
  int rank_loc;
  MPI_Cart_coords(grid2D,myrank,2,coord);
  MPI_Cart_rank(grid2D,coord,&rank_loc);
  //  printf("je suis le procs %d et coord (%d;%d) \n",myrank,coord[0],coord[1]);

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

  double matrix_recv_A[bloc_size*bloc_size];
  double matrix_recv_B[bloc_size*bloc_size];
  
  for(k=0;k<nb_bloc_1D;k++){
    //STEP 1
    //loop on the lines
    for(i=0;i<nb_bloc_1D;i++){
	MPI_Bcast(matrix_recv_A, 1, MATRIX_BLOC, (i+k)%nb_bloc_1D, comm_line);
      }
    
    //STEP 2
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, const int M, const int N,
                 const int K, 1.0, matrix_recv_A,
                 const int lda, const double *B, const int ldb,
                 1.0, C, const int ldc);

    //STEP 3
  }
  


  MPI_Finalize();

  free(A);
  free(B);
  free(C);
  return 0;
}
