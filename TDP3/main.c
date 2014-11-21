#include <mpi.h>
#include <string.h>
#include "cblas.h"

#include "load.h"
#include "matrix.h"

int main(int agrc, char**argv){
  int myrank, nb_proc,nb_bloc_1D,size,bloc_size;
  int k,i,j;
  MPI_Status status;
  char *name_A="A.txt";
  char *name_B="B.txt";
  FILE* file = fopen(name_A, "r");
  if(!file){
    fprintf(stderr, "Cannot open file %s\n", name_A);
  }
  
  fscanf(file,"%d %d",&size,&bloc_size);
  fclose(file);


  nb_bloc_1D=size/bloc_size;
  double A[size*size];
  double B[size*size];
  double C[size*size];
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

  //DEBUG
  //(myrank != 0) ? : fprintf(stderr, "size : %d, bloc_size : %d\n", size, bloc_size);
  //DEBUG

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
  double local_A[bloc_size*bloc_size];
  double recv_B[bloc_size*bloc_size];

  for( i = 0; i < nb_proc; i++) {
    sendcounts[i] = 1;
    displs[i] = (i%nb_bloc_1D) + (i/nb_bloc_1D) * nb_bloc_1D * bloc_size;
  }


  MPI_Datatype MATRIX_BLOC;
  MPI_Type_vector(bloc_size, bloc_size, nb_bloc_1D*bloc_size, MPI_DOUBLE, &MATRIX_BLOC);
  MPI_Type_create_resized(MATRIX_BLOC, 0, bloc_size * sizeof(double), &MATRIX_BLOC);
  MPI_Type_commit(&MATRIX_BLOC);

  
  MPI_Scatterv(A, sendcounts,displs, MATRIX_BLOC, local_A, bloc_size*bloc_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatterv(B, sendcounts,displs, MATRIX_BLOC, recv_B, bloc_size*bloc_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  //  print_matrix(local_A,bloc_size);

  int dims[2]={nb_bloc_1D,nb_bloc_1D};
  int periods[2]={1,1};
  int reorder = 0;
  MPI_Comm grid2D;
  
  MPI_Cart_create(MPI_COMM_WORLD, 2,dims,periods,reorder, &grid2D);
  
  int coord[2];
  int rank_loc;
  MPI_Cart_coords(grid2D,myrank,2,coord);
  MPI_Cart_rank(grid2D,coord,&rank_loc);
  //  printf("je suis le procs %d et coord (%d;%d) \n",myrank,coord[0],coord[1]);

  //we keep only the lines, corresponding to the 1st dimension
  int remain_dims_lines[2] = {1,0};
  MPI_Comm comm_line;
  int rang_local_ligne;
  MPI_Cart_sub(grid2D, remain_dims_lines, &comm_line);
  MPI_Comm_rank(comm_line, &rang_local_ligne);

  int remain_dims_column[2] = {0,1};
  MPI_Comm comm_column;
  int rang_local_colonne;
   MPI_Cart_sub(grid2D, remain_dims_column, &comm_column);
  MPI_Comm_rank(comm_column, &rang_local_colonne);

  MPI_Request send_req;
  MPI_Request recv_req;

  double bloc_C[bloc_size*bloc_size];
  double current_A[bloc_size*bloc_size];
  double send_B[bloc_size*bloc_size];
  
  
  
  for(k=0;k<nb_bloc_1D;k++){ 
    //STEP 1
    if(rang_local_ligne == (rang_local_colonne+k)%nb_bloc_1D){
      memcpy(current_A, local_A, bloc_size*bloc_size*sizeof(double));
      /*if(rang_local_colonne == 0){
	printf("k=%d, myrank=%d, rang_local_colonne=%d, rang_local_ligne=%d, about to broadcast :\n", k, myrank, rang_local_colonne, rang_local_ligne);
	print_matrix(current_A, bloc_size);
	}*/
      MPI_Bcast(current_A, bloc_size*bloc_size, MPI_DOUBLE, (rang_local_colonne+k)%nb_bloc_1D, comm_line);
    }
    else{
      MPI_Bcast(current_A, bloc_size*bloc_size, MPI_DOUBLE, (rang_local_colonne+k)%nb_bloc_1D, comm_line);
      /*if(myrank==0){
	printf("k=%d, just received a broadcast :\n", k);
	print_matrix(current_A, bloc_size);
      }*/
    }
 
    //STEP 2
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, bloc_size, bloc_size,
                 bloc_size, 1.0, current_A,
                 bloc_size, recv_B, bloc_size,
                 1.0, bloc_C, bloc_size);

    //STEP 3
    //+nb_bloc_1D added in the modulo, because the C-modulo of a negative number returns a negative number  
    //TODO : utiliser MPI_Sendrecv_replace pour n'utiliser qu'un seul buffer
    memcpy(send_B, recv_B, bloc_size*bloc_size*sizeof(double));
    MPI_Sendrecv(send_B, bloc_size*bloc_size, MPI_DOUBLE, (rang_local_colonne-1+nb_bloc_1D)%nb_bloc_1D, 100, recv_B, bloc_size*bloc_size, MPI_DOUBLE, (rang_local_colonne+1)%nb_bloc_1D, 100, comm_column, &status);


  }

  
  MPI_Gatherv(bloc_C, bloc_size*bloc_size, MPI_DOUBLE,
	      C, sendcounts, displs,MATRIX_BLOC, 0, MPI_COMM_WORLD);

  if(myrank==0){
    print_matrix(C, bloc_size*nb_bloc_1D);
  }

  MPI_Type_free(& MATRIX_BLOC);
  MPI_Finalize();
  return 0;
}
