double compute_distance(Particle *partA, Particle *partB){
  double dx = (partB.x-partA.x)*(partB.x-partA.x);
  double dy = (partB.y-partA.y)*(partB.y-partA.y);
  double dist = sqrt(dx + dy);
  return dist;
}


