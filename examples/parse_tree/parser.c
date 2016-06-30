#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "parser.h"

typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_EOF,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_NUM,
} TokenType;

#define MAX_TOKEN_LENGTH 100

TokenType ttype = TOKEN_UNKNOWN;
char token[MAX_TOKEN_LENGTH];
int line = 1;
bool active_token = false;

static void skip_spaces()
{
    char c = ' ';
    while (isspace(c) && !feof(stdin))
    {
        line += (c == '\n');
        c = getchar();
    }
    if (!feof(stdin))
        ungetc(c, stdin);
}

static int scan_number()
{
    int len = 0;
    char c;

    c = getchar();
    if (isdigit(c))
    {
        if (c == '0')
        {
            token[len++] = c;
        }
        else
        {
            while (isdigit(c) && !feof(stdin) && len < MAX_TOKEN_LENGTH)
            {
                token[len++] = c;
                c = getchar();
            }
            if (!feof(stdin))
                ungetc(c, stdin);
        }
        token[len++] = '\0';
        return TOKEN_NUM;
    }
    else
    {
        return TOKEN_UNKNOWN;
    }
}

TokenType next_token()
{
    char c;

    if (active_token)
    {
        active_token = false;
        return ttype;
    }

    skip_spaces();
    token[0] = 0;
    c = getchar();

    switch(c)
    {
        case '+': ttype = TOKEN_PLUS;   return ttype;
        case '-': ttype = TOKEN_MINUS;  return ttype;
        case '*': ttype = TOKEN_MULT;   return ttype;
        case '/': ttype = TOKEN_DIV;    return ttype;
        case '(': ttype = TOKEN_LPAREN; return ttype;
        case ')': ttype = TOKEN_RPAREN; return ttype;
        default:
            if (isdigit(c))
            {
                ungetc(c, stdin);
                ttype = scan_number();
                return ttype;
            }
            else if (c == EOF)
            {
                ttype = TOKEN_EOF;
                return ttype;
            }
            else
            {
                ttype = TOKEN_UNKNOWN;
                return ttype;
            }
    }
}

void unget_token()
{
    active_token = true;
}

// -----------------------------------------------------------------------------

void syntax_error(const char* func, const char* msg)
{
    fprintf(stderr, "Syntax error in %s: %s, line %d\n", func, msg, line);
    exit(1);
}

void do_expect(int desired_type, const char* func, const char* msg)
{
    next_token();
    if (ttype != desired_type)
        syntax_error(func, msg);
}

void peek_token()
{
    next_token();
    unget_token();
}

#define expect(token) do_expect(token, __func__, "expected " #token)
#define ALLOC(T)    (T*) calloc(1, sizeof(T))

struct ExprNode * parse_expr(void);
struct TermNode * parse_term(void);
struct FactorNode * parse_factor(void);

struct ExprNode * parse_expr(void)
{
    struct ExprNode * e = ALLOC(struct ExprNode);
    // expr -> term PLUS expr
    // expr -> term MINUS expr
    // expr -> term
    e->term = parse_term();
    peek_token();
    if (ttype == TOKEN_MINUS)
    {
        // expr -> term MINUS expr
        expect(TOKEN_MINUS);
        e->op = '-';
        e->expr = parse_expr();
    }
    else if (ttype == TOKEN_RPAREN || ttype == TOKEN_EOF)
    {
        // expr -> term
        e->op = '\0';
        e->expr = NULL;
    }
    else if (ttype == TOKEN_PLUS)
    {
        // expr -> term PLUS expr
        expect(TOKEN_PLUS);
        e->op = '+';
        e->expr = parse_expr();
    }
    else
    {
        syntax_error(__func__, "expected +, -, ), EOF");
    }
    return e;
}

struct TermNode * parse_term(void)
{
    struct TermNode * t = ALLOC(struct TermNode);
    // term -> factor MULT term
    // term -> factor DIV term
    // term -> factor
    t->factor = parse_factor();
    peek_token();
    if (ttype == TOKEN_RPAREN || ttype == TOKEN_EOF || ttype == TOKEN_PLUS || ttype == TOKEN_MINUS)
    {
        // term -> factor
        t->op = '\0';
        t->term = NULL;
    }
    else if (ttype == TOKEN_DIV)
    {
        // term -> factor DIV term
        expect(TOKEN_DIV);
        t->op = '/';
        t->term = parse_term();
    }
    else if (ttype == TOKEN_MULT)
    {
        // term -> factor MULT term
        expect(TOKEN_MULT);
        t->op = '*';
        t->term = parse_term();
    }
    else
    {
        syntax_error(__func__, "expected +, -, *, /, ), EOF");
    }
    return t;
}

struct FactorNode * parse_factor(void)
{
    struct FactorNode * f = ALLOC(struct FactorNode);
    // factor -> LPAREN expr RPAREN
    // factor -> NUM
    peek_token();
    if (ttype == TOKEN_NUM)
    {
        // factor -> NUM
        expect(TOKEN_NUM);
        f->tag = 0;
        f->num = atoi(token);
    }
    else if (ttype == TOKEN_LPAREN)
    {
        // factor -> LPAREN expr RPAREN
        expect(TOKEN_LPAREN);
        f->tag = 1;
        f->lparen = '(';
        f->expr = parse_expr();
        f->rparen = ')';
        expect(TOKEN_RPAREN);
    }
    else
    {
        syntax_error(__func__, "expected (, NUM");
    }
    return f;
}

struct ExprNode * parse(void)
{
    struct ExprNode * expr;

    expr = parse_expr();
    expect(TOKEN_EOF);
    return expr;
}

