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

    int id_tarea = 0;
    //ASUMO CANT ARCHIVOS >= CANT NODOS

    MPI_Request req;

    int cant_archivos = params.size();

    int i = 0;
    list<string>::iterator it = params.begin();
    while(i < int(np-1)){
        int nodo_libre = nodos_libres.front();
        nodos_libres.pop();


        //le aviso al nodo que quiero hacer load
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


        i++;
        it++;
    }

    //Ya le mande a cada nodo un archivo
    //Espero que terminen para seguir mandado

    int cant_terminados = 0;
    int cant_asignados = np-1;

    cout<<"asignados por ahora: "<<cant_asignados<<endl;
    while(true){

        MPI_Status status;
        // receive message from any source
        int ready;

        //ESTE RECV(+ el sned del nodo) DEBERIA SER NO BLOQUEANTE????
        MPI_Recv(&ready, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        int nodo_que_termino = status.MPI_SOURCE;
        cout << "listo: " << nodo_que_termino << endl;

        cant_terminados++;
        cout <<"terminados: "<<cant_terminados<<"/"<<cant_archivos<<endl;
        cout<<"asignados: "<<cant_asignados<<"/"<<cant_archivos<<endl;
        if(cant_asignados < cant_archivos){
            //mando proximo archivo

            //le aviso al nodo que quiero hacer load
            MPI_Send(&id_tarea, 1, MPI_INT, nodo_que_termino, 0, MPI_COMM_WORLD);

            //Le mando la len del path
            string arch_i = *it;
            int len_i = arch_i.size();
            
            MPI_Send(&len_i, 1, MPI_INT, nodo_que_termino, 0, MPI_COMM_WORLD);

            //Le mando el path
            for (int i = 0; i < len_i; i++){
                char letra = arch_i[i];
                MPI_Send(&letra, 1, MPI_CHAR, nodo_que_termino, 0, MPI_COMM_WORLD);
            }
            cant_asignados++;
            it++;
        }else if(cant_terminados < cant_archivos){
            //vuelvo a iterar
        }else{
            break;
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
