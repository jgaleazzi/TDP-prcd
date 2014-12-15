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

//#define BIG_PRIME 1501139
#define BIG_PRIME 47

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

static void compute_block(BYTE* img, int num_block)
{
  int begin_block_i = (num_block/(Img.Pixel.j/BLOCK_SIZE))*BLOCK_SIZE;
  int begin_block_j = (num_block%(Img.Pixel.j/BLOCK_SIZE))*BLOCK_SIZE;

  for(int i=0; i<BLOCK_SIZE; i++)
    for(int j=0; j<BLOCK_SIZE; j++)
    {
      COLOR color = pixel_basic(begin_block_i + i, begin_block_j + j);
      int pixel_base = Img.Pixel.j*(begin_block_j + j) + begin_block_i + i;
      img[3*pixel_base] = color.r < 1.0 ? 255.0*color.r : 255.0;
      img[3*pixel_base+1] = color.g < 1.0 ? 255.0*color.g : 255.0;
      img[3*pixel_base+2] = color.b < 1.0 ? 255.0*color.b : 255.0;
    }
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

  BYTE* img = calloc(Img.Pixel.i*Img.Pixel.j*3, sizeof(BYTE));

  for(int j=i*q; j<(i+1)*q && j<C; j++)
  {
    int num_block = (j*BIG_PRIME)%C;
    compute_block(img,num_block);
  }
  
  
  if(i==0)
    MPI_Reduce(MPI_IN_PLACE, img, Img.Pixel.i*Img.Pixel.j*3, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);
  else
    MPI_Reduce(img, img, Img.Pixel.i*Img.Pixel.j*3, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);

  if(i==0)
  {
    STRING Name;
    FILE   *FileImg;
    strcpy (Name, FileNameImg);
    strcat (Name, ".ppm");
    INIT_FILE (FileImg, Name, "w");
    fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);

    for(int k=0; k<Img.Pixel.i*Img.Pixel.j*3; k++)
    {
      putc (img[k], FileImg);
    }

    EXIT_FILE(FileImg);
  }

  free(img);  

  MPI_Finalize(); 
}
