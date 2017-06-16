#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "consola.hpp"
#include "HashMap.hpp"
#include "mpi.h"

#include <queue>

using namespace std;

#define CMD_LOAD    "load"
#define CMD_ADD     "addAndInc"
#define CMD_MEMBER  "member"
#define CMD_MAXIMUM "maximum"
#define CMD_QUIT    "quit"
#define CMD_SQUIT   "q"

static unsigned int np;
static queue<int> nodos_libres;


// Crea un ConcurrentHashMap distribuido
static void load(list<string> params) {

    for (list<string>::iterator it=params.begin(); it != params.end(); ++it) {
        int nodo_libre = nodos_libres.front();
        nodos_libres.pop();

        int id_tarea = 0;
        
        MPI_Send(&id_tarea, 1, MPI_INT, nodo_libre, 0, MPI_COMM_WORLD);
        
        //Le mando la len del path
        string arch_i = *it;
        int len_i = arch_i.size();
        
        MPI_Send(&len_i, 1, MPI_INT, nodo_libre, 0, MPI_COMM_WORLD);
 
        //Le mando el path
        for (int i = 0; i < len_i; i++){
            char letra = arch_i[i];
            MPI_Send(&letra, 1, MPI_CHAR, nodo_libre, 0, MPI_COMM_WORLD);
        }

        //Espero a que me envie una R (Ready)
        char ready;
        MPI_Recv(&ready, 1, MPI_CHAR, nodo_libre, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(ready=='R'){
            cout<<"listo el pollo, a encolar "<< nodo_libre <<" " <<endl;
            nodos_libres.push(nodo_libre);
        }




    }

    cout << "La listá esta procesada" << endl;
}

// Esta función debe avisar a todos los nodos que deben terminar
static void quit() {
    // TODO: Implementar
}

// Esta función calcula el máximo con todos los nodos
static void maximum() {
    int id_tarea = 3;
        MPI_Send(&id_tarea, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
}

// Esta función busca la existencia de *key* en algún nodo
static void member(string key) {
   int id_tarea = 2;
        MPI_Send(&id_tarea, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
}


// Esta función suma uno a *key* en algún nodo
static void addAndInc(string key) {

    // TODO: Implementar
    int id_tarea = 1;
        MPI_Send(&id_tarea, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

    cout << "Agregado: " << key << endl;
}


/* static int procesar_comandos()
La función toma comandos por consola e invoca a las funciones correspondientes
Si devuelve true, significa que el proceso consola debe terminar
Si devuelve false, significa que debe seguir recibiendo un nuevo comando
*/

static bool procesar_comandos() {

    char buffer[BUFFER_SIZE];
    size_t buffer_length;
    char *res, *first_param, *second_param;

    // Mi mamá no me deja usar gets :(
    res = fgets(buffer, sizeof(buffer), stdin);

    // Permitimos salir con EOF
    if (res==NULL)
        return true;

    buffer_length = strlen(buffer);
    // Si es un ENTER, continuamos
    if (buffer_length<=1)
        return false;

    // Sacamos último carácter
    buffer[buffer_length-1] = '\0';

    // Obtenemos el primer parámetro
    first_param = strtok(buffer, " ");

    if (strncmp(first_param, CMD_QUIT, sizeof(CMD_QUIT))==0 ||
        strncmp(first_param, CMD_SQUIT, sizeof(CMD_SQUIT))==0) {

        quit();
        return true;
    }

    if (strncmp(first_param, CMD_MAXIMUM, sizeof(CMD_MAXIMUM))==0) {
        maximum();
        return false;
    }

    // Obtenemos el segundo parámetro
    second_param = strtok(NULL, " ");
    if (strncmp(first_param, CMD_MEMBER, sizeof(CMD_MEMBER))==0) {
        if (second_param != NULL) {
            string s(second_param);
            member(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_ADD, sizeof(CMD_ADD))==0) {
        if (second_param != NULL) {
            string s(second_param);
            addAndInc(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_LOAD, sizeof(CMD_LOAD))==0) {
        list<string> params;
        while (second_param != NULL)
        {
            string s(second_param);
            params.push_back(s);
            second_param = strtok(NULL, " ");
        }

        load(params);
        return false;
    }

    printf("Comando no reconocido");
    return false;
}

void consola(unsigned int np_param) {
    queue<int> nodos_libres_aux;
    nodos_libres = nodos_libres_aux;

    np = np_param;
    cout<<"NP:::    "<<np<<endl;
    for (int i = 0; i < np-1; i++) {
        nodos_libres.push(i+1);
    }
    printf("Comandos disponibles:\n");
    printf("  "CMD_LOAD" <arch_1> <arch_2> ... <arch_n>\n");
    printf("  "CMD_ADD" <string>\n");
    printf("  "CMD_MEMBER" <string>\n");
    printf("  "CMD_MAXIMUM"\n");
    printf("  "CMD_SQUIT"|"CMD_QUIT"\n");

    bool fin = false;
    while (!fin) {
        printf("> ");
        fflush(stdout);
        fin = procesar_comandos();
    }
}
