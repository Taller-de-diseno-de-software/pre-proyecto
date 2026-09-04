/*
 * TAD: Tabla de Símbolos (archivo.h)

 * Se definen las estructuras y operaciones necesarias para gestionar 
 * el ciclo de vida y alcance (scope) de los identificadores durante la 
 * compilación.
 *
 * Arquitectura principal:
 * - Implementa una Pila de Niveles encadenada, donde cada nivel representa 
 *   un contexto local (bloque) y contiene su propia lista de símbolos.
 * 
 * - Se cambia la def de nodo del AST para que ahora tenga un puntero directo
 *  al símbolo correspondiente en la tabla de símbolos
 *
 * - Ciclo de vida: al cerrar un nivel se descarta su lista de nodos, pero los
 *   Simbolo quedan vivos porque el AST los referencia (nodo->simbolo). Esa
 *   memoria la conserva el AST hasta que termina el programa.
 *
*/

#ifndef TABLA_SIMBOLOS_H
#define TABLA_SIMBOLOS_H

/*ESTRUCTURA DE LOS SIMBOLOS*/

//Enumerado de flags
typedef enum {
    FLAG_VARIABLE,
    FLAG_FUNCION,
    FLAG_PARAMETRO
} FlagSimbolo;

// El registro individual para cada identificador (Datos puros)
typedef struct simbolo{
    FlagSimbolo flag;
    char *nombre;
    char *tipo;
    int valor; //Solo lo voy a usar para crear constantes porque la tabla de simbolos no debe actualizar ni guardar valores, solo direcciones y nombres
    int inicializado; //1 si ya hubo una asignacion previa a la variable; lo usa el analisis semantico
} Simbolo;

// Nodo para armar la lista enlazada de símbolos dentro del mismo nivel
typedef struct nodoSimbolo {
    Simbolo *simbolo;
    struct nodoSimbolo *siguiente;
} NodoSimbolo;

/* ESTRUCTURA DEL NIVEL DE LA PILA*/

typedef struct nodoNivel {
    NodoSimbolo *primer_nodo; // Cada nivel contiene una lista de nodos de símbolos[cite: 2]
    struct nodoNivel *anterior; // Puntero al nivel anterior
} NodoNivel;

extern NodoNivel *topeTablaSimbolos;

/* OPERACIONES DEL TAD*/

void inicializarTablaSimbolos(void);
void abrirNivel(void);
void cerrarNivel(void);
Simbolo* insertarSimbolo(FlagSimbolo flag, char *nombre, char *tipo);
Simbolo* buscarSimbolo(char *nombre);

/* CONEXIÓN CON EL AST */
typedef struct nodoAST {
    char *tipo;                 // "SUMA", "DECLARACION", etc.
    char *valor;                // Texto, solo para hojas
    Simbolo *simbolo;           // Referencia directa al elemento de la tabla de símbolos
    struct nodoAST *hijos[3];   // Arreglo para soportar árboles ternarios[cite: 3]
} nodoAST;

// Métodos de tu AST (actualizados para usar nodoAST)
nodoAST *crearNodo(const char *tipo, char *valor, nodoAST *hijoIzq, nodoAST *hijoMed, nodoAST *hijoDer);
void imprimirArbol(nodoAST *nodo, int nivel);

#endif