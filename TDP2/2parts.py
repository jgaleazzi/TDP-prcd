import numpy as np
import sys

def gen():
    nb_proc = 1
    nb_part = 2


    Masse_tab = [10, 100]
    x_tab =  [100, 200]
    y_tab =  [100, 200]
    vit_x_tab = [10, 0]
    vit_y_tab = [15, 0]
    nb_element_file = nb_part/nb_proc
    reste = nb_part%nb_proc

    NomFichier = 'proc_0.txt'
    Fichier = open(NomFichier,'w')
    for j in range(nb_part):
        Fichier.write( str(Masse_tab[j])+ " " + str(x_tab[j])+ " " + str(y_tab[j])+ " " + str(vit_x_tab[j])+  " " + str(vit_y_tab[j])+ '\n')
    Fichier.close()
        
        
gen()
