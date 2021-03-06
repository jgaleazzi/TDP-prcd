#include "model.h"

double compute_norm(Particle *partA, Particle *partB){
  double dx = (partB->x-partA->x)*(partB->x-partA->x);
  double dy = (partB->y-partA->y)*(partB->y-partA->y);
  double dist = sqrt(dx + dy);
  return dist;
}

void compute_gravitation(Particle *partA, Particle *partB, Movement *moveA){
  double norm =  compute_norm(partA,partB);
  if(norm == 0.0)
    return;

  if (partA->dist == 0.0)
    {
      partA->dist = norm;
    }
  partA->dist = my_min(partA->dist,norm);
  double force = G* partB->mass/(norm*norm);
  //decomposition on x and y (unit vector)
  double vect_x = (partB->x - partA->x)/norm;
  double vect_y= (partB->y - partA->y)/norm;

  moveA->a_x += force*vect_x;
  moveA->a_y += force*vect_y;

}

void gravitation(Particle *partsA,Particle *partsB, Movement *moves){
  int i,j;
  //describe all the parts
  for(i=0;i<nb_part;i++){
    for(j=0;j<nb_part;j++){
      compute_gravitation(&partsA[i],&partsB[j],&moves[i]);
    }
  }
}

void gravitation_inter(Particle *parts, Movement *moves){
  int i,j;
  //describe all the parts
  for(i=0;i<nb_part;i++){
    for(j=0;j<nb_part;j++){
      if(i!=j)
       	compute_gravitation(&parts[i],&parts[j],&moves[i]);
    }
  }
}

void update_moves(Particle *parts, Movement *moves){
  int i;
  for(i=0;i<nb_part;i++){
    moves[i].v_x += moves[i].a_x*dt;
    moves[i].v_y += moves[i].a_y*dt;
    parts[i].x+= moves[i].v_x*dt +  moves[i].a_x*(dt*dt)/2;
    parts[i].y+= moves[i].v_y*dt +  moves[i].a_y*(dt*dt)/2;
  }
}

double update_dt(Particle *parts, Movement *moves){
  /* Solve for each part
     AX²/2+BX-0.1*C=0
     where A = ||acc_part ||
     B = || vit_part ||
     C = part_distmin
  */

  double A, B, C,delta,new_dt;
  new_dt=DT_max;
  double dt_tmp = new_dt;
  int i;
  for(i=0;i<nb_part;i++){
    A = sqrt(moves[i].a_x*moves[i].a_x + moves[i].a_y*moves[i].a_y); 
    B = sqrt(moves[i].v_x*moves[i].v_x + moves[i].v_y*moves[i].v_y); 
    C = parts[i].dist;
    delta = B*B + 0.2 * A*C;
    
    dt_tmp = (sqrt(delta)- B)/A;
 
   
    new_dt = my_min(new_dt,dt_tmp); 
    parts[i].dist = 0.0;
  }

  return new_dt;
}

double my_min(double a, double b){
  if (a<b)
    return a;
  return b;
}
