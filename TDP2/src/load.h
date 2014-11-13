#ifndef __LOAD_H__
#define __LOAD_H__

#include "model.h"

int nb_lines(char *name);

void load(Particle *particle,Movement* move, FILE* file, int nb_line);

#endif
