import numpy as np
import sys
import math as m

def gen(name_mat):
    if (len(sys.argv) <3):
        print("Please enter a number of proc and bloc size")
    else:
        nb_proc = int(sys.argv[1])
        bloc_size = int(sys.argv[2])

    NomFichier = name_mat+".txt"
    Fichier = open(NomFichier,'w')
    Fichier.write(str(int(m.sqrt(nb_proc))*bloc_size)+" "+str(bloc_size)+"\n")
    for i in range(int(m.sqrt(nb_proc))*bloc_size):
        for j in range(int(m.sqrt(nb_proc))*bloc_size):
            Fichier.write(str(j)+" ")
        Fichier.write("\n")

    Fichier.close()


gen("A")
gen("B")
