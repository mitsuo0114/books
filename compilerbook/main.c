//
// Created by oono0 on 2019/01/14.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

int pos = 0;
Token tokens[100];
Vector *vtokens;
Node *code[100];

Vector *new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
    return vec;
}

void vec_push(Vector *vec, void *elem) {
    if (vec->capacity == vec->len) {
        vec->capacity += 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
}


Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

Node *new_node_ident(char* val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = *val;
    return node;
}

int consume(int ty) {
    if (tokens[pos].ty != ty) {
        return 0;
    }
    pos++;
    return 1;
}

Node *add() {
    Node *node = mul();
    for (;;) {
        if (consume('+')) {
            node = new_node('+', node, mul());
        } else if (consume('-')) {
            node = new_node('-', node, mul());
        } else {
            return node;
        }
    }
}

Node *mul() {
    Node *node = term();

    for (;;) {
        if (consume('*')) {
            node = new_node('*', node, term());
        } else if (consume('/')) {
            node = new_node('/', node, term());
        } else {
            return node;
        }
    }
}

Node *term() {
    if (consume('(')) {
        Node *node = add();
        if (!consume(')')) {
            fprintf(stderr, "Close blanket was not found: %s", tokens[pos].input);
        }
        return node;
    }

    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }

    if (tokens[pos].ty == TK_IDENT) {
        return new_node_ident(tokens[pos++].input);
    }
    // fprintf(stderr, "Unexpected Token: [%s]", tokens[pos].input);
}

Node *program() {
    int i = 0;
    while (tokens[pos].ty != TK_EOF) {
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node *stmt() {
    Node *node = assign();
    if (!consume(';')) {
        fprintf(stderr, "Not found [;] %s ", tokens[pos].input);
    }
    return node;
}

Node *assign() {
    Node *node = add();
    if (consume('=')) {
        node = assign();
    }
    return node;
}

void tokenize(char *p) {
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (*p == ';') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }
        if (*p == '=') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            tokens[i].ty = TK_IDENT;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }
        fprintf(stderr, "Unexpected Character error : %s", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

void gen_lval(Node *node) {
    if (node->ty != ND_IDENT) {
        fprintf(stderr, "left value is not variable");
    }

    int offset = ('z' - node->name + 1) * 8;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");

}

void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    if(node->ty == ND_IDENT){
        gen_lval(node);
        printf(" pop rax\n");
        printf(" mov rax, [rax]\n");
        printf(" push rax\n");
        return;
    }

    if(node->ty == '='){
        gen_lval(node->lhs);
        gen(node->rhs);

        printf(" pop rdi\n");
        printf(" pop rax\n");
        printf(" mov [rax], rdi\n");
        printf(" push rdi\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf(" pop rdi\n");
    printf(" pop rax\n");

    switch (node->ty) {
        case '+':
            printf("  add rax, rdi\n");
            break;
        case '-':
            printf("  sub rax, rdi\n");
            break;
        case '*':
            printf("  mul rdi\n");
            break;
        case '/':
            printf("  mov rdx, 0\n");
            printf("  div rdi\n");
            break;
    }
    printf("  push rax\n");
}


void error(int i) {
    fprintf(stderr, "Unexpected token : %d, %s\n", i, tokens[i].input);
    exit(1);
}


int expect(char *file, int line, int expected, int actual) {
    if (expected == actual) {
        return 0;
    }
    fprintf(stderr, "%s : LINE  %d, %d is expected, but got %d\n", file, line, expected, actual);
    exit(1);
}

void runtest() {
    Vector *vec = new_vector();
    expect(__FILE__, __LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++) {
        vec_push(vec, (void *) i);
    }
    expect(__FILE__, __LINE__, 100, vec->len);
    expect(__FILE__, __LINE__, 0, (int) vec->data[0]);
    expect(__FILE__, __LINE__, 50, (int) vec->data[50]);
    expect(__FILE__, __LINE__, 99, (int) vec->data[99]);

    printf("runtest() OK\n");
}

int main(int argc, char **argv) {
    if (strcmp(argv[1], "-test") == 0) {
        runtest();
        return 0;
    }

    if (argc != 2) {
        fprintf(stderr, "Invalid args");
        return 1;
    }
    vtokens = new_vector();
    tokenize(argv[1]);

    program();
    Node *node = add();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for (int i = 0; code[i] != NULL; i++){
        gen(code[i]);
        printf(" pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}
