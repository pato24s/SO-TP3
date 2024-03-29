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
        MPI_Request request;
    	MPI_Recv(&tarea, 1, MPI_INT, CONSOLE_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (DEBUG & 2) cout << "[" << rank << "] " << "Recibo tarea " << tarea << endl;
    
        if (tarea == ID_LOAD) {
            
            // creo buffer y lo limpio por las dudas
            char nombre[MAX_FILE_LEN];
            // memset(nombre, 0, MAX_FILE_LEN);            
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
            // MPI_Send(&rank_int, 1, MPI_INT, CONSOLE_RANK, 0, MPI_COMM_WORLD);
            MPI_Isend(&rank_int, 1, MPI_INT, CONSOLE_RANK, 0, MPI_COMM_WORLD, &request);            
            
        } else if (tarea == ID_ADD) {

            //quieren hacer add, tengo que ganarle a todos los otros nodos y responder mi rank rapido
            int rank_int = rank;
            trabajarArduamente();
            MPI_Isend(&rank_int, 1, MPI_INT, CONSOLE_RANK, 0, MPI_COMM_WORLD, &request);

            char resultado[MAX_WORD_LEN];
            // memset(resultado, 0, MAX_WORD_LEN);

            MPI_Recv(&resultado, MAX_FILE_LEN, MPI_CHAR, CONSOLE_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string result = string(resultado);

            if (result == "exito") {
                //espero la key a agregar
                char key_c[MAX_WORD_LEN];            
                // memset(key_c, 0, MAX_WORD_LEN);
                MPI_Recv(key_c, MAX_WORD_LEN, MPI_CHAR, CONSOLE_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                string key = string(key_c);

                //ya tengo la key, tengo que hacer addAndInc
                h.addAndInc(key);

                // le aviso al nodo consola que ya termine con el addAndInc
                trabajarArduamente();
                MPI_Isend(&rank_int, 1, MPI_INT, CONSOLE_RANK, 0, MPI_COMM_WORLD, &request);
            } else {
                MPI_Request_free(&request);
                if (DEBUG & 2) cout << "[" << rank << "] " << "Elimine mensaje enviado antes" << endl;
            }       

        } else if (tarea == ID_MEMBER) {

            // creo buffer y lo limpio por las dudas            
            char key_c[MAX_WORD_LEN];            
            // memset(key_c, 0, MAX_WORD_LEN);
            // recibo clave a buscar
            MPI_Bcast(key_c, MAX_WORD_LEN, MPI_CHAR, CONSOLE_RANK, MPI_COMM_WORLD);
            if (DEBUG & 2) printf("[%d] Recibi por bcast la clave %s", rank, key_c);
            string key = string(key_c);
            if (DEBUG & 2) cout << "[" << rank << "] Me pasan la clave: " << key << endl;          
            
            // busco clave en el hash map
            bool res = h.member(key);
            if (DEBUG & 2) cout << "[" << rank << "] " << "Empiezo a hacer member" << endl;            
            
            // trabajo arduamente y luego le mando al nodo consola el resultado de member            
            trabajarArduamente();
            if (DEBUG & 2) cout << "[" << rank << "] " << "Le aviso al nodo consola que termine" << endl;	        
            MPI_Isend(&res, 1, MPI_C_BOOL, CONSOLE_RANK, 0, MPI_COMM_WORLD, &request);            

        } else if (tarea == ID_MAXIMUM || tarea == ID_PRINT) {	

            for (HashMap::iterator it = h.begin(); it != h.end(); it++) {
                // le mando una palabra del hashmap al nodo consola
                string palabra_s = *it;           
                char palabra[MAX_WORD_LEN];           
                strncpy(palabra, palabra_s.c_str(), palabra_s.size()+1);    

                // esto es para que solo se llame una vez a trabarjarArduamente
                if (it == h.begin())                
                    trabajarArduamente();
                MPI_Isend(palabra, strlen(palabra), MPI_CHAR, CONSOLE_RANK, 0, MPI_COMM_WORLD, &request);
            }

            // mando mensaje de finalizacion, indicando que termine de enviar palabras
            char msj_fin[] = END_STRING;
            trabajarArduamente();            
            MPI_Isend(msj_fin, strlen(msj_fin)+1, MPI_CHAR, CONSOLE_RANK, 0, MPI_COMM_WORLD, &request);
            
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
