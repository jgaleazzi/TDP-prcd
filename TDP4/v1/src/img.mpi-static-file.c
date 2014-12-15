/***************************************************************************
 *                       Lanceur de rayons simplifie                       *
 ***************************************************************************
 * ENSERB, 3me annee Informatique                               1995-1996  *
 * Option AAMP, Module MP2                                                 *
 *                                                                         *
 ***************************************************************************
 * Author : Gilles Subrenat                                                *
 * Date   : 15 janvier 1996                                                *
 *                                                                         *
 * $Id$
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include "img.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "lanceur.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"


typedef struct {
  COUPLE  Pixel;
} IMG_BASIC;

static IMG_BASIC  Img;


BOOL
file_img (FILE *File)
{
  GET_COUPLE (Img.Pixel);
  return (TRUE);
}


static COLOR
pixel_basic (INDEX i, INDEX j)
{
  RAY    Ray;
  HIT    Hit;
  REAL   u, v;

  Ray.Level = 1;
  Ray.Color = White;
  u = 2.0 * i / Img.Pixel.i - 1.0;
  v = 2.0 * j / Img.Pixel.j - 1.0;
  cmr (&Ray, u, v);

  if (! hit_bnd (&Ray, &Hit))
    return (MainScn->Atm);
  get_scn (&Ray, &Hit);

  return (Ray.Color);
}

#define BLOCK_SIZE 8

static void compute_block(COLOR* block, int num_block)
{
  int begin_block_i = (num_block/(Img.Pixel.j/BLOCK_SIZE))*BLOCK_SIZE;
  int begin_block_j = (num_block%(Img.Pixel.j/BLOCK_SIZE))*BLOCK_SIZE;

  for(int i=0; i<BLOCK_SIZE*BLOCK_SIZE; i++)
  {
    block[i] = pixel_basic(begin_block_i + i/BLOCK_SIZE, begin_block_j + i%BLOCK_SIZE);
  }
}

static void commit_block(COLOR* block, int num_block)
{
  
}

void img(const char *FileNameImg)
{
  MPI_Init(NULL,NULL);
  int i,P;
  MPI_Comm_rank(MPI_COMM_WORLD, &i);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  assert(0 == Img.Pixel.i%BLOCK_SIZE );
  assert(0 == Img.Pixel.j%BLOCK_SIZE );
  int C = (Img.Pixel.j/BLOCK_SIZE)*(Img.Pixel.i/BLOCK_SIZE);
  int q = (C+P-1)/P;

  COLOR block[BLOCK_SIZE*BLOCK_SIZE];

  for(int num_block=i*q; num_block<(i+1)*q-1 && num_block<C; num_block++)
  {
    compute_block(block,num_block);
    commit_block(block,num_block);
  }

  MPI_Finalize();


  /*Old
  FILE   *FileImg;   
  COLOR	 *TabColor, *Color;
  STRING Name;
  INDEX	 i, j;
  BYTE   Byte;

  strcpy (Name, FileNameImg);
  strcat (Name, ".ppm");
  INIT_FILE (FileImg, Name, "w");
  fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
  INIT_MEM (TabColor, Img.Pixel.i, COLOR);

  MPI_Init(NULL,NULL);
  int myrank,nb_proc;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

  
  
  for (j = 0; j < Img.Pixel.j; j++) {
    for (i = 0; i < Img.Pixel.i; i++) {
      TabColor [i] = pixel_basic (i, j);
    }
    for (i = 0, Color = TabColor; i < Img.Pixel.i; i++, Color++) {
      Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
      putc (Byte, FileImg);
      Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
      putc (Byte, FileImg);
      Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
      putc (Byte, FileImg);
    }
    fflush (FileImg);
  }

  MPI_Finalize();

  EXIT_MEM (TabColor);
  EXIT_FILE (FileImg);*/
  
}
