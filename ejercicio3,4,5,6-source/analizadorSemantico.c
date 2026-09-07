#include "tablaSimbolos.h"
#include "analizadorSemantico.h"
#include <stdio.h>

static void visitarBloque(nodoAST *bloque);
static void visitarDeclaraciones(nodoAST *declaraciones);
static void visitarSentencias(nodoAST *sentencias);
static void visitarDeclaracion(nodoAST *declaracion);
static void visitarSentencia(nodoAST *sentencia);
static TipoDato visitarExpresion(nodoAST *expresion);

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

    Simbolo *simbolo = insertarSimbolo(FLAG_VARIABLE, id->valor, tipoDesdeTexto(tipo->valor));

    //Guardamos en el arbol el resultado 
    declaracion->simbolo = simbolo;
    id->simbolo = simbolo;
}

static void visitarSentencia(nodoAST *sentencia){
    if(!sentencia){
        return;
    }

    if(sentencia->tipo == NODO_OP_ASIG){
        nodoAST *id = sentencia->hijos[0];
        nodoAST *E = sentencia->hijos[2];

        //Verificamos el id
        Simbolo *simbolo = buscarSimbolo(id->valor);

        if(!simbolo){
            fprintf(stderr,"Ese simbolo no fue declarado\n");
        }

        id->simbolo = simbolo;

        //Verificamos la expresion (antes de marcar inicializada, asi "x = x + 1" detecta el uso previo)
        TipoDato tipoExpresion = visitarExpresion(E);

        if(simbolo && tipoExpresion != TIPO_INDEFINIDO && tipoExpresion != simbolo->tipo){
            fprintf(stderr,"No podes asignar eso a ese id\n");
        }

        //Ya hay una asignacion valida: la variable queda inicializada
        if(simbolo){
            simbolo->inicializado = 1;
        }
    }else if(sentencia->tipo == NODO_RETURN){
        nodoAST *E = sentencia->hijos[1];

        if(E){ //!NULL
            visitarExpresion(E);
        }
    }else{
        fprintf(stderr,"Que decis che\n");
    }
}

static TipoDato visitarExpresion(nodoAST *expresion){
    if(!expresion){
        return TIPO_INDEFINIDO;
    }

    if(expresion->tipo == NODO_ID){
        Simbolo *simbolo = buscarSimbolo(expresion->valor);
        if(!simbolo){
            fprintf(stderr,"No existe el id ese\n");
            return TIPO_INDEFINIDO;
        }
        if(!simbolo->inicializado){
            fprintf(stderr,"Variable '%s' usada sin inicializar\n", expresion->valor);
        }
        expresion->simbolo = simbolo;
        return simbolo->tipo;
    }

    if(expresion->tipo == NODO_CTE_LOGICA){
        return TIPO_BOOL;
    }


    if(expresion->tipo == NODO_CTE_ENTERA){
        return TIPO_INT;
    }

    if(expresion->tipo == NODO_OP_SUMA || expresion->tipo == NODO_OP_PROD){
        TipoDato tipoIzquierdo = visitarExpresion(expresion->hijos[0]);
        TipoDato tipoDerecho = visitarExpresion(expresion->hijos[2]);

        if(tipoIzquierdo != TIPO_INDEFINIDO && tipoDerecho != TIPO_INDEFINIDO && tipoIzquierdo == tipoDerecho){
            return tipoIzquierdo;
        }else{
            fprintf(stderr,"Como vas a hacer eso en la operación\n");
            return TIPO_INDEFINIDO;
        }
    }

    fprintf(stderr,"Expresion erronea\n");
    return TIPO_INDEFINIDO;
}

//Punto de entrada del analisis semantico: recibe la raiz (PROGRAMA) del AST
void analizarSemantica(nodoAST *raiz){
    if(!raiz){
        return;
    }

    inicializarTablaSimbolos();
    visitarBloque(raiz->hijos[1]); //PROGRAMA -> BLOQUE (hijo medio)
}