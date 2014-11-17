#include "matrix.h"

void print_matrix(const double const *A, const int size){
  int i,j;
  for(i = 0; i < size; ++i){
    for(j = 0; j < size; ++j){
      printf("%lf ", A[j*size + i]);
    }
    printf("\n");
  }
}

void fill_matrix(double const *A, const int size, const double value){
  int i,j;
  for(i = 0; i < size; ++i){
    for(j = 0; j < size; ++j){
      printf("%lf ", A[j*size + i]);
    }
    printf("\n");
  }
}
