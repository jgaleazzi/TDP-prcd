#ifndef __MODELE_H__
#define __MODELE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct _Particle {
  double mass;
  double x;
  double y;
  double v;
} Particle;

double compute_dt();

#endif
