#include <iostream>
#include <stdio.h>
#include <memory>
#include <vector>
#include <list>
#include <map>

extern std::list<std::string> regs;
extern int line_num;

class expression_node {
    protected: 
        std::string get_reg() {
            std::string reg = regs.front();
            regs.pop_front();
            return reg;
        }
    public:
        std::list<std::string> code; // storage for generated code
        std::string addr; // register location for each node
        expression_node* left;
        expression_node* right;
        virtual void print() = 0;
        virtual int evaluate() = 0;
        virtual std::list<std::string>* compile() = 0;
        virtual void free_reg()  { regs.push_front(addr); addr = ""; }
        
};

class num_node : public expression_node {
    protected:
        int val;
    public:
        num_node(int val_) : val(val_) { }
        void print() { std::cout << val; }
        int evaluate() { return val; }
        std::list<std::string>* compile() {
            addr = get_reg();
            // build the load immediate instruction to load the value
            std::string num_code = "(LIMM, " + addr + ", " + std::to_string(val) + ")";
            code.push_front(num_code);
            line_num++;

            return &code;
        }
};

class var_node : public expression_node {
    protected:
        std::string id;
    public:
        int val;
        var_node(std::string id_) : id(std::string(id_)) { addr = get_reg(); }
        void print() { std::cout << id; }
        int evaluate() { return val; }
        std::list<std::string>* compile() {
            return new std::list<std::string>();
        }
        void free_reg() { return; } 
};

class add_node : public expression_node {
    public:
        add_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " + ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() + right->evaluate();
        }
        std::list<std::string>* compile() {
            // getting temporary address to store result in
            addr = get_reg();
            // assembling code for ADD
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string add_code = "(MATH, " + addr + ", " + left->addr + ", " + right->addr + ", ADD)";
            code.push_back(add_code);
            line_num++;

            // cleaning up registers to be reused
            right->free_reg();
            left->free_reg();

            return &code;
        }
};

class sub_node : public expression_node {
    public:
        sub_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " - ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() - right->evaluate();
        }
        std::list<std::string>* compile() {
            // getting temporary address to store result in
            addr = get_reg();
            // assembling code for SUB
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string sub_code = "(MATH, " + addr + ", " + left->addr + ", " + right->addr + ", SUB)";
            code.push_back(sub_code);
            line_num++;

            // cleaning up registers to be reused
            right->free_reg();
            left->free_reg();

            return &code;
        }
};

class mult_node : public expression_node {
    public:
        mult_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " * ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() * right->evaluate();
        }
        std::list<std::string>* compile() {
            // getting temporary address to store result in
            addr = get_reg();
            // assembling code for MULT
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string mult_code = "(MATH, " + addr + ", " + left->addr + ", " + right->addr + ", MULT)";
            code.push_back(mult_code);
            line_num++;

            // cleaning up registers to be reused
            right->free_reg();
            left->free_reg();

            return &code;
        }
};

class div_node : public expression_node {
    public:
        div_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " / ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() / right->evaluate();
        }
        std::list<std::string>* compile() {
            // getting temporary address to store result in
            addr = get_reg();
            // assembling code for DIV
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string div_code = "(MATH, " + addr + ", " + left->addr + ", " + right->addr + ", DIV)";
            code.push_back(div_code);
            line_num++;

            // cleaning up registers to be reused
            left->free_reg();
            right->free_reg();

            return &code;
        }
        
};

class gt_node : public expression_node {
    public:
        gt_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " > ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() > right->evaluate();
        }
        std::list<std::string>* compile() {
            addr = get_reg();
            // assembling code for GT comparison
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string gt_code = "(COMP, " + left->addr + ", " + right->addr + ", GT)";
            code.push_back(gt_code);
            line_num++;

            return &code;
        }
};

class lt_node : public expression_node {
    public:
        lt_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " < ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() < right->evaluate();
        }
        std::list<std::string>* compile() {
            addr = get_reg();
            // assembling code for LT comparison
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string gt_code = "(COMP, " + left->addr + ", " + right->addr + ", LT)";
            code.push_back(gt_code);
            line_num++;

            return &code;
        }
};

