#include "tablaSimbolos.h"
#include "analizadorSemantico.h"
#include <stdio.h>
#include <string.h>

static void visitarBloque(nodoAST *bloque);
static void visitarDeclaraciones(nodoAST *declaraciones);
static void visitarSentencias(nodoAST *sentencias);
static void visitarDeclaracion(nodoAST *declaracion);
static void visitarSentencia(nodoAST *sentencia);
static char *visitarExpresion(nodoAST *expresion);

static void visitarBloque(nodoAST *bloque){
    if(!bloque){
        return;
    }

    abrirNivel();

    nodoAST *declaraciones = bloque->hijos[0]; 
    nodoAST *sentencias = bloque->hijos[2];

    visitarDeclaraciones(declaraciones);
    visitarSentencias(sentencias);

    cerrarNivel();
}

static void visitarDeclaraciones(nodoAST *declaraciones){
    if(!declaraciones){
        return;
    }

    nodoAST *declaracion = declaraciones->hijos[0];
    nodoAST *declaracionesAux = declaraciones->hijos[2];

    visitarDeclaracion(declaracion);
    visitarDeclaraciones(declaracionesAux);
}

static void visitarSentencias(nodoAST *sentencias){
    if(!sentencias){
        return;
    }

    nodoAST *sentencia = sentencias->hijos[0];
    nodoAST *sentenciasAux = sentencias->hijos[2];

    visitarSentencia(sentencia);
    visitarSentencias(sentenciasAux);
}

static void visitarDeclaracion(nodoAST *declaracion){
    if(!declaracion){
        return;
    }

    nodoAST *tipo = declaracion->hijos[0];
    nodoAST *id = declaracion->hijos[2];

    Simbolo *simbolo = insertarSimbolo(FLAG_VARIABLE, id->valor, tipo->valor);

    //Guardamos en el arbol el resultado 
    declaracion->simbolo = simbolo;
    id->simbolo = simbolo;
}

static void visitarSentencia(nodoAST *sentencia){
    if(!sentencia){
        return;
    }

    if(strcmp(sentencia->tipo,"OP_ASIG") == 0){
        nodoAST *id = sentencia->hijos[0];
        nodoAST *E = sentencia->hijos[2];
        
        //Verificamos el id
        Simbolo *simbolo = buscarSimbolo(id->valor);

        if(!simbolo){
            fprintf(stderr,"Ese simbolo no fue declarado\n");
        }

        id->simbolo = simbolo;

        //Verificamos la expresion (antes de marcar inicializada, asi "x = x + 1" detecta el uso previo)
        char *tipoExpresion = visitarExpresion(E);

        if(simbolo && tipoExpresion && strcmp(tipoExpresion, simbolo->tipo) != 0){
            fprintf(stderr,"No podes asignar eso a ese id\n");
        }

        //Ya hay una asignacion valida: la variable queda inicializada
        if(simbolo){
            simbolo->inicializado = 1;
        }
    }else if(strcmp(sentencia->tipo,"RETURN") == 0){
        nodoAST *E = sentencia->hijos[1];

        if(E){ //!NULL
            visitarExpresion(E);
        }
    }else{
        fprintf(stderr,"Que decis che\n");
    }
}

static char *visitarExpresion(nodoAST *expresion){
    if(!expresion){
        return NULL;
    }

    if(strcmp(expresion->tipo,"ID") == 0){
        Simbolo *simbolo = buscarSimbolo(expresion->valor);
        if(!simbolo){
            fprintf(stderr,"No existe el id ese\n");
            return NULL;
        }
        if(!simbolo->inicializado){
            fprintf(stderr,"Variable '%s' usada sin inicializar\n", expresion->valor);
        }
        expresion->simbolo = simbolo;
        return simbolo->tipo;
    }

    if(strcmp(expresion->tipo,"CTE_LOGICA") == 0){
        return "bool";
    }


    if(strcmp(expresion->tipo,"CTE_ENTERA") == 0){
        return "int";
    }

    if(strcmp(expresion->tipo,"OP_SUMA") == 0 || strcmp(expresion->tipo,"OP_PROD") == 0){
        char *tipoIzquierdo = visitarExpresion(expresion->hijos[0]);
        char *tipoDerecho = visitarExpresion(expresion->hijos[2]);

        if(tipoIzquierdo && tipoDerecho && strcmp(tipoIzquierdo, tipoDerecho) == 0){
            return tipoIzquierdo;
        }else{
            fprintf(stderr,"Como vas a hacer eso en la operación\n");
            return NULL;
        }
    }

    fprintf(stderr,"Expresion erronea\n");
    return NULL;
}

//Punto de entrada del analisis semantico: recibe la raiz (PROGRAMA) del AST
void analizarSemantica(nodoAST *raiz){
    if(!raiz){
        return;
    }

    inicializarTablaSimbolos();
    visitarBloque(raiz->hijos[1]); //PROGRAMA -> BLOQUE (hijo medio)
}