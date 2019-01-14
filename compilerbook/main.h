enum {
    ND_NUM = 256,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

enum {
    TK_NUM = 256,
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

int consume(int ty);

void error(int i);

void tokenize(char *p);

void gen(Node *node);

int main(int argc, char **argv);