#include <iostream>
#include <stdio.h>
#include <memory>
#include <vector>
#include <map>

extern std::map<std::string, int> symbols;


class expression_node {
    public:
        std::string code; // storage for generated code
        expression_node* left;
        expression_node* right;
        virtual void print() = 0;
        virtual int evaluate() = 0;
};

class num_node : public expression_node {
    protected:
        int val;
    public:
        num_node(int val_) : val(val_) {}
        void print() { std::cout << val; }
        int evaluate() { return val; };
};

class var_node : public expression_node {
    protected:
        std::string id;
    public:
        var_node(const char* id_) : id(std::string(id_)) {}
        void print() { std::cout << id; };
        int evaluate() { return symbols[id]; };
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
};

class mult_node : public expression_node {
    public:
        mult_node(expression_node* left_, expression_node* right_) { left = left_; right = right_;}
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
        }
};


class statement {
    public:
        std::string code; // storage for concatenated code strings
        expression_node* expression; // expression for each statement
        statement* statement1; // THEN/DO statement for IF-THEN/WHILE
        statement* statement2; // ELSE statement for IF-THEN-ELSE
        virtual void print() = 0;
        virtual void evaluate() = 0;
};

class assign_statement : public statement {
    public:
        std::string id;
        assign_statement(const char* id_, expression_node* exp_) { expression = exp_; id = std::string(id_); }
        void print() {
            std::cout << id << " := ";
            expression->print();
            std::cout << " = " << expression->evaluate() << std::endl;
        }
        void evaluate() {
            int result = expression->evaluate();
            symbols[id] = result;
        }
};

class program {
    protected:
        std::vector<statement*> *statement_list; // AST representation of the program's statements
        std::string code; // storage for concatenated code strings
    public:
        program(std::vector<statement*> *statements) : statement_list(statements) {}
        void evaluate() {
            std::vector<statement*>::iterator i;
            for (i = statement_list->begin(); i != statement_list->end(); i++) {
                (*i)->print();
                (*i)->evaluate();
            }
        }
};

