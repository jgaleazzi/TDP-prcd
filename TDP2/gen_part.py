import numpy as np
import sys

def gen():
    if (len(sys.argv) <3):
        print("Please enter a number of proc and number of particle")
    else:
        nb_proc = int(sys.argv[1])
        nb_part = int(sys.argv[2])


    Masse_tab = (10000*np.random.rand(nb_part)).tolist()
    x_tab =  (100*np.random.rand(nb_part)).tolist()
    y_tab =  (100*np.random.rand(nb_part)).tolist()
    vit_tab = (100*np.random.rand(nb_part)).tolist()
    nb_element_file = nb_part/nb_proc
    reste = nb_part%nb_proc

    for i in range(nb_proc):
        NomFichier = 'proc_'+str(i)+'.txt'
        Fichier = open(NomFichier,'w')
        for j in range(nb_element_file):
            Fichier.write( str(Masse_tab[j])+ " " + str(x_tab[j])+ " " + str(y_tab[j])+ " " + str(vit_tab[j])+ '\n')
        for k in range(reste):
            Fichier.write( str(Masse_tab[k])+ " " + str(x_tab[k])+ " " + str(y_tab[k])+ " " + str(vit_tab[k])+ '\n')
        Fichier.close()


gen()
