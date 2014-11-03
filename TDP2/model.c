#include "model.h"

double compute_norm(Particle *partA, Particle *partB){
  double dx = (partB->x-partA->x)*(partB->x-partA->x);
  double dy = (partB->y-partA->y)*(partB->y-partA->y);
  double dist = sqrt(dx + dy);
  return dist;
}

void compute_gravitation(Particle *partA, Particle *partB, Movement *moveA){
  double norm =  compute_norm(partA,partB);
  double force = G* partB->mass/(norm*norm);
  //decomposition on x and y (unit vector)
  double vect_x = (partB->x - partA->x)/norm;
  double vect_y=(partB->y - partA->y)/norm;

  moveA->a_x += force*vect_x;
  moveA->a_y += force*vect_y;
}

void gravitation(Particle *parts, Movement *moves, int nb_part){
  int i,j;
  //describe all the parts
  for(i=0;i<nb_part;i++){
    for(j=0;j<nb_part;i++){
      if(i!=j)
	compute_gravitation(&parts[i],&parts[j],&moves[i]);
    }
  }
}


void update_moves(Particle *parts, Movement *moves, int nb_part){
  int i;
  for(i=0;i<nb_part;i++){
    moves[i]->v_x += moves[i]->a_x*dt;
    moves[i]->v_y += moves[i]->a_y*dt;
    parts[i]->x+= moves[i]->v_x*dt +  moves[i]->a_x*(dt*dt)/2;
    parts[i]->y+= moves[i]->v_y*dt +  moves[i]->a_y*(dt*dt)/2;
  }
}
