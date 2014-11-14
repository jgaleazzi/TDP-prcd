import numpy as np
import sys

def gen(name_mat):
    if (len(sys.argv) <3):
        print("Please enter a number of proc and bloc size")
    else:
        nb_bloc_1D = int(sys.argv[1])
        bloc_size = int(sys.argv[2])

    NomFichier = name_mat+".txt"
    Fichier = open(NomFichier,'w')
    Fichier.write(str(nb_bloc_1D*bloc_size)+" "+str(nb_bloc_1D)+"\n")
    for i in range(nb_bloc_1D*bloc_size):
        for j in range(nb_bloc_1D*bloc_size):
            Fichier.write(str(j)+" ")
        Fichier.write("\n")

    Fichier.close()


gen("A")
gen("B")
