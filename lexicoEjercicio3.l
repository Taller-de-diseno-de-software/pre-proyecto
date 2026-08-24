/* lexico.l -- analizador lexico (inciso 3)
 * flex lexico.l && gcc -o lexico lex.yy.c
 * ./lexico programa.txt
 */

/* archivo de flex: definiciones %% reglas %% codigo C */

/* no hace falta yywrap() */
%option noyywrap
/* guarda el numero de linea en yylineno */
%option yylineno

%{
#include <stdio.h>
#include "sintactico.tab.h"
%}

/* macros para no repetir las ER, se usan como {ID} {NRO} */
LETRA   [a-zA-Z]
DIGITO  [0-9]
ID      {LETRA}({LETRA}|{DIGITO})*
NRO     {DIGITO}+

%%

  /* match mas largo gana, y si empatan gana la regla de arriba
     -> por eso las reservadas van antes que ID */
"int"       { printf("RESERVADA    %s\n", yytext); return INT;}
"bool"      { printf("RESERVADA    %s\n", yytext); return BOOL;}
"void"      { printf("RESERVADA    %s\n", yytext); return VOID;}
"main"      { printf("RESERVADA    %s\n", yytext); return MAIN;}
"return"    { printf("RESERVADA    %s\n", yytext); return RETURN;}

"true"      { printf("CTE_LOGICA   %s\n", yytext); return CTE_LOGICA;}
"false"     { printf("CTE_LOGICA   %s\n", yytext); return CTE_LOGICA;}

{NRO}       { printf("CTE_ENTERA   %s\n", yytext); return CTE_ENTERA;}

{ID}        { printf("ID           %s\n", yytext); return ID;}

"+"         { printf("OP_SUMA      %s\n", yytext); return OP_SUMA;}
"*"         { printf("OP_PROD      %s\n", yytext); return OP_PROD;}
"="         { printf("OP_ASIG      %s\n", yytext); return OP_ASIG;}

";"         { printf("PUNTO_COMA   %s\n", yytext); return PUNTO_COMA;}
"("         { printf("PAR_IZQ      %s\n", yytext); return PAR_IZQ;}
")"         { printf("PAR_DER      %s\n", yytext); return PAR_DER;}
"{"         { printf("LLAVE_IZQ    %s\n", yytext); return LLAVE_IZQ;}
"}"         { printf("LLAVE_DER    %s\n", yytext); return LLAVE_DER;}

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