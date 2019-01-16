enum {
    ND_NUM = 256,
    ND_IDENT,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_EOF
};

typedef struct {
    int ty;
    long int val;
    char *input;
} Token;

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Node *new_node(int ty, Node *lhs, Node *rhs);

Node *new_node_num(int val);

Node *mul();

Node *add();

Node *term();

Node *program();

Node *stmt();

Node *assign();

int consume(int ty);

void error(int i);

void tokenize(char *p);

void gen(Node *node);

int main(int argc, char **argv);