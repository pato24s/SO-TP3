#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include "defines.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void nodo(unsigned int rank) {
    HashMap h;
    while (true) {
        int tarea;
    	MPI_Recv(&tarea, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (DEBUG & 1) cout << "[" << rank << "] " << "Recibí tarea " << tarea << endl;
    
        if (tarea == ID_LOAD) {
            if (DEBUG & 1) cout << "[" << rank << "] " << "Empiezo a hacer load" << endl;			
            int longitud;
            // recibo longitud de path
            MPI_Recv(&longitud, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (DEBUG & 1) cout << "[" << rank << "] " << "Me pasan la longitud del path: " << longitud << endl;	

            char nombre[longitud];
            // recibo path
            MPI_Recv(&nombre, longitud, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);				
            // casteo a string
            string nombre_string = string(nombre);
            if (DEBUG & 1) cout << "[" << rank << "] Me pasan el path: " << nombre_string << endl;
            // cargo archivo en el hashmap
            h.load(nombre_string);

            // trabajo arduamente y luego le aviso a la consola que termine
            int rank_int = rank;
            trabajarArduamente();
            MPI_Send(&rank_int, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            
        } else if (tarea == ID_ADD) {

            cout << "add and inc!!" << endl;

        } else if (tarea == ID_MEMBER) {

            cout << "MEEEEEEEEEEEEEEEEEMBER" << endl;

        } else if (tarea == ID_MAXIMUM) {	

            cout << "MAXXXXX" << endl;

        } else {
            cout << "QUIT" << endl;
        }
        
        if (DEBUG & 1) cout << "[" << rank << "] " << "Termine con la tarea que me dieron" << endl;	
    }

    if (DEBUG & 1) cout << "[" << rank << "] " << "Finalizo" << endl;	
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
