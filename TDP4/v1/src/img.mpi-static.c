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

#include "img.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "lanceur.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"

#define BLOCK_SIZE 8

static int myrank, nb_proc;

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

/**
/* * Create the mpi datatype used to describe the blocks in the matrix
 * @param MPI_Type_block is the datatype to create (output)
 * @param block_size a block contains block_size^2 elements
 * @param mat_size a block contains mat_size^2 elements
 **/
static void create_datatype_bloc(MPI_Datatype* MPI_Type_block, int block_size, int mat_size)
{
  MPI_Type
  MPI_Type_vector(block_size, block_size, mat_size, MPI_Type_Color, MPI_Type_block);
  MPI_Type_create_resized(*MPI_Type_block, 0, block_size*sizeof(double), MPI_Type_block);
  MPI_Type_commit(MPI_Type_block);
}

struct matrix matrix_gather(int N, double* block, int root, MPI_Comm comm)
{
  if(myrank == root)
  {
    //Allocate memory for the gathered matrix
    res = //TODO

    MPI_Datatype MPI_Type_block;
    create_datatype_bloc(&MPI_Type_block,BLOCK_SIZE,res.length);
    int recvcounts[nb_proc];
    int displs[nb_proc];
    int i;
    for(i=0; i<nb_proc; i++)
    {
      recvcounts[i] = 1;
      displs[i] = i%grid->N + (i/grid->N)*grid->N*n;
    }
    MPI_Gatherv(block->tab, n*n, MPI_DOUBLE, res.tab, recvcounts, displs, MPI_Type_block, root, comm);
  }
  else
  {
    MPI_Gatherv(block->tab, n*n, MPI_DOUBLE, NULL, NULL, NULL, NULL, root, grid->comm_grid);
  }

  return res;
}

void
img (const char *FileNameImg)
{
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
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM8WORLD, &nb_proc);

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
  EXIT_FILE (FileImg);
  
}
