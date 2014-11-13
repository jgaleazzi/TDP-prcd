import numpy as np
import sys

def gen():
    if (len(sys.argv) <3):
        print("Please enter a number of proc and number of particle")
    else:
        nb_proc = int(sys.argv[1])
        nb_part = int(sys.argv[2])


    Masse_tab = (1000*np.random.rand(nb_part)).tolist()
    x_tab =  (10*np.random.rand(nb_part)).tolist()
    y_tab =  (10*np.random.rand(nb_part)).tolist()
    vit_x_tab = (10*np.random.rand(nb_part)).tolist()
    vit_y_tab = (10*np.random.rand(nb_part)).tolist()
    nb_element_file = nb_part/nb_proc
    reste = nb_part%nb_proc
    print Masse_tab
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
                
       
        

    Fichier.close()


gen()
