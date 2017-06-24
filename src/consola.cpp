#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <queue>
#include "consola.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include "defines.h"

using namespace std;

static unsigned int np;
static queue<int> nodos_libres;

// Crea un ConcurrentHashMap distribuido
// TODO: preguntar si el nodo consola tambien tiene que trabajar arduamente antes de cada send
static void load(list<string> params) {

    int tarea = ID_LOAD; 
    int cant_archivos = params.size();
    int nuevo_nodo_libre;

    // itero sobre la lista de archivos hasta recorrerla entera
    list<string>::iterator it = params.begin();
    while(it != params.end()) {
        if (!nodos_libres.empty()) {
            int nodo_libre = nodos_libres.front();
            nodos_libres.pop();
            
            // le aviso al nodo que quiero hacer load
            MPI_Send(&tarea, 1, MPI_INT, nodo_libre, 0, MPI_COMM_WORLD);
            if (DEBUG & 1) cout << "[0] Le doy tarea load del archivo " << *it << " a " << nodo_libre << endl;
            
            string archivo = *it;
            int longitud = archivo.size();
            const char* archivo_c = archivo.c_str();
            // le mando el archivo
            MPI_Send(archivo_c, longitud, MPI_CHAR, nodo_libre, 0, MPI_COMM_WORLD);
            it++;
            continue;
        }

        // si hay archivos por parsear, pero no hay nodos disponibles, recibe de manera bloqueante, a
        // la espera de que algun nodo termine
        MPI_Recv(&nuevo_nodo_libre, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        nodos_libres.push(nuevo_nodo_libre);
        if (DEBUG & 1) cout << "[0] El nodo " << nuevo_nodo_libre << " me aviso que termino y lo encole" << endl;
    }

    // calculo la cantidad de nodos que tengo que esperar a que terminen
    int cant_recv = (int(np-1) < cant_archivos)? np-1 : cant_archivos;
    for (int i = 0; i < cant_recv; i++) {
        MPI_Recv(&nuevo_nodo_libre, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        nodos_libres.push(nuevo_nodo_libre);
        if (DEBUG & 1) cout << "[0] El nodo " << nuevo_nodo_libre << " me aviso que termino y lo encole" << endl;
    }

    cout << "El load ha finalizado" << endl;
}

// Esta función debe avisar a todos los nodos que deben terminar
static void quit() {
    int tarea = ID_QUIT;
    for (unsigned int i = 1; i < np; i++) {
        // le aviso al nodo i que quiero hacer quit
        MPI_Send(&tarea, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
}

// Esta función calcula el máximo con todos los nodos
static void maximum() {
    int tarea = ID_MAXIMUM;
    MPI_Send(&tarea, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
}

// Esta función busca la existencia de *key* en algún nodo
static void member(string key) {
    int tarea = ID_MEMBER;
    for (unsigned int i = 1; i < np; i++) {
        // le aviso al nodo i que quiero hacer member
        MPI_Send(&tarea, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    // todo esto para obtener el string como char * (no const)
    int longitud = key.size();
    const char* key_c_aux = key.c_str();
    char key_c[MAX_WORD_LEN];
    strcpy(key_c, key_c_aux);
    // broadcast de key, para todos los nodos         
    MPI_Bcast(key_c, longitud, MPI_CHAR, CONSOLE_RANK, MPI_COMM_WORLD);

    // espero respuesta de cada nodo
    bool res = false;
    for (unsigned int i = 1; i < np; i++) {
        // recibo respuesta de alguno de los nodos
        bool res_member_i;
        MPI_Recv(&res_member_i, 1, MPI_C_BOOL, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        res |= res_member_i;
    }

    if (res)
        cout << "La clave " << key << " ESTA DEFINIDA" << endl;
    else
        cout << "La clave " << key << " NO ESTA DEFINIDA" << endl;
        
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
    for (unsigned int i = 0; i < np-1; i++) {
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