class gte_node : public expression_node {
    public:
        gte_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " >= ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() >= right->evaluate();
        }
        std::list<std::string>* compile() {
            addr = get_reg();
            // assembling code for GTE comparison
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string gt_code = "(COMP, " + left->addr + ", " + right->addr + ", GTE)";
            code.push_back(gt_code);
            line_num++;

            return &code;
        }
};

class lte_node : public expression_node {
    public:
        lte_node(expression_node* left_, expression_node* right_) { left = left_; right = right_; }
        void print() {
            std::cout << "(";
            left->print();
            std::cout << " <= ";
            right->print();
            std::cout << ")";
        }
        int evaluate() {
            return left->evaluate() <= right->evaluate();
        }
        std::list<std::string>* compile() {
            addr = get_reg();
            // assembling code for LTE comparison
            std::list<std::string> *left_code = left->compile();
            std::list<std::string>::iterator i;
            for (i = left_code->begin(); i != left_code->end(); i++) {
                code.push_back(*i);
            }
            std::list<std::string> *right_code = right->compile();
            for (i = right_code->begin(); i != right_code->end(); i++) {
                code.push_back(*i);
            }
            std::string gt_code = "(COMP, " + left->addr + ", " + right->addr + ", LTE)";
            code.push_back(gt_code);
            line_num++;

            return &code;
        }
};


class statement {
    protected:
        std::list<std::string> code; // storage for concatenated code strings
        expression_node* expression; // expression for each statement
    public:
        virtual void print() = 0;
        virtual void evaluate() = 0;
        virtual std::list<std::string>* compile() = 0;
};

class assign_statement : public statement {
    protected:
        var_node* id;
    public:
        assign_statement(var_node* id_, expression_node* exp) : id(id_)  { expression = exp; }
        void print() {
            id->print();
            std::cout << " := ";
            expression->print();
        }
        void evaluate() {
            int result = expression->evaluate();
            id->val = result;
        }
        std::list<std::string>* compile() {
            // getting code from members and generating assign code
            std::list<std::string>* expr_code = expression->compile();
            std::string assign_code = "(MOV, " + id->addr + ", " + expression->addr + ")";

            // concatenating code
            std::list<std::string>::iterator i;
            for (i = expr_code->begin(); i != expr_code->end(); i++) {
                code.push_back(*i);
            }
            code.push_back(assign_code);
            line_num++;

            // cleaning up expression register for reuse
            expression->free_reg();

            return &code;
        }
};

class if_statement : public statement {
    protected:
        std::vector<statement*>* statement_list;
    public:
        if_statement(expression_node* exp, std::vector<statement*> *then_statement) : statement_list(then_statement) { expression = exp; }
        void print() {
            std::cout << "IF ";
            expression->print();
            std::cout << " THEN: ";
            std::vector<statement*>::iterator stmt;
            std::cout << "{" << std::endl;
            for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                std::cout << "\t";
                (*stmt)->print();
            }
            std::cout << "}" << std::endl;
        }
        void evaluate() {
            std::cout << "Evaluating IF cond: " << bool(expression->evaluate()) << std::endl;
            if (expression->evaluate()) {
                std::cout << "Evaluating statements: ";
                std::vector<statement*>::iterator stmt;
                for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                    (*stmt)->print();
                    (*stmt)->evaluate();
                }
            }
        }
        std::list<std::string>* compile() {
            std::list<std::string>* expr_code = expression->compile();
            std::list<std::string>::iterator i;
            for (i = expr_code->begin(); i != expr_code->end(); i++) {
                code.push_back(*i);
            }

            // flags for looking for first code of THEN statement for JUMP
            std::list<std::string>::iterator first_code;
            bool is_first = true;

            // looping through statement_list code
            std::vector<statement*>::iterator stmt;
            for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                std::list<std::string>* stmt_code = (*stmt)->compile();
                for (i = stmt_code->begin(); i != stmt_code->end(); i++) {
                    if (is_first) {
                        is_first = false;
                        code.push_back(*i);
                        first_code = --code.end();
                    }
                    else {
                        code.push_back(*i);
                    }
                }
            }

            // backpatching the jump out of THEN block
            std::string jump_code = "(JMPC, " + std::to_string(++line_num) + ")";
            code.insert(first_code, jump_code);

            // cleaning up registers
            expression->free_reg();

            return &code;
        }
};

