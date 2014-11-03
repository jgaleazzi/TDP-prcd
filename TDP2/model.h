#ifndef __MODELE_H__
#define __MODELE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define CHK(X, Y) do { if ((X)<0) {perror(Y); exit(1);} } while(0)
#define G 1.0 //gravitation coefficient
#define DT_max 0.1

double dt;
int nb_part;
typedef struct _Particle {
  double mass;
  double x;
  double y;
  double dist;
} Particle;

typedef struct _Movement{
  double v_x;
  double v_y;
  double a_x;
  double a_y;
}Movement;

//computes the new dt by solving a degree-2 polynom
double compute_dt(const double acceleration_norm, const double speed_norm, double minimal_distance);

//computes distance between 2 part
double compute_norm(Particle *partA, Particle *partB);

//compute the gravitation force between two parts
void compute_gravitation(Particle *partA, Particle *partB, Movement *moveA);

//compute the gravitation force for each part
void gravitation(Particle *parts, Movement *moves);

//update of the speed and positions
void update_moves(Particle *parts, Movement *moves);

//update dt
void update_dt(Particle *parts, Movement *moves);

double min(double a, double b);
#endif
