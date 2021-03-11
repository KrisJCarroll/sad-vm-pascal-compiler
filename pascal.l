%option noyywrap
%{
    // code to load beforehand
    #include <map>
    #include <vector>
    #include <string>
    #include "AST.h"
    #include "parser.h"

%}

%%

"PROGRAM "  { return PROGRAM; }
"VAR"       { return VAR; }
"INTEGER"   { return INTEGER; }
"BEGIN"     { return BEG; }
"END"       { return END; }
"IF"        { return IF; }
"THEN"      { return THEN; }
"ELSE"      { return ELSE; }
"WHILE"     { return WHILE; }
"DO"        { return DO; }
"WRITELN"   { return WRITELN; }
"TRUE"      { return TRUE; }
"FALSE"     { return FALSE; }
":="        { return ASSIGN; }
"."         { return PERIOD; }
":"         { return COLON; }
";"         { return SEMI; }
"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MULT; }
"/"         { return DIV; }
">="        { return GTE; }
"<="        { return LTE; }
">"         { return GT; }
"<"         { return LT; }
","         { return ','; }
"("         { return '('; }
")"         { return ')'; }
[0-9]+      { int parsed_num = atoi(yytext); yylval.num = parsed_num; return NUM; }
[A-Za-z]*   { yylval.id = strdup(yytext); return ID; }
[ \t\n\r]   { /* ignoring whitespace */ }


%%