class if_else_statement : public statement {
    protected:
        std::vector<statement*> *then_list;
        std::vector<statement*> *else_list;
    public:
        if_else_statement(expression_node* exp, std::vector<statement*> *then_statement, std::vector<statement*> *else_statement) :
            then_list(then_statement), else_list(else_statement) {expression = exp; }
        void print() {
            std::cout << "IF ";
            expression->print();
            std::cout << " THEN {" << std::endl;
            std::vector<statement*>::iterator stmt;
            for (stmt = then_list->begin(); stmt != then_list->end(); stmt++) {
                std::cout << "\t";
                (*stmt)->print();
            }
            std::cout << "}" << std::endl;
            std::cout << "ELSE {" << std::endl;
            for (stmt = else_list->begin(); stmt != else_list->end(); stmt++) {
                std::cout << "\t";
                (*stmt)->print();
            }
            std::cout << "}" << std::endl;
        }

        void evaluate() {
            std::cout << "Evaluating IF cond: " << expression->evaluate() << std::endl;
            if (expression->evaluate()) {
                std::cout << "Evaluating statements: ";
                std::vector<statement*>::iterator stmt;
                for (stmt = then_list->begin(); stmt != then_list->end(); stmt++) {
                    (*stmt)->print();
                    (*stmt)->evaluate();
                }
            }
            else {
                std::cout << "Evaluating statements: ";
                std::vector<statement*>::iterator stmt;
                for (stmt = else_list->begin(); stmt != else_list->end(); stmt++) {
                    (*stmt)->print();
                    (*stmt)->evaluate();
                }
            }

        }
        std::list<std::string>* compile() {
            std::list<std::string>* expr_code = expression->compile();
            std::list<std::string>::iterator i;
            for (i = expr_code->begin(); i != expr_code->end(); i++) {
                code.push_back(*i);
            }

            // flags for looking for first code of THEN statement for JUMP
            std::list<std::string>::iterator first_code;
            bool is_first = true;

            // looping through THEN statements
            std::vector<statement*>::iterator stmt;
            for (stmt = then_list->begin(); stmt != then_list->end(); stmt++) {
                std::list<std::string>* stmt_code = (*stmt)->compile();
                for (i = stmt_code->begin(); i != stmt_code->end(); i++) {
                    if (is_first) {
                        is_first = false;
                        code.push_back(*i);
                        first_code = --code.end();
                    }
                    else {
                        code.push_back(*i);
                    }
                }
            }

            // backpatching the jump instruction to get to ELSE
            std::string jump_code = "(JMPC, " + std::to_string(line_num + 1) + ")";
            line_num++;
            code.insert(first_code, jump_code);

            // grabbing the last instruction of THEN to backpatch jump past ELSE
            std::list<std::string>::iterator last_code = --code.end();

            // looping through ELSE statements
            for (stmt = else_list->begin(); stmt != else_list->end(); stmt++) {
                std::list<std::string>* stmt_code = (*stmt)->compile();
                for (i = stmt_code->begin(); i != stmt_code->end(); i++) {
                    code.push_back(*i);
                }
            }

            // backpatching jump past ELSE
            std::string jump_past_else = "(JMPC, " + std::to_string(++line_num) + ")";
            code.insert(last_code, jump_past_else);

            // cleaning up registers
            expression->free_reg();

            return &code;
        }
};

