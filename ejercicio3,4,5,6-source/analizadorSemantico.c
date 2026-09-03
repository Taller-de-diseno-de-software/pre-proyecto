#include "tablaSimbolos.h"

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

    nodoAST *declaraciones = bloque->hijos[0]; 
    nodoAST *sentencias = bloque->hijos[2];

    visitarDeclaraciones(declaraciones);
    visitarSentencias(sentencias);
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

static void visitarDeclaracion(nodoAst *declaracion){
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
            fprintf(stderr,"Ese simbolo no fue declarado");
        }

        id->simbolo = simbolo;

        //Verificamos la expresion
        char *tipoExpresion = visitarExpresion(E);
    }else if(strcmp(sentencia->tipo,"RETURN") == 0){
        nodoAST *E = semtencia->hijos[1];

        if(E){ //!NULL
            visitarExpresion(E);
        }
    }else{
        fprintf(stderr,"Que decis che");
    }
}

static char *visitarExpresion(nodoAST *expresion){
    if(!expresion){
        return NULL;
    }

    if(strcmp(expresion->tipo,"ID") == 0){
        Simbolo *simbolo = buscarSimbolo(expresion->valor);
        if(!simbolo){
            fprintf(stderr,"No existe el id ese")
            return NULL;
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
        
        if(tipoIzquierdo == tipoDerecho){
            return tipoIzquierdo;
        }else{
            fprintf(stderr,"Como vas a hacer eso en la suma");
            return NULL;
        }
    }

    fprintf(stderr,"Expresion erronea")
    return NULL;
}

int main(){
    
}