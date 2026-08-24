/*
 * Parser sintáctico generado con Bison.
 *
 * Este archivo define la gramática del lenguaje y contiene la función main
 * que lee el archivo de entrada y llama a yyparse().
 *
 * Compilar:
 *   bison -d -o sintactico.tab.c sintactico.y
 *   flex -o lex.yy.c lexicoEjercicio3.l
 *   gcc -o parser sintactico.tab.c lex.yy.c -lfl
 *
 * Ejecutar:
 *   ./parser archivo_entrada.txt
 *
 * También se puede ejecutar sin argumento para leer desde la entrada estándar.
 */

%{
#include <stdio.h>
int yylex(void);
void yyerror(const char *s);
%}

%token INT BOOL VOID MAIN RETURN
%token CTE_LOGICA CTE_ENTERA ID
%token OP_SUMA OP_PROD OP_ASIG
%token PUNTO_COMA PAR_IZQ PAR_DER LLAVE_IZQ LLAVE_DER


%left OP_SUMA
%left OP_PROD

%%

P
    : T MAIN PAR_IZQ PAR_DER LLAVE_IZQ D S LLAVE_DER

T
    : INT
    | BOOL
    | VOID
    ;

Tv
    : INT
    | BOOL
    ;

D
    : D Dec
    | Dec
    ;

Dec
    : Tv ID PUNTO_COMA
    ;

S
    : S Sent
    |
    ;

Sent
    : ID OP_ASIG E PUNTO_COMA
    | RETURN E PUNTO_COMA
    | RETURN PUNTO_COMA
    ;

E
    : E OP_SUMA E
    | E OP_PROD E
    | PAR_IZQ E PAR_DER
    | CTE_ENTERA
    | CTE_LOGICA
    | ID
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error sintactico: %s\n", s);
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
    if (yyparse() == 0)
        printf("Programa aceptado\n");
    return 0;
}