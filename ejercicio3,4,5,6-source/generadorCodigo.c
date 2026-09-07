#include "tablaSimbolos.h"
#include "generadorCodigo.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static FILE *salida = NULL;

// Emite una línea de código: por pantalla y, si se pudo abrir, al archivo.
static void emitir(const char *formato, ...){
    va_list args;

    va_start(args, formato);
    vprintf(formato, args);
    va_end(args);
    printf("\n");

    if(salida){
        va_start(args, formato);
        vfprintf(salida, formato, args);
        va_end(args);
        fprintf(salida, "\n");
    }
}

// Nombre a usar para un identificador: preferimos el Simbolo decorado por el
// semántico; si por algún error quedó sin enganchar, caemos al texto del nodo.
static const char *nombreDe(nodoAST *id){
    return id->simbolo ? id->simbolo->nombre : id->valor;
}

// Traduce una constante lógica ("true"/"false") a su valor entero.
static int valorLogico(const char *texto){
    return (texto && strcmp(texto, "true") == 0) ? 1 : 0;
}

/* ---------- SECCIÓN DE DATOS ---------- */

// Recorre el árbol de DECLARACIONES reservando una celda por variable.
static void generarDeclaraciones(nodoAST *declaraciones){
    if(!declaraciones){
        return;
    }

    nodoAST *declaracion = declaraciones->hijos[0];
    nodoAST *resto = declaraciones->hijos[2];

    if(declaracion){
        nodoAST *tipo = declaracion->hijos[0];   // TIPOVAR
        nodoAST *id = declaracion->hijos[2];     // ID
        emitir("    %-8s RESW 1        ; %s", nombreDe(id), tipo->valor);
    }

    generarDeclaraciones(resto);
}

/* ---------- EXPRESIONES ---------- */

/*
 * Genera el código de una expresión. El resultado siempre queda en R0.
 * Para los operadores binarios usa la pila: evalúa el operando izquierdo,
 * lo apila, evalúa el derecho y recompone la operación en R0.
 */
static void generarExpresion(nodoAST *expresion){
    if(!expresion){
        return;
    }

    if(expresion->tipo == NODO_CTE_ENTERA){
        emitir("    MOV   R0, #%s", expresion->valor);
        return;
    }

    if(expresion->tipo == NODO_CTE_LOGICA){
        emitir("    MOV   R0, #%d       ; %s", valorLogico(expresion->valor), expresion->valor);
        return;
    }

    if(expresion->tipo == NODO_ID){
        emitir("    LOAD  R0, %s", nombreDe(expresion));
        return;
    }

    if(expresion->tipo == NODO_OP_SUMA || expresion->tipo == NODO_OP_PROD){
        const char *instruccion = (expresion->tipo == NODO_OP_SUMA) ? "ADD" : "MUL";

        generarExpresion(expresion->hijos[0]);   // operando izquierdo -> R0
        emitir("    PUSH  R0");
        generarExpresion(expresion->hijos[2]);   // operando derecho -> R0
        emitir("    POP   R1");                   // izquierdo -> R1
        emitir("    %-5s R1, R0", instruccion);   // R1 <- R1 (op) R0
        emitir("    MOV   R0, R1");
        return;
    }

    fprintf(stderr, "Generador: expresion no reconocida '%s'\n", nodeKindNombre(expresion->tipo));
}

/* ---------- SENTENCIAS ---------- */

static void generarSentencia(nodoAST *sentencia){
    if(!sentencia){
        return;
    }

    if(sentencia->tipo == NODO_OP_ASIG){
        nodoAST *id = sentencia->hijos[0];
        nodoAST *expresion = sentencia->hijos[2];

        emitir("    ; %s = ...", nombreDe(id));
        generarExpresion(expresion);
        emitir("    STORE %s, R0", nombreDe(id));
        return;
    }

    if(sentencia->tipo == NODO_RETURN){
        nodoAST *expresion = sentencia->hijos[1];

        if(expresion){
            emitir("    ; return ...");
            generarExpresion(expresion);
            emitir("    MOV   RET, R0");
        }
        emitir("    RET");
        return;
    }

    fprintf(stderr, "Generador: sentencia no reconocida '%s'\n", nodeKindNombre(sentencia->tipo));
}

// Recorre la lista enlazada de SENTENCIAS respetando el orden de aparición.
static void generarSentencias(nodoAST *sentencias){
    if(!sentencias){
        return;
    }

    generarSentencia(sentencias->hijos[0]);
    generarSentencias(sentencias->hijos[2]);
}

/* ---------- PROGRAMA ---------- */

void generarCodigo(nodoAST *raiz){
    if(!raiz){
        return;
    }

    salida = fopen("salida.asm", "w");
    if(!salida){
        fprintf(stderr, "Generador: no se pudo crear salida.asm (igual imprimo por pantalla)\n");
    }

    nodoAST *bloque = raiz->hijos[1];            // PROGRAMA -> BLOQUE
    nodoAST *declaraciones = bloque ? bloque->hijos[0] : NULL;
    nodoAST *sentencias = bloque ? bloque->hijos[2] : NULL;

    printf("\n===== SEUDO-ASSEMBLY GENERADO =====\n");

    emitir(".DATA");
    generarDeclaraciones(declaraciones);

    emitir(".CODE");
    emitir("main:");
    generarSentencias(sentencias);
    emitir("    HALT");

    if(salida){
        fclose(salida);
        salida = NULL;
        printf("\n(codigo tambien guardado en salida.asm)\n");
    }
}
