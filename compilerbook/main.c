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
    fprintf(stderr, "Unexpected Token: %s", tokens[pos].input);
}

void tokenize(char *p) {
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
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

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }
        fprintf(stderr, "Unexpected Charactor error : %s", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
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
    fprintf(stderr, "Unexpected token : %s\n", tokens[i].input);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid args");
        return 1;
    }
    tokenize(argv[1]);
    Node *node = add();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf(" pop rax\n");
    printf(" ret \n");
    return 0;
}