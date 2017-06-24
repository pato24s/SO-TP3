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
    	MPI_Recv(&tarea, 1, MPI_INT, CONSOLE_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (DEBUG & 2) cout << "[" << rank << "] " << "Recibo tarea " << tarea << endl;
    
        if (tarea == ID_LOAD) {
            
            // creo buffer y lo limpio por las dudas
            char nombre[MAX_FILE_LEN];
            memset(nombre, 0, MAX_FILE_LEN);            
            // recibo path
            MPI_Recv(&nombre, MAX_FILE_LEN, MPI_CHAR, CONSOLE_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);				
            // casteo a string
            string nombre_string = string(nombre);
            if (DEBUG & 2) cout << "[" << rank << "] Me pasan el archivo " << nombre_string << endl;
            
            // cargo archivo en el hashmap
            h.load(nombre_string);
            if (DEBUG & 2) cout << "[" << rank << "] " << "Empiezo a hacer load" << endl;			

            // trabajo arduamente y luego le aviso al nodo consola que termine el load
            int rank_int = rank;
            trabajarArduamente();
            if (DEBUG & 2) cout << "[" << rank << "] " << "Le aviso al nodo consola que termine" << endl;
            MPI_Send(&rank_int, 1, MPI_INT, CONSOLE_RANK, 0, MPI_COMM_WORLD);
            
        } else if (tarea == ID_ADD) {

            //quieren hacer add, tengo que ganarle a todos los otros nodos y responder mi rank rapido
            int rank_int = rank;
            trabajarArduamente();
            MPI_Send(&rank_int, 1, MPI_INT, CONSOLE_RANK, 0, MPI_COMM_WORLD);

            char resultado[MAX_WORD_LEN];
            memset(resultado, 0, MAX_WORD_LEN);

            MPI_Recv(&resultado, MAX_FILE_LEN, MPI_CHAR, CONSOLE_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string result = string(resultado);


            if(result == "exito"){
                //espero la key a agregar
                char key_c[MAX_WORD_LEN];            
                memset(key_c, 0, MAX_WORD_LEN);
                MPI_Recv(key_c, MAX_WORD_LEN, MPI_CHAR, CONSOLE_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                string key = string(key_c);

                //ya tengo la key, tengo que hacer add and inc
                h.addAndInc(key);

            }           

        } else if (tarea == ID_MEMBER) {

            // creo buffer y lo limpio por las dudas            
            char key_c[MAX_WORD_LEN];            
            memset(key_c, 0, MAX_WORD_LEN);
            // recibo clave a buscar
            MPI_Bcast(key_c, MAX_WORD_LEN, MPI_CHAR, CONSOLE_RANK, MPI_COMM_WORLD);
            string key = string(key_c);
            if (DEBUG & 2) cout << "[" << rank << "] Me pasan la clave: " << key << endl;          
            
            // busco clave en el hash map
            bool res = h.member(key);
            if (DEBUG & 2) cout << "[" << rank << "] " << "Empiezo a hacer member" << endl;            
            
            // trabajo arduamente y luego le mando al nodo consola el resultado de member            
            trabajarArduamente();
            if (DEBUG & 2) cout << "[" << rank << "] " << "Le aviso al nodo consola que termine" << endl;	        
            MPI_Send(&res, 1, MPI_C_BOOL, CONSOLE_RANK, 0, MPI_COMM_WORLD);            

        } else if (tarea == ID_MAXIMUM) {	

            cout << "MAXXXXX" << endl;

        } else {
            break;
        }        
    }

    if (DEBUG & 2) cout << "[" << rank << "] " << "Finalizo" << endl;	
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
