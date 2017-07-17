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
    MPI_Request request;

    // itero sobre la lista de archivos hasta recorrerla entera
    list<string>::iterator it = params.begin();
    while(it != params.end()) {
        if (!nodos_libres.empty()) {
            int nodo_libre = nodos_libres.front();
            nodos_libres.pop();
            
            // le aviso al nodo que quiero hacer load
            // MPI_Send(&tarea, 1, MPI_INT, nodo_libre, 0, MPI_COMM_WORLD);
            MPI_Isend(&tarea, 1, MPI_INT, nodo_libre, 0, MPI_COMM_WORLD, &request);            
            if (DEBUG & 1) cout << "[0] Le doy tarea load del archivo " << *it << " a " << nodo_libre << endl;
            
            string archivo = *it;
            char archivo_c[MAX_WORD_LEN];
            strncpy(archivo_c, archivo.c_str(), archivo.size()+1); 
            // le mando el archivo
            // MPI_Send(archivo_c, strlen(archivo_c)+1, MPI_CHAR, nodo_libre, 0, MPI_COMM_WORLD);
            MPI_Isend(archivo_c, strlen(archivo_c)+1, MPI_CHAR, nodo_libre, 0, MPI_COMM_WORLD, &request);
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
    MPI_Request request;    
    for (unsigned int i = 1; i < np; i++) {
        // le aviso al nodo i que quiero hacer quit
        MPI_Isend(&tarea, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }
}

// Esta función calcula el máximo con todos los nodos
static void maximum() {
    int tarea = ID_MAXIMUM;
    MPI_Request request;        
    for (unsigned int i = 1; i < np; i++) {
        // le aviso al nodo i que quiero hacer maximum
        MPI_Isend(&tarea, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }

    HashMap h;
    unsigned int finalizados = 0;
    char palabra[MAX_WORD_LEN];
 
    while (finalizados != np - 1) {
        // limpio buffer
        memset(palabra, 0, MAX_WORD_LEN);
        // recibo una palabra de algun nodo
        MPI_Recv(&palabra, MAX_WORD_LEN, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // me fijo si es mensaje de finalizacion, y en caso de que no, agrego la palabra al hashmap
        if (strcmp(palabra, END_STRING) == 0) 
            finalizados++;
        else
            h.addAndInc(string(palabra));
    }

    kv_pair maximo = h.maximum();
    cout << "El maximo es <" << maximo.first << ", " << maximo.second << ">" << endl;
  
}

// Esta función busca la existencia de *key* en algún nodo
static void member(string key) {
    int tarea = ID_MEMBER;
    MPI_Request request;        
    for (unsigned int i = 1; i < np; i++) {
        // le aviso al nodo i que quiero hacer member
        MPI_Isend(&tarea, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }

    // broadcast de key, para todos los nodos         
    char key_c[MAX_WORD_LEN];
    strncpy(key_c, key.c_str(), key.size()+1);
    if (DEBUG & 1) printf("[0] Mando por bcast la clave %s", key_c);
    MPI_Bcast(key_c, strlen(key_c)+1, MPI_CHAR, CONSOLE_RANK, MPI_COMM_WORLD);
    // espero respuesta de cada nodo
    bool res = false;
    bool res_printed = false;
    for (unsigned int i = 1; i < np; i++) {
        // recibo respuesta de alguno de los nodos
        bool res_member_i;
        MPI_Recv(&res_member_i, 1, MPI_C_BOOL, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        res |= res_member_i;
        // si todavia no imprimi la respuesta, y ya se que esta definida, entonces imprimo
        if (!res_printed && res) {
            cout << "La clave " << key << " ESTA DEFINIDA" << endl;
            res_printed = true;
        }
    }
    
    // si no esta definida, imprimo
    if (!res)
        cout << "La clave " << key << " NO ESTA DEFINIDA" << endl;
}


// Esta función suma uno a *key* en algún nodo
static void addAndInc(string key) {
    int tarea = ID_ADD;
    MPI_Request request;
    for (unsigned int i = 1; i < np; i++) {
        // le aviso al nodo i que quiero hacer add
        MPI_Isend(&tarea, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }

    unsigned int nodo_ganador;
    // recibo respuesta del nodo mas rapido y me guardo quien es
    MPI_Recv(&nodo_ganador, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (unsigned int i = 1; i < np; i++) {
        // le aviso a cada nodo su resultado
        if (i != nodo_ganador) {
            // le aviso al nodo q no tiene q hacer nada porque es una lenteja
            char msj[] = "fracaso";
            MPI_Isend(msj, strlen(msj)+1, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);

        } else {
            // le aviso al nodo que tiene que hacer add and inc porque fue el mas rápido
            char msj[] = "exito";
            MPI_Isend(msj, strlen(msj)+1, MPI_CHAR, nodo_ganador, 0, MPI_COMM_WORLD, &request);

            // le mando la key
            char key_c[MAX_WORD_LEN];
            strncpy(key_c, key.c_str(), key.size()+1);
            MPI_Isend(key_c, strlen(key_c)+1, MPI_CHAR, nodo_ganador, 0, MPI_COMM_WORLD, &request);
        }
    }

    // espero a que el nodo ganador termine con el addAndInc
    MPI_Recv(&nodo_ganador, 1, MPI_INT, nodo_ganador, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // consumo los mensajes que quedaron pendientes de los otros nodos para limpiar la cola de mensajes
    int basura;
    for (unsigned int i = 0; i < np - 2; i++) {
        MPI_Recv(&basura, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);    
    }

    if (DEBUG & 1) cout << "El nodo mas veloz del condado es: " << nodo_ganador << endl;
    cout << "La clave " << key << " ha sido agregada correctamente" << endl;
}

// Esta función imprime el contendo del DistributedHashMap
static void print() {
    int tarea = ID_PRINT;
    MPI_Request request;
    for (unsigned int i = 1; i < np; i++) {
        // le aviso al nodo i que quiero hacer print
        MPI_Isend(&tarea, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }

    HashMap h;
    unsigned int finalizados = 0;
    char palabra[MAX_WORD_LEN];
 
    while (finalizados != np - 1) {
        // limpio buffer
        memset(palabra, 0, MAX_WORD_LEN);
        // recibo una palabra de algun nodo
        MPI_Recv(&palabra, MAX_WORD_LEN, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (DEBUG & 1) cout << "[0] Recibi la palabra: " << string(palabra) << endl;        
        // me fijo si es mensaje de finalizacion, y en caso de que no, agrego la palabra al hashmap
        if (strcmp(palabra, END_STRING) == 0) 
            finalizados++;
        else
            h.addAndInc(string(palabra));
    }

    h.printAll();
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

    if (strncmp(first_param, CMD_PRINT, sizeof(CMD_PRINT))==0) {
        print();
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
    printf("  "CMD_PRINT"\n");

    bool fin = false;
    while (!fin) {
        printf("> ");
        fflush(stdout);
        fin = procesar_comandos();
    }
}
