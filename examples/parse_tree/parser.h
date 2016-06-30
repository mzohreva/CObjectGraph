#ifndef __PARSER_H__
#define __PARSER_H__

struct ExprNode;
struct TermNode;
struct FactorNode;

struct ExprNode {
    struct TermNode * term;
    char op;        // '+' or '-' or '\0'
    struct ExprNode * expr;
};

struct TermNode {
    struct FactorNode * factor;
    char op;        // '*' or '/' or '\0'
    struct TermNode * term;
};

struct FactorNode {
    int tag;        // 0: factor -> NUM, 1: factor -> ( expr )
    int num;
    char lparen;    // '('
    struct ExprNode * expr;
    char rparen;    // ')'
};


struct ExprNode * parse(void);


#endif
