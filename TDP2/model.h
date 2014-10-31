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

//computes the new dt by solving a degree-2 polynom
double compute_dt(const double acceleration_norm, const double speed_norm, double minimal_distance);

#endif
