/*
 * Analizador sintáctico generado con Bison. (Parser)
 *
 * Su objetivo principal es recibir la secuencia abstracta de tokens producidos por el analizadorLexico 
 * y verificar que dicha secuencia pueda ser generada por la gramática libre de contexto del lenguaje fuente.
 * Producto resultante: El AST
 *
 * Compilar:
 *   bison -d -o analizadorSintactico.tab.c analizadorSintactico.y
 *   flex -o lex.yy.c analizadorLexico.l
 *   gcc -o parser analizadorSintactico.tab.c lex.yy.c -lfl
 *
 * Ejecutar:
 *   ./parser archivo_entrada.txt
 *
 * También se puede ejecutar sin argumento para leer desde la entrada estándar.
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "tablaSimbolos.h"

#define nHijos 3;
//Creacion del nodo raiz, los nodos ahora serán nodosAST definidos en tablaSimbolos.h
nodoAST *raiz;


//Metodo para creacion de nodos hijos
nodoAST *crearNodo(const char *tipo, char *valor, nodoAST *hijoIzq, nodoAST *hijoMed, nodoAST *hijoDer){
    nodoAST *nuevoNodo = malloc(sizeof(nodoAST));
    nuevoNodo->tipo = strdup(tipo);
    nuevoNodo->valor = valor;
    
    nuevoNodo->hijos[0] = hijoIzq;
    nuevoNodo->hijos[1] = hijoMed;
    nuevoNodo->hijos[2] = hijoDer;

    return nuevoNodo;
}

//Metodo para imprimir arbol
void imprimirArbol(nodoAST *nodo, int nivel){
    if (!nodo) return;
    for (int i = 0; i < nivel; i++){
        printf("  ");
    } 
    
    printf("%s%s%s\nodo", nodo->tipo, nodo->valor ? " : " : "", nodo->valor ? nodo->valor : "");
    
    for (int i = 0; i < nHijos; i++) {
        imprimirArbol(nodo->hijos[i], nivel + 1);
    }
}

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

%union{
    nodoAST *nodo;
    char *str;
}

%token INT BOOL VOID MAIN RETURN
%token <str> CTE_LOGICA CTE_ENTERA ID       //El valor semantico de los tokens de esta linea estaran guardados en el campo <str> de la union
%token OP_SUMA OP_PROD OP_ASIG
%token PUNTO_COMA PAR_IZQ PAR_DER LLAVE_IZQ LLAVE_DER

%type <nodo> P T Tv D Dec S Sent E

%left OP_SUMA
%left OP_PROD

%%

P
    : T MAIN PAR_IZQ PAR_DER LLAVE_IZQ D S LLAVE_DER {
        nodoAST *bloque = crearNodo("BLOQUE", NULL, $6, NULL, $7);
        $$ = crearNodo("PROGRAMA",NULL, $1, bloque, NULL);
        raiz = $$;
    } 

T
    : INT {$$ = crearNodo("TIPOFUNC", "int", NULL, NULL, NULL);}
    | BOOL {$$ = crearNodo("TIPOFUNC", "bool", NULL, NULL, NULL);}
    | VOID {$$ = crearNodo("TIPOFUNC", "void", NULL, NULL, NULL);}
    ;

Tv
    : INT {$$ = crearNodo("TIPOVAR", "int", NULL, NULL, NULL);}
    | BOOL {$$ = crearNodo("TIPOVAR", "bool", NULL, NULL, NULL);}
    ;

D
    : Dec D {$$ = crearNodo("DECLARACIONES", NULL, $1, NULL, $2);}
    | Dec {$$ = crearNodo("DECLARACIONES", NULL, $1, NULL, NULL);}
    ;

Dec
    : Tv ID PUNTO_COMA {
        nodoAST *nodo = crearNodo("ID", $2, NULL, NULL, NULL);
        $$ = crearNodo("DECLARACION", NULL, $1, NULL, nodo);
    } 
    ;

S
    : Sent S {$$ = crearNodo("SENTENCIAS", NULL, $1, NULL,$2);}
    |          {$$ = NULL;}
    ;

Sent 
    : ID OP_ASIG E PUNTO_COMA {
        nodoAST *nodo = crearNodo("ID", $1, NULL, NULL, NULL);             //hijo medio
        $$ = crearNodo("OP_ASIG", NULL, nodo, NULL, $3);                //ID = E;
    } 
    | RETURN E PUNTO_COMA {$$ = crearNodo("RETURN", NULL, NULL, $2, NULL);}
    | RETURN PUNTO_COMA {$$ = crearNodo("RETURN", NULL, NULL, NULL, NULL);}
    ;

E
    : E OP_SUMA E {$$ = crearNodo("OP_SUMA", NULL, $1, NULL, $3);}
    | E OP_PROD E {$$ = crearNodo("OP_PROD", NULL, $1, NULL, $3);}
    | PAR_IZQ E PAR_DER {$$ = $2;}
    | CTE_ENTERA    {$$ = crearNodo("CTE_ENTERA", $1, NULL, NULL, NULL);}
    | CTE_LOGICA    {$$ = crearNodo("CTE_LOGICA", $1, NULL, NULL, NULL);}
    | ID    {$$ = crearNodo("ID", $1, NULL, NULL, NULL);}    //$$ = $1
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error sintactico en linea %d: %s\n", yylineno, s);
}

int main(int argc, char **argv) {
    extern FILE *yyin;
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            printf("No se pudo abrir %s\n", argv[1]);
            return 1;
        }
    }
    if (yyparse() == 0) {
        printf("Programa aceptado\n");
        imprimirArbol(raiz, 0);
    }
    return 0;
}