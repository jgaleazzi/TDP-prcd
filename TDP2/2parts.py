import numpy as np
import sys

def gen():
    nb_proc = 2
    nb_part = 2


    Masse_tab = [1000000, 1]
    x_tab =  [0, 1000000000]
    y_tab =  [0, 0]
    vit_x_tab =[0, 0]
    vit_y_tab = [0, 30000]
    nb_element_file = nb_part/nb_proc
    reste = nb_part%nb_proc

    count=0 
    for i in range(nb_proc):
        NomFichier = 'proc_'+str(i)+'.txt'
        Fichier = open(NomFichier,'w')
        for j in range(nb_element_file):
            Fichier.write( str(Masse_tab[count])+ " " + str(x_tab[count])+ " " + str(y_tab[count])+ " " + str(vit_x_tab[count])+  " " + str(vit_y_tab[count])+ '\n')
            count+=1        
        if(reste != 0):
            Fichier.write( str(Masse_tab[count])+ " " + str(x_tab[count])+ " " + str(y_tab[count])+ " " + str(vit_x_tab[count])+  " " + str(vit_y_tab[count])+ '\n')
            count+=1
            reste-=1
        
gen()
