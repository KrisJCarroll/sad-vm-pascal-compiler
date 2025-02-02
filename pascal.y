%{
    /* pascal.y
       Author: Kristopher J. Carroll
       Description:
          YACC/Bison file for parsing input code from a small subset of the Pascal programming language.
          As the YACC format is generally well documented and fairly simple to follow, very little explanation
          will be provided.
    */
    // include code needed at the beginning here
    #include <iostream>
    #include <list>
    #include <map>
    #include <string>
    #include "AST.h"
    #include "parser.h"
    std::map<std::string, var_node*> symbols;
    void insert_symbol(std::string symbol);
    var_node* lookup_symbol(std::string symbol);
    program* root;
    int yyerror(const char* s);
    int yylex();
    int line_num = 0;

    std::list<std::string> regs = {"R_0", "R_1", "R_2", "R_3", "R_4", "R_5", "R_6", 
                                   "R_7", "R_8", "R_9", "R_10", "R_11", "R_12", "R_13"}; 

%}
// Type union for YYLVAL
%union {
    char* id;
    int num;
    std::vector<const char*> *id_list;
    expression_node* expr_node;
    std::vector<statement*> *statement_list;
    statement* statement;
    program* program;
}
%token <num> NUM
%token <id> ID
%token PROGRAM BEG END PERIOD ASSIGN SEMI
%token VAR COLON INTEGER
%token IF THEN ELSE WHILE DO
%token WRITELN
%token TRUE FALSE
%token '(' ')' ','
%left LT GT GTE LTE
%left ADD SUB
%left MULT DIV
// these are used to set proper precedence for unary minus and IF vs. IF-ELSE productions
%right UMINUS
%nonassoc IFX
%nonassoc ELSE
// all of these nonterminals have the type of expression_node from AST.h
%type <expr_node> expression additive_expression multiplicative_expression unary_expression primary_expression
// further types for nonterminals from AST.h
%type <id_list> id_list
%type <statement_list> statement_list block
%type <statement> statement
%type <program> program



%%
program: PROGRAM ID SEMI decl block PERIOD { $$ = new program($5); root = $$; }
;


decl: VAR id_list COLON INTEGER SEMI
;


id_list: ID { insert_symbol($1); } 
       | id_list ',' ID { insert_symbol($3); }
;

block: BEG statement_list END { $$ = $2; }
     | statement {$$ = new std::vector<statement*>(); $$->push_back($1); }
;

statement_list: statement_list SEMI statement { $1->push_back($3); $$ = $1; }   
              | statement { $$ = new std::vector<statement*>(); $$->push_back($1); } 
              
;

statement: ID ASSIGN expression { $$ = new assign_statement(lookup_symbol($1), $3); }
         | IF expression THEN block %prec IFX { $$ = new if_statement($2, $4); }
         | IF expression THEN block ELSE block { $$ = new if_else_statement($2, $4, $6); }
         | WHILE expression DO block { $$ = new while_statement($2, $4); }
         | WRITELN expression { $$ = new write_statement($2); }
;

expression: expression GT additive_expression  { $$ = new gt_node($1, $3); }
          | expression LT additive_expression  { $$ = new lt_node($1, $3); }
          | expression GTE additive_expression { $$ = new gte_node($1, $3); }
          | expression LTE additive_expression { $$ = new lte_node($1, $3); }
          | additive_expression { $$ = $1; }
;

additive_expression: additive_expression ADD multiplicative_expression { $$ = new add_node($1, $3); }
                   | additive_expression SUB multiplicative_expression { $$ = new sub_node($1, $3); }
                   | multiplicative_expression { $$ = $1; }
;

multiplicative_expression: multiplicative_expression MULT unary_expression { $$ = new mult_node($1, $3); }
                         | multiplicative_expression DIV unary_expression { $$ = new div_node($1, $3); }
                         | unary_expression { $$ = $1; }
;

unary_expression: SUB unary_expression %prec UMINUS { $$ = new num_node(-($2->evaluate())); }
                | primary_expression { $$ = $1; }
;

primary_expression: ID { $$ = lookup_symbol($1); } 
                  | NUM { $$ = new num_node($1); }
                  | '(' expression ')' { $$ = $2; }
;

%%

int main() {
    yyparse();
    root->evaluate();
    root->compile();
}

int yyerror(const char* s) {
    printf("Error during parse: %s\n", s);
    return 1;
}

// helper function for inserting symbols into the symbol map
// will report an error if the same symbol is attempted to be declared twice
void insert_symbol(std::string symbol) {
    if (symbols.find(symbol) == symbols.end()) {
        symbols[symbol] = new var_node(symbol);
    }
    else {
        std::string error_msg = "symbol previously declared: ";
        error_msg = error_msg + symbol;
        yyerror(&error_msg[0]);
        exit(-1);
    }
}

// helper function for looking up a symbol in the symbol table
// will report an error if a symbol not previously declared is attempted to be used
var_node* lookup_symbol(std::string symbol) {
    if (symbols.find(symbol) != symbols.end()) {
        return symbols[symbol];
    }
    else {
        std::string error_msg = "symbol not previously declared: ";
        error_msg = error_msg + symbol;
        yyerror(&error_msg[0]);
        exit(-1);
    }
}