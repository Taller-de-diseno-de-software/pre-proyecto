/*
 * Analizador semántico.
 *
 * Recorre el AST que produjo el parser y verifica las reglas que la
 * gramática libre de contexto no puede expresar:
 *  - toda variable usada fue declarada (y una sola vez por nivel)
 *  - toda variable usada en una expresión fue inicializada antes
 *  - los tipos son coherentes (asignaciones y operadores)
 *
 * De paso decora el AST: engancha en cada nodo ID el puntero al
 * Simbolo correspondiente de la tabla de símbolos.
 */

#ifndef ANALIZADOR_SEMANTICO_H
#define ANALIZADOR_SEMANTICO_H

#include "tablaSimbolos.h"

// Punto de entrada. Recibe la raíz del AST (nodo "PROGRAMA").
void analizarSemantica(nodoAST *raiz);

#endif
