#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tablaSimbolos.h"

NodoNivel *topeTablaSimbolos = NULL;

// Convierte el texto del token de tipo ("int"/"bool"/"void") al enum correspondiente
TipoDato tipoDesdeTexto(const char *texto) {
    if (!texto) return TIPO_INDEFINIDO;
    if (strcmp(texto, "int") == 0) return TIPO_INT;
    if (strcmp(texto, "bool") == 0) return TIPO_BOOL;
    if (strcmp(texto, "void") == 0) return TIPO_VOID;
    return TIPO_INDEFINIDO;
}

// Nombre textual de una etiqueta de nodo del AST (para impresión/depuración)
const char *nodeKindNombre(TipoDeNodo tipo) {
    switch (tipo) {
        case NODO_PROGRAMA:      return "PROGRAMA";
        case NODO_BLOQUE:        return "BLOQUE";
        case NODO_DECLARACIONES: return "DECLARACIONES";
        case NODO_DECLARACION:   return "DECLARACION";
        case NODO_SENTENCIAS:    return "SENTENCIAS";
        case NODO_OP_ASIG:       return "OP_ASIG";
        case NODO_RETURN:        return "RETURN";
        case NODO_OP_SUMA:       return "OP_SUMA";
        case NODO_OP_PROD:       return "OP_PROD";
        case NODO_CTE_ENTERA:    return "CTE_ENTERA";
        case NODO_CTE_LOGICA:    return "CTE_LOGICA";
        case NODO_ID:            return "ID";
        case NODO_TIPOFUNC:      return "TIPOFUNC";
        case NODO_TIPOVAR:       return "TIPOVAR";
    }
    return "DESCONOCIDO";
}

//Primer nivel de la tabla de simbolos
void inicializarTablaSimbolos(void) {
    topeTablaSimbolos = NULL;
}

//Creo nuevo nivel y lo pongo en la cima de la pila
void abrirNivel(void) {
    NodoNivel *nuevoNivel = (NodoNivel *)malloc(sizeof(NodoNivel));
    if (!nuevoNivel) {
        fprintf(stderr, "Error: Memoria insuficiente para abrir un nuevo nivel.\n");
        exit(1);
    }
    
    nuevoNivel->primer_nodo = NULL;
    nuevoNivel->anterior = topeTablaSimbolos;
    
    topeTablaSimbolos = nuevoNivel;
}

//Cerramos/eliminamos el ultimo nivel
void cerrarNivel(void) {
    if (topeTablaSimbolos == NULL) return;

    //Guardo el nivel a cerrar
    NodoNivel *nivelACerrar = topeTablaSimbolos;
    NodoSimbolo *actual = nivelACerrar->primer_nodo;

    /*
     * Liberamos solo el andamiaje del nivel (los NodoSimbolo de la lista).
     * Los Simbolo en si NO se liberan aca: durante el analisis semantico el
     * AST quedo decorado con punteros nodo->simbolo, y el generador de codigo
     * que corre despues los necesita. La propiedad de cada Simbolo pasa al
     * AST, que vive hasta el final del programa.
     */
    while (actual != NULL) {
        NodoSimbolo *siguiente = actual->siguiente;
        free(actual);          // Libero solo el nodo de la lista
        actual = siguiente;
    }

    //El tope ahora es el anterior al tope anterior
    topeTablaSimbolos = nivelACerrar->anterior;
    free(nivelACerrar);
}

//Inserto un simbolo en el nivel actual de la tabla de simbolos (el tope de la pila)
Simbolo* insertarSimbolo(FlagSimbolo flag, char *nombre, TipoDato tipo) {
    if (topeTablaSimbolos == NULL) {
        fprintf(stderr, "Error: No hay un nivel abierto en la Tabla de Símbolos.\n");
        return NULL;
    }
    
    // Verificamos si ya existe en el MISMO nivel para evitar redeclaraciones
    NodoSimbolo *actual = topeTablaSimbolos->primer_nodo;
    while (actual != NULL) {
        //Verifico si el nombre del simbolo ya existe en el nivel actual
        if (strcmp(actual->simbolo->nombre, nombre) == 0) {
            printf("Identificador redeclarado!!!\n"); // Mensaje exacto de tus diapositivas
            return NULL; 
        }
        actual = actual->siguiente;
    }
    
    // Si no existe en el nivel actual, creamos los datos del símbolo
    Simbolo *nuevoSimbolo = (Simbolo *)malloc(sizeof(Simbolo));
    nuevoSimbolo->flag = flag;
    nuevoSimbolo->nombre = strdup(nombre);
    nuevoSimbolo->valor = 0; //Nota amadeo: Es 0 porque a la hora de insertar un simbolo no sabemos su valor...
    nuevoSimbolo->inicializado = 0; //recien declarada, todavia sin asignar
    nuevoSimbolo->tipo = tipo;
    
    // Creamos el nodo para insertarlo en la lista enlazada del nivel
    NodoSimbolo *nuevoNodo = (NodoSimbolo *)malloc(sizeof(NodoSimbolo));
    nuevoNodo->simbolo = nuevoSimbolo;
    nuevoNodo->siguiente = topeTablaSimbolos->primer_nodo;
    
    topeTablaSimbolos->primer_nodo = nuevoNodo;
    
    return nuevoSimbolo;
}

//Busca un simbolo en la tabla de simbolos, empezando desde el nivel actual y subiendo hasta el nivel global
Simbolo* buscarSimbolo(char *nombre) {
    // En este punto buscamos en la TS los identificadores[cite: 1]
    NodoNivel *nivelActual = topeTablaSimbolos;
    
    // Recorremos desde el nivel más profundo (actual) hacia el global
    while (nivelActual != NULL) {
        NodoSimbolo *nodoActual = nivelActual->primer_nodo;
        
        // Buscamos en la lista enlazada del nivel actual
        while (nodoActual != NULL) {
            if (strcmp(nodoActual->simbolo->nombre, nombre) == 0) {
                return nodoActual->simbolo; // Lo encontramos y devolvemos la info semántica pura
            }
            nodoActual = nodoActual->siguiente;
        }
        
        // Si no está en este bloque, bajamos al bloque circundante[cite: 2]
        nivelActual = nivelActual->anterior;
    }
    
    return NULL; // No está definido en ningún alcance visible
}