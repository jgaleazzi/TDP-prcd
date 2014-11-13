
import pylab
import sys

def plot():

    nb_proc = int(sys.argv[1])

    list_of_files=range(0,nb_proc)
    for i in range(0,nb_proc):
        list_of_files[i]= "test/"+str(i)+"_out.txt", str(i)+"_out.txt"

    
    datalist = [ ( pylab.loadtxt(filename), label ) for filename, label in list_of_files ]
    
    for data, label in datalist:
        pylab.plot( data[:,0], data[:,1],"o", label=label )

    pylab.legend()
    pylab.title("Resultat simulation 20 particules 100 iterations")
    pylab.xlabel("X")
    pylab.ylabel("Y")
    pylab.show()
#    pylab.savefig("simu.png")
plot()
