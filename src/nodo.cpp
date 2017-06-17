#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void nodo(unsigned int rank) {
    MPI_Request req;

    printf("Soy un nodo. Mi rank es %d \n", rank);

    HashMap h;
    while (true) {

    	int id_tarea;
    	MPI_Recv(&id_tarea, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	if(id_tarea == 0){

    		cout << "its load time"<<rank<<" " << endl;

    		//Tengo que esperar el nombre de archivo
    		int len;
    		// cout << "se viene un archivo con nombre de largo: " <<endl;
    		MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    		char nombre[len];

    		for (int i = 0; i < len; i++){
    			char letra;
    			MPI_Recv(&letra, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    			nombre[i]=letra;

    		}
    		string nombreString = string(nombre);

    		h.load(nombreString);

    		//TENGO QUE AVISARLE A CONSOLA QUE TERMINE

    		int ready = rank;

            MPI_Send(&ready, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);




    	}else if(id_tarea == 1){

    		cout << "add and inc!!" << endl;

    	}else if(id_tarea == 2){

    		cout << "MEEEEEEEEEEEEEEEEEMBER" << endl;

    	}else{	

    		cout << "MAXXXXX" << endl;

    	}



    }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