class while_statement : public statement {
    protected:
        std::vector<statement*> *statement_list;
    public:
        while_statement(expression_node* exp, std::vector<statement*> *loop_body) : statement_list(loop_body) { expression = exp; }
        void print() {
            std::cout << "WHILE ";
            expression->print();
            std::cout << " DO: {" << std::endl;
            std::vector<statement*>::iterator stmt;
            for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                std::cout << "\t";
                (*stmt)->print();
            }
            std::cout << std::endl << "}" << std::endl;
        }
        void evaluate() {
            std::cout << "Evaluating WHILE cond: " << expression->evaluate() << std:: endl;
            while (expression->evaluate()) {
                std::cout << "Evaluating statements: ";
                std::vector<statement*>::iterator stmt;
                for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                    (*stmt)->print();
                    (*stmt)->evaluate();
                }
            }
        }
        std::list<std::string>* compile() {
            // compiling expression
            std::list<std::string>* expr_code = expression->compile();
            std::list<std::string>::iterator i;
            for (i = expr_code->begin(); i != expr_code->end(); i++) {
                code.push_back(*i);
            }
            // grabbing line number and index of COMP instruction for backpatching jump statement if false
            int comp_line_num = line_num;

            // preparing flags to grab the first instruction of statements for backpatching jump out
            std::list<std::string>::iterator begin_loop;
            bool is_first = true;

            // compiling statements
            std::vector<statement*>::iterator stmt;
            for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                std::list<std::string>* stmt_code = (*stmt)->compile();
                for (i = stmt_code->begin(); i != stmt_code->end(); i++) {
                    if (is_first) {
                        is_first = false;
                        code.push_back(*i);
                        begin_loop = --code.end();
                    }
                    else {
                        code.push_back(*i);
                    }
                }
            }
            // jumping back to top of loop
            code.push_back("(JMP, " + std::to_string(comp_line_num) + ")");
            line_num++;

            // backpatching jump out of loop
            std::string loop_jump = "(JMPC, " + std::to_string(++line_num + 1) + ")";
            code.insert(begin_loop, loop_jump);

            // cleaning up registers for expression
            expression->free_reg();

            return &code;
        }
};

class write_statement : public statement {
    public:
        write_statement(expression_node* exp) { expression = exp; } 
        void print() { 
            std::cout << "WRITELN ";
            expression->print();
            std::cout << std::endl;
        }
        void evaluate() {
            std::cout << expression->evaluate() << std::endl;
        }
        std::list<std::string>* compile() {
            std::string write_code = "(MEM, IO_OUT, " + expression->addr + ", STOR)";
            code.push_back(write_code);
            line_num++;
            return &code;
        }
};

class program {
    protected:
        std::vector<statement*> *statement_list; // AST representation of the program's statements
        std::list<std::string> code; // storage for concatenated code strings
    public:
        program(std::vector<statement*> *statements) : statement_list(statements) {}
        void evaluate() {
            std::vector<statement*>::iterator i;
            std::cout << "Printing parsed statements:" << std::endl;
            for (i = statement_list->begin(); i != statement_list->end(); i++) {
                (*i)->print();
                std::cout << std::endl;
            }
            std::cout << std::endl;

            std::cout << "Evaluating parsed statements:" << std::endl;
            for (i = statement_list->begin(); i != statement_list->end(); i++) {
                (*i)->evaluate();
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        void compile() {
            std::vector<statement*>::iterator i;
        /*
            // outputting compiled instructions with line numbers
            int x = 0;
            std::cout << "Outputting compiled SADGE VM instructions:" << std::endl;
            for (i = statement_list->begin(); i != statement_list->end(); i++) {
                std::list<std::string>* stmt_code = (*i)->compile();
                std::list<std::string>::iterator j;
                for (j = stmt_code->begin(); j != stmt_code->end(); j++) {
                    std::cout << x++ << ": " << *j << std::endl;
                }
            }
            std::cout << x << ": (JMP, None)" << std::endl;
            std::cout << std::endl;
        */

            // outputting compiled instructions in copy-paste format
            std::cout << "Copy/paste format for input into SADGE VM:" << std::endl;
            for (i = statement_list->begin(); i != statement_list->end(); i++) {
                std::list<std::string>* stmt_code = (*i)->compile();
                std::list<std::string>::iterator j;
                for (j = stmt_code->begin(); j != stmt_code->end(); j++) {
                    std::cout << *j << "," << std::endl;
                }
            }
            // outputting exit instruction 
            std::cout << "(JMP, None)" << std::endl;
        }
};

extern std::map<std::string, var_node *> symbols;
