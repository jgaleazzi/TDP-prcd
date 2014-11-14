#ifndef __LOAD_H__
#define __LOAD_H_

#include <stdio.h>
#include <stdlib.h>

#define CHK(X, Y) do { if ((X)<0) {perror(Y); exit(1);} } while(0)

void load(const char *name, double *matrix,int size);

#endif
