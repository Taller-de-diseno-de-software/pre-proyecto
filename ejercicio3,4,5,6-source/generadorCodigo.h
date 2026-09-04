/*
 * Generador de código.
 *
 * Recorre el AST decorado (ya pasó por el análisis semántico, así que
 * cada nodo ID tiene su puntero a Simbolo) y emite un seudo-assembly
 * para una máquina simple de acumulador + pila:
 *
 *   Registros:  R0  -> acumulador (resultado de toda expresión queda acá)
 *               R1  -> registro auxiliar para operaciones binarias
 *               RET -> valor de retorno de la función
 *
 *   Instrucciones que se generan:
 *     MOV   Rd, #n        Rd <- constante n
 *     LOAD  Rd, var       Rd <- contenido de la variable var
 *     STORE var, Rd       var <- contenido de Rd
 *     PUSH  Rs            apila Rs
 *     POP   Rd           desapila en Rd
 *     ADD   Rd, Rs        Rd <- Rd + Rs
 *     MUL   Rd, Rs        Rd <- Rd * Rs
 *     RET                retorna (usa RET como valor)
 *
 * La salida se imprime por pantalla y además se escribe en "salida.asm".
 *
 * Corre después del análisis semántico y usa el AST decorado: cada nodo ID
 * apunta a su Simbolo en la tabla (esos Simbolo siguen vivos, ver
 * tablaSimbolos.h).
 */

#ifndef GENERADOR_CODIGO_H
#define GENERADOR_CODIGO_H

#include "tablaSimbolos.h"

// Punto de entrada. Recibe la raíz del AST (nodo "PROGRAMA").
void generarCodigo(nodoAST *raiz);

#endif
