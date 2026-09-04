/*
 * Analizador léxico desarrollado con Flex.
 *
 * Reconoce palabras reservadas, identificadores, constantes,
 * operadores y delimitadores y agrupa estas cadenas en TOKENS
 *
 * Compilar:
 *   bison -d -o analizadorSintactico.tab.c analizadorSintactico.y
 *   flex -o lex.yy.c analizadorLexico.l
 *   gcc -o parser analizadorSintactico.tab.c lex.yy.c tablaSimbolos.c analizadorSemantico.c generadorCodigo.c -lfl
 *
 * Ejecutar:
 *   ./parser archivo_entrada.txt
 */

%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include "tablaSimbolos.h"
#include "analizadorSintactico.tab.h"
%}

/* macros para no repetir las ER, se usan como {ID} {NRO} */
LETRA   [a-zA-Z]
DIGITO  [0-9]
ID      {LETRA}({LETRA}|{DIGITO})*
NRO     {DIGITO}+

%%

  /* match mas largo gana, y si empatan gana la regla de arriba
     -> por eso las reservadas van antes que ID */
"int"       { 
              printf("RESERVADA    %s\n", yytext); 
              yylval.str = strdup(yytext);
              return INT;
            }

"bool"      { 
              printf("RESERVADA    %s\n", yytext); 
              yylval.str = strdup(yytext);
              return BOOL;
            }

"void"      { 
              printf("RESERVADA    %s\n", yytext); 
              yylval.str = strdup(yytext);
              return VOID;
            }

"main"      { 
              printf("RESERVADA    %s\n", yytext);
              yylval.str = strdup(yytext); 
              return MAIN;
            }

"return"    { 
              printf("RESERVADA    %s\n", yytext); 
              yylval.str = strdup(yytext);
              return RETURN;
            }

"true"      { 
              printf("CTE_LOGICA   %s\n", yytext);
              yylval.str = strdup(yytext);
              return CTE_LOGICA;}
"false"     { 
              printf("CTE_LOGICA   %s\n", yytext); 
              yylval.str = strdup(yytext);
              return CTE_LOGICA;}

{NRO}       { printf("CTE_ENTERA   %s\n", yytext); 
              yylval.str = strdup(yytext);
              return CTE_ENTERA;
            }

{ID}        { 
              printf("ID           %s\n", yytext); 
              yylval.str = strdup(yytext);
              return ID;
            }

"+"         {   
              printf("OP_SUMA      %s\n", yytext); 
              yylval.str = strdup(yytext);
              return OP_SUMA;
            }

"*"         {   
              printf("OP_PROD      %s\n", yytext); 
              yylval.str = strdup(yytext);
              return OP_PROD;}
"="         {   
              printf("OP_ASIG      %s\n", yytext); 
              yylval.str = strdup(yytext);
              return OP_ASIG;
            }

";"         {   
              printf("PUNTO_COMA   %s\n", yytext); 
              yylval.str = strdup(yytext);
              return PUNTO_COMA;
            }

"("         {   
              printf("PAR_IZQ      %s\n", yytext); 
              yylval.str = strdup(yytext);
              return PAR_IZQ;
            }

")"         {   
              printf("PAR_DER      %s\n", yytext); 
              yylval.str = strdup(yytext);
              return PAR_DER;
            }

"{"         {   
              printf("LLAVE_IZQ    %s\n", yytext); 
              yylval.str = strdup(yytext);
              return LLAVE_IZQ;
            }

"}"         {   
              printf("LLAVE_DER    %s\n", yytext); 
              yylval.str = strdup(yytext);
              return LLAVE_DER;
            }

[ \t\r\n]+  { }   /* se ignoran */

.           { printf("ERROR linea %d: caracter no reconocido '%s'\n", yylineno, yytext); }

%%
/*
*int main(int argc, char **argv)
*{
*    if (argc > 1) {                      si pasan un archivo, leer de ahi 
*        yyin = fopen(argv[1], "r");
*        if (yyin == NULL) {
*            printf("No se pudo abrir %s\n", argv[1]);
*            return 1;
*        }
*    }
*
*    yylex();
*
*    return 0;
*}
*/