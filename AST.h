#include <iostream>
#include <stdio.h>
#include <memory>
#include <vector>
#include <list>
#include <map>

extern std::list<std::string> regs;


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

            // cleaning up registers to be reused
            regs.push_front(right->addr);
            regs.push_front(left->addr);
            right->addr = "";
            left->addr = "";
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

            // cleaning up registers to be reused
            regs.push_front(right->addr);
            regs.push_front(left->addr);
            right->addr = "";
            left->addr = "";
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

            // cleaning up registers to be reused
            regs.push_front(right->addr);
            regs.push_front(left->addr);
            right->addr = "";
            left->addr = "";
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

            // cleaning up registers to be reused
            regs.push_front(right->addr);
            regs.push_front(left->addr);
            right->addr = "";
            left->addr = "";
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
            std::string add_code = "(COMP, " + left->addr + ", " + right->addr + ", LT)";
            // assembling code for add
            code.push_back(*(left->compile()->begin()));
            code.push_back(*(right->compile()->begin()));
            code.push_back(add_code);
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
            std::string add_code = "(COMP, " + left->addr + ", " + right->addr + ", GTE)";
            // assembling code for add
            code.push_back(*(left->compile()->begin()));
            code.push_back(*(right->compile()->begin()));
            code.push_back(add_code);
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
            std::string add_code = "(COMP, " + left->addr + ", " + right->addr + ", LTE)";
            // assembling code for add
            code.push_back(*(left->compile()->begin()));
            code.push_back(*(right->compile()->begin()));
            code.push_back(add_code);
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
            std::cout << " = " << expression->evaluate() << std::endl;
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

            // cleaning up expression register for reuse
            regs.push_front(expression->addr);
            expression->addr = "";
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
            for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                (*stmt)->print();
            }
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
            return new std::list<std::string>();
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
            std::cout << " THEN: ";
            std::vector<statement*>::iterator stmt;
            for (stmt = then_list->begin(); stmt != then_list->end(); stmt++) {
                (*stmt)->print();
            }
            std::cout << "ELSE: ";
            for (stmt = else_list->begin(); stmt != else_list->end(); stmt++) {
                (*stmt)->print();
            }
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
            return new std::list<std::string>();
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
            std::cout << " DO: ";
            std::vector<statement*>::iterator stmt;
            for (stmt = statement_list->begin(); stmt != statement_list->end(); stmt++) {
                (*stmt)->print();
            }
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
            return new std::list<std::string>();
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
            return new std::list<std::string>();
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
            for (i = statement_list->begin(); i != statement_list->end(); i++) {
                (*i)->print();
                (*i)->evaluate();
            }
        }
        void compile() {
            std::cout << "Outputting compiled SADGE VM instructions:" << std::endl;
            std::vector<statement*>::iterator i;
            for (i = statement_list->begin(); i != statement_list->end(); i++) {
                std::list<std::string>* stmt_code = (*i)->compile();
                std::list<std::string>::iterator j;
                for (j = stmt_code->begin(); j != stmt_code->end(); j++) {
                    std::cout << *j << /*"," << */ std::endl;
                }
            }
        }
};

extern std::map<std::string, var_node *> symbols;
